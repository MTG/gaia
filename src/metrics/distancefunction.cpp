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
#include "distancefunction.h"
#include "utils.h"

// distance functions
#include "euclideandistance.h"
#include "keydistance.h"
#include "linearcombinationdistance.h"
#include "manhattandistance.h"
#include "minkowskidistance.h"
#include "nulldistance.h"
#include "kullbackleiblerdistance.h"
#include "cosinesimilarity.h"
#include "cosineangledistance.h"
#include "exponentialcompressdistance.h"
#include "rhythmdistance.h"
#include "weightedpearsondistance.h"
#include "bpfdistance.h"
#include "weightedeuclideandistance.h"
#include "randomdistance.h"
#include "summedsegmentdistance.h"
#include "semanticclassifierdistance.h"
#include "crosssegmentdistance.h"
#include "forceidentitydistance.h"
#include "resistoraveragedistance.h"

namespace gaia2 {

#define REGISTER_METRIC(name) MetricFactory::Registrar<name##Distance> register##name(#name);


/**
 * Registers all the metrics available inside the MetricFactory.
 * <b>NB:</b> For the python docstrings to work correctly, the name with which
 * the distance is registered needs to be the exact same name as the class name,
 * with the trailing "Distance" removed (case-sensitive).
 */
void registerMetrics() {
  REGISTER_METRIC(Euclidean);
  REGISTER_METRIC(WeightedEuclidean);
  REGISTER_METRIC(Key);
  REGISTER_METRIC(LinearCombination);
  REGISTER_METRIC(Manhattan);
  REGISTER_METRIC(Minkowski);
  REGISTER_METRIC(Null);
  REGISTER_METRIC(KullbackLeibler);
  MetricFactory::Registrar<CosineSimilarity> registerCosineSimilarity("CosineSimilarity");
  REGISTER_METRIC(CosineAngle);
  REGISTER_METRIC(ExponentialCompress);
  REGISTER_METRIC(Rhythm);
  REGISTER_METRIC(WeightedPearson);
  REGISTER_METRIC(BPF);
  REGISTER_METRIC(Random);
  REGISTER_METRIC(SummedSegment);
  REGISTER_METRIC(SemanticClassifier);
  REGISTER_METRIC(CrossSegment);
  REGISTER_METRIC(ForceIdentity);
  REGISTER_METRIC(ResistorAverage);
}


DistanceFunction::DistanceFunction(const PointLayout& layout, const ParameterMap& params) :
  _layout(layout), _params(params) {

  // these should be made available to all (most) metrics
  validParams << "descriptorNames" << "except";

  // they also have default values, so a call to Analyzer::ctor should never fail
  QStringList descs = selectDescriptors(layout, UndefinedType,
                                        params.value("descriptorNames", "*").toStringList(),
                                        params.value("except", QStringList()).toStringList());

  _region = layout.descriptorLocation(descs);
}

const Region& DistanceFunction::checkFixedLength(const Region& region) const {
  if (!region.isLengthTypeOnlyFrom(FixedLength)) {
    throw FixedLengthException("The %1 metric can only operate on fixed-length descriptors");
  }

  return region;
}


} // namespace gaia2
