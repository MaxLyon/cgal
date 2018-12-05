#define CGAL_PARAMETERIZATION_ARAP_VERBOSE

#include <CGAL/Simple_cartesian.h>

#include <CGAL/Surface_mesh.h>

#include <CGAL/Surface_mesh_parameterization/IO/File_off.h>
#include <CGAL/Surface_mesh_parameterization/parameterize.h>
#include <CGAL/surface_mesh_parameterization.h>

#include <CGAL/Polygon_mesh_processing/measure.h>

#include <iostream>
#include <fstream>

typedef CGAL::Simple_cartesian<double>          Kernel;
typedef Kernel::Point_2                         Point_2;
typedef Kernel::Point_3                         Point_3;
typedef CGAL::Surface_mesh<Kernel::Point_3>     SurfaceMesh;

typedef boost::graph_traits<SurfaceMesh>::vertex_descriptor     vertex_descriptor;
typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor   halfedge_descriptor;
typedef boost::graph_traits<SurfaceMesh>::face_descriptor       face_descriptor;

namespace SMP = CGAL::Surface_mesh_parameterization;

int main(int argc, char** argv)
{
  std::ifstream in((argc>1) ? argv[1] : "data/quad.off");
  if(!in) {
    std::cerr << "Problem loading the input data" << std::endl;
    return EXIT_FAILURE;
  }

  SurfaceMesh sm;
  in >> sm;

  // a halfedge on the border
  halfedge_descriptor bhd = CGAL::Polygon_mesh_processing::longest_border(sm).first;

  // The UV property map that holds the parameterized values
  typedef SurfaceMesh::Property_map<vertex_descriptor, Point_2>  UV_pmap;
  UV_pmap uv_map = sm.add_property_map<vertex_descriptor, Point_2>("h:uv").first;

  // Indices map
  typedef boost::unordered_map<vertex_descriptor, int> Indices;
  Indices indices;
  CGAL::Polygon_mesh_processing::connected_component(
    face(opposite(bhd, sm), sm),
    sm,
    boost::make_function_output_iterator(
              SMP::internal::Index_map_filler<SurfaceMesh, Indices>(sm, indices)));
  boost::associative_property_map<Indices> vipm(indices);

  // Parameterized bool pmap
  boost::unordered_set<vertex_descriptor> vs;
  SMP::internal::Bool_property_map< boost::unordered_set<vertex_descriptor> > vpm(vs);

  SMP::ARAP_parameterizer_3<SurfaceMesh> parameterizer;
  parameterizer.parameterize(sm, bhd, uv_map, vipm, vpm);

  std::cout << "done" << std::endl;
  std::ofstream out("result.off");
  SMP::IO::output_uvmap_to_off(sm, bhd, uv_map, out);
  std::cout << "done" << std::endl;

  return EXIT_SUCCESS;
}
