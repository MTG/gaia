#include "renameapplier.h"
using namespace gaia2;

RenameApplier::~RenameApplier() {}

RenameApplier::RenameApplier(const Transformation& transfo) : Applier(transfo) {

  _oldName = transfo.params.value("fullName");
  _newName = transfo.params.value("newName");

  QStringList others = selectDescriptors(transfo.layout, UndefinedType,
                                         QStringList() << "*",
                                         QStringList() << _oldName);


  Region desc = transfo.layout.descriptorLocation(_oldName);
  DescriptorType dtype = desc.type();
  DescriptorLengthType ltype = desc.lengthType();

  // Create new layout
  _newLayout = transfo.layout.copy();
  // remove oldName first, to allow oldName = newName
  _newLayout.remove(_oldName);

  if (ltype == FixedLength)
    _newLayout.add(_newName, dtype, FixedLength, desc.dimension());
  else
    _newLayout.add(_newName, dtype, VariableLength, 1);


  // Create mappings
  Region region = _newLayout.descriptorLocation(others);
  createIndexMappings(transfo.layout, _newLayout, region, _realMap, _stringMap, _enumMap);

  // add the mapping for our renamed descriptor
  switch (dtype) {

  case RealType:
    _realMap << qMakePair(_newLayout.descriptorLocation(_newName).segment(),
                          transfo.layout.descriptorLocation(_oldName).segment());
    break;

  case StringType:
    _stringMap << qMakePair(_newLayout.descriptorLocation(_newName).segment(),
                            transfo.layout.descriptorLocation(_oldName).segment());
    break;

  case EnumType:
    _stringMap << qMakePair(_newLayout.descriptorLocation(_newName).segment(),
                            transfo.layout.descriptorLocation(_oldName).segment());
    break;

  default:
    QStringList msg;
    msg << "Could not rename " << _oldName << " to " << _newName << " because " << " desc is not a single descriptor";
    throw GaiaException(msg);
  }

}


Point* RenameApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());

  return gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
}
