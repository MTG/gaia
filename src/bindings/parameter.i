
// Conversion functions from C++ to Python
%{

QString errmsg;

PyObject* parameterToPython(const QVariant* p) {
  PyObject* result;

  switch ((int)p->type()) {

  case QVariant::String: {
    result = PyString_FromString(p->toString().toUtf8().data());
    break;
  }

  case QVariant::ByteArray: {
    QByteArray array = p->toByteArray();
    result = PyBytes_FromStringAndSize(array.data(), array.size());
    break;
  }

  case QVariant::Int: {
    result = PyInt_FromLong(p->toInt());
    break;
  }

#if QT_VERSION >= 0x40600
  case QMetaType::Float:   // QVariant::Float, which is not defined...
#endif
  case QVariant::Double: {
    result = PyFloat_FromDouble(p->toDouble());
    break;
  }

  case QVariant::Bool: {
    result = PyBool_FromLong(p->toBool());
    break;
  }

  case QVariant::StringList: {
    QStringList qlist = p->toStringList();
    int size = qlist.size();
    result = PyList_New(size);
    for (int i=0; i<size; i++) {
      PyList_SetItem(result, i, PyString_FromString(qlist[i].toUtf8().data()));
    }
    break;
  }

  case QVariant::Map: {
    QVariantMap qmap = p->toMap();
    result = PyDict_New();
    PyObject *pykey, *pyvalue;

    foreach (const QString& key, qmap.keys()) {
      pykey = PyString_FromString(key.toUtf8().data());
      const QVariant& value = qmap.value(key);

      pyvalue = parameterToPython(&value);
      if (!pyvalue) {
        errmsg = QString("[\"") + key + "\"]" + errmsg;
        PyErr_SetString(PyExc_TypeError, errmsg.toUtf8().data());
        return NULL;
      }

      // add it in dictionary
      PyDict_SetItem(result, pykey, pyvalue);
    }
    break;
  }

  case QVariant::List:
  case QVariant::UserType: {
    if (p->canConvert(QVariant::List)) { // check only necessary for UserType
      QVariantList qlist = p->toList();
      int size = qlist.size();
      result = PyList_New(size);
      PyObject* pyvalue;

      for (int i=0; i<size; i++) {
        pyvalue = parameterToPython(&qlist.at(i));
        if (!pyvalue) {
          errmsg = QString("[") + QString::number(i) + "]" + errmsg;
          PyErr_SetString(PyExc_TypeError, errmsg.toUtf8().data());
          return NULL;
        }

        PyList_SetItem(result, i, pyvalue);
      }
      break;
    }

    // otherwise, fall through in the default case and raise an exception
  }

  default:
    errmsg = QString(": Unrecognized Parameter type: ") +
      QVariant::typeToName(p->type()) + " (" +
      QString::number((int)p->type()) + ")";

    PyErr_SetString(PyExc_TypeError, errmsg.toUtf8().data());
    result = NULL;
  }

  return result;
}

// This leaks if there is an exception being thrown...
PyObject* pmapToPython(const ParameterMap* pm) {
  PyObject* result = PyDict_New();
  PyObject *pykey, *pyvalue;

  foreach (const QString& key, pm->keys()) {
    pykey = PyString_FromString(key.toUtf8().data());
    const QVariant& value = pm->value(key);

    pyvalue = parameterToPython(&value);
    if (!pyvalue) {
      errmsg = QString("Error at pmap[\"") + key + "\"]" + errmsg;
      qDebug() << errmsg;
      PyErr_SetString(PyExc_TypeError, errmsg.toUtf8().data());
      return NULL;
    }

    // add it in dictionary
    PyDict_SetItem(result, pykey, pyvalue);
  }

  return result;
}

%}

%extend gaia2::Parameter {
  PyObject* toPython() const {
    return parameterToPython(self);
  }

  std::string qtype() const {
    return QVariant::typeToName(self->type());
  }
}


%extend gaia2::ParameterMap {

  void __setitem__(const std::string& key,  double d) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()), d);
  }

  void __setitem__(const std::string& key,  int i) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()), i);
  }

  void __setitem__(const std::string& key, const std::string& s) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()),
                 QString::fromUtf8(s.c_str(), s.size()));
  }

  void __setitem__(const std::string& key, const std::vector<std::string>& s) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()),
                 convert::VectorString_to_QStringList(s));
  }

  void __setitem__(const std::string& key, const std::vector<float>& s) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()),
                 convert::VectorFloat_to_RealDescriptor(s));
  }

  void __setitem__(const std::string& key, const std::vector<std::pair<float, float> >& v) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()),
                 convert::VectorPair_to_QPointList(v));
  }

  // EVIL FUNCTION: I have no idea why, but defining this function makes SWIG
  // go completely crazy and spit out weird errors in weird places...
  /*
  void __setitem__(const std::string& key, const ParameterMap* pm) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()),
                 pm);
  }
  */

  // this function replaces the previous evil one
  void setParameterMap(const std::string& key, const ParameterMap& m) {
    self->insert(QString::fromUtf8(key.c_str(), key.size()), m);
  }

  const gaia2::Parameter& __getitem__(const std::string& key) const {
    return self->value(QString::fromUtf8(key.c_str(), key.size()));
  }

  std::vector<std::string> keys() const {
    return convert::QStringList_to_VectorString(self->keys());
  }

  PyObject* toPython() const {
    return pmapToPython(self);
  }

#define CC(type, name) qDebug() << #type << ":" << self->value(name).canConvert(QVariant::type)

  void dtype(const std::string& key) {
    QString name = QString::fromUtf8(key.c_str(), key.size());
    qDebug() << "orig type:" << QVariant::typeToName(self->value(name).type());
    CC(Int, name);
    CC(Double, name);
    CC(List, name);
    CC(String, name);
    CC(StringList, name);
  }
}
