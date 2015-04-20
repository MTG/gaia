// hack to remove this, because otherwise it gets replaced even in the
// python code (swig wtf?) where it is normally a reserved keyword
#undef range

%pythoncode {

import sys
import os.path

__version__ = _gaia2.cvar.version

#### ParameterMap and Factories adjustments ###################################

/* shortcut for easily creating ParameterMap given a python map */
def pmap(param_map):
    if isinstance(param_map, ParameterMap):
        return param_map

    if not isinstance(param_map, dict):
        raise TypeError('Argument is not a map, it is a %s' % type(param_map))

    pm = ParameterMap()
    for key, value in param_map.items():
        if not isinstance(key, basestring):
            raise TypeError('A key in the map is not a string; can not convert it')

        /* until python 3, everything should always be utf-8 encoded bytestrings */
        if isinstance(key, unicode):
            key = key.encode('utf-8')
        if isinstance(value, unicode):
            value = value.encode('utf-8')

        if isinstance(value, (dict, ParameterMap)): # if value is a python map, convert it to ParameterMap
            pm.setParameterMap(key, pmap(value))
        else: # all other types just go inside the map directly
            pm[key] = value

    return pm

ParameterMap.setParameterMap = lambda pm, key, value: _gaia2.ParameterMap_setParameterMap(pm, key, pmap(value))

/* converts a TransfoChain to a python list of maps */
def TransfoChain_toPython(history):
    result = []
    for i in range(history.size()):
        result.append(history[i].toPython())
    return result

TransfoChain.toPython = TransfoChain_toPython


/* useful typedefs */
MetricFactory = DistanceFunctionFactory
InputSpace = ResultSet

/* oh my, now that's some hack: we don't want python users to access Filter* directly
but through the FilterWrapper, so we override (python) the Filter class with the
FilterWrapper one... */
Filter = FilterWrapper

/* make conversion to ParameterMap automatic when calling AnalyzerFactory::create() */
def _AnalyzerFactory_create(name, params = {}):
    result = _gaia2.AnalyzerFactory_create(name, pmap(params))
    try:
        result.__doc__ = _gaia2_docDict['analyzer'][name.lower()]
    except: pass
    return result

AnalyzerFactory.create = staticmethod(_AnalyzerFactory_create)

/* make conversion to ParameterMap automatic when calling MetricFactory::create() */
def _MetricFactory_create(name, layout, params = {}):
    result = _gaia2.DistanceFunctionFactory_create(name, layout, pmap(params))
    try:
        result.__doc__ = _gaia2_docDict['metric'][name.lower()]
    except: pass
    return result

MetricFactory.create = staticmethod(_MetricFactory_create)

def _FrozenDistanceFactory_create(name, layout, params = {}):
    result = _gaia2.FrozenDistanceFactory_create(name, layout._dataset, pmap(params))
    try:
        result.__doc__ = _gaia2_docDict['metric'][name.lower()]
    except: pass
    return result

FrozenDistanceFactory.create = staticmethod(_FrozenDistanceFactory_create)

FrozenMetricFactory = FrozenDistanceFactory

/* complete transformation in just one function call */
def transform(dataset, transfoName, params = {}):
    analyzer = AnalyzerFactory.create(transfoName, params)
    return analyzer.analyze(dataset).applyToDataSet(dataset)


#### Generators classes #######################################################

def DataSet_points(self):
    i = 0
    while i < self.size():
        yield self[i]
        i += 1

DataSet.points = DataSet_points


#### Ownership adjustments ####################################################

def addParentRef(cls, method):
    origMethod = getattr(cls, method)

    def newMethod(*args, **kwargs):
        result = origMethod(*args, **kwargs)
        result._parentRef = args[0] # args[0] == self
        return result

    setattr(cls, method, newMethod)


def addMultiChildRef(cls, method):
    origMethod = getattr(cls, method)

    def newMethod(*args, **kwargs):
        result = origMethod(*args, **kwargs)
        /* args[0] == self */
        try:
            args[0]._childRef += [ (args[1:], kwargs) ]
        except AttributeError:
            args[0]._childRef = [ (args[1:], kwargs) ]

        return result

    setattr(cls, method, newMethod)


def addChildRef(cls, method):
    origMethod = getattr(cls, method)

    def newMethod(*args, **kwargs):
        result = origMethod(*args, **kwargs)
        args[0]._childRef = (args[1:], kwargs)
        return result

    setattr(cls, method, newMethod)


addParentRef(DataSet, 'history')
addParentRef(DataSet, 'point')
addParentRef(DataSet, 'samplePoint')
addChildRef(DataSet, 'setReferenceDataSet')

addParentRef(Point, 'layout')

addChildRef(View, '__init__')


#### API pythonization ########################################################

try:
    all
except NameError:
    def all(l):
        if l == []:
            return True
        return l[0] and all(l[1:])

def autoSetValue(p, name, value):
    if isinstance(value, str) or (isinstance(value, list) and all([ isinstance(item, str) for item in value ])):
        p.setLabel(name, value)
    else:
        p.setValue(name, value)

def autoGetValue(p, name):
    try:
        return p.value(name)
    except:
        try:
            return p.label(name)
        except:
            raise NameError('Descriptor %s doesn\'t exist' % name)

Point.__getitem__ = autoGetValue
Point.__setitem__ = autoSetValue


#### Descriptors pythonization ################################################

/* print methods */
def printDescriptor(x):
    result = []
    i = 0
    while i < x.size():
        result.append(x[i])
        i += 1

    if len(result) == 1:
        result = result[0]

    return repr(result)

/* remove surrounding quotes only if it is the empty string or a single string */
/* (repr for list of strings do not have these surrounding quotes) */
def stripQuotes(x):
    if x.size() < 2:
        return repr(x)[1:-1]
    return repr(x)

RealDescriptor.__repr__ = printDescriptor
StringDescriptor.__repr__ = printDescriptor
StringDescriptor.__str__ = lambda x: stripQuotes(x)

Parameter.__repr__ = lambda x: repr(_gaia2.Parameter_toPython(x))
ParameterMap.__repr__ = lambda x: repr(_gaia2.ParameterMap_toPython(x))
Transformation.__repr__ = lambda x: repr(_gaia2.Transformation_toPython(x))

PointLayout.__str__ = lambda x: x.toYaml()

/* automatic coercion */
RealDescriptor.__float__ = lambda x: x.toSingleValue()

/* real descriptors comparisons */
def _realDescComp(a, b):
    if isinstance(b, list) or isinstance(b, float) or isinstance(b, int):
        return a == RealDescriptor(b)
    return _gaia2.RealDescriptor___eq__(a, b)

RealDescriptor.__eq__ = _realDescComp

/* commutativity of RealDescriptor operators */
RealDescriptor.__radd__ = RealDescriptor.__add__
RealDescriptor.__rsub__ = lambda x, y: -RealDescriptor.__sub__(x, y)

/* string comparison (between StringDescriptor and python string, for instance) */
StringDescriptor.__eq__ = lambda x, y: str(x) == str(y)
StringDescriptor.__neq__ = lambda x, y: str(x) != str(y)
StringDescriptor.__hash__ = lambda x: hash(str(x))


/* enhanced polymorphism for Parameters */

/* should be self.toParameterMap(), but it seems it gets deleted before we even 
reach the end of the line... */
Parameter.__getitem__ = lambda self, x: self.toPython().__getitem__(x)


#### Utilities functions ######################################################


def addImportPath(path):
    '''Function that adds the specified path to the import path. The path can be
    absolute or relative to the calling file.'''
    callingFile = sys._getframe(1).f_globals['__file__']

    importPath = os.path.join(os.path.dirname(callingFile), path)
    sys.path.append(importPath)

def filedir():
    '''Return the directory that the calling file is in.'''
    callingFile = sys._getframe(1).f_globals['__file__']
    return os.path.dirname(callingFile)

def rootdir():
    '''Return the root directory of the Gaia python module.'''
    return os.path.dirname(__file__)


} //# %pythoncode
