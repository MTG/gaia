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
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include "svmpredict.h"
#include "utils.h"
#include <QTemporaryFile>
using namespace gaia2;


SVMPredict::SVMPredict(const Transformation& transfo) : Applier(transfo) {
  _classMapping = transfo.params.value("classMapping").toStringList();
  _className = transfo.params.value("className").toString();
  _descriptorNames = transfo.params.value("descriptorNames").toStringList();
  _modelData = transfo.params.value("modelData").toByteArray();
  _probability = transfo.params.value("probability").toBool();

  _model = loadModel();

  // create reverse labels map, needed for correct probability estimates output
  int nclasses = svm_get_nr_class(_model);
  _cmap = new int[nclasses];
  int* labels = new int[nclasses];
  svm_get_labels(_model, labels);
  for (int i=0; i<nclasses; i++) _cmap[labels[i]] = i;
  delete[] labels;
}


SVMPredict::~SVMPredict() {
  svm_destroy_model(_model);
  delete[] _cmap;
}


Point* SVMPredict::svmPredict(const Point* p, Point* result,
                              const struct svm_model* model, const Region& region,
                              DescriptorLengthType classLType, int classIdx,
                              DescriptorLengthType classProbLType, int classProbIdx) const {
  int dimension = region.dimension();

  svm_node* x = new svm_node[dimension+1];
  int dim = 0;

  Q_ASSERT(p->numberSegments() == result->numberSegments());

  FORSEG(*p) {
    // transfer point data to a struct suitable for libsvm
    for (int s=0; s<region.segments.size(); s++) {
      const Segment& seg = region.segments[s];
      for (int i=seg.begin; i<seg.end; i++) {
        x[dim].index = dim + 1;
        if (seg.type == RealType) x[dim].value = p->frealData(nseg).at(i);
        else                      x[dim].value = p->fenumData(nseg).at(i);
        dim++;
      }
    }

    x[dim].index = -1;

    Q_ASSERT(dim == dimension);

    // predict class (and probabilities)
    double predictedClass;
    if (_probability) {
      QVarLengthArray<double, 32> classProbability(_classMapping.size());
      predictedClass = svm_predict_probability(model, x, classProbability.data());

      Real* probs = 0;
      const int psize = classProbability.size();
      switch (classProbLType) {

      case FixedLength:
        probs = &(result->frealData(nseg)[classProbIdx]);
        break;

      case VariableLength: {
        RealDescriptor& probsd = result->vrealData(nseg)[classProbIdx];
        probsd.resize(psize);
        probs = &(probsd[0]);
        break;
      }

      default: Q_ASSERT(false);
      }

      for (int i=0; i<psize; i++) probs[i] = classProbability[_cmap[i]];
    }
    else {
      predictedClass = svm_predict(model, x);
    }

    // write back data to point
    switch (classLType) {
    case FixedLength:    result->fstringData(nseg)[classIdx] = _classMapping[(int)predictedClass]; break;
    case VariableLength: result->vstringData(nseg)[classIdx] = _classMapping[(int)predictedClass]; break;
    default: Q_ASSERT(false);
    }

  }

  delete[] x;

  return result;
}

Point* SVMPredict::mapPoint(const Point* p) const {
  Region region = p->layout().descriptorLocation(_descriptorNames);

  int classIdx = -1, classProbIdx = -1;
  DescriptorLengthType classLType, classProbLType = FixedLength; // default value so gcc doesn't complain
  Point* result = 0;

  if (_probability) {
    // if we also want the class probabilities, we need to create a new layout
    // with a new field for it
    PointLayout addedLayout;
    addedLayout.add(_className + "Probability", RealType);
    PointLayout newLayout = mergeLayouts(p->layout(), addedLayout);
    IndexMap realMap, stringMap, enumMap;
    Region r = newLayout.descriptorLocation(p->layout().descriptorNames());
    createIndexMappings(p->layout(), newLayout, r, realMap, stringMap, enumMap);

    Region classRegion = newLayout.descriptorLocation(_className);
    classLType = classRegion.lengthType();
    classIdx = classRegion.index();
    Region classProbRegion = newLayout.descriptorLocation(_className+"Probability");
    classProbLType = classProbRegion.lengthType();
    classProbIdx = classProbRegion.index();

    //------------

    result = gaia2::mapPoint(p, newLayout, realMap, stringMap, enumMap);
  }
  else { // !probability
    // if we don't want probabilities estimates, we can just use the same
    // layout as we had before
    Region classRegion = p->layout().descriptorLocation(_className);
    classLType = classRegion.lengthType();
    classIdx = classRegion.index();
    result = new Point(*p);
  }

  svmPredict(p, result, _model, region, classLType, classIdx, classProbLType, classProbIdx);

  return result;
}

struct svm_model* SVMPredict::loadModel() const {
  QTemporaryFile modelFile;
  modelFile.open();
  QString filename = modelFile.fileName(); // need to keep the filename while it's open
  modelFile.write(_modelData.constData());
  modelFile.close();

  struct svm_model* model = svm_load_model(filename.toUtf8().constData());
  if (!model) {
    throw GaiaException("SVMPredict: could not load model");
  }

  return model;
}

DataSet* SVMPredict::mapDataSet(const DataSet* dataset) const {
  MAPDATASET_PREPARE;

  Region region = dataset->layout().descriptorLocation(_descriptorNames);

  // only used when probability = true
  PointLayout newLayout;
  int classIdx = -1, classProbIdx = -1;
  DescriptorLengthType classLType, classProbLType = FixedLength; // default value so gcc doesn't complain
  IndexMap realMap, stringMap, enumMap;

  if (_probability) {
    // if we also want the class probabilities, we need to create a new layout
    // with a new field for it
    PointLayout addedLayout;
    addedLayout.add(_className + "Probability", RealType);
    newLayout = mergeLayouts(dataset->layout(), addedLayout);

    Region r = newLayout.descriptorLocation(dataset->layout().descriptorNames());
    createIndexMappings(dataset->layout(), newLayout, r, realMap, stringMap, enumMap);

    Region classRegion = newLayout.descriptorLocation(_className);
    classLType = classRegion.lengthType();
    classIdx = classRegion.index();
    Region classProbRegion = newLayout.descriptorLocation(_className+"Probability");
    classProbLType = classProbRegion.lengthType();
    classProbIdx = classProbRegion.index();
  }
  else { // !probability
    // if we don't want probabilities estimates, we can just use the same
    // layout as we had before
    Region classRegion = dataset->layout().descriptorLocation(_className);
    classLType = classRegion.lengthType();
    classIdx = classRegion.index();
  }

  MAPDATASET_LOOP_POINT;
  Point* newp;
  if (_probability) {
    newp = gaia2::mapPoint(p, newLayout, realMap, stringMap, enumMap);
  }
  else {
    newp = new Point(*p);
  }
  svmPredict(p, newp, _model, region, classLType, classIdx, classProbLType, classProbIdx);
  MAPDATASET_END_LOOP_NO_RETURN;

  return result;
}
