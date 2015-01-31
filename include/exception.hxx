#ifndef EXCEPTION_HXX_JN7LIESK
#define EXCEPTION_HXX_JN7LIESK

#include "string.hxx"
#include <stdexcept>

namespace miniml
{

/// Base class for exceptions used inside MiniML.
struct Exception: public std::exception
{
  virtual ~Exception() noexcept {}
};

inline OStream &operator<<(OStream &out, const Exception &e)
{
  return out << e.what();
}

}

#endif /* end of include guard: EXCEPTION_HXX_JN7LIESK */
