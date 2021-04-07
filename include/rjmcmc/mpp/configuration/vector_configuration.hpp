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

#ifndef VECTOR_CONFIGURATION_HPP
#define VECTOR_CONFIGURATION_HPP

#include "configuration.hpp"
#include "rjmcmc/util/variant.hpp"

namespace marked_point_process {

    template<typename T, typename UnaryEnergy, typename BinaryEnergy, typename Accelerator=trivial_accelerator>
    class vector_configuration
    {
	typedef	std::vector<T> container;
	container	m_container;
	UnaryEnergy	m_unary_energy;
	BinaryEnergy	m_binary_energy;
	Accelerator	m_accelerator;
    public:
        typedef typename container::const_iterator const_iterator;
        typedef typename container::iterator       iterator;
        typedef T					value_type;
	typedef vector_configuration<T,UnaryEnergy, BinaryEnergy, Accelerator> self;
        typedef internal::modification<self>	        modification;


        vector_configuration(UnaryEnergy unary_energy, BinaryEnergy binary_energy, Accelerator accelerator=Accelerator())
            : m_unary_energy(unary_energy), m_binary_energy(binary_energy), m_accelerator(accelerator)
	{}


        // objects
        inline size_t size() const { return m_container.size(); }
        inline bool empty() const {	return m_container.empty(); }
        inline iterator begin() { return m_container.begin(); }
        inline iterator end  () { return m_container.end  (); }
        inline const_iterator begin() const { return m_container.begin(); }
        inline const_iterator end  () const { return m_container.end  (); }
        inline const value_type& value( const_iterator v ) const { return *v; }


        // container
	inline void clear() { m_container.clear(); }

        template<typename U>
        void insert(const U&u) {
            m_container.push_back(u);
	}
        void remove( const_iterator  v ) {
            std::swap(const_cast<value_type&>(*v),m_container.back());
            m_container.pop_back();
	}

        template<typename F> inline void for_each(F f)       { std::for_each(m_container.begin(),m_container.end(),f); }
        template<typename F> inline void for_each(F f) const { std::for_each(m_container.begin(),m_container.end(),f); }

        // energy
        inline double energy () const {
            return unary_energy()+binary_energy();
        }

        double unary_energy() const
        {
            double e = 0.;
            for (const_iterator it = m_container.begin(); it != m_container.end(); ++it)
                e += rjmcmc::apply_visitor(m_unary_energy, *it );
            return e;
        }

        double binary_energy() const
        {
            double e = 0.;
            for (const_iterator i = m_container.begin(); i != m_container.end(); ++i)
                for (const_iterator j = i+1; j != m_container.end(); ++j)
                    e += rjmcmc::apply_visitor(m_binary_energy, *i, *j );
            return e;
        }

        // delta energy
        template <typename Modification> double delta_energy(const Modification &modif) const
        {
            return delta_birth(modif)+delta_death(modif);
        }

        template <typename Modification> double delta_birth(const Modification &modif) const
        {
            double delta = 0;
            typedef typename Modification::birth_type::const_iterator bci;
            typedef typename Modification::death_type::const_iterator dci;
            bci bbeg = modif.birth().begin();
            bci bend = modif.birth().end();
            dci dbeg = modif.death().begin();
            dci dend = modif.death().end();
            for(bci it=bbeg; it!=bend; ++it) {
                delta += rjmcmc::apply_visitor(m_unary_energy,*it);
                for (const_iterator it2=m_container.begin(); it2 != m_container.end(); ++it2)
                    if (std::find(dbeg,dend,it2)==dend)
                        delta += rjmcmc::apply_visitor(m_binary_energy, *it, value(it2) );
                for (bci it2=it+1; it2 != bend; ++it2)
                    delta += rjmcmc::apply_visitor(m_binary_energy, *it, *it2);
            }
            return delta;
        }

        template <typename Modification> double delta_death(const Modification &modif) const
        {
            double delta = 0;
            typedef typename Modification::death_type::const_iterator dci;
            dci dbeg = modif.death().begin();
            dci dend = modif.death().end();
            for(dci it=dbeg; it!=dend; ++it) {
                delta -= rjmcmc::apply_visitor(m_unary_energy, value(*it));
                for (const_iterator it2=m_container.begin(); it2 != m_container.end(); ++it2)
                    if(std::find(it,dend,it2)==dend)
                        delta -= rjmcmc::apply_visitor(m_binary_energy, value(*it), *it2 );
            }
            return delta;
        }

        // audit energy
	inline double audit_unary_energy () const { return unary_energy();}
	inline double audit_binary_energy() const { return binary_energy();}
	inline unsigned int audit_structure() const { return 0; }
    };


}; // namespace marked_point_process

#endif // VECTOR_CONFIGURATION_HPP
