#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL/Polygon_mesh_processing/repair.h>

#include <iostream>
#include <fstream>

//note : when
//CGAL::get_default_random()::get_seed() = 1473902576
//the last test (on trihole.off) does not terminate
//

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Surface_mesh<K::Point_3> Surface_mesh;

void fix(const char* fname)
{
  std::cout << "test " << fname << std::endl;

  std::ifstream input(fname);
  Surface_mesh mesh;
  if (!input || !(input >> mesh) || mesh.is_empty()) {
    std::cerr << fname << " is not a valid off file.\n";
    exit(1);
  }

  typedef boost::graph_traits<Surface_mesh>::face_descriptor  face_descriptor;
  std::vector<face_descriptor> dfaces;
  CGAL::Polygon_mesh_processing::degenerate_faces(mesh, std::back_inserter(dfaces));
  std::cout << dfaces.size() << " degenerate faces in input" << std::endl;

  CGAL::Polygon_mesh_processing::remove_degenerate_faces(mesh);

  dfaces.clear();
  CGAL::Polygon_mesh_processing::degenerate_faces(mesh, std::back_inserter(dfaces));
  std::cout << dfaces.size() << " degenerate faces after cleaning" << std::endl;

  assert( CGAL::is_valid_polygon_mesh(mesh) );
}

int main()
{
//  fix("data_degeneracies/deg_on_border.off");
  fix("data_degeneracies/existing_flip.off");
//  fix("data_degeneracies/degtri_2dt_1edge_split_twice.off");
//  fix("data_degeneracies/degtri_four-2.off");
//  fix("data_degeneracies/degtri_four.off");
//  fix("data_degeneracies/degtri_on_border.off");
//  fix("data_degeneracies/degtri_three.off");
//  fix("data_degeneracies/degtri_single.off");
//  fix("data_degeneracies/trihole.off");

  return 0;
}
