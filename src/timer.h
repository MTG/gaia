#ifndef GAIA_TIMER_H
#define GAIA_TIMER_H

#include "osdetect.h"

#ifndef OS_WIN32
#include <sys/time.h>

namespace gaia2 {

/**
 * This class allows you to measure a certain amount of time, for instance
 * if you want to know how long the execution of a given function takes.
 * Its use is very easy: you create the object, you start it, you stop it, and
 * then you ask it how much time there was between the start and stop calls.
 * You can start it and stop it multiple times; in this case, the result
 * will be the sum of each lapse of time.
 */
class Timer {
  double _total;
  struct timeval _begin, _end;
  bool _isRunning;

  static double diff(const struct timeval& end, const struct timeval& begin) {
    return (end.tv_sec - begin.tv_sec) + 1e-6 * (end.tv_usec - begin.tv_usec);
  }

 public:
  /**
   * The constructor of the class.
   */
  Timer();

  /**
   * Starts the timer. Total is not reset to 0.
   */
  void start();

  /**
   * Restarts the timer. Has same effect as <code>reset()</code> followed by
   * <code>start()</code> Total is reset to 0.
   */
  void restart() {
    reset();
    start();
  }

  /**
   * Stops the timer.
   */
  void stop();

  /**
   * Reset the total to 0.
   */
  void reset();

  /**
   * Returns the cumulative number of seconds elapsed between each call
   * to the <code>start()/stop()</code> pair.
   * If you didn't call one of these two functions at least once or if
   * the timer is still running, this functions returns 0.
   * @return the number of elapsed seconds between <code>start()</code>
   *         and <code>stop()</code>, 0 if an error has occured.
   */
  double elapsed() const;

};

} // namespace gaia2

#else // OS_WIN32

namespace gaia2 {

/**
 * This class allows you to measure a certain amount of time, for instance
 * if you want to know how long the execution of a given function takes.
 * Its use is very easy: you create the object, you start it, you stop it, and
 * then you ask it how much time there was between the start and stop calls.
 * You can start it and stop it multiple times; in this case, the result
 * will be the sum of each lapse of time.
 */
class Timer {

 public:
  /**
   * The constructor of the class.
   */
  Timer() {}

  /**
   * Starts the timer. Total is not reset to 0.
   */
  void start() {}

  /**
   * Restarts the timer. Has same effect as <code>reset()</code> followed by
   * <code>start()</code> Total is reset to 0.
   */
  void restart() {
    reset();
    start();
  }

  /**
   * Stops the timer.
   */
  void stop() {}

  /**
   * Reset the total to 0.
   */
  void reset() {}

  /**
   * Returns the cumulative number of seconds elapsed between each call
   * to the <code>start()/stop()</code> pair.
   * If you didn't call one of these two functions at least once or if
   * the timer is still running, this functions returns 0.
   * @return the number of elapsed seconds between <code>start()</code>
   *         and <code>stop()</code>, 0 if an error has occured.
   */
  double elapsed() const { return 0; }

};

} // namespace gaia2

#endif // OS_WIN32

#endif // GAIA_TIMER_H
