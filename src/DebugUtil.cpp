#include "Global.h"

using namespace std;

boost::recursive_mutex _recursive_mutex;
safeostream safecerr(std::cerr,_recursive_mutex);
safeostream safecout(std::cout,_recursive_mutex);
safeostream safeclog(std::clog,_recursive_mutex);

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
void debugmsg(char const* file, unsigned long line, char const * function, char const * date, char const * time) {
  DEBUG_OSTREAM << "[DEBUG] " << file << ":" << line << " " << function
             << " [" << date << " " << time << "] ";
}
