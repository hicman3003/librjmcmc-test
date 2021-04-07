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

#ifndef OSTREAM_VISITOR_HPP
#define OSTREAM_VISITOR_HPP

#include <iostream>
#include <iomanip>

#if USE_CPP11
#include <chrono>
#else
#include <time.h>
#endif

namespace simulated_annealing {

#if USE_CPP11
  	using namespace std::chrono;
#endif

    class ostream_visitor {
    private:
        unsigned int *m_proposed;
        unsigned int *m_accepted;

#if USE_CPP11
	high_resolution_clock::time_point m_clock_begin, m_clock;
#else
        clock_t m_clock_begin, m_clock;
#endif

        unsigned int m_dump;
        unsigned int m_iter;
        int w;
	int p;
        std::ostream& m_out;
        bool m_add_endline; // todo: compile time with mpl::true_/false_

    public:
        ostream_visitor(std::ostream& out=std::cout, bool add_endline=true) : m_proposed(NULL), m_accepted(NULL), m_out(out), m_add_endline(add_endline) {}
        ~ostream_visitor() {
            if(m_accepted) delete m_accepted;
            if(m_proposed) delete m_proposed;
        }

        void init(unsigned int dump, unsigned int) {
            m_iter = 0;
            m_dump = dump;
        }

        template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler& sampler, double)
        {
            unsigned int kernel_size =  sampler.kernel_size();

            if(m_accepted) delete m_accepted;
            if(m_proposed) delete m_proposed;
            m_accepted = new unsigned int[kernel_size];
            m_proposed = new unsigned int[kernel_size];
            for (unsigned int i=0; i<kernel_size; ++i) m_accepted[i] = m_proposed[i] = 0;

            m_out.fill(' ');
            w = 20; // TODO make that configurable?
            p = 4; // TODO make that configurable?
            m_out << std::setw(w) << "Iteration" << std::setw(w)<< "Objects";
            for(unsigned int i=0; i<kernel_size; ++i)
            {
                std::string s = sampler.kernel_name(i);
                m_out << std::setw(w)<<s;
             //   m_out << std::setw(w)<<("P"+s);
             //   m_out << std::setw(w)<<("A"+s);
            }
            m_out << std::setw(w)<<"Accept"<< std::setw(w)<<"Time(microseconds)";
            m_out << std::setw(w)<<"Temp"<< std::setw(w)<<"U_1";
            m_out << std::setw(w)<<"U_2"<< std::setw(w)<<"U";
            m_out << std::setw(w) << "Accept_prob";
            m_out << std::setw(w) << "Delta";
            m_out << std::setw(w) << "Green_ratio";
            m_out << std::setw(w) << "Accepted";
            m_out << std::setw(w) << "Kernel_ratio";
            m_out << std::setw(w) << "Ref_pdf_ratio";

            if(m_add_endline)
                m_out << std::endl;
            m_out << std::flush;

#if USE_CPP11
	m_clock_begin = m_clock = std::chrono::high_resolution_clock::now();
#else
        m_clock_begin = m_clock = clock();
#endif
	    
        }
        template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler&, double)
        {
            m_out << "Iterations finished" << std::endl;
#if USE_CPP11
	    high_resolution_clock::time_point clock_end = std::chrono::high_resolution_clock::now();
            m_out << "Total elapsed time (s) :  " << duration_cast<duration<double>>(clock_end - m_clock_begin).count() << std::endl;
#else
            clock_t clock_end = clock();
            m_out << "Total elapsed time (s) :  " << double(clock_end - m_clock_begin) / CLOCKS_PER_SEC << std::endl;
#endif
            
            m_out << "Graph Data energy integrity : " << config.audit_unary_energy() << "=" << config.unary_energy() << std::endl;
            m_out << "Graph Prior energy integrity: " << config.audit_binary_energy() << "=" << config.binary_energy()<< std::endl;
            m_out << "Graph Structure integrity : " << config.audit_structure() << std::endl;
            //  m_out << config;
            m_out << std::endl << std::flush;
        }

        template<typename Configuration, typename Sampler>
        void visit(const Configuration& config, const Sampler& sampler, double t) {
            unsigned int kernel_size =  sampler.kernel_size();

            m_proposed[sampler.kernel_id()]++;
            if( sampler.accepted() ) m_accepted[sampler.kernel_id()]++;

            ++m_iter;
            if (m_dump && (m_iter % m_dump == 0) )
            {
                m_out << std::setw(w) << m_iter;
                m_out << std::setw(w) << config.size();

                unsigned int total_accepted =0;
                for(unsigned int k=0; k<kernel_size; ++k)
                {
                   // m_out << std::setw(w) << 100.* m_proposed[k] / m_dump;
                   // if(m_proposed[k]) m_out << std::setw(w) << (100.* m_accepted[k]) / m_proposed[k];
		   m_out << std::setw(w) << std::setprecision(p) << (m_proposed[k] ? (100.* double(m_accepted[k])) / double(m_proposed[k]) : 100.);
                   total_accepted += m_accepted[k];
                   m_accepted[k] = m_proposed[k] = 0;
                }
                m_out << std::setw(w) << std::setprecision(p) << (100.*total_accepted) / m_dump;
#if USE_CPP11
	        high_resolution_clock::time_point clock_temp = high_resolution_clock::now();
		m_out << std::setw(w) << std::setprecision(p) << double(std::chrono::duration_cast<std::chrono::microseconds>(clock_temp - m_clock).count())/ m_dump;
#else
            	clock_t clock_temp = clock();
		m_out << std::setw(w) << std::setprecision(p) << ((clock_temp - m_clock)*1000.)/ m_dump;
#endif                
                m_clock = clock_temp;
                m_out << std::setw(w) << std::setprecision(p) << t;
                m_out << std::setw(w) << std::setprecision(p) << config.unary_energy();
                m_out << std::setw(w) << std::setprecision(p) << config.binary_energy();
                m_out << std::setw(w) << std::setprecision(p) << config.energy();
                m_out << std::setw(w) << std::setprecision(p) << sampler.acceptance_probability();
                //m_out << std::setw(w) << std::setprecision(p) << sampler.temperature();
                m_out << std::setw(w) << std::setprecision(p) << sampler.delta();
                m_out << std::setw(w) << std::setprecision(p) << sampler.green_ratio();
                m_out << std::setw(w) << std::setprecision(p) << sampler.accepted();
                m_out << std::setw(w) << std::setprecision(p) << sampler.kernel_ratio();
                m_out << std::setw(w) << std::setprecision(p) << sampler.ref_pdf_ratio();

                if(m_add_endline)
                    m_out << std::endl;
                m_out << std::flush;
            }
        }
    };

}; // namespace simulated_annealing

#endif // OSTREAM_VISITOR_HPP
