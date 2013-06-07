%extend gaia2::BaseView<gaia2::DataSet, gaia2::Point, gaia2::SearchPoint, gaia2::DistanceFunction> {

  void indexOn(const std::string& descriptorName) {
    self->indexOn(QString::fromUtf8(descriptorName.c_str(), descriptorName.size()));
  }


  // nnSearch with gaia2::Point

  gaia2::ResultSet nnSearch(const Point& p, const DistanceFunction* dist, const std::string& filter) {
    return self->nnSearch(p, dist, QString::fromUtf8(filter.c_str(), filter.size()));
  }

  gaia2::ResultSet nnSearch(const Point& p, ResultSet inputSpace,
                            const DistanceFunction* dist, const std::string& filter) {
    return self->nnSearch(p, inputSpace, dist, QString::fromUtf8(filter.c_str(), filter.size()));
  }


  // nnSearch with std::string PointID

  gaia2::ResultSet nnSearch(const std::string& id, const DistanceFunction* dist, const Filter* filter = 0) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), dist, filter);
  }

  gaia2::ResultSet nnSearch(const std::string& id, const DistanceFunction* dist, const std::string& filter) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }
  gaia2::ResultSet nnSearch(const std::string& id, gaia2::ResultSet inputSpace,
                            const DistanceFunction* dist, const Filter* filter = 0) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), inputSpace, dist, filter);
  }

  gaia2::ResultSet nnSearch(const std::string& id, gaia2::ResultSet inputSpace,
                            const DistanceFunction* dist, const std::string& filter) {
    return self->nnSearch(QString::fromUtf8(id.c_str(), id.size()), inputSpace, dist,
                          QString::fromUtf8(filter.c_str(), filter.size()));
  }

}
