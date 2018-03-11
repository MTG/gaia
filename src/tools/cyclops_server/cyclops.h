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

#ifndef GAIA_CYCLOPS_H
#define GAIA_CYCLOPS_H

#include <QObject>
//#include "cairo/dataset.h"
#include "../../dataset.h"
#include "../../metrics/distancefunction.h"
#include "../../searchspace.h"

typedef QList<QString> CyclopsResultsHeader;
typedef QList<QList<QVariant> > CyclopsResultsValues;

class CyclopsResults {
 public:
  gaia2::SearchResults results;
  CyclopsResultsHeader header;
  CyclopsResultsValues values;

  void append(const gaia2::Result& r, const QList<QVariant>& values);

  /** this works in place */
  void mid(int pos, int length);

  int size() const { return results.size(); }

  // NB: this is quite inefficient at the moment and could be optimized quite easily
  void sort();
};

class Cyclops : public QObject {
Q_OBJECT
 public:

  Cyclops();
  virtual ~Cyclops();

  /**
   * Load a dataset into memory and use the given name as identifier.
   * If there was already a dataset with the same name, it will be overwritten.
   *
   * @param datasetName the identifier to be used for the loaded dataset
   * @param filename the filename where the dataset is stored
   */
  void load(const QString& datasetName, const QString& filename, int start = 0, int offset = -1);

  /**
   * Load the n-th part of a dataset.
   */
  void loadNthPart(const QString& datasetName, const QString& filename, int idx, int total);

  /**
   * Unload a dataset from memory, and frees the associated memory.
   *
   * @param datasetName the dataset identifier
   */
  void unload(const QString& datasetName);

  /**
   * Return the list of datasets currently loaded in memory.
   */
  QStringList datasetNames() const;

  /**
   * Return the layout of the specified dataset.
   *
   * @param datasetName the dataset identifier
   */
  QString layout(const QString& datasetName) const;

  /**
   * Return the size (number of points) of the specified dataset.
   *
   * @param datasetName the dataset identifier
   */
  int size(const QString& datasetName) const;

  /**
   * Set the second dataset as a reference for the first one.
   *
   * @param dataset the dataset for which you want to set a reference
   * @param referenceDataset the dataset which will serve as reference for the other one
   */
  void setReferenceDataSet(const QString& dataset, const QString& referenceDataset);

  /**
   * Create a distance to be used for nn-queries.
   * If there was already a distance with the same name, it will be overwritten.
   *
   * @param distanceName the identifier to be used for this distance
   * @param datasetName the dataset on which this distance will operate
   * @param distance the name of the distance to create (its ID in the factory)
   * @param params the parameters used to create the distance
   */
  void createDistance(const QString& distanceName,
                              const QString& datasetName,
                              const QString& distance,
                              const gaia2::ParameterMap& params = gaia2::ParameterMap());

  /**
   * Delete the distance identified by the given name.
   *
   * @param distanceName the distance identifier
   */
  void deleteDistance(const QString& distanceName);

  /**
   * Return a list of distances currently created in memory.
   */
  QStringList distanceNames() const;

  /**
   * Indexes a dataset/view on the given descriptor name.
   */
  void indexOn(const QString& datasetName, const QString& descriptorName);

  /**
   * Perform a chain of searches, where each result of a query is used as inputSpace
   * for the next one.
   * WARNING: you should not be calling this method directly, rather use the
   * facilities provided by the python client which will make all this machinery
   * transparent and have everything work the same as if using directly the
   * standard python bindings.
   */
  CyclopsResults chainedSearch(const gaia2::yaml::Sequence& queries, const gaia2::yaml::Sequence& header, int numberResults, int offset = 0);

  /**
   * Perform a query-by-id on the given ID.
   *
   * @param queryID the ID of the point to query
   * @param datasetName the identifier of the dataset in which to perform the query
   * @param distanceName the identifier of the distance to be used for the query
   * @param numberResults the number of results to be returned
   * @param offset the offset in the list of results
   */
  gaia2::SearchResults nnSearchById(const QString& queryID,
                                    const QString& datasetName,
                                    const QString& distanceName,
                                    int numberResults,
                                    int offset = 0);

  /**
   * Perform a query-by-id on the given ID and using a filter.
   *
   * @param queryID the ID of the point to query
   * @param datasetName the identifier of the dataset in which to perform the query
   * @param distanceName the identifier of the distance to be used for the query
   * @param filter a string representing the filter to be used
   * @param numberResults the number of results to be returned
   * @param offset the offset in the list of results
   */
  gaia2::SearchResults nnSearchByIdWithFilter(const QString& queryID,
                                              const QString& datasetName,
                                              const QString& distanceName,
                                              const QString& filter,
                                              int numberResults,
                                              int offset = 0);

  gaia2::SearchResults nnSearchByExample(const gaia2::Point& query,
                                         const QString& datasetName,
                                         const QString& distanceName,
                                         int numberResults,
                                         int offset = 0);

  gaia2::SearchResults nnSearchByExampleWithFilter(const gaia2::Point& query,
                                                   const QString& datasetName,
                                                   const QString& distanceName,
                                                   const QString& filter,
                                                   int numberResults,
                                                   int offset = 0);

  /**
   * Get the point with the given ID from the specified dataset.
   */
  const gaia2::Point* getPoint(const QString& datasetName, const QString& pointName) const;

  /**
   * Get the list of points with the given IDs from the specified dataset as the base64
   * representation of a new DataSet containing them.
   */
  std::string getPoints(const QString& datasetName, const QStringList& pointNames, bool failOnError = true) const;

  /**
   * Read the yaml config file and sets up the specified datasets and distances.
   */
  void setup(const QString& filename);

  void setupFromYaml(const gaia2::yaml::Mapping& config);

 protected:
  gaia2::ResultSet nnSearch(const QString& methodName, const gaia2::yaml::Sequence& args,
                            const gaia2::ResultSet* inputSpace = 0);

  QMap<QString, gaia2::DataSet*> _datasets;
  QMap<QString, gaia2::View*> _views; // there should be a corresponding view for each dataset
  QMap<QString, gaia2::DistanceFunction*> _dfuncs;
};


#endif // GAIA_CYCLOPS_H
