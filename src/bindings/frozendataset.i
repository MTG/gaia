%exception __getitem__ {
   try {
      $action
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_IndexError, "Point index out of range");
      return NULL;
   }
}


%pythoncode %{

import fnmatch
import collections
import struct
import base64
import gaia2.fastyaml as yaml

class FrozenLayout(object):
    def __init__(self, dataset):
        self._dataset = dataset
        if dataset is not None:
            descs = dataset.descriptorNames()
            self._layout = dict(zip(descs,
                                    map(lambda x: dataset.descriptorLocation(x), descs)))

    @staticmethod
    def fromLayout(layout):
        result = FrozenLayout(None)
        descs = layout.descriptorNames()

        def dloc(desc):
            loc = layout.descriptorLocation(desc).segment()
            return (loc.begin, loc.end)

        result._layout = dict(zip(descs,
                                  map(dloc, descs)))

        return result


    def __eq__(self, other):
        return self._layout == other._layout

    def __ne__(self, other):
        return self._layout != other._layout


    def descriptorNames(self, pattern = None):
        if pattern is None or pattern == RealType:
            return self._layout.keys()
        if pattern in (StringType, EnumType):
            return []

        if isinstance(pattern, str):
            pattern = pattern.lower()
            # try to be a bit lenient here by allowing to not specify the namespace of a descriptor
            if pattern[0] not in ('*', '.'):
                pattern = '*.' + pattern

            return [ desc for desc in self._layout.keys() if fnmatch.fnmatch(desc, pattern) ]

        raise NotImplementedError('You need to specify either the type of descriptors you want or a pattern for their name')

    def descriptorLocation(self, descname):
        dnames = self.descriptorNames(descname)
        if not dnames:
            raise Exception('Unknown descriptor: %s - available descriptors: %s' % (descname, self.descriptorNames()))
        if len(dnames) > 1:
            raise Exception('Ambiguous descriptor name: %s' % descname)

        return self._layout[dnames[0]]


    def _getDescriptorTree(self):
        def createDefaultDict():
            return collections.defaultdict(createDefaultDict)

        tree = collections.defaultdict(createDefaultDict)
        for desc, region in self._layout.items():
            parts = desc.split('.')
            try:
                parts.remove('')
            except ValueError:
                pass
            node = tree
            for part in parts[:-1]:
                node = node[part]
            node[parts[-1]] = region

        return tree

    def _printTree(self, t, offset):
        result = ''
        for k, v in sorted(t.items()):
            if isinstance(v, tuple):
                result += '%s%s: Real FL [ %d %d ]\n' % (' ' * offset, k, v[0], v[1])
            else:
                result += '%s%s:\n%s' % (' ' * offset, k, self._printTree(v, offset + 2))

        return result

    def toYaml(self):
        tree = self._getDescriptorTree()
        result = 'enums:\ndescriptors:\n' + self._printTree(tree, offset = 2)
        return result[:-1] # remove trailing new-line character

    def __str__(self):
        return self.toYaml()

    def toBase64(self):
        if '_layout' in self.__dict__:
            layout = self._layout
        else:
            layout = None
        return base64.b64encode(yaml.dump(layout))

    @staticmethod
    def fromBase64(str64):
        result = FrozenLayout(None)
        result._layout = yaml.load(base64.b64decode(str64))
        return result


class FrozenPoint(tuple):
    def __new__(cls, values, name, dataset, layout = None):
        return tuple.__new__(cls, values)

    def __init__(self, values, name, dataset, layout = None):
        self._name = name
        self._dataset = dataset
        self._layout = layout

    def name(self):
        return self._name

    def setName(self, name):
        self._name = name

    def dataset(self):
        return self._dataset

    def layout(self):
        if self._layout is None:
            self._layout = FrozenLayout(self._dataset)
        return self._layout

    def setLayout(self):
        raise NotImplementedError()

    def __getitem__(self, desc):
        if isinstance(desc, str):
            region = self.layout().descriptorLocation(desc)
            return self[region[0]:region[1]]

        else:
            return tuple.__getitem__(self, desc)

    def value(self, desc):
        return self.__getitem__(desc)

    @staticmethod
    def checkValidFrozenPoint(p):
        l = p.layout()
        invalid = []
        for desc in l.descriptorNames():
            if l.descriptorLocation(desc).lengthType() == VariableLength:
                invalid.append(desc)
        if invalid:
            raise ValueError('Point cannot be frozen because it contains the following variable-length descriptors: %s' % invalid)

        invalid = l.descriptorNames(StringType) + l.descriptorNames(EnumType)
        if invalid:
            raise ValueError('Can only freeze points that contain only real descriptors (no strings or enums), however the following are strings: %s' % invalid)


    @staticmethod
    def fromPoint(p):
        FrozenPoint.checkValidFrozenPoint(p)

        flayout = FrozenLayout.fromLayout(p.layout())

        return FrozenPoint(p.frealData(), p.name(), dataset = None, layout = flayout)


    def toBase64(self):
        name64 = base64.b64encode(self._name)
        nsize64 = '%04d' % len(name64)
        data64 = base64.b64encode(struct.pack('f'*len(self), *self))
        size64 = '%04d' % len(data64)
        layout64 = self.layout().toBase64()
        return nsize64 + name64 + size64 + data64 + layout64


    @staticmethod
    def fromBase64(str64):
        offset = 0
        nsize64 = int(str64[offset : offset+4])
        name64 = str64[offset+4 : offset+4+nsize64]

        offset = offset+4+nsize64
        size64 = int(str64[offset : offset+4])
        data64 = str64[offset+4 : offset+4+size64]
        layout64 = str64[offset+4+size64:]

        name = base64.b64decode(name64)
        packed_data = base64.b64decode(data64)
        data = struct.unpack('f'*(len(packed_data)/4), packed_data)
        result = FrozenPoint(data, name, None)
        result._layout = FrozenLayout.fromBase64(layout64)

        return result

%}

%extend gaia2::FrozenDataSet {

  void load(const std::string& filename) {
    self->load(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  void save(const std::string& filename) {
    self->save(QString::fromUtf8(filename.c_str(), filename.size()));
  }

  int size() const { return self->size(); }

  int pointIndex(const std::string& name) const {
    return self->pointIndex(QString::fromUtf8(name.c_str(), name.size()));
  }

  std::vector<int> descriptorLocation(const std::string& name) const {
    QPair<int, int> pos = self->descriptorLocation(QString::fromUtf8(name.c_str(), name.size()));
    std::vector<int> result(2);
    result[0] = pos.first;
    result[1] = pos.second;
    return result;
  }

  std::vector<float> _point_tuple(const std::string name) const {
    gaia2::FrozenPoint result = self->point(QString::fromUtf8(name.c_str(), name.size()));
    return std::vector<float>(result.data(),
                              result.data() + self->dimension());
  }

  std::vector<float> _getitem_tuple(int i) {
    if (i < 0) i = self->size() + i;

    if (i >= self->size() || i < 0) {
      throw std::out_of_range("");
    }
    return std::vector<float>(self->row(i).data(),
                              self->row(i).data() + self->dimension());
  }

  %pythoncode %{
    def point(self, name):
      return FrozenPoint(self._point_tuple(name), name, self)

    def __getitem__(self, idx):
      return FrozenPoint(self._getitem_tuple(idx), self.pointName(idx), self)

    def layout(self):
        return FrozenLayout(self)

  %}


  std::vector<std::string> pointNames() const {
    return convert::QStringList_to_VectorString(self->pointNames());
  }

  std::string _pointName(int idx) const {
    return self->pointName(idx).toUtf8().data();
  }

  %pythoncode %{
    def pointName(self, idx):
      return self._pointName(idx)
  %}

  std::vector<std::string> descriptorNames() const {
    return convert::QStringList_to_VectorString(self->descriptorNames());
  }

}
