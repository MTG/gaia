%exception __getitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "Point index out of range");
      return NULL;
   }
}


%extend gaia2::DataSet {

  std::string _name() const {
    return self->name().toUtf8().data();
  }

  gaia2::PointLayout _layout() const {
    return self->layout();
  }

  %pythoncode %{
    def name(self):
      return self._name()

    def layout(self):
      return self._layout()
  %}


  void setName(const std::string& name) {
    self->setName(QString::fromUtf8(name.c_str(), name.size()));
  }

  void load(const std::string& filename) {
    self->load(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  void save(const std::string& filename) {
    self->save(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  int size() const { return self->size(); }

  bool contains(const std::string& name) const {
    return self->contains(QString::fromUtf8(name.c_str(), name.size()));
  }

  Point* point(const std::string& name) {
    return self->point(QString::fromUtf8(name.c_str(), name.size()));
  }

  const Point* __getitem__(int i) {
    if (i < 0) i = self->size() + i;

    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    return self->at(i);
  }

  std::vector<std::string> pointNames() const {
    return convert::QStringList_to_VectorString(self->pointNames());
  }

  void addPoints(const std::vector<Point*>& points) {
    self->addPoints(QVector<Point*>::fromStdVector(points));
  }

  static DataSet* mergeFiles(const std::map<std::string, std::string>& sigfiles,
                             const std::vector<std::string>& descsSelect = std::vector<std::string>(1, "*"),
                             const std::vector<std::string>& descsExclude = std::vector<std::string>(),
                             int start = 0, int end = 1000000) {
    QMap<QString, QString> smap;
    std::map<std::string, std::string>::const_iterator it = sigfiles.begin();
    for (; it != sigfiles.end(); ++it) {
      smap.insert(QString::fromUtf8(it->first.c_str(), it->first.size()),
                  QString::fromUtf8(it->second.c_str(), it->second.size()));
    }
    return DataSet::mergeFiles(smap,
                               convert::VectorString_to_QStringList(descsSelect),
                               convert::VectorString_to_QStringList(descsExclude),
                               start, end);
  }

  void removePoint(const std::string& name) {
    self->removePoint(QString::fromUtf8(name.c_str(), name.size()));
  }

  void removePoints(const std::vector<std::string>& names) {
    self->removePoints(convert::VectorString_to_QStringList(names));
  }


}
