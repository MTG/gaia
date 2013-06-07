#include "timer.h"
using namespace gaia2;

#ifndef OS_WIN32

Timer::Timer() {
  _begin.tv_sec = _begin.tv_usec = 0;
  _end.tv_sec = _end.tv_usec = 0;
  _total = 0.0;
  _isRunning = false;
}

void Timer::start() {
  _isRunning = true;
  gettimeofday(&_begin, 0);
}

void Timer::reset() {
  _total = 0.0;
  _isRunning = false;
}

void Timer::stop() {
  if (!_isRunning) return;

  gettimeofday(&_end, 0);
  _isRunning = false;
  _total += diff(_end, _begin);

}

double Timer::elapsed() const {
  // can only measure time when timer is stopped
  if (!_isRunning) return _total;

  // if timer was running, measure time online
  struct timeval end;
  gettimeofday(&end, 0);

  return _total + diff(end, _begin);
}

#endif

