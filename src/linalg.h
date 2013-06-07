#ifndef GAIA_LINALG_H
#define GAIA_LINALG_H

#include "dataset.h"
#include "Eigen/Eigen"


namespace gaia2 {

/**
 * Computes the covariance matrix given a set of points.
 * @param v the set of points for which to compute the covariance matrix.
 * @param region the Region containing the descriptors to be included in the
 *        covariance matrix computation.
 */
Eigen::MatrixXd covarianceMatrix(const PointArray& v,
                                 const Region& region);

Eigen::MatrixXf covarianceMatrixSinglePrecision(const PointArray& v,
                                                const Region& region);



/**
 * Sort the eigen vectors and their corresponding values according to the given
 * compare function.
 */
template <typename T, typename Compare>
void sortEigenVectors(Eigen::Matrix<T, Eigen::Dynamic, 1>& values,
                      Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& vectors,
                      Compare cmp) {
  const int n = values.size();

  // bubble sort, stable sort implementation, should be enough for our purposes
  for (int i=0; i<n-1; i++) {
    for (int j=n-1; j>i; j--) {

      if (cmp(std::abs(values[j]), std::abs(values[j-1]))) {

	// swap values in eigenvalues vector
        std::swap(values[j], values[j-1]);

	// swap columns in eigenvectors matrix
        vectors.col(j).swap(vectors.col(j-1));
      }
    }
  }
}

/**
 * Sort the eigen vectors and their corresponding values in ascending order.
 */
template <typename T>
void sortEigenVectors(Eigen::Matrix<T, Eigen::Dynamic, 1>& values,
                      Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& vectors) {
  return sortEigenVectors(values, vectors, std::less<T>());
}


} // namespace gaia2

#endif // GAIA_LINALG_H
