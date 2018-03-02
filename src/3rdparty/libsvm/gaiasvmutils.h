#ifndef GAIASVMUTILS_H
#define GAIASVMUTILS_H

#include "../../dataset.h"
#include "svm.h"

namespace gaia2 {
namespace svm {


QStringList createClassMapping(const DataSet* dataset,
                               const QString& className);


void writeTrainingSetToFile(const QString& filename, const DataSet* dataset,
                            const QString& className, const Region& region);


struct svm_problem
dataSetToLibsvmProblem(const DataSet* dataset, const QString& className, const Region& region,
                       const QStringList& classList);

/**
 * Frees the memory that's been allocated for the problem by the
 * dataSetToLibsvmProblem function.
 */
void destroyProblem(const struct svm_problem& prob);


} // namespace svm
} // namespace gaia2

#endif // GAIASVMUTILS_H
