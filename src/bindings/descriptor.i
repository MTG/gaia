%exception __getitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "Descriptor index out of range");
      return NULL;
   }
}

%exception __setitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "Descriptor index out of range");
      return NULL;
   }
}

%extend gaia2::SingleValueMorphable<gaia2::Real> {

  const float& __getitem__(int i) const {
    if (i < 0) i = self->size() + i;
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    return self->operator[](i);
  }

  void __setitem__(int i, float x) {
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    self->operator[](i) = x;
  }

  std::vector<float> toList() const {
    std::vector<float> result(self->size(), 0);
    for (int i=0; i<self->size(); i++) result[i] = self->operator[](i);
    return result;
  }

  int size() const {
    return self->size();
  }

  %pythoncode {
    def __abs__(self):
      result = RealDescriptor(self)
      result.abs()
      return result
  }
}

%extend gaia2::SingleValueMorphable<QString> {

  std::string __getitem__(int i) const {
    if (i < 0) i = self->size() + i;
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    return self->operator[](i).toUtf8().data();
  }

  void __setitem__(int i, const std::string& x) {
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    self->operator[](i) = QString::fromUtf8(x.c_str(), x.size());
  }

  std::vector<std::string> toList() const {
    std::vector<std::string> result(self->size());
    for (int i=0; i<self->size(); i++) result[i] = self->operator[](i).toUtf8().data();
    return result;
  }

  int size() const {
    return self->size();
  }

}
