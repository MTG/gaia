//%ignore gaia2::Point::frealData;
%ignore gaia2::Point::fenumData;
%ignore gaia2::Point::fstringData;
%ignore gaia2::Point::vrealData;
%ignore gaia2::Point::venumData;
%ignore gaia2::Point::vstringData;

%extend gaia2::Point {

  void load(const std::string& filename,
            const std::vector<std::string>& select = std::vector<std::string>(1, "*"),
            const std::vector<std::string>& exclude = std::vector<std::string>()) {
    self->load(QString::fromUtf8(filename.c_str(), filename.size()),
               convert::VectorString_to_QStringList(select),
               convert::VectorString_to_QStringList(exclude));
  }

  void loadFromString(const std::string& data,
                      const std::vector<std::string>& select = std::vector<std::string>(1, "*"),
                      const std::vector<std::string>& exclude = std::vector<std::string>()) {
    self->loadFromString(data,
                         convert::VectorString_to_QStringList(select),
                         convert::VectorString_to_QStringList(exclude));
  }

  std::string name() const {
    return self->name().toUtf8().data();
  }

  void setName(const std::string& s) {
    self->setName(QString::fromUtf8(s.c_str(), s.size()));
  }

  // scope methods
  std::vector<Real> _scope(int segment) const {
    std::vector<Real> result(2, 0.0);
    result[0] = self->scope(segment).start;
    result[1] = self->scope(segment).end;
    return result;
  }

  %pythoncode {
    def scope(self, seg):
      return self._scope(seg)
  }

  void setScope(int segment, Real start, Real end) {
    self->scope(segment).start = start;
    self->scope(segment).end = end;
  }


  // value methods
  std::vector<float> _value(const std::string& name) const {
    return convert::RealDescriptor_to_VectorFloat(self->value(QString::fromUtf8(name.c_str(), name.size())));
  }

  std::vector<float> _value(int nsegment, const std::string& name) const {
    return convert::RealDescriptor_to_VectorFloat(self->value(nsegment,
                                                              QString::fromUtf8(name.c_str(), name.size())));
  }

  %pythoncode {
    def value(self, *args):
      result = self._value(*args)
      if len(result) == 1:
        return result[0]
      return result
  }

  void setValue(const std::string& name, const RealDescriptor& value) {
    self->setValue(QString::fromUtf8(name.c_str(), name.size()), value);
  }

  // TODO: deprecate those, as we shouldn't be using the RealDescriptor wrapper in python anymore
  void setValue(int nsegment, const std::string& name, const RealDescriptor& value) {
    self->setValue(nsegment, QString::fromUtf8(name.c_str(), name.size()), value);
  }

  void setValue(const std::string& name, const std::vector<Real>& value) {
    self->setValue(QString::fromUtf8(name.c_str(), name.size()),
                   convert::VectorFloat_to_RealDescriptor(value));
  }

  void setValue(int nsegment, const std::string& name, const std::vector<Real>& value) {
    self->setValue(nsegment, QString::fromUtf8(name.c_str(), name.size()),
                   convert::VectorFloat_to_RealDescriptor(value));
  }

  void setValue(const std::string& name, Real value) {
    self->setValue(QString::fromUtf8(name.c_str(), name.size()), value);
  }

  void setValue(int nsegment, const std::string& name, Real value) {
    self->setValue(nsegment, QString::fromUtf8(name.c_str(), name.size()), value);
  }

  // label methods
  std::vector<std::string> _label(const std::string& name) const {
    return convert::StringDescriptor_to_VectorString(self->label(QString::fromUtf8(name.c_str(), name.size())));
  }

  std::vector<std::string> _label(int nsegment, const std::string& name) const {
    return convert::StringDescriptor_to_VectorString(self->label(nsegment, QString::fromUtf8(name.c_str(), name.size())));
  }

  %pythoncode {
    def label(self, *args):
      result = self._label(*args)
      if len(result) == 1:
        return result[0]
      return result
  }


  // TODO: deprecate those, as we shouldn't be using the RealDescriptor wrapper in python anymore
  void setLabel(const std::string& name, const StringDescriptor& label) {
    self->setLabel(QString::fromUtf8(name.c_str(), name.size()), label);
  }

  void setLabel(int nsegment, const std::string& name, const StringDescriptor& label) {
    self->setLabel(nsegment, QString::fromUtf8(name.c_str(), name.size()), label);
  }

  void setLabel(const std::string& name, const std::vector<std::string>& label) {
    self->setLabel(QString::fromUtf8(name.c_str(), name.size()),
                   convert::VectorString_to_StringDescriptor(label));
  }

  void setLabel(int nsegment, const std::string& name, const std::vector<std::string>& label) {
    self->setLabel(nsegment, QString::fromUtf8(name.c_str(), name.size()),
                   convert::VectorString_to_StringDescriptor(label));
  }

  void setLabel(const std::string& name, const std::string& label) {
    self->setLabel(QString::fromUtf8(name.c_str(), name.size()),
                   QString::fromUtf8(label.c_str(), label.size()));
  }

  void setLabel(int nsegment, const std::string& name, const std::string& label) {
    self->setLabel(nsegment,
                   QString::fromUtf8(name.c_str(), name.size()),
                   QString::fromUtf8(label.c_str(), label.size()));
  }

}
