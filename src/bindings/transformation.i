%extend gaia2::Transformation {

  void load(const std::string& filename) {
    return self->load(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  void save(const std::string& filename) {
    return self->save(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  PyObject* toPython() const {
    PyObject* result = PyDict_New();

    PyDict_SetItem(result, PyString_FromString("Analyzer name"),
                   PyString_FromString(self->analyzerName.toUtf8().data()));
    PyDict_SetItem(result, PyString_FromString("Analyzer parameters"),
                   pmapToPython(&self->analyzerParams));

    PyDict_SetItem(result, PyString_FromString("Applier name"),
                   PyString_FromString(self->applierName.toUtf8().data()));
    PyDict_SetItem(result, PyString_FromString("Applier parameters"),
                   pmapToPython(&self->params));

    PyDict_SetItem(result, PyString_FromString("Additional info"),
                   pmapToPython(&self->info));

    return result;
  }

}


%exception __getitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "TransfoChain index out of range");
      return NULL;
   }
}

%exception __setitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "TransfoChain index out of range");
      return NULL;
   }
}


%extend gaia2::TransfoChain {

  void load(const std::string& filename) {
    return self->load(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  void save(const std::string& filename) {
    return self->save(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  void append(const Transformation& transfo) {
    self->append(transfo);
  }

  int size() const {
    return self->size();
  }

  const Transformation& __getitem__(int i) const {
    if (i < 0) i = self->size() + i;
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    return self->at(i);
  }

  void __setitem__(int i, const Transformation& transfo) {
    if (i < 0) i = self->size() + i;
    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    (*self)[i] = transfo;
  }

}
