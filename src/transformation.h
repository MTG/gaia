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
#ifndef GAIA_TRANSFORMATION_H
#define GAIA_TRANSFORMATION_H

#include <QList>
#include <QString>
#include "parameter.h"
#include "pointlayout.h"

namespace gaia2 {

class DataSet;
class Point;
class Analyzer;
class Applier;

/**
 * Class containing all info pertaining to a specific transformation, ie:
 * the name of the analyzer class, the input parameters, the results of this
 * analysis (= the applier parameters), and the name of the applier class.
 * This class may also contain pre-instantiated analyzer and applier objects.
 */
class Transformation {
 public:
  QString name;                // necessary?  or even used?
  QString analyzerName;        // factory name of the analyzer
  QString applierName;         // factory name of the applier
  ParameterMap analyzerParams; // parameters for the analysis
  ParameterMap params;         // parameters extracted from the analysis

  ParameterMap info;           // used to pass additional, but not necessary,
                               // info to the user

  PointLayout layout;          // original layout of the analyzed dataset

 protected:
  mutable Analyzer* _analyzer; // useless?? maybe not, can contain info such as
                               // variance accountability, etc...
  mutable Applier* _applier;

 public:
  Transformation() :
    name(""), analyzerName(""), applierName(""),
    _analyzer(0), _applier(0) {}

  Transformation(const PointLayout& l) :
    name(""), analyzerName(""), applierName(""), layout(l),
    _analyzer(0), _applier(0) {}

  Transformation(const Transformation& rhs);
  Transformation& operator=(const Transformation& rhs);

  bool operator==(const Transformation& rhs) const;

  ~Transformation();

  const Analyzer* analyzer() const;
  const Applier* applier() const;

  /**
   * Apply the current transformation with its parameters to the DataSet.
   */
  DataSet* applyToDataSet(const DataSet* dataset) const;

  /**
   * Redo the full analysis+apply to the given dataset. The resulting transformation
   * that is applied will have the same analyzer parameters, but different applier ones.
   */
  DataSet* retransformDataSet(const DataSet* dataset) const;

  Point* applyToPoint(const Point* point) const;

  void load(const QString& filename);
  void save(const QString& filename);

  /**
   * Updates (if necessary) the parameters for this transformation from the
   * specified original version to the current one.
   */
  void updateOldTransformation(int version);

  // serialization methods
  friend QDataStream& operator<<(QDataStream& out, const Transformation& transfo);
  friend QDataStream& operator>>(QDataStream& in, Transformation& transfo);
};


class TransfoChain : public QList<Transformation> {
 public:

  Point* mapPoint(const Point* point, bool takeOwnership = false) const;
  DataSet* mapDataSet(const DataSet* dataset, bool takeOwnership = false) const;

  /**
   * Only maps a part of the transformation chain, you need to specify the
   * indices of first and after-last transformation.
   * If @c end == 0, then all the transformations starting at index @c start
   * will be applied.
   */
  DataSet* partialMapDataSet(const DataSet* dataset, int start, int end = 0) const;

  /**
   * Retransforms the dataset using the given partial history.
   * @see partialMapDataSet
   */
  DataSet* retransformDataSet(const DataSet* dataset, int start, int end = 0) const;

  void load(const QString& filename);
  void save(const QString& filename) const;

  bool contains(const QString& analyzerName) const;

  bool operator==(const TransfoChain& rhs) const;

  // serialization methods
  friend QDataStream& operator<<(QDataStream& out, const TransfoChain& transfo);
  friend QDataStream& operator>>(QDataStream& in, TransfoChain& transfo);
};


DataSet* transform(DataSet* ds, const QString& name, const ParameterMap& params = ParameterMap());

DataSet* applyTransfoChain(DataSet* ds, const QString& yamlTransfoChain);

inline DataSet* applyTransfoChain(DataSet* ds, const std::string& yamlTransfoChain) {
  return applyTransfoChain(ds, QString::fromStdString(yamlTransfoChain));
}

} // namespace gaia2

#endif // GAIA_TRANSFORMATION_H
