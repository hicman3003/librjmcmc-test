/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef BUILDING_FOOTPRINT_RECTANGLE_HPP
#define BUILDING_FOOTPRINT_RECTANGLE_HPP

#include "rjmcmc/util/random.hpp"
//<-
/************** application-specific types ****************/
//->

//[building_footprint_rectangle_definition_geometry
#include "geometry.hpp"
typedef Rectangle_2 object;
//]

//[building_footprint_rectangle_definition_kernels
#include "rjmcmc/rjmcmc/kernel/raster_variate.hpp"
#include "rjmcmc/mpp/kernel/uniform_birth.hpp"
typedef marked_point_process::uniform_birth<object,rjmcmc::raster_variate<5> > uniform_birth;
#include "rjmcmc/mpp/kernel/uniform_birth_death_kernel.hpp"
typedef marked_point_process::uniform_birth_death_kernel<uniform_birth>::type  birth_death_kernel;

#include "rjmcmc/rjmcmc/kernel/transform.hpp"
#include "rjmcmc/geometry/transform/rectangle_corner_translation_transform.hpp"
#include "rjmcmc/geometry/transform/rectangle_edge_translation_transform.hpp"
#include "rjmcmc/geometry/transform/rectangle_split_merge_transform.hpp"
typedef geometry::rectangle_edge_translation_transform<0>    edge_transform0;
typedef geometry::rectangle_edge_translation_transform<1>    edge_transform1;
typedef geometry::rectangle_edge_translation_transform<2>    edge_transform2;
typedef geometry::rectangle_edge_translation_transform<3>    edge_transform3;
typedef geometry::rectangle_corner_translation_transform<0>  corner_transform0;
typedef geometry::rectangle_corner_translation_transform<1>  corner_transform1;
typedef geometry::rectangle_corner_translation_transform<2>  corner_transform2;
typedef geometry::rectangle_corner_translation_transform<3>  corner_transform3;
typedef geometry::rectangle_split_merge_transform_v2 split_merge_transform;

#include "rjmcmc/mpp/kernel/uniform_kernel.hpp"
typedef marked_point_process::uniform_kernel<object,1,1,edge_transform0>::type  edge_kernel0;
typedef marked_point_process::uniform_kernel<object,1,1,edge_transform1>::type  edge_kernel1;
typedef marked_point_process::uniform_kernel<object,1,1,edge_transform2>::type  edge_kernel2;
typedef marked_point_process::uniform_kernel<object,1,1,edge_transform3>::type  edge_kernel3;
typedef marked_point_process::uniform_kernel<object,1,1,corner_transform0>::type  corner_kernel0;
typedef marked_point_process::uniform_kernel<object,1,1,corner_transform1>::type  corner_kernel1;
typedef marked_point_process::uniform_kernel<object,1,1,corner_transform2>::type  corner_kernel2;
typedef marked_point_process::uniform_kernel<object,1,1,corner_transform3>::type  corner_kernel3;
typedef marked_point_process::uniform_kernel<object,1,2,split_merge_transform>::type  split_merge_kernel;
//]

//[building_footprint_rectangle_definition_energies
#include "rjmcmc/rjmcmc/energy/energy_operators.hpp"
#include "rjmcmc/mpp/energy/image_gradient_unary_energy.hpp"
#include "rjmcmc/image/gradient_functor.hpp"
#include "rjmcmc/image/oriented.hpp"
typedef oriented<gradient_image_t>                          oriented_gradient_image;
typedef image_gradient_unary_energy<oriented_gradient_image> unary_energy;

#include "rjmcmc/mpp/energy/intersection_area_binary_energy.hpp"
typedef intersection_area_binary_energy<>                   binary_energy;
//]

//<-
/************** rjmcmc library types ****************/
//->

//[building_footprint_rectangle_definition_simulated_annealing
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "rjmcmc/simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/
#include "rjmcmc/simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test     end_test;
//]

//[building_footprint_rectangle_definition_configuration

#include "rjmcmc/mpp/energy/image_center_unary_energy.hpp"
typedef oriented<boost::gil::gray16_image_t> mask_type;
#include "rjmcmc/mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<
        object,
        minus_energy<constant_energy<>,multiplies_energy<constant_energy<>,unary_energy> >,
        multiplies_energy<constant_energy<>,binary_energy>
        > configuration;
//]

//[building_footprint_rectangle_definition_distribution
#include "rjmcmc/rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution                           distribution;
//]

//[building_footprint_rectangle_definition_sampler
#include "rjmcmc/rjmcmc/sampler/sampler.hpp"

#include "rjmcmc/mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,uniform_birth> d_sampler;

#include "rjmcmc/rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

//typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel> sampler;
typedef rjmcmc::sampler<d_sampler,acceptance
        ,birth_death_kernel
        ,split_merge_kernel
        ,edge_kernel0
        ,edge_kernel1
        ,edge_kernel2
        ,edge_kernel3
        ,corner_kernel0
        ,corner_kernel1
        ,corner_kernel2
        ,corner_kernel3
        > sampler;
//]


//[building_footprint_rectangle_bbox_accessors
Iso_rectangle_2 get_bbox(const param *p) {
    int x0 = p->get<int>("xmin");
    int x1 = p->get<int>("xmax");
    int y0 = p->get<int>("ymin");
    int y1 = p->get<int>("ymax");
    if(x0>x1) std::swap(x0,x1);
    if(y0>y1) std::swap(y0,y1);
    return Iso_rectangle_2(x0,y0,x1,y1);
}

void set_bbox(param *p, const Iso_rectangle_2& r) {
    p->set("xmin",(int) r.min().x());
    p->set("ymin",(int) r.min().y());
    p->set("xmax",(int) r.max().x());
    p->set("ymax",(int) r.max().y());
}
//]

//[building_footprint_rectangle_create_configuration
#include "rjmcmc/image/conversion_functor.hpp"
#include <boost/gil/extension/io_new/tiff_write.hpp>

void create_configuration(const param *p, const oriented_gradient_image& grad, configuration *&c) {
    std::string mask_file = p->get<boost::filesystem::path>("mask" ).string();
    if(mask_file!="")
    {
        Iso_rectangle_2 bbox = get_bbox(p);
        clip_bbox(bbox,mask_file);
        mask_type mask(mask_file , bbox, conversion_functor() );
        boost::gil::write_view( mask_file+"_x0.tif" , boost::gil::nth_channel_view(grad.view(),0), boost::gil::tiff_tag() );
        boost::gil::write_view( mask_file+"_y0.tif" , boost::gil::nth_channel_view(grad.view(),1), boost::gil::tiff_tag() );
        for(int j=0; j<grad.view().height();++j)
        {
            for(int i=0; i<grad.view().width();++i)
            {
                std::cout << mask.view()(i,j)<< " ";
                if (i < mask.view().width() && j < mask.view().height() && mask.view()(i,j)<=0)
                {
                    boost::gil::at_c<0>(grad.view()(i,j)) = boost::gil::at_c<1>(grad.view()(i,j)) = 0;
                }
            }
        }
        boost::gil::write_view( mask_file+"_x.tif" , boost::gil::nth_channel_view(grad.view(),0), boost::gil::tiff_tag() );
        boost::gil::write_view( mask_file+"_y.tif" , boost::gil::nth_channel_view(grad.view(),1), boost::gil::tiff_tag() );
    }

    // empty initial configuration
    c = new configuration( p->get<double>("energy")-(p->get<double>("ponderation_grad")*unary_energy(grad)),
                           p->get<double>("ponderation_surface")*binary_energy());
}
//]

//[building_footprint_rectangle_create_sampler
void create_sampler(const param *p, sampler *&s) {
    Iso_rectangle_2 r = get_bbox(p);
    double maxsize  = p->get<double>("maxsize");
    double maxratio = p->get<double>("maxratio");
    double minratio = 1./maxratio;

    int size[] = {1,1,1,1,1};
    short unsigned prob0[] = {1};
    short unsigned *prob = prob0;
    std::string birth_prob_file = p->get<boost::filesystem::path>("birth_prob" ).string();
    if(birth_prob_file!="")
    {
        unsigned short birth_prob_offset = (unsigned short) (p->get<double>("birth_prob_offset" ));
        clip_bbox(r,birth_prob_file);
        mask_type birth_prob(birth_prob_file , r, conversion_functor() );
        mask_type::view_t view = birth_prob.view();
        size[0] = view.width ();
        size[1] = view.height();
        prob = boost::gil::interleaved_view_get_raw_data(view); // should work in general on single channel images
        for(int i=0; i< size[0]*size[1]; ++i) prob[i] = std::max(prob[i],birth_prob_offset)-birth_prob_offset;
    }

    K::Vector_2 v(maxsize,maxsize);
    uniform_birth birth(
            Rectangle_2(r.min(),-v,minratio),
            Rectangle_2(r.max(), v,maxratio),
            rjmcmc::raster_variate<5>(prob,size)
            );

    distribution cs(p->get<double>("poisson"));

    d_sampler ds( cs, birth );

    typedef rjmcmc::mt19937_generator Engine;
    Engine& e = rjmcmc::random();
    marked_point_process::graph_configuration<object, constant_energy<>, constant_energy<> > c(1,1);
    ds(e,c);
    double p_birthdeath  = p->get<double>("p_birthdeath");
    double p_birth  = p->get<double>("p_birth");
    double p_edge   = 0.25*p->get<double>("p_edge");
    double p_corner = 0.25*p->get<double>("p_corner");
    double p_split_merge = p->get<double>("p_split_merge");
    double p_split       = p->get<double>("p_split");
    birth_death_kernel k_birth_death = marked_point_process::make_uniform_birth_death_kernel(birth, p_birthdeath, p_birth );
    edge_kernel0 k_edge0 = marked_point_process::make_uniform_kernel<object,1,1>(edge_transform0(minratio,maxratio),p_edge);
    edge_kernel1 k_edge1 = marked_point_process::make_uniform_kernel<object,1,1>(edge_transform1(minratio,maxratio),p_edge);
    edge_kernel2 k_edge2 = marked_point_process::make_uniform_kernel<object,1,1>(edge_transform2(minratio,maxratio),p_edge);
    edge_kernel3 k_edge3 = marked_point_process::make_uniform_kernel<object,1,1>(edge_transform3(minratio,maxratio),p_edge);
    corner_kernel0 k_corner0 = marked_point_process::make_uniform_kernel<object,1,1>(corner_transform0(),p_corner);
    corner_kernel1 k_corner1 = marked_point_process::make_uniform_kernel<object,1,1>(corner_transform1(),p_corner);
    corner_kernel2 k_corner2 = marked_point_process::make_uniform_kernel<object,1,1>(corner_transform2(),p_corner);
    corner_kernel3 k_corner3 = marked_point_process::make_uniform_kernel<object,1,1>(corner_transform3(),p_corner);
    k_birth_death.name(0,"birth");
    k_birth_death.name(1,"death");
    k_edge0.name(0,"edge00");
    k_edge0.name(1,"edge01");
    k_edge1.name(0,"edge10");
    k_edge1.name(1,"edge11");
    k_edge2.name(0,"edge20");
    k_edge2.name(1,"edge21");
    k_edge3.name(0,"edge30");
    k_edge3.name(1,"edge31");
    k_corner0.name(0,"corner00");
    k_corner0.name(1,"corner01");
    k_corner1.name(0,"corner10");
    k_corner1.name(1,"corner11");
    k_corner2.name(0,"corner20");
    k_corner2.name(1,"corner21");
    k_corner3.name(0,"corner30");
    k_corner3.name(1,"corner31");


    split_merge_kernel k_split_merge = marked_point_process::make_uniform_kernel<object,1,2>(split_merge_transform(),p_split_merge, p_split);
    k_split_merge.name(0,"split");
    k_split_merge.name(1,"merge");

    s = new sampler( ds, acceptance()
                     , k_birth_death
                     , k_split_merge
                     , k_edge0
                     , k_edge1
                     , k_edge2
                     , k_edge3
                     , k_corner0
                     , k_corner1
                     , k_corner2
                     , k_corner3
                     );
}
//]

//[building_footprint_rectangle_create_schedule
void create_schedule(const param *p, schedule *&t)
{
    t = new schedule(
            p->get<double>("temp"),
            p->get<double>("deccoef")
            );
}
//]

//[building_footprint_rectangle_create_end_test
void create_end_test(const param *p, end_test *&e)
{
    e = new end_test(
            p->get<int>("nbiter")
            );
}
//]

//[building_footprint_rectangle_init_visitor
template<typename Visitor>
void init_visitor(const param *p, Visitor& v)
{
    v.init(
            p->get<int>("nbdump"),
            p->get<int>("nbsave")
            );
}
//]

//[building_footprint_rectangle_image_include_tpl_instanciations
#include "rjmcmc/image/image_types.hpp"
#include "rjmcmc/image/oriented_inc.hpp"
#include "rjmcmc/image/gradient_functor_inc.hpp"
//]

//[building_footprint_rectangle_optimization
#include "rjmcmc/simulated_annealing/simulated_annealing.hpp"
//]

#endif // BUILDING_FOOTPRINT_RECTANGLE_HPP
