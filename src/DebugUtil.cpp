/*
 * DebugUtil.cpp
 * This file defines the utility functions which can be used for debugging purpose
 *
 *   Created on:
 *      Author: renewang
 */

#include "Global.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/lock_guard.hpp>

#ifndef NDEBUG
boost::recursive_mutex _recursive_mutex;
safeostream safecerr(std::cerr, _recursive_mutex);
safeostream safecout(std::cout, _recursive_mutex);
safeostream safeclog(std::clog, _recursive_mutex);
#endif
#ifdef BOOST_ENABLE_ASSERT_HANDLER
void boost::assertion_failed(char const * expr, char const * function,
                             char const * file, long line) {
#if !defined(BOOST_NO_STDC_NAMESPACE)
  using std::printf;
#endif

  printf("Expression: %s\nFunction: %s\nFile: %s\nLine: %ld\n\n", expr,
         function, file, line);
}
//define BOOST_ENABLE_ASSERT_HANDLER
void boost::assertion_failed_msg(char const * expr, char const * msg,
                                 char const * function, char const * file,
                                 long line) {
#if !defined(BOOST_NO_STDC_NAMESPACE)
  using std::printf;
#endif

  printf("Expression: %s Message: %s\nFunction: %s\nFile: %s\nLine: %ld\n\n",
         expr, msg, function, file, line);
}
#endif
#ifndef NDEBUG
void debugmsg(char const* file, unsigned long line, char const * function,
              char const * date, char const * time) {
  DEBUG_OSTREAM<< "[DEBUG] " << file << ":" << line << " " << function
  << " [" << date << " " << time << " " << hexgame::chrono::duration_cast<hexgame::chrono::milliseconds>(hexgame::chrono::system_clock::now().time_since_epoch()).count()%1000<<"] ";
}
#endif
