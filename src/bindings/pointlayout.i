%extend gaia2::PointLayout {

  std::vector<std::string> _symmetricDifferenceWith(const gaia2::PointLayout& other) {
    return convert::QStringList_to_VectorString(self->symmetricDifferenceWith(other));
  }

  std::vector<std::string> _differenceWith(const gaia2::PointLayout& other) {
    return convert::QStringList_to_VectorString(self->differenceWith(other));
  }

  %pythoncode %{
    def symmetricDifferenceWith(self, other):
      return self._symmetricDifferenceWith(other)

    def differenceWith(self, other):
      return self._differenceWith(other)
  %}

  Region descriptorLocation(const std::string& name) const {
    return self->descriptorLocation(QString::fromUtf8(name.c_str(), name.size()));
  }

  Region descriptorLocation(const std::vector<std::string>& fullName) const {
    QStringList qlist;
    for (uint i=0; i<fullName.size(); i++) {
      qlist += QString::fromUtf8(fullName[i].c_str(), fullName[i].size());
    }
    return self->descriptorLocation(qlist);
  }

  std::string descriptorName(DescriptorType type, DescriptorLengthType ltype, int index) const {
    return self->descriptorName(type, ltype, index).toUtf8().data();
  }

  std::vector<std::string> descriptorNames(const std::string& pattern = "*") {
    QStringList qlist = self->descriptorNames(QStringList() << QString::fromUtf8(pattern.c_str(),
                                                                                 pattern.size()));
    return convert::QStringList_to_VectorString(qlist);
  }

  std::vector<std::string> descriptorNames(const std::vector<std::string>& patterns) {
    QStringList qlist = self->descriptorNames(convert::VectorString_to_QStringList(patterns));
    return convert::QStringList_to_VectorString(qlist);
  }

  std::vector<std::string> descriptorNames(DescriptorType type, const std::string& pattern = "*") {
    QStringList qlist = self->descriptorNames(type,
                                              QStringList(QString::fromUtf8(pattern.c_str(),
                                                                            pattern.size())));
    return convert::QStringList_to_VectorString(qlist);
  }

  void add(const std::string& name, DescriptorType type = UndefinedType,
           DescriptorLengthType ltype = VariableLength, int size = 1) {
    self->add(QString::fromUtf8(name.c_str(), name.size()), type, ltype, size);
  }

  void add(const std::string& parentName, const std::string& name,
           DescriptorType type = UndefinedType,
           DescriptorLengthType ltype = VariableLength, int size = 1) {
    self->add(QString::fromUtf8(parentName.c_str(), parentName.size()),
              QString::fromUtf8(name.c_str(), name.size()),
              type, ltype, size);
  }

  void remove(const std::string& name) {
    self->remove(QString::fromUtf8(name.c_str(), name.size()));
  }

  std::string toYaml() const {
    return self->toYaml().toUtf8().data();
  }

}
