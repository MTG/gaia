#ifndef GAIA_POINT_H
#define GAIA_POINT_H

#include <QStringList>
#include <QTextStream>
#include <vector>
#include "types.h"
#include "pointlayout.h"
#include "descriptor.h"
#include "yamlcpp.h"


namespace gaia2 {


class UnscopedData {
 public:
  RealDescriptor freal;
  Array<RealDescriptor> vreal;
  StringDescriptor flabel;
  Array<StringDescriptor> vlabel;
  EnumDescriptor fenumeration;
  Array<EnumDescriptor> venumeration;

  UnscopedData() : freal(0, Real()), flabel(0, QString()), fenumeration(0, Enum()) {}

  bool operator==(const UnscopedData& rhs) const {
    return (this->vreal == rhs.vreal)
      && (this->vlabel == rhs.vlabel)
      && (this->venumeration == rhs.venumeration)
      && (this->freal == rhs.freal)
      && (this->flabel == rhs.flabel)
      && (this->fenumeration == rhs.fenumeration);
  }
};

class Scope {
 public:
  QString name;
  Real start, end;
  Scope() : name(""), start(0.0), end(0.0) {}

  bool operator==(const Scope& rhs) const {
    return (this->name == rhs.name)
      && (this->start == rhs.start)
      && (this->end == rhs.end);
  }

  friend QDataStream& operator<<(QDataStream& out, const Scope& scope);
  friend QDataStream& operator>>(QDataStream& in, Scope& scope);
};

class ScopedData {
 public:
  Scope scope;
  UnscopedData data;

  bool operator==(const ScopedData& rhs) const {
    return (this->scope == rhs.scope) && (this->data == rhs.data);
  }

  bool operator!=(const ScopedData& rhs) const {
    return !operator==(rhs);
  }
};

QDataStream& operator<<(QDataStream& out, const ScopedData& s);
QDataStream& operator>>(QDataStream& in, ScopedData& s);


// actual point representation is point.scope.name.idx
// (e.g.: p->_data[scope_idx].data.real[name_idx][index_in_desc]
// is it smarter to have it like point.name.scope.idx
// (e.g.: p->_data.real[name_idx][scope_idx][index_in_desc]
// the first one is smarter when we want to be able to easily compute distances
// between two segments, including all descriptors, the second one is better if
// we want to compute distances between 2 descriptors, the scope maybe
// differents (e.g.: compute the distance between 2 beat segmentations...)
//
// first solution also means that we may want to have a layout for each scope
// e.g.: global -> only including bpm, key, etc...
//       segments -> centroid, zcr, etc...

/**
 * @todo write description.
 */
class Point {

 public:

  /**
   * Returns the point name, which also acts as its unique identifier.
   */
  // NOTE: refrain from returning a const ref here, as it creates memory problems
  //       with (at least) swig
  QString name() const { return _name; }

  /**
   * Sets the point name, which also acts as its unique identifier.
   */
  void setName(const QString& name) { _name = name; }

  /**
   * Loads the given filename into this Point. Can specify filters for
   * specifying which data to load.
   */
  void load(const QString& filename,
            const QStringList& select = QStringList() << "*",
            const QStringList& exclude = QStringList());

  /**
   * Loads the given string as this Point's data. Can specify filters for
   * specifying which data to load.
   */
  void loadFromString(const std::string& str,
                      const QStringList& select = QStringList() << "*",
                      const QStringList& exclude = QStringList());


  /**
   * Loads a Point from its base64 representation.
   */
  void fromBase64(const std::string& data);

  /**
   * Loads a Point from its base64 representation.
   */
  void fromBase64(const QByteArray& data);

  /**
   * Returns a base64 representation for this Point.
   */
  std::string toBase64() const;


  Point() : _data(1) {}
  Point(const Point& rhs) : _name(rhs._name), _data(rhs._data), _layout(rhs._layout) {}
  Point(const PointLayout& layout);
  Point(const QString& name, const PointLayout& layout);
  Point(const std::string& name, const PointLayout& layout);

  Point& operator=(const Point& rhs) {
    _name = rhs._name;
    _data = rhs._data;
    _layout = rhs._layout;
    return *this;
  }

  ~Point();

  bool operator==(const Point& rhs) const;

  /**
   * Returns a new point that is composed only of the specified segment.
   * @param point the Point from which to take the segment
   * @param nsegment the number of the segment to be taken
   */
  static Point* fromSingleSegment(const Point* point, int nsegment);

  /**
   * Copy the segment from the given Point to a segment into this Point.
   * This method can be used to build a multi-segment point from its
   * constituent segments given as single points.
   * WARNING: if you call this method on a Point which has fewer segments than what
   *          you require, it will create those segments, but the value of the
   *          data inside those newly created segments will be undefined.
   * @param nsegment the number of the segment to fill
   * @param point the Point from which to extract the data
   * @param origsegment the segment inside the original Point to copy
   */
  void setSegment(int nsegment, const Point* point, int origsegment = 0);

  /**
   * Returns the current layout of the point.
   */
  const PointLayout& layout() const { return _layout; }

  /**
   * Returns the current layout of the point.
   */
  PointLayout& layout() { return _layout; }

  /**
   * Uses the given layout for this point. This method can optionally take
   * the number of segments this point will have. If unspecified, or 0, the
   * number of segments will be left untouched.
   * WARNING: this invalidates all the data in the point.
   */
  void setLayout(const PointLayout& layout, int nsegments = 0);

  /**
   * Switches the point to use the given layout. This only works if the
   * new layout is the same as the one the point had before. This is
   * used to update the enum values in the points.
   */
  void switchLayout(const PointLayout& layout);

  //------- data access methods -------------------------------------
  const Array<ScopedData>& data() const { return _data; }
  Array<ScopedData>& data() { return _data; }

  int numberSegments() const { return _data.size(); }

  RealDescriptor value(const QString& name) const;
  RealDescriptor value(int nsegment, const QString& name) const;

  StringDescriptor label(const QString& name) const;
  StringDescriptor label(int nsegment, const QString& name) const;

  void setValue(const QString& name, const RealDescriptor& value);
  void setValue(int nsegment, const QString& name, const RealDescriptor& value);

  void setLabel(const QString& name, const StringDescriptor& label);
  void setLabel(int nsegment, const QString& name, const StringDescriptor& label);

  const Array<RealDescriptor>& vrealData(int i=0) const { return _data[i].data.vreal; }
  Array<RealDescriptor>& vrealData(int i=0) { return _data[i].data.vreal; }

  const Array<StringDescriptor>& vstringData(int i=0) const { return _data[i].data.vlabel; }
  Array<StringDescriptor>& vstringData(int i=0) { return _data[i].data.vlabel; }

  const Array<EnumDescriptor>& venumData(int i=0) const { return _data[i].data.venumeration; }
  Array<EnumDescriptor>& venumData(int i=0) { return _data[i].data.venumeration; }

  const RealDescriptor& frealData(int i=0) const { return _data[i].data.freal; }
  RealDescriptor& frealData(int i=0) { return _data[i].data.freal; }

  const StringDescriptor& fstringData(int i=0) const { return _data[i].data.flabel; }
  StringDescriptor& fstringData(int i=0) { return _data[i].data.flabel; }

  const EnumDescriptor& fenumData(int i=0) const { return _data[i].data.fenumeration; }
  EnumDescriptor& fenumData(int i=0) { return _data[i].data.fenumeration; }

  const Scope& scope(int i=0) const { checkValidSegment(this, i); return _data[i].scope; }
  Scope& scope(int i=0) { checkValidSegment(this, i); return _data[i].scope; }

 protected:
  QString _name;

  Array<ScopedData> _data;

  // should we have a layout for each different scope?
  // QMap<Scope, PointLayout*> _layouts;
  // e.g.: global -> only including bpm, key, etc...
  //       segments -> centroid, zcr, etc...
  PointLayout _layout;

  /**
   * WARNING: Use this function only if you know 100% what you're doing!
   *          This neither make sure the layout is correctly applied to the data
   *          section of the point, nor maps the enum values...
   */
  void setLayoutUnsafe(const PointLayout& layout) {
    _layout = layout;
  }

  /**
   * WARNING: this doesn't check whether the layout is valid or not, and
   *          doesn't apply the layout either.
   */
  void remapLayoutAndEnums(const PointLayout& layout);

  /**
   * Throws an exception if given segment number is not a valid one.
   */
  static void checkValidSegment(const Point* point, int segment);

  /**
   * Once a layout has been determined, this method resizes the _data member
   * variable so that it can accomodate for all values given the layout schema.
   * This does not set the values to default ones nor reset previous values.
   * @param nsegments the total number of segments in this point, including the
   *                  main one
   */
  void applyLayout(int nsegments);

  void yamlNodeToLayout(const yaml::Node& root, PointLayout& layout,
                        const QString& parentName = "");

  void fillLayout(const yaml::Node& root, const PointLayout& layout,
                  UnscopedData& data, const QString& parentName = "");

  void load(yaml::Node& node,
            const QStringList& select,
            const QStringList& exclude);

  friend class DataSet; // mostly to be able to access setLayoutUnsafe

  friend QDataStream& operator<<(QDataStream& out, const Point& point);
  friend QDataStream& operator>>(QDataStream& in, Point& point);
};


// some useful defines that will be used in lots of places
#define FORSEG(p) for (int nseg=0; nseg<(p).numberSegments(); nseg++)

} // namespace gaia2

#endif // GAIA_POINT_H
