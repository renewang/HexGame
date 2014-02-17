/*
 * global.h
 *
 *  Created on: Jan 13, 2014
 *      Author: renewang
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

//#include <boost/config.hpp>
#include <iostream>

// enforce to use boost::tr1
// Remove, if compiler has its own tr1 implementation
/*
 namespace std {
 using namespace tr1 = ::boost;
 }*/
//for debug
#define DEBUGHEADER() (std::cerr <<"[DEBUG]" << __FILE__ << ":" << __LINE__ << " "<< __func__<<" ")
//for turning off unused error_category in boost.

#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif
#ifndef BOOST_THREAD_THROW_IF_PRECONDITION_NOT_SATISFIED
#define BOOST_THREAD_THROW_IF_PRECONDITION_NOT_SATISFIED 1
#endif
#endif /* GLOBAL_H_ */
