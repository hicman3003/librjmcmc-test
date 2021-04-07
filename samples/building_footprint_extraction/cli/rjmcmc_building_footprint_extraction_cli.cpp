// parameters
#include "rjmcmc/param/parameter.hpp"
typedef parameters< parameter > param;
#include "core/building_footprint_extraction_parameters_inc.hpp"

// images
#include "rjmcmc/image/image_types.hpp"

// optimization
#include "core/building_footprint_extraction.hpp"
#include "rjmcmc/simulated_annealing/simulated_annealing.hpp"

// visitors
#include "rjmcmc/simulated_annealing/visitor/ostream_visitor.hpp"

// template instanciations
#include "rjmcmc/image/oriented_inc.hpp"
#include "rjmcmc/image/gradient_functor_inc.hpp"
#include "core/global_reconstruction_unary_energy_inc.hpp"

int main(int argc , char** argv)
{
  param *p = param::instance();
  initialize_parameters(p);
  if (!p->parse(argc, argv)) return -1;
  
  simulated_annealing::ostream_visitor visitor;
  init_visitor(p,visitor);
  
  Iso_rectangle_2 bbox = get_bbox(p);
  std::string  dsm_file = p->get<boost::filesystem::path>("dsm" ).string();
  std::string ndvi_file = p->get<boost::filesystem::path>("ndvi").string();
  clip_bbox(bbox,dsm_file );
  clip_bbox(bbox,ndvi_file);
  
  gradient_functor gf(p->get<double>("sigmaD"));
  oriented_gradient_view grad_view(dsm_file,  bbox, gf);
  oriented_ndvi_view     ndvi_view(ndvi_file, bbox);
  
  configuration *conf;
  sampler       *samp;
  schedule      *sch;
  end_test      *end;
  
  create_configuration(p,grad_view,ndvi_view,conf);
  create_sampler      (p,samp);
  create_schedule     (p,sch);
  create_end_test     (p,end);
  
  simulated_annealing::optimize(*conf,*samp,*sch,*end,visitor);
  
  return 0;
}
