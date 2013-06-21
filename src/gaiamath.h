/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 *
 * Gaia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#ifndef GAIA_GAIAMATH_H
#define GAIA_GAIAMATH_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // needed for M_PI in Windows
#endif

#include <algorithm>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "types.h"

namespace gaia2 {

static const float EPSILON = 1e-7f;

template <typename T>
class SortNode {
 public:
  float dist;
  T* ptr;
  SortNode(float d = 0.0, T* p = 0) : dist(d), ptr(p) {}
  bool operator<(const SortNode& x) const { return dist < x.dist; }
};


//////////////////////////////////////////////////////////////////////////////
// ----[ C99 compliance: isnan & isinf functions ]--------------------------//

#ifdef OS_WIN32
#include <float.h>

// isnan returns a nonzero value if the argument x is a NAN; otherwise it returns 0.
template <typename T>
inline bool isnan(T x) { return _isnan(x) != 0; }

// _finite returns a nonzero value if its argument x is not infinite,
// that is, if -INF < x < +INF. It returns 0 if the argument is infinite or a NAN.
template <typename T>
inline bool isinf(T x) { return _finite(x) == 0; }

#else // OS_WIN32

template <typename T>
inline bool isnan(T x) { return std::isnan(x); }

template <typename T>
inline bool isinf(T x) { return std::isinf(x); }

#endif // OS_WIN32



//////////////////////////////////////////////////////////////////////////////
// ----[ Statistics functions ]---------------------------------------------//

/**
 * Computes the mean of an array.
 */
template <typename T>
T mean(const T* array, uint n) {
  T result = 0.0;
  for (uint i=0; i<n; i++) {
    result += array[i];
  }
  result /= n;
  return result;
}

template <typename T> T mean(const std::vector<T>& v) {
  return mean(&v.front(), v.size());
}

/**
 * Computes the variance of an array.
 */
template <typename T>
T variance(const T* array, uint n) {
  T m = mean(array, n);
  T tmp, result = 0.0;
  for (uint i=0; i<n; i++) {
    tmp = array[i] - m;
    result += tmp * tmp;
  }
  result /= (n - 1); // unbiased estimator
  return result;
}

template <typename T> T variance(const std::vector<T>& v) {
  return variance(&v.front(), v.size());
}

/**
 * Computes the skewness of an array. It is mathematically defined as:
 * skewness = mu_3 / mu_2**{3/2}, where mu_i is the i_th central moment.
 */
template <typename T>
T skewness(const T* array, uint n) {
  T m = mean(array, n);
  T v = variance(array, n);
  T tmp, result = 0.0;
  for (uint i=0; i<n; i++) {
    tmp = array[i] - m;
    result += tmp * tmp * tmp;
  }
  result /= pow(v, (T)1.5);
  return result;
}

template <typename T> T skewness(const std::vector<T>& v) {
  return skewness(&v.front(), v.size());
}

/**
 * Computes the kurtosis of an array. It is mathematically defined as:
 * kurtosis = mu_4 / mu_2**{2}, where mu_i is the i_th central moment.
 */
template <typename T>
T kurtosis(const T* array, uint n) {
  T m = mean(array, n);
  T v = variance(array, n);
  T tmp, result = 0.0;
  for (uint i=0; i<n; i++) {
    tmp = array[i] - m;
    result += tmp * tmp * tmp * tmp;
  }
  result /= (v * v);
  return result;
}

template <typename T> T kurtosis(const std::vector<T>& v) {
  return kurtosis(&v.front(), v.size());
}

template <typename RandomAccessIterator>
void sort(RandomAccessIterator first, RandomAccessIterator last) {
  return std::sort(first, last);
}

template <typename RandomAccessIterator, typename StrictWeakOrdering>
void sort(RandomAccessIterator first, RandomAccessIterator last,
          StrictWeakOrdering comp) {
  return std::sort(first, last, comp);
}

template <typename Container>
void sort(Container& container) {
  return sort(container.begin(), container.end());
}


/**
 * Given a set of values, computes the associated histogram. This method is
 * designed to work the same way as in MatLab/Octave. It is based on the
 * algorithms used in Octave rather than Matlab. The result structures
 * (n_arr, x_arr) have to be allocated before calling this function.
 * @param array the input array, containing the data values
 * @param n the number of elements of this array
 * @param n_array the array where the distribution will be written
 * @param x_array the array that will contain the centers of each bin
 * @param n_bins the number of desired bins for the distribution
 */
template <typename T>
void hist(const T* array, uint n, int* n_array, T* x_array, uint n_bins) {
  T miny = *min_element(array, array+n);
  T maxy = *max_element(array, array+n);

  // x contains the center of the bins
  for (uint i=0; i<n_bins; i++) {
    x_array[i] = (0.5 + i)*(maxy - miny)/n_bins + miny;
  }

  // cutoff contains the boundaries between the bins
  std::vector<T> cutoff(n_bins - 1);
  for (uint i=0; i<n_bins-1; i++) {
    cutoff[i] = (x_array[i] + x_array[i+1]) / 2.0;
  }

  // algo: either build the cumulated histogram by 2-level
  //       for-loops, and then build the diff, or first
  //       sort the distribution and do it directly.
  // 1st version: O(n^2) time, O(1) space
  // 2nd version: O(n log(n)) time, O(n) space

  // implementation of 2nd version
  std::vector<T> dist(array, array+n);
  sort(dist.begin(), dist.end());
  uint current_cutoff_idx = 0;
  T current_cutoff = cutoff[0];
  for (uint i=0; i<n_bins; i++) n_array[i] = 0;

  for (uint i=0; i<n; i++) {
    while (dist[i] > current_cutoff) {
      // last case; skip the rest and fill in the last bin
      if (current_cutoff_idx == n_bins-2) {
	n_array[n_bins-1] = n-i; // fill in the last bin with what's left
	i = n; // to jump out of the 2nd loop (the 'for' one)
	n_array[n_bins-2]--; // to compensate for the last one that will be added before jumping out of the loop
	break;
      }
      current_cutoff_idx++;
      current_cutoff = cutoff[current_cutoff_idx];
    }
    n_array[current_cutoff_idx]++;
  }
}

template <typename T>
T sgn(T x) { return x==0.0 ? 0.0 : (x<0.0 ? -1.0 : 1.0); }

/**
 * Computes the integral power n of x.
 */
template <typename T>
T intpow(T x, int n) {
  if (n < 0) throw GaiaException("intpow: power needs to be >= 1");
  if (n == 0) return 1;

  T xn2 = intpow(x, n/2);

  if (n % 2 == 0)
    return xn2*xn2;
  else
    return x * xn2*xn2;
}

/**
 * Computes the inverse error function (http://en.wikipedia.org/wiki/Error_function).
 */
double erfinv(double P);

/**
 * Computes the inverse of the normal cdf with parameter mu and sigma.
 */
double norminv(double P, double mu = 0, double sigma = 1);

/**
 * Computes the inverse of the chi-square cdf with v degrees of freedom.
 */
double chi2inv(double P, int v);

/**
 * Iterative function for binary search (more efficient than recursive one).
 * If <code>n</code> is the size of the vector, the returned value will be
 * between 0 and n (inclusive) and correspond to the interval between numbers.
 * Ex: r = 0 means it is lower than the lowest value in v, r = n means it is
 *     greater than the greatest value in v.
 */
template <template<typename> class Container, typename T>
int binarySearch(const Container<T>& v, T value) {
  int start = 0, end = v.size()-1;

  if (value < v[0]) return 0;
  if (value > v[end]) return end + 1;

  while ((end - start) > 1) {
    uint pivotIdx = (start + end) / 2;
    T pivot = v[pivotIdx];
    if (value < pivot) {
      end = pivotIdx;
    }
    else {
      start = pivotIdx;
    }
  }

  return end;
}


// forward calls to sqrt from gaia2 to std, so we don't shadow sqrt with the
// specialization for the RealDescriptor
template <typename T> T sqrt(const T& x) {
  return std::sqrt(x);
}

/**
 * Returns the given value clipped inside the specified interval.
 */
inline Real clip(Real value, Real minv, Real maxv) {
  return qMax(qMin(value, maxv), minv);
}


} // namespace gaia2

#endif // GAIA_GAIAMATH_H
