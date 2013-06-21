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
#ifndef GAIA_RCA_H
#define GAIA_RCA_H

#include "analyzer.h"
#include "Eigen/Core"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The RCA Analyzer computes the <a href="http://www.cs.huji.ac.il/~daphna">Relevant Components Analysis</a>
 * of the given DataSet using the specified descriptors.
 * @b Note: the use of <code>classFile</code>, <code>classMap</code> and
 *          <code>className</code> is exclusive, ie: you need to specify one and
 *          only one of them.
 * @param descriptorNames the names of the descriptors to be included in the
 *        RCA analysis. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param resultName the name of the resulting descriptor. (default: some
 *        name constructed from 'rca', the number of dimensions, and the original
 *        descriptor names)
 * @param dimension the target (final) dimension.
 * @param classFile the name of the file containing the list of classes (chunklets
 *        in RCA language).
 * @param classMap the groundtruth map, from point IDs to their class value.
 * @param className the name of the descriptor in the PointLayout that will be
 *        used as class label.
 */
class RCA : public Analyzer {

 protected:
  QString _resultName;
  int _targetDimension;
  QString _classFile, _className;
  QMap<QString, QString> _classMap;

  /**
   * Computes the within chunklet covariance matrix. The implementation follows
   * rigorously the description from the paper, however some doubts subsist
   * (most probably due to my ignorance):
   * - why do we normalize all of them at once, instead of normalizing each one
   *   separately?
   * - why do we normalize by n instead of (n-1) (probably not worth much...)
   */
  Eigen::MatrixXd computeCovarianceMatrix(const QList<const PointArray*>& chunklets,
                                          const Region& region) const;

  Eigen::MatrixXd whitenMatrix(const Eigen::MatrixXd& cov) const;


 public:
  RCA(const ParameterMap& params);
  virtual ~RCA() {};

  Transformation analyze(const DataSet* dataset, const Region& region) const;


  // static helper functions
  static QList<const PointArray*> cleanChunklets(QList<PointArray*>& chunklets);

  static QList<const PointArray*> getChunkletsFromMap(const QMap<QString, QString>& filename,
                                                      const DataSet* inputDataSet);

  static QList<const PointArray*> getChunkletsFromFile(const QString& filename,
                                                       const DataSet* inputDataSet);

  static QList<const PointArray*> getChunkletsFromLabel(const QString& label,
                                                        const DataSet* inputDataSet);



};

}

#endif // GAIA_RCA_H
