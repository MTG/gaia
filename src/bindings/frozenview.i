
%extend gaia2::BaseView<gaia2::FrozenDataSet, gaia2::FrozenPoint, gaia2::FrozenSearchPoint, gaia2::FrozenDistance> {

  void indexOn(const std::string& descriptorName) {
    self->indexOn(QString::fromUtf8(descriptorName.c_str(), descriptorName.size()));
  }

  // nnSearch with gaia2::FrozenPoint

  gaia2::FrozenResultSet nnSearch(const std::vector<float>& query,
                                  const gaia2::FrozenDistance* dist) {
    return self->nnSearch(FrozenPoint::Map(&query[0], query.size()), dist);
  }

  gaia2::FrozenResultSet nnSearch(const std::vector<float>& query,
                                  const gaia2::FrozenDistance* dist,
                                  const std::string& filter) {
    return self->nnSearch(FrozenPoint::Map(&query[0], query.size()), dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }

  gaia2::FrozenResultSet nnSearch(const std::vector<float>& query,
                                  gaia2::FrozenResultSet inputSpace,
                                  const gaia2::FrozenDistance* dist,
                                  const std::string& filter) {
    return self->nnSearch(FrozenPoint::Map(&query[0], query.size()), inputSpace, dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }

  // nnSearch with std::string PointID

  gaia2::FrozenResultSet nnSearch(const std::string& id, const FrozenDistance* dist, const Filter* filter = 0) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), dist, filter);
  }

  gaia2::FrozenResultSet nnSearch(const std::string& id, const FrozenDistance* dist, const std::string& filter) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }
  gaia2::FrozenResultSet nnSearch(const std::string& id, gaia2::FrozenResultSet inputSpace,
                                  const FrozenDistance* dist, const Filter* filter = 0) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), inputSpace, dist, filter);
  }

  gaia2::FrozenResultSet nnSearch(const std::string& id, gaia2::FrozenResultSet inputSpace,
                                  const FrozenDistance* dist, const std::string& filter) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), inputSpace, dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }

}

