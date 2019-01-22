#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh/IO.h>

#include <CGAL/boost/graph/io.h>
#include <CGAL/boost/graph/selection.h>

#include <boost/type_traits/is_same.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

typedef CGAL::Exact_predicates_inexact_constructions_kernel           EPICK;
typedef CGAL::Exact_predicates_exact_constructions_kernel             EPECK;

typedef CGAL::Polyhedron_3<EPICK, CGAL::Polyhedron_items_with_id_3>   Polyhedron;
typedef CGAL::Polyhedron_3<EPECK, CGAL::Polyhedron_items_with_id_3>   Exact_polyhedron;
typedef CGAL::Surface_mesh<EPICK::Point_3>                            Surface_mesh;

template<typename Mesh>
void initialize_IDs(Mesh& sm) { CGAL::set_halfedgeds_items_id(sm); }
void initialize_IDs(Surface_mesh&) { }

template<typename Mesh>
void test_full_mesh(const char* filename,
                    const bool expected_result)
{
  std::cout << "testing " << filename << "..." << std::endl;

  Mesh sm;
  std::ifstream input(filename);
  if(!input || !(input >> sm))
  {
    std::cerr << "Error: could not read input: " << filename << std::endl;
    assert(false);
  }

  initialize_IDs(sm);

  assert(CGAL::is_face_range_a_topological_disk(faces(sm), sm) == expected_result);
  assert(CGAL::is_polygon_mesh_a_topological_disk(sm) == expected_result); // same thing, but different function
}

template<typename Mesh>
void test_mesh_subset(const char* mesh_filename,
                      const char* mesh_selection_filename,
                      const bool expected_result)
{
  std::cout << "Testing " << mesh_filename << " ";
  std::cout << "with selection " << mesh_selection_filename << "..." << std::endl;

  typedef typename boost::graph_traits<Mesh>::face_descriptor        face_descriptor;

  Mesh sm;
  std::ifstream input(mesh_filename);
  if(!input || !(input >> sm))
  {
    std::cerr << "Error: could not read input: " << mesh_filename << std::endl;
    assert(false);
  }

  std::ifstream selection_input(mesh_selection_filename);
  std::list<face_descriptor> selected_faces;
  std::string line;
  // skip the first line (faces are on the second line)
  if(!selection_input || !std::getline(selection_input, line) || !std::getline(selection_input, line))
  {
    std::cerr << "Error: could not read input: " << mesh_filename << std::endl;
    assert(false);
  }

  initialize_IDs(sm);

  std::istringstream face_line(line);
  std::size_t face_id;
  while(face_line >> face_id)
    selected_faces.push_back(*(faces(sm).begin() + face_id));
  std::cout << selected_faces.size() << " faces selected" << std::endl;

  assert(CGAL::is_face_range_a_topological_disk(selected_faces, sm) == expected_result);
}

int main(int, char**)
{
  test_full_mesh<Surface_mesh>("data/7_faces_triangle.off", true);
  test_full_mesh<Exact_polyhedron>("data/cube-quads.off", false); // topo ball
  test_full_mesh<Polyhedron>("data/genus3.off", false); // genus 3
  test_full_mesh<Surface_mesh>("data/hedra_open.off", true);
  test_full_mesh<Exact_polyhedron>("data/prim.off", false); // topo ball with mixed elements
  test_full_mesh<Polyhedron>("data/quad.off", true); // single quad
  test_full_mesh<Surface_mesh>("data/triangular_hole.off", false); // genus 1
  test_full_mesh<Surface_mesh>("data/head.off", false); // topo disk with holes
  test_full_mesh<Surface_mesh>("data/pinched_mesh.off", false); // disk with a non manifold vertex

  // test the non-manifold vertex detection
  test_mesh_subset<Surface_mesh>("data/pinched_mesh.off", "data/pinched_mesh_topo_disk_1.selection.txt", true);
  test_mesh_subset<Surface_mesh>("data/pinched_mesh.off", "data/pinched_mesh_topo_disk_2.selection.txt", true);
  test_mesh_subset<Surface_mesh>("data/pinched_mesh.off", "data/pinched_mesh_not_topo_disk_1.selection.txt", false);
  test_mesh_subset<Surface_mesh>("data/pinched_mesh.off", "data/pinched_mesh_not_topo_disk_2.selection.txt", false);
  test_mesh_subset<Surface_mesh>("data/pinched_mesh.off", "data/pinched_mesh_not_topo_disk_3.selection.txt", false);

  std::cout << "OK" << std::endl;

  return EXIT_SUCCESS;
}
