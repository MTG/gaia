#include <QFile>
#include "transformation.h"
#include "algorithms/analyzer.h"
#include "algorithms/applier.h"
#include "utils.h"
using namespace std;

namespace gaia2 {

Transformation& Transformation::operator=(const Transformation& rhs) {
  this->name = rhs.name;
  this->analyzerName = rhs.analyzerName;
  this->applierName = rhs.applierName;
  this->analyzerParams = rhs.analyzerParams;
  this->params = rhs.params;
  this->info = rhs.info;
  this->layout = rhs.layout;

  // do not copy the pointer to analyzer and/or applier to avoid double delete
  this->_analyzer = 0;
  this->_applier = 0;

  return *this;
}

bool Transformation::operator==(const Transformation& rhs) const {
  return this->analyzerName == rhs.analyzerName
    && this->applierName == rhs.applierName
    && this->analyzerParams == rhs.analyzerParams
    && this->params == rhs.params
    && this->layout == rhs.layout;
}


Transformation::Transformation(const Transformation& rhs) {
  operator=(rhs);
}

Transformation::~Transformation() {
  delete _analyzer;
  delete _applier;
}

const Analyzer* Transformation::analyzer() const {
  if (!_analyzer) {
    _analyzer = AnalyzerFactory::create(analyzerName, analyzerParams);
  }
  return _analyzer;
}

const Applier* Transformation::applier() const {
  if (!_applier) {
    _applier = ApplierFactory::create(applierName, *this);
  }
  return _applier;
}

DataSet* Transformation::retransformDataSet(const DataSet* dataset) const {
  return analyzer()->analyze(dataset).applyToDataSet(dataset);
}

DataSet* Transformation::applyToDataSet(const DataSet* dataset) const {
  return applier()->mapDataSet(dataset);
}

Point* Transformation::applyToPoint(const Point* point) const {
  return applier()->mapPoint(point);
}


void Transformation::load(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream in(&file);
  in >> *this;
}

void Transformation::save(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream out(&file);
  out << *this;
}


void Transformation::updateOldTransformation(int version) {
  // in 2.1, we added clipping possibility to dotproduct
  if (version == Gaia_2_0 && applierName == "dotproduct") {
    ParameterMap coeffs = params;
    ParameterMap newParams;
    newParams.insert("applyClipping", false);
    newParams.insert("coeffs", coeffs);
    params = newParams;
  }

  // in 2.3, we changed the name of svm::class to svm::className
  if ((version == Gaia_2_0 || version == Gaia_2_1) && applierName == "svmpredict") {
    params.insert("className", params.value("class"));
    params.remove("class");
  }

}


QDataStream& operator<<(QDataStream& out, const Transformation& transfo) {
  out << transfo.name << transfo.analyzerName << transfo.applierName
      << Parameter(transfo.analyzerParams) << Parameter(transfo.params)
      << Parameter(transfo.info) << Parameter(transfo.layout);
  return out;
}

QDataStream& operator>>(QDataStream& in, Transformation& transfo) {
  in >> transfo.name >> transfo.analyzerName >> transfo.applierName;
  Parameter analyzerParam, param, info, layout;

  switch (gaiaVersion(in)) {

  case Gaia_2_3:
    G_DEBUG(GIO, "Loading transformation, gaia 2.3 format");
    in >> analyzerParam >> param >> info >> layout;
    break;

  case Gaia_2_1:
    G_DEBUG(GIO, "Loading transformation, gaia 2.1 format");
    in >> analyzerParam >> param >> info;
    break;

  case Gaia_2_0:
    G_DEBUG(GIO, "Loading transformation, gaia 2.0 format");
    in >> analyzerParam >> param;
    break;

  default:
    throw GaiaException("Unsupported data format when loading transformation");
  }

  transfo.analyzerParams = analyzerParam.toParameterMap();
  transfo.params = param.toParameterMap();
  transfo.info = info.toParameterMap();
  transfo.layout = layout.toPointLayout();

  return in;
}


Point* TransfoChain::mapPoint(const Point* point, bool takeOwnership) const {
  /** @todo check if the layout is the same */

  if (this->isEmpty()) {
    return takeOwnership ? const_cast<Point*>(point) : new Point(*point);
  }
  // we have at least 1 transfo in our chain

  // optimization: in the case we do not take ownership, we still don't have to
  // perform the first copy if we also take care of not deleting it
  Point* result = this->at(0).applyToPoint(point);
  if (takeOwnership) delete point;

  for (int i=1; i<this->size(); i++) {
    Point* oldPoint = result;
    result = this->at(i).applyToPoint(result);
    delete oldPoint;
  }

  return result;
}

DataSet* TransfoChain::mapDataSet(const DataSet* dataset, bool takeOwnership) const {
  /** @todo check if the layout is the same */

  if (this->isEmpty()) {
    return takeOwnership ? const_cast<DataSet*>(dataset) : dataset->copy();
  }
  // we have at least 1 transfo in our chain

  // optimization: in the case we do not take ownership, we don't have to
  // perform the first copy if we also take care of not deleting it
  DataSet* result = this->at(0).applyToDataSet(dataset);
  if (takeOwnership) delete dataset;

  for (int i=1; i<this->size(); i++) {
    DataSet* oldDataset = result;
    result = this->at(i).applyToDataSet(result);
    delete oldDataset;
  }

  return result;
}

DataSet* TransfoChain::partialMapDataSet(const DataSet* dataset, int start, int end) const {
  if (end == 0) end = this->size();
  if (start < 0 || start > this->size() ||
      end < 0 || end > this->size() || end <= start) {
    QStringList msg;
    msg << "Invalid start/end indices: start = " << QString::number(start)
        << " / end = " << QString::number(end)
        << " / size = " << QString::number(this->size());
    throw GaiaException(msg);
  }

  DataSet* result = this->at(start).applyToDataSet(dataset);

  for (int i=start+1; i<end; i++) {
    DataSet* oldDataset = result;
    result = this->at(i).applyToDataSet(result);
    delete oldDataset;
  }

  return result;
}

DataSet* TransfoChain::retransformDataSet(const DataSet* dataset, int start, int end) const {
  if (end == 0) end = this->size();
  if (start < 0 || start > this->size() ||
      end < 0 || end > this->size() || end <= start) {
    QStringList msg;
    msg << "Invalid start/end indices: start = " << QString::number(start)
        << " / end = " << QString::number(end)
        << " / size = " << QString::number(this->size());
    throw GaiaException(msg);
  }

  DataSet* result = this->at(start).retransformDataSet(dataset);

  for (int i=start+1; i<end; i++) {
    DataSet* oldDataset = result;
    result = this->at(i).retransformDataSet(result);
    delete oldDataset;
  }

  return result;
}



bool TransfoChain::operator==(const TransfoChain& rhs) const {
  if (this->size() != rhs.size()) return false;

  for (int i=0; i<this->size(); i++) {
    if (!(this->at(i) == rhs.at(i))) return false;
  }

  return true;
}


#define MAGIC_NUMBER 0x6AEA723D

void TransfoChain::load(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream in(&file);

  // read magic number & file format version here
  quint32 magic;
  in >> magic;

  if (magic != MAGIC_NUMBER) {
    throw GaiaException("File does not seem to be a Gaia TransfoChain file.");
  }

  checkValidDataStream(in);

  in >> *this;
}


void TransfoChain::save(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream out(&file);

  // write magic number & file format version here
  out << (quint32)MAGIC_NUMBER;
  setDataStreamVersion(out);

  out << *this;
}

bool TransfoChain::contains(const QString& analyzerName) const {
  for (int i=0; i<size(); i++) {
    if (at(i).analyzerName == analyzerName) return true;
  }
  return false;
}


QDataStream& operator<<(QDataStream& out, const TransfoChain& transfo) {
  out << (QList<Transformation>&)transfo;
  return out;
}

QDataStream& operator>>(QDataStream& in, TransfoChain& transfo) {
  in >> (QList<Transformation>&)transfo;

  if (gaiaVersion(in) < Gaia_2_3) {
    // starting from Gaia 2.3, the original layout is stored inside the first
    // transformation of the history.
    PointLayout dummy;
    in >> dummy;
  }

  // update transformations loaded from an older version that might need
  // parameters in a different form
  for (int i=0; i<transfo.size(); i++) {
    transfo[i].updateOldTransformation(gaiaVersion(in));
  }

  // we can only make valid those histories created with Gaia >= 2.3
  if (gaiaVersion(in) < Gaia_2_3 && !transfo.isEmpty()) {
    throw GaiaException("You cannot load a History/DataSet with non-empty history which was created with Gaia < 2.3");
  }

  return in;
}


DataSet* transform(DataSet* ds, const QString& name, const ParameterMap& params) {
  Analyzer* transfo = AnalyzerFactory::create(name, params);
  DataSet* result = transfo->analyze(ds).applyToDataSet(ds);
  delete transfo;
  return result;
}


DataSet* applyTransfoChain(DataSet* ds, const QString& yamlTransfoChain) {
  QByteArray tchain = yamlTransfoChain.toUtf8();
  DataSet* result, *old;

  try {
    yaml::Sequence chain = yaml::load(tchain.data(), tchain.size()).sequence();

    if (chain.isEmpty()) return ds->copy();

    result = transform(ds,
                       chain[0].mapping().value("transfo").scalar(),
                       toParameterMap(chain[0].mapping().value("params", yaml::Mapping())));
    old = result;

    for (int i=1; i<chain.size(); i++) {
      result = transform(old,
                         chain[i].mapping().value("transfo").scalar(),
                         toParameterMap(chain[i].mapping().value("params", yaml::Mapping())));
      delete old;
      old = result;
    }
  }
  catch (GaiaException& e) {
    throw GaiaException("Given yaml string for transformations is not valid:\n", e.what());
  }

  return result;
}

} // namespace gaia2
