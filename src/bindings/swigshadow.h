#ifndef GAIA_SWIGSHADOW_H
#define GAIA_SWIGSHADOW_H

// This file contains specialization of functions so that SWIG knows how
// to overload them and doesn't crash when compiling template specializations
// that don't make sense
// @todo obviously, it would be better to tell SWIG to ignore these, but I still
//       haven't found how to do it... :(

namespace gaia2 {

#define NO_IMPL(func) template <> inline func { throw GaiaException("Template function " #func " not implemented."); }

// NB: operator += is also disabled because it has other semantics than for
//     Real, but as they share the same template code, there's no way to get
//     both behaviours

NO_IMPL(StringDescriptor& StringDescriptor::operator+=(const StringDescriptor& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator-=(const StringDescriptor& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator*=(const StringDescriptor& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator/=(const StringDescriptor& rhs));

NO_IMPL(StringDescriptor StringDescriptor::operator+(const StringDescriptor& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator-(const StringDescriptor& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator*(const StringDescriptor& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator/(const StringDescriptor& rhs) const);

NO_IMPL(StringDescriptor StringDescriptor::operator-() const);

NO_IMPL(StringDescriptor& StringDescriptor::operator+=(const QString& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator-=(const QString& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator*=(const QString& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::operator/=(const QString& rhs));

NO_IMPL(StringDescriptor StringDescriptor::operator+(const QString& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator-(const QString& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator*(const QString& rhs) const);
NO_IMPL(StringDescriptor StringDescriptor::operator/(const QString& rhs) const);

NO_IMPL(StringDescriptor& StringDescriptor::minimum(const StringDescriptor& rhs));
NO_IMPL(StringDescriptor& StringDescriptor::maximum(const StringDescriptor& rhs));

NO_IMPL(bool StringDescriptor::isnan() const);
NO_IMPL(bool StringDescriptor::isinf() const);

NO_IMPL(StringDescriptor& StringDescriptor::inverse());
NO_IMPL(StringDescriptor& StringDescriptor::opposite());
NO_IMPL(StringDescriptor& StringDescriptor::sqrt());
NO_IMPL(StringDescriptor& StringDescriptor::abs());

NO_IMPL(QString StringDescriptor::norm() const);

} // namespace gaia2

#endif // GAIA_SWIGSHADOW_H
