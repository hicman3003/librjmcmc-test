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

#ifndef RANDOM_APPLY_HPP
#define RANDOM_APPLY_HPP

#include "tuple.hpp"

namespace rjmcmc {

    /*
 random_apply(x,t,f) applies
- the function object f
- to a random element of the tuple t (according to their probability())
- x should be drawn uniformly in [0,1]
*/

    namespace detail {

        template <unsigned int I, unsigned int N> struct random_apply_impl
        {
            template <typename T, typename F>
                    inline typename F::result_type operator()(unsigned int& i, double x, T& t, F &f) {
                double y = x - get<I>(t).probability();
                if(y>0) return random_apply_impl<I+1,N>()(i,y,t,f);
                i = I;
                return f(x,get<I>(t));
            }
        };

        template <unsigned int N> struct random_apply_impl<N,N>
        {
            template <typename T, typename F>
                    inline typename F::result_type operator()(unsigned int& i, double x, T& t, F &f) {
                i = N;
                return typename F::result_type();
            }
        };

        template <unsigned int I, unsigned int N> struct random_apply_normalisation
        {
            template <typename T>
                    inline double operator()(const T& t) {
                return get<I>(t).probability()+random_apply_normalisation<I+1,N>()(t);
            }
        };

        template <unsigned int N> struct random_apply_normalisation<N,N>
        {
            template <typename T>
                    inline double operator()(const T& t) {
                return 0;
            }
        };
    };

    template <typename T, typename F>
            inline typename F::result_type random_apply(unsigned int& i, double x, T& t, F &f) {
        double normalisation = detail::random_apply_normalisation<0,tuple_size<T>::value>()(t);
        return detail::random_apply_impl<0,tuple_size<T>::value>()(i,x*normalisation,t,f);
    }

}; // namespace rjmcmc

#endif // RANDOM_APPLY_HPP
