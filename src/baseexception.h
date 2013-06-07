#ifndef GAIA_BASEEXCEPTION_H
#define GAIA_BASEEXCEPTION_H

#include <exception>
#include <string>

namespace gaia2std {

class GException : public std::exception {
 protected:
  std::string _message;

 public:
  GException() : exception() {}
  GException(const char* msg) : exception(), _message(msg) {}
  GException(const std::string& msg) : exception(), _message(msg) {}

  virtual ~GException() throw() {}
  virtual const char* what() const throw() { return _message.c_str(); }

  const std::string& message() const throw() { return _message; }
};

} // namespace gaia2std

#endif // GAIA_BASEEXCEPTION_H
