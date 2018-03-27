/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/

#include <QSet>
#include <QFile>
#include <QTextStream>
#include "gaiasvmutils.h"
#include "../../utils.h"

namespace gaia2 {
namespace svm {


QStringList createClassMapping(const DataSet* dataset,
                               const QString& className) {

  G_DEBUG(GAlgorithms, "Creating mapping from class instances to integer");
  int classIndex = dataset->layout().descriptorLocation(className).index();
  DescriptorLengthType ltype = dataset->layout().descriptorLocation(className).lengthType();

  QSet<QString> allClasses;

  int i = 0;
  try {
    for (; i<dataset->size(); i++) {
      const Point* p = dataset->at(i);
      FORSEG(*p) {
        switch (ltype) {
        case VariableLength:  allClasses.insert(p->vstringData(nseg).at(classIndex).toSingleValue()); break;
        case FixedLength:     allClasses.insert(p->fstringData(nseg).at(classIndex)); break;
        default:
          Q_ASSERT(false);
        }
      }
    }
  }
  catch (GaiaException&) {
    QStringList msg;
    msg << "SVM::createClassMapping: can only use single class, however point with ID '"
        << dataset->at(i)->name() << "' has the following labels for property '"
        << className << "': " << dataset->at(i)->label(className).toString();
    throw GaiaException(msg);
  }

  QStringList result = allClasses.toList();
  qSort(result.begin(), result.end());

  G_INFO("Found the following classes:");
  foreach (QString cname, result) G_INFO(" - " << cname);

  return result;
}

// Note: only works with fixed-length descriptors (throws an exception otherwise)
void writeTrainingSetToFile(const QString& filename, const DataSet* dataset,
                            const QString& className, const Region& region) {

  G_DEBUG(GAlgorithms, "Writing dataset to libsvm file format");
  region.checkLengthTypeOnlyFrom(FixedLength, &dataset->layout());

  QVector<int> listIndices = region.listIndices(RealType, VariableLength);

  Region classRegion = dataset->layout().descriptorLocation(className);
  int classIndex = classRegion.index();
  DescriptorLengthType classLType = classRegion.lengthType();
  QList<QString> classList = createClassMapping(dataset, className);

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QTextStream out(&file);

  for (int n=0; n<dataset->size(); n++) {
    const Point* p = dataset->at(n);
    FORSEG(*p) {
      QString clas;
      if (classLType == FixedLength) {
        clas = p->fstringData(nseg).at(classIndex);
      }
      else {
        clas = p->vstringData(nseg).at(classIndex).toSingleValue();
      }
      out << classList.indexOf(clas);
      int dim = 1; // dimension in svmlite files start at 1

      foreach (const Segment& seg, region.segments) {
        for (int i=seg.begin; i<seg.end; i++) {
          out << QString(" %1:%2").arg(dim).arg(p->frealData(nseg).at(i));

          dim++; // next dimension in output space
        }
      }

      out << "\n";
    }
  }
}


struct svm_problem
dataSetToLibsvmProblem(const DataSet* dataset,
                       const QString& className, const Region& region,
                       const QStringList& classList) {
  struct svm_problem prob;

  region.checkLengthTypeOnlyFrom(FixedLength, &dataset->layout());

  int classIndex = dataset->layout().descriptorLocation(className).index();
  DescriptorLengthType ltype = dataset->layout().descriptorLocation(className).lengthType();

  int nsegments = dataset->totalSegments();
  int dimension = region.dimension();

  // allocate memory
  // libsvm uses the svm_problem struct, which contains the following variables:
  //  - l, which is the total number of points
  //  - y, which is the array of ground-truth, ie: the class of each point
  //  - x, which is the array of data, each containing the data contained in svm_nodes,
  //       +1 dimension which index should be set to -1 (sentinel)
  typedef struct svm_node* svm_node_ptr;
  prob.l = nsegments;
  prob.y = new double[nsegments];
  prob.x = new svm_node_ptr[nsegments];

  // initialize memory, ground truth and sentinels
  struct svm_node* x_space = new struct svm_node[nsegments * (dimension + 1)];

  int n = 0;
  for (int k=0; k<dataset->size(); k++) {
    const Point* p = dataset->at(k);

    FORSEG(*p) {
      prob.x[n] = &x_space[n * (dimension+1)];
      prob.x[n][dimension].index = -1;
      // transfer class value
      if (ltype == VariableLength)   prob.y[n] = classList.indexOf(p->vstringData(nseg).at(classIndex).toSingleValue());
      else if (ltype == FixedLength) prob.y[n] = classList.indexOf(p->fstringData(nseg).at(classIndex));
      else throw GaiaException("SVM: internal error at SVM::dataSetToLibsvmProblem");
      n++;
    }
  }
  Q_ASSERT(n == nsegments);


  // copy data from gaia points into libsvm problem structure
  int dim = 0;

  // transfer fixed-length variables
  for (int s=0; s<region.segments.size(); s++) {
    const Segment& seg = region.segments[s];
    for (int i=seg.begin; i<seg.end; i++) {

      int n = 0;
      for (int k=0; k<dataset->size(); k++) {
        FORSEG(*dataset->at(k)) {
          prob.x[n][dim].index = dim + 1;  // dimension in svmlite files start at 1
          if (seg.type == RealType) prob.x[n][dim].value = dataset->at(k)->frealData(nseg).at(i);
          else                      prob.x[n][dim].value = dataset->at(k)->fenumData(nseg).at(i);
          n++;
        }
      }
      Q_ASSERT(n == nsegments);

      dim++; // next dimension in output space
    }
  }

  Q_ASSERT(dim == dimension);
  // make sure of that as well because it's what we use to get the dimension
  // back from the libsvm problem
  Q_ASSERT(prob.x[1] - prob.x[0] == dimension+1);

  return prob;
}


} // namespace svm
} // namespace gaia2
