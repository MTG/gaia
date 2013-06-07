#ifndef GAIA_SVMPREDICT_H
#define GAIA_SVMPREDICT_H

#include <QByteArray>
#include "applier.h"
#include "3rdparty/libsvm/svm.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * SVMPredict applier class. Predicts the class of a point using a given SVM model.
 */
class SVMPredict : public Applier {

 public:
  SVMPredict(const Transformation& transfo);
  virtual ~SVMPredict();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

  virtual DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  Point* svmPredict(const Point* p, Point* result,
                    const struct svm_model* model, const Region& region,
                    DescriptorLengthType classLType, int classIdx,
                    DescriptorLengthType classProbLType, int classProbIdx) const;

  struct svm_model* loadModel() const;

  QStringList _classMapping;
  QString _className;
  QStringList _descriptorNames;
  QByteArray _modelData;
  bool _probability;
  struct svm_model* _model;
  int* _cmap; // reverse class map, used for getting correct class number from libsvm probabilities
};

} // namespace gaia2

#endif // GAIA_SVMPREDICT_H
