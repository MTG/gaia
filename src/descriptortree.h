#ifndef GAIA_DESCRIPTORTREE_H
#define GAIA_DESCRIPTORTREE_H

#include <QList>
#include <QStringList>
#include "region.h"
#include "gaia.h"

// OMFG is there no other way to do this?
#define NOCONST_CALL(f) const_cast<DescriptorTree*>(const_cast<const DescriptorTree*>(this)->f)

namespace gaia2 {

/**
 * @todo look whether this can be implemented as a radix-tree or a judy array,
 *       or some other well-known data structure (or just decide it is useless)
 */
class DescriptorTree {

 public:
  // constructors
  DescriptorTree();
  DescriptorTree(const QString& name, DescriptorType type,
                 DescriptorLengthType ltype = VariableLength, int size = 1);

  // destructor
  ~DescriptorTree();

  // copy-constructor, assignment operator and equality comparison
 protected:
  DescriptorTree(const DescriptorTree& rhs);
  DescriptorTree& operator=(const DescriptorTree& rhs);

 public:
  bool operator==(const DescriptorTree& rhs) const;

  /**
   * Returns whether this tree is a subset of the given one (e.g.: all the
   * branches and leaves of this tree are also contained in the given one).
   */
  bool subtreeOf(const DescriptorTree* other) const;

  /**
   * Make a deep-copy of this region-tree. If this operation is called on a node
   * which is not the root of the tree in which it belongs, it will throw an
   * exception. You need to call this function if you want to make a copy of a
   * DescriptorTree as the copy-constructor is protected.
   */
  DescriptorTree* copy() const;

  /**
   * Resets this tree to a single root node. Deletes all the children it had
   * before.
   */
  void clear();

  /**
   * Returns the name of the current node.
   */
  QString name() const { return _segment.name; }

  /**
   * Returns the fully-qualified name of this node, eg: using the names of the
   * nodes from the root down to this one.
   * @param recurseDown if this node only leads to a single leaf node and recurseDown
   *        is true, then this method returns the full name of the leaf node;
   *        otherwise it returns the full name of the current node.
   */
  QString fullName(bool recurseDown = true) const;

  /**
   * Returns the type of this node. This method will throw an exception if it
   * is not called on a leaf (terminal node).
   */
  DescriptorType type() const;

  /**
   * Returns the length type of this node. This method will throw an exception
   * if it is not called on a leaf (terminal node).
   */
  DescriptorLengthType lengthType() const;

  const DescriptorTree* parent() const { return _parent; }
  DescriptorTree* parent() { return _parent; }

  /**
   * Returns whether this node is a leaf (terminal node) or not.
   */
  bool isLeaf() const { return _children.empty(); }

  /**
   * Returns the root of the tree which this node pertains to. Const version.
   */
  const DescriptorTree* root() const;

  /**
   * Returns the root of the tree which this node pertains to.
   */
  DescriptorTree* root() { return NOCONST_CALL(root()); }

  /**
   * Add the given DescriptorTree to this node's children. Takes ownership of
   * the child passed as argument.
   */
  void addChild(DescriptorTree* t);

  /**
   * Add a child with specified name, type and lengthType.
   * If ltype = VariableLength, then the size argument has to be 1. If child
   * already existed before return false (and do nothing), else if the new child
   * was succesfully inserted, return true.
   */
  bool addChild(const QString& name, DescriptorType type,
                DescriptorLengthType ltype = VariableLength, int size = 1);

  /**
   * Removes given DescriptorTree from this node's children. This method throws
   * an exception if the given tree was not a child of this node.
   */
  void removeChild(DescriptorTree* child);

  /**
   * Remove the node with the given fully-qualified name. If recurse is true,
   * and the node removed was the last child of its parent, then also remove
   * the parent (and go on recursively until the node removed wasn't the last
   * child).
   */
  void removeNode(const QString& name, bool recurse = true);

  /**
   * Remove the list of nodes with the given fully-qualified name. If recurse is
   * true and a removed node was the last child of its parent, then also remove
   * the parent (and go on recursively until the node removed wasn't the last
   * child).
   */
  void removeNodes(const QStringList& names, bool recurse = true);



  /**
   * Sets this descriptor to be of the specified fixed length. This method
   * will throw an exception if this DescriptorTree does not correspond to a
   * single descriptor (ie: is not a leaf).
   */
  void fixLength(int length);

  /**
   * Sets this descriptor to be an enumeration. This method will throw an
   * exception if this DescriptorTree does not correspond to a single descriptor
   * of if it is not of type StringType.
   */
  void enumerate();

  /**
   * Returns the name of the descriptor which is located at given position.
   */
  QString descriptorName(DescriptorType type, DescriptorLengthType ltype, int index) const;

  /**
   * Returns a list of all the descriptor names contained in this layout. If
   * this layout contains only the root node, the returned list is empty.
   */
  QStringList descriptorNames() const;


  /**
   * Returns a Region formed by the merged segments of all the children of
   * this node.
   */
  Region correspondingRegion() const;


  /**
   * This function takes all the segments of the current DescriptorTree, orders
   * them by type and by alphabetical order, and assign new Segments to each
   * leaf so that they are contiguous.
   */
  void updateLayout();

  /**
   * Find the node in this tree that has the given (short) name.
   */
  DescriptorTree* findSimple(const QString& name);

  /**
   * Find the node with given name (be it a leaf or not).
   * If the string is empty, returns the root node.
   */
  const DescriptorTree* find(const QString& name) const;
  DescriptorTree* find(const QString& name) { return NOCONST_CALL(find(name)); }

  /**
   * Find the node with the given name (be it a leaf or not).
   * The given name must the the full name, anchored at the root node.
   * If the string is empty, returns the root node.
   */
  const DescriptorTree* findFromRoot(const QString& name) const;
  DescriptorTree* findFromRoot(const QString& name) { return NOCONST_CALL(findFromRoot(name)); }


  /**
   * Find the leaf with given name.
   */
  const DescriptorTree* findLeaf(const QString& name) const;
  DescriptorTree* findLeaf(const QString& name) { return NOCONST_CALL(findLeaf(name)); }

  int regionSize(DescriptorType type, DescriptorLengthType ltype) const;

  // intended for debug only
  void printLeafSegments() const;

  /**
   * Returns a yaml string representing the hierachy of descriptors that this
   * tree represents. The root node is called "descriptors" (instead of the
   * empty string).
   */
  QString toYaml(int spaces = 0) const;

  // this checks if the regiontree is in a consistent state. This includes the
  // following criteria up to now:
  // - the segment in each leaf node has to be of size 1;
  // - all leaf nodes should have a type which is not Undefined
  void checkConsistency() const;

  /**
   * Function to be called by each non const function as soon as it modifies
   * the tree, so as to know when to regenerate cached values.
   */
  void modify();


 protected:
  Segment _segment; // should always be of size 1
                      // that is not necessarily true anymore, now that we have fixed length descriptors in the game, or is it?
  DescriptorTree* _parent;
  QList<DescriptorTree*> _children;


  bool _modified;
  bool _hashModified;

  struct CachedValues {
    uint hash;
    QList<DescriptorTree*> dfs;
    QList<const DescriptorTree*> dfsConst;
    QList<DescriptorTree*> leaves;
    QList<const DescriptorTree*> leavesConst;
    int realSize, stringSize, enumSize;
    int realSizeFixed, stringSizeFixed, enumSizeFixed;
  };

  mutable struct CachedValues _cache;

  /**
   * Returns all the nodes of this tree in a list, corresponding to a DFS
   * search.
   */
  QList<DescriptorTree*> nodes();
  QList<DescriptorTree*> leaves();

  QList<const DescriptorTree*> nodes() const;
  QList<const DescriptorTree*> leaves() const;

  QList<const DescriptorTree*> dfsNoCache() const;

  int regionSizeNoCache(DescriptorType type, DescriptorLengthType ltype) const;

  void reorder() const;
  void recomputeCaches() const;
  void recomputeHash() const;

  /**
   * Returns a hash value for this DescriptorTree, which makes comparisons
   * really fast.
   */
  uint hash() const;

  /**
   * detach this tree from its (possible) current parent.
   */
  void detach();

  /**
   * Updates the segments' begin and end indices so that all segments are
   * contiguous and sorted alphabetically.
   */
  void updateSegments(const QList<DescriptorTree*>& lrt) const;

  // find the given branch within this tree, starting at its root. Returns 0 if not found.
  // if onlyLeaves is set to true, only returns those branches that end in a leaf
  const DescriptorTree* find(const QStringList& nodeList, bool onlyLeaves, int i=0) const;

  friend QDataStream& operator<<(QDataStream& out, const DescriptorTree& tree);
  friend QDataStream& operator>>(QDataStream& in, DescriptorTree& tree);
};


} // namespace gaia2


#endif // GAIA_DESCRIPTORTREE_H
