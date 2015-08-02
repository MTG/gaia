

%extend gaia2::BaseResultSet<gaia2::SearchPoint, gaia2::DataSet> {

  std::vector<std::pair<std::string, float> > _get(int n, int offset = 0) {
    gaia2::SearchResults qlist = self->get(n, offset);
    return convert::SearchResults_to_StlTypes(qlist);
  }

  %pythoncode %{
    def get(self, n, offset = 0):
      return self._get(n, offset)
  %}

  void addPoints(const DataSet* dataset, const std::vector<std::string>& ids) {
    self->addPoints(dataset, convert::VectorString_to_QStringList(ids));
  }

  void removePoints(const std::vector<std::string>& ids) {
    self->removePoints(convert::VectorString_to_QStringList(ids));
  }

}


%extend gaia2::BaseResultSet<gaia2::FrozenSearchPoint, gaia2::FrozenDataSet> {

  std::vector<std::pair<std::string, float> > _get(int n, int offset = 0) {
    gaia2::SearchResults qlist = self->get(n, offset);
    return convert::SearchResults_to_StlTypes(qlist);
  }

  %pythoncode %{
    def get(self, n, offset = 0):
      return self._get(n, offset)
  %}
}
