/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 *
 * Gaia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#include <QStringList>
#include "descriptortree.h"
#include "gaia.h"
#include "utils.h"
using namespace gaia2;

DescriptorTree::DescriptorTree() : _segment(Segment::undefined), _parent(0),
                                   _modified(true), _hashModified(true) {}

DescriptorTree::DescriptorTree(const QString& name, DescriptorType type,
                               DescriptorLengthType ltype, int size) :
  _segment(name, type, ltype, 0, size), _parent(0),
  _modified(true), _hashModified(true) {

  // if we have a variable-length descriptor, its size has to be 1
  if (ltype == VariableLength && size != 1) {
    throw GaiaException("DescriptorTree: Cannot create a variable-length descriptor and at the same time specify its size");
  }

  // make sure the name doesn't contain more than 1024 chars; if it does there is
  // a bug in libyaml that prevent us from loading a serialized PointLayout
  if (_segment.name.toUtf8().size() > 1023) {
    G_WARNING("WARNING: descriptor name contains more than 1024 chars, truncating it to:");
    _segment.name.resize(1000); // should be enough
    Q_ASSERT(_segment.name.toUtf8().size() < 1024);
    G_WARNING(_segment.name);
  }
}


DescriptorTree::~DescriptorTree() {
  clear();
}

// makes a deep-copy of this tree, with no parent set
DescriptorTree::DescriptorTree(const DescriptorTree& rhs) :
  _segment(rhs._segment), _parent(0) {

  Q_ASSERT(_children.empty());
  foreach (DescriptorTree* child, rhs._children) {
    addChild(new DescriptorTree(*child));
  }

  modify();
}

DescriptorTree* DescriptorTree::copy() const {
  if (_parent) {
    throw GaiaException("Can only copy trees from the root node...");
  }

  return new DescriptorTree(*this);
}


QString DescriptorTree::fullName(bool recurseDown) const {
  if (!_parent) return name();
  // if there's no ambiguity, we want the name of the leaf node
  if (recurseDown && _children.size() == 1) {
    return _children[0]->fullName();
  }
  return _parent->fullName(false) + NAME_SEPARATOR + name();
}


DescriptorType DescriptorTree::type() const {
  if (!isLeaf()) {
    throw GaiaException("Taking the type of a non-terminal node in a DescriptorTree is undefined...");
  }

  return _segment.type;
}

DescriptorLengthType DescriptorTree::lengthType() const {
  if (!isLeaf()) {
    throw GaiaException("Taking the length type of a non-terminal node in a DescriptorTree is undefined...");
  }

  return _segment.ltype;
}


DescriptorTree& DescriptorTree::operator=(const DescriptorTree& rhs) {
  _segment = rhs._segment;
  _parent = 0;
  _children.clear();
  foreach (DescriptorTree* child, rhs._children) {
    _children.append(new DescriptorTree(*child));
  }

  modify();
  return *this;
}

void DescriptorTree::clear() {
  // reset segment
  _segment = Segment::undefined;

  // delete children
  foreach (DescriptorTree* child, _children) {
    child->clear();
    delete child;
  }
  _children.clear();

  modify();
}

const DescriptorTree* DescriptorTree::root() const {
  if (!_parent) return this;
  return _parent->root();
}

void DescriptorTree::modify() {
  _modified = true;
  _hashModified = true;
  if (_parent) root()->modify();
}


bool DescriptorTree::addChild(const QString& name, DescriptorType type,
                              DescriptorLengthType ltype, int size) {
  foreach (DescriptorTree* child, _children) {
    if ((child->name() == name)) {
      return false;
    }
  }
  addChild(new DescriptorTree(name, type, ltype, size));
  return true;
}

void DescriptorTree::detach() {
  if (this->_parent) {
    this->_parent->removeChild(this);
  }
  modify();
}


void DescriptorTree::addChild(DescriptorTree* t) {
  // detach this tree from its (possible) current parent
  t->detach();

  // attach it here
  t->_parent = this;
  _children.append(t);

  modify();
}

void DescriptorTree::removeChild(DescriptorTree* child) {
  if (_children.removeAll(child) == 0) {
    throw GaiaException("Impossible to remove unknown child '", child->name(), "'");
  }

  child->_parent = 0;

  modify();
}

void DescriptorTree::removeNode(const QString& name, bool recurse) {
  if (name.isEmpty()) {
    throw GaiaException("Can not remove root node");
  }

  removeNodes(QStringList() << name, recurse);
}

void DescriptorTree::removeNodes(const QStringList& names, bool recurse) {
  QList<DescriptorTree*> nodes;

  // first find all the nodes, which needs the DescriptorTree to be in a valid state to do so
  foreach (const QString& name, names) {
    nodes << find(name); // TODO: should find a way to use findFromRoot here
  }

  // now we can proceed to remove them one by one
  foreach (DescriptorTree* node, nodes) {
    DescriptorTree* parent = node->_parent;

    node->detach();
    delete node;

    if (recurse) {
      // if node removed was the last child, also remove the parent, so we don't
      // have a leaf which actually wasn't one before (hence was not containing
      // a descriptor)
      while (parent->isLeaf() && (parent != root())) {
        node = parent;
        parent = node->_parent;
        node->detach();
        delete node;
      }
    }
  }

  modify();
}

void DescriptorTree::fixLength(int length) {
  if (!isLeaf()) {
    throw GaiaException("Trying to fix the length of multiple descriptors at the same time");
  }

  _segment.ltype = FixedLength;
  _segment.end = _segment.begin + length;

  // do not forget to call updateLayout from the root of this tree, or from the
  // containing PointLayout!
}

void DescriptorTree::enumerate() {
  if (!isLeaf()) {
    throw GaiaException("Trying to enumerate multiple descriptors at the same time");
  }

  if (_segment.type != StringType) {
    QStringList msg;
    msg << "Trying to enumerate a descriptor (" << fullName() << ") "
        << "which isn't of type String, but of type " << typeToString(_segment.type);
    throw GaiaException(msg);
  }

  _segment.type = EnumType;

  // do not forget to call updateLayout from the root of this tree, or from the
  // containing PointLayout!
}


QString DescriptorTree::descriptorName(DescriptorType type, DescriptorLengthType ltype, int index) const {
  foreach (const DescriptorTree* leaf, leaves()) {
    if ((type  == leaf->_segment.type) &&
        (ltype == leaf->_segment.ltype) &&
        (index >= leaf->_segment.begin && index < leaf->_segment.end)) {
      return leaf->fullName();
    }
  }

  throw GaiaException("INTERNAL ERROR @ DescriptorTree::descriptorName");
}

QStringList DescriptorTree::descriptorNames() const {
  QStringList result;

  foreach (const DescriptorTree* leaf, leaves()) {
    result << leaf->fullName();
  }

  // if we only have the root node here (eg: tree empty), do not return it but
  // rather an empty list then
  if ((result.size() == 1) && (result[0] == "")) {
    return QStringList();
  }

  return result;
}



Region DescriptorTree::correspondingRegion() const {
  G_DEBUG(GDescriptorTree, "DescriptorTree::correspondingRegion()");
  Region result;
  result.name = this->name();

  // if we're a leaf, the region is defined by the one enclosing the segment
  if (isLeaf()) {
    result.segments += this->_segment;
    return result;
  }

  // otherwise, merge children's regions together and return that
  foreach (DescriptorTree* child, _children) {
    result.merge(child->correspondingRegion());
  }
  result.canonical();

  return result;
}


/**
 * @todo surely there is a smarter way to implement this...
 */
DescriptorTree* DescriptorTree::findSimple(const QString& name) {
  foreach (DescriptorTree* node, nodes()) {
    if (node->name() == name) {
      return node;
    }
  }

  throw GaiaException("Couldn't find node with name '", name, "'");
}


const DescriptorTree* DescriptorTree::findLeaf(const QString& name) const {
  QStringList sname = splitName(name);

  foreach (const DescriptorTree* node, nodes()) {
    const DescriptorTree* branch = node->find(sname, true);
    if (branch) {
      return branch;
    }
  }

  throw GaiaException("Couldn't find node with name '", name, "'");
}


const DescriptorTree* DescriptorTree::find(const QStringList& name,
                                           bool onlyLeaves, int i) const {

  if (i >= name.size()) return 0;

  // if we don't have the same name beginning as the current node, we are
  // already in a false branch, return false
  if (this->name() != name[i]) return 0;

  // we win if the name is a single one...
  if (name.size() == i+1) {
    // ...except if we asked only for leaves and got an inner node!
    if (onlyLeaves && !isLeaf()) {
      return 0;
    }
    return this;
  }

  // otherwise, just recurse down the possible paths with the shorter name
  foreach (DescriptorTree* child, _children) {
    const DescriptorTree* branch = child->find(name, onlyLeaves, i+1);
    if (branch) return branch;
  }

  // we found none, return false
  return 0;
}


const DescriptorTree* DescriptorTree::find(const QString& name) const {
  // special case: the root node is the only one to be unnamed
  if (name == "") return root();

  QStringList sname = splitName(name);

  foreach (const DescriptorTree* node, nodes()) {
    const DescriptorTree* result = node->find(sname, false);
    if (result) {
      return result;
    }
  }

  throw GaiaException("Couldn't find node with name '", name, "'");
}

const DescriptorTree* DescriptorTree::findFromRoot(const QString& name) const {
  // special case: the root node is the only one to be unnamed
  if (name == "") return root();

  QStringList sname = splitName(name, true);

  const DescriptorTree* result = root()->find(sname, false);
  if (result) {
    return result;
  }

  throw GaiaException("Couldn't find node with name '", name, "'");
}


QList<const DescriptorTree*> DescriptorTree::dfsNoCache() const {
  QList<const DescriptorTree*> result;

  // add current node
  result += this;

  // add all children nodes
  foreach (const DescriptorTree* child, _children) {
    result += child->dfsNoCache();
  }

  return result;
}


template <typename T>
QList<T*> unconstifyList(const QList<const T*>& clist) {
  QList<T*> result;
  foreach (const T* ptr, clist) {
    result << const_cast<T*>(ptr);
  }
  return result;
}

inline bool cmpNodes(const DescriptorTree* n1, const DescriptorTree* n2) {
  return n1->name() < n2->name();
}

void DescriptorTree::reorder() const {
  DescriptorTree* node = const_cast<DescriptorTree*>(this);
  sort(node->_children.begin(), node->_children.end(), cmpNodes);

  foreach (DescriptorTree* child, node->_children) {
    child->reorder();
  }
}

void DescriptorTree::recomputeCaches() const {
  // 1- reeorder tree so that it is in alphanumerical order
  //    this prevents bugs where layouts constructed in a different order
  //    whould appear different when they're not
  reorder();

  // 2- compute list of all nodes
  _cache.dfsConst = this->dfsNoCache();
  _cache.dfs = unconstifyList(_cache.dfsConst);

  // 3- compute list of all leaves
  _cache.leavesConst.clear();
  foreach (const DescriptorTree* node, _cache.dfsConst) {
    if (node->isLeaf()) _cache.leavesConst << node;
  }

  _cache.leaves = unconstifyList(_cache.leavesConst);

  // 4- compute sizes of each region type
  _cache.realSize   = regionSizeNoCache(RealType,   VariableLength);
  _cache.stringSize = regionSizeNoCache(StringType, VariableLength);
  _cache.enumSize   = regionSizeNoCache(EnumType,   VariableLength);
  _cache.realSizeFixed   = regionSizeNoCache(RealType,   FixedLength);
  _cache.stringSizeFixed = regionSizeNoCache(StringType, FixedLength);
  _cache.enumSizeFixed   = regionSizeNoCache(EnumType,   FixedLength);

  // X- done!!
  const_cast<DescriptorTree*>(this)->_modified = false;
}

void DescriptorTree::recomputeHash() const {
  _cache.hash = qHash(toYaml());
  const_cast<DescriptorTree*>(this)->_hashModified = false;
}


// WARNING: do NOT call nodes(), leaves(), etc... but access the cached
//          values directly to avoid infinite recursion
int DescriptorTree::regionSizeNoCache(DescriptorType type, DescriptorLengthType ltype) const {
  int size = 0;
  foreach (const DescriptorTree* node, _cache.dfsConst) {
    if (node->isLeaf() && node->type() == type && node->lengthType() == ltype) {
      size = qMax(size, node->_segment.end);
    }
  }
  return size;
}



uint DescriptorTree::hash() const {
  if (_hashModified) recomputeHash();
  return _cache.hash;
}

QList<DescriptorTree*> DescriptorTree::nodes() {
  Q_ASSERT(root() == this); // can only call caching functions on root node
  if (_modified) recomputeCaches();
  return _cache.dfs;
}

QList<const DescriptorTree*> DescriptorTree::nodes() const {
  Q_ASSERT(root() == this); // can only call caching functions on root node
  if (_modified) recomputeCaches();
  return _cache.dfsConst;
}

QList<DescriptorTree*> DescriptorTree::leaves() {
  Q_ASSERT(root() == this); // can only call caching functions on root node
  if (_modified) recomputeCaches();
  return _cache.leaves;
}

QList<const DescriptorTree*> DescriptorTree::leaves() const {
  Q_ASSERT(root() == this); // can only call caching functions on root node
  if (_modified) recomputeCaches();
  return _cache.leavesConst;
}

int DescriptorTree::regionSize(DescriptorType type, DescriptorLengthType ltype) const {
  Q_ASSERT(root() == this); // can only call caching functions on root node
  if (_modified) recomputeCaches();
  switch (type) {
  case RealType:   return (ltype == VariableLength) ? _cache.realSize   : _cache.realSizeFixed;
  case StringType: return (ltype == VariableLength) ? _cache.stringSize : _cache.stringSizeFixed;
  case EnumType:   return (ltype == VariableLength) ? _cache.enumSize   : _cache.enumSizeFixed;
  default:
    throw GaiaException("DescriptorTree::regionSize unknown descriptor type...");
  }
}


void DescriptorTree::updateSegments(const QList<DescriptorTree*>& lrts) const {
  QList<QPair<QString, DescriptorTree*> > lrt;
  foreach (DescriptorTree* rt, lrts) {
    lrt << qMakePair(rt->fullName(), rt);
  }
  sort(range(lrt));

  int fixIdx = 0, varIdx = 0;
  QPair<QString, DescriptorTree*> rtp;
  foreach (rtp, lrt) {
    DescriptorTree* rt = rtp.second;
    int size = rt->_segment.end - rt->_segment.begin;
    switch (rt->_segment.ltype) {
    case VariableLength:
      rt->_segment.begin = varIdx;
      rt->_segment.end = varIdx + size;
      varIdx += size;
      break;
    case FixedLength:
      rt->_segment.begin = fixIdx;
      rt->_segment.end = fixIdx + size;
      fixIdx += size;
      break;
    }
  }
}

void DescriptorTree::updateLayout() {
  // if we're an empty tree, don't do nothing, because the only node, which is the
  // root, is also a leaf. And this is the only case where a leaf that has an undefined
  // type is allowed, so just let it slip through
  if (isLeaf()) return;

  QList<DescriptorTree*> floatSegments, stringSegments, enumSegments;

  // find all leaves (which correspond to actual descriptors) and classify them
  // wrt their type
  foreach (DescriptorTree* leaf, leaves()) {
    switch (leaf->type()) {
    case RealType:   floatSegments.append(leaf);  break;
    case StringType: stringSegments.append(leaf); break;
    case EnumType:   enumSegments.append(leaf);  break;
    default:
      throw GaiaException("Unknown segment type when parsing DescriptorTree, node name = ",
                          leaf->fullName());
    }
  }

  // now we've got our segments classified, sort them & update their layout
  updateSegments(floatSegments);
  updateSegments(stringSegments);
  updateSegments(enumSegments);

  modify();

  // make various checks to make sure we're in good shape
  checkConsistency();
}

void DescriptorTree::printLeafSegments() const {
  G_DEBUG(GDescriptorTree, "PRINTING LAYOUT:");
  foreach (const DescriptorTree* leaf, leaves()) {
    G_DEBUG(GDescriptorTree, leaf->fullName() << '[' << leaf->_segment.begin << ',' << leaf->_segment.end << ']');
  }
}

void DescriptorTree::checkConsistency() const {
  // 1- check that each leaf node of VariableLength type has a segment of size exactly 1
  foreach (const DescriptorTree* leaf, leaves()) {
    if (leaf->_segment.ltype == VariableLength && leaf->_segment.size() != 1) {
      throw GaiaException(QStringList() << "Leaf with fullname '" << leaf->name()
                          << "' has a VL segment of size " << QString::number(leaf->_segment.size())
                          << " (should be 1)");
    }
  }

  // 2- check that each leaf node has a defined type
  foreach (const DescriptorTree* leaf, leaves()) {
    if (leaf->type() == UndefinedType) {
      throw GaiaException(QStringList() << "Leaf with fullname '" << leaf->name()
                          << "' is of type Undefined");
    }
  }
}

bool DescriptorTree::operator==(const DescriptorTree& rhs) const {
  G_DEBUG(GDescriptorTree, "comparing region trees");
  return hash() == rhs.hash();
}

bool DescriptorTree::subtreeOf(const DescriptorTree* other) const {
  GaiaMap<QString, const DescriptorTree*> leaves1, leaves2;
  foreach (const DescriptorTree* leaf, this->leaves()) {
    leaves1.insert(leaf->fullName(), leaf);
  }
  foreach (const DescriptorTree* leaf, other->leaves()) {
    leaves2.insert(leaf->fullName(), leaf);
  }

  try {
    foreach (const QString& name, leaves1.keys()) {
      if (!(leaves1[name]->_segment.name == leaves2[name]->_segment.name &&
            leaves1[name]->_segment.type == leaves2[name]->_segment.type &&
            leaves1[name]->_segment.ltype == leaves2[name]->_segment.ltype)) {
        G_DEBUG(GDescriptorTree, "subtreeOf returns false because of" << name);
        return false;
      }
    }
  }
  catch (GaiaException& e) {
    G_DEBUG(GDescriptorTree, "caught exception in subtree:" << e.what());
    return false;
  }

  return true;
}


QString DescriptorTree::toYaml(int spaces) const {
  QStringList result;

  if (this == root()) {
    result << "descriptors:";
  }
  else {
    QStringList nodeName;
    nodeName << QString(spaces, ' ') << name();
    if (isLeaf()) {
      nodeName << ": " << _segment.toString();
    }
    else {
      nodeName << ":";
    }

    result << nodeName.join("");
  }

  foreach (const DescriptorTree* child, _children) {
    result << child->toYaml(spaces + 2);
  }

  return result.join("\n");
}

/////////// Serialization functions /////////////////////////////

QDataStream& gaia2::operator<<(QDataStream& out, const DescriptorTree& tree) {
  out << tree._segment;
  out << (quint32)tree._children.size();
  foreach (DescriptorTree* child, tree._children) {
    out << *child;
  }
  return out;
}

QDataStream& gaia2::operator>>(QDataStream& in, DescriptorTree& tree) {
  // first deallocate current object
  tree.clear();

  // then load data from stream
  in >> tree._segment;

  quint32 size;
  in >> size;

  for (uint i=0; i<size; i++) {
    tree.addChild(new DescriptorTree);
    in >> *tree._children.back();
  }

  tree.modify();

  return in;
}
