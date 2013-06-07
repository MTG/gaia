#ifndef GAIA_STREAMUTIL_H
#define GAIA_STREAMUTIL_H

#include <ostream>
#include <fstream>
#include <vector>

namespace gaia2std {

// printing and debugging utilities
#define PR(x) std::cout << #x << ": " << x << std::endl

/**
 * Provides serialization for all forward containers.
 */
template <typename T, template<typename> class C>
std::ostream& operator<<(std::ostream& out, const C<T>& c) {
  out << '{'; if (!c.empty()) {
    out << *c.begin(); typename C<T>::const_iterator it = c.begin(); ++it;
    for (; it != c.end(); ++it) out << ", " << *it;
  }
  return out << '}';
}

/**
 * Provides a specialization of the serialization method for
 * <code>vector</code>s.
 */
template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
  out << '['; if (!v.empty()) {
    out << *v.begin(); typename std::vector<T>::const_iterator it = v.begin();
    for (++it; it != v.end(); ++it) out << ", " << *it;
  }
  return out << ']';
}

/**
 * Provides serialization for <code>pair</code>s.
 */
template <typename T, typename U>
std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& p) {
  return out << '<' << p.first << ',' << p.second << '>';
}

/**
 * Specialization to avoid ambiguous overload.
 */
inline std::ostream& operator<<(std::ostream& out, const std::string& str) {
  return out << str.c_str();
}



} // namespace gaia2std

#endif // GAIA_STREAMUTIL_H
