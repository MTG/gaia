#ifndef GAIA_GAIA_H
#define GAIA_GAIA_H

#include "types.h"
#include "gaiaexception.h"

/**
 * @todo turn on undocumented warnings in Doxyfile. (WARN_IF_UNDOCUMENTED   = YES)
 * @todo check for const-correctness the whole library.
 */
class __TODO__ {};

/**
 * Main Gaia namespace, which contains all the library functions.
 */
namespace gaia2 {

/**
 * Init function that makes sure that all the factories have correctly
 * registered all the types they can create. You need to call this function
 * once in your program, as soon as you can.
 */
void init();

/**
 * Shutdown function that mostly frees up resources (statically) allocated for
 * having better performance in gaia.
 */
void shutdown();

/**
 * Flag that controls whether gaia should be silent or not, in general. That
 * could include information such as transformation progress, time needed for
 * doing queries, etc...
 */
extern bool verbose;

extern const char* version;

} // namespace gaia2




template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

/**
 * Returns the size of a statically defined C-style array.
 * Safe version, found here: http://www.viva64.com/en/a/0074/
 * (actually comes from Chromium)
 */
#define ARRAYSIZE(array) (sizeof(ArraySizeHelper(array)))

/**
 * Special define that provide a nice way of describing a range of values.
 */
#define range(c) (c).begin(), (c).end()


// adds serialization of QString into a std::ostream
inline std::ostream& operator<<(std::ostream& out, const QString& str) {
  return out << str.toUtf8().data();
}

#define NAME_SEPARATOR '.'


#endif // GAIA_GAIA_H
