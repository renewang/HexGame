/*
 * global.h
 *
 *  Created on: Jan 13, 2014
 *      Author: renewang
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

//#include <boost/config.hpp>
#include <ctime>
#include <sstream>
#include <iostream>

//using boost::shared_ptr for prior C++11 compiler
#if __cplusplus > 199711L

#include <tuple>
#include <memory>
#include <chrono>
#include <random>
#include <atomic>
#include <unordered_set>
#include <unordered_map>

namespace hexgame = std;

#else //prior C++11, using boost
#undef GTEST_LANG_CXX11
#ifdef  BOOST_HAS_TR1_TUPLE
#define GTEST_HAS_TR1_TUPLE 1
#endif
#include <boost/atomic.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/tr1/memory.hpp>
#include <boost/tr1/random.hpp>
#include <boost/chrono/include.hpp>
#include <boost/tr1/unordered_set.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/thread/detail/memory.hpp>

namespace hexgame
{
  namespace tr1 = std::tr1;
  namespace chrono {
    using namespace boost::chrono;
  }
    using boost::atomic;
    using boost::default_delete;
    using boost::random::uniform_real_distribution;
    using boost::memory_order_release;
    using boost::memory_order_consume;
    using boost::memory_order_acquire;
    using boost::memory_order_relaxed;
    using namespace tr1;
}
#define nullptr 0
#endif

//for turning off warnings in boost.
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif

#ifndef BOOST_THREAD_THROW_IF_PRECONDITION_NOT_SATISFIED
#define BOOST_THREAD_THROW_IF_PRECONDITION_NOT_SATISFIED 1
#endif

#define DEBUGHEADER() (std::clog<< "[DEBUG] "<< __FILE__ << ":" << __LINE__ <<" "<< __func__<<" [" << __DATE__<<" "<<__TIME__ <<"] ")

/*inline const char* DEBUGMSG() {
 std::stringstream buffer;
 hexgame::chrono::system_clock::time_point timestamp =
 hexgame::chrono::system_clock::now();
 std::time_t tt = hexgame::chrono::system_clock::to_time_t(timestamp);
 std::string format(ctime(&tt));
 buffer << "[";
 buffer << format.substr(0, (format.size() - 1));
 buffer << "] ";
 return buffer.str().c_str();
 }*/

/*extern boost::recursive_mutex _recursive_mutex;
 extern boost::externally_locked_stream<std::ostream> safecerr(std::cerr, _recursive_mutex);
 extern boost::externally_locked_stream<std::ostream> safecout(std::cout, _recursive_mutex);*/

#endif /* GLOBAL_H_ */
