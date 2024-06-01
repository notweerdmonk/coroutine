/**
 * @file coroutine.h
 * @author notweerdmonk
 * @brief Implementation of coroutines using Duff's device.
 * @see https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
 */

#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <stdint.h>

#undef thread_local
#ifdef __GNUC__
#define thread_local __thread
#else
#define thread_local _Thread_local
#endif

/**
 * @brief Begin a coroutine inside a function.
 *
 * There can be only one coroutine inside a function.
 */
#define COROUTINE_BEGIN() \
  static thread_local int16_t cr_state = 0; switch(cr_state) { case 0:

/**
 * @brief Return from the function using a coroutine. 
 *
 * Execution resumes from this point when the function gets called again.
 *
 * @return Address of the coroutine state variable.
 */
#define COROUTINE_YIELD() \
  do { \
    cr_state = __LINE__; return &cr_state; case __LINE__:; \
  } while (0)

/**
 * @brief Call another function containing a coroutine and return.
 *
 * The callee function gets called every time the caller function
 * is called untill COROUTINE_STOP() is used inside the callee. In that case
 * execution continues normally from this point in the caller function.
 *
 * @return Address of the coroutine state variable.
 */
#define COROUTINE_DELEGATE(func, ...) \
  do { \
    case __LINE__:; cr_state = *(uint16_t*)func(__VA_ARGS__); \
    if (cr_state > 0) { cr_state = __LINE__; return &cr_state; } \
  } while (0)

/**
 * @brief Return from a function without altering the state of its coroutine.
 * 
 * Execution will continue from the point of previous COROUTINE_YIELD() or
 * COROUTINE_BEGIN() when the function is called again.
 *
 * @return Address of the coroutine state variable.
 */
#define COROUTINE_RETURN() \
  return &cr_state

/**
 * @brief Reset a coroutine.
 *
 * Execution will start from COROUTINE_BEGIN() when the function is called
 * again.
 *
 * @return Address of the coroutine state variable.
 */
#define COROUTINE_RESET() \
  do { \
    cr_state = 0; return &cr_state; \
  } while (0)

/**
 * @brief Reset a coroutine and return NULL to mark occurence of an error.
 *
 * Execution will start from COROUTINE_BEGIN() when the function is called
 * again.
 *
 * @return NULL
 */
#define COROUTINE_ERROR() \
  do { \
    cr_state = 0; return NULL; \
  } while (0)

/**
 * @brief Stop a coroutine.
 *
 * Execution will start from COROUTINE_BEGIN() when the function is called
 * again.  This macro should be used to exit a delegated coroutine.
 *
 * @return Address of the coroutine state variable.
 *
 * @see To return from a coroutine see COROUTINE_RETURN().
 * @see To reset a coroutine see COROUTINE_RESET().
 */
#define COROUTINE_STOP() \
  } COROUTINE_RESET()

/**
 * @brief End a coroutine.
 *
 * Execution will start from COROUTINE_BEGIN() when the function is called
 * again.
 *
 * @return Address of the coroutine state variable.
 */
#define COROUTINE_END() \
  } return &cr_state

#endif /* _COROUTINE_H_ */
