#ifndef DUP_HXX_SLXYIH1A
#define DUP_HXX_SLXYIH1A

#include "ptr.hxx"

namespace miniml
{

/// Base class for values that can be duplicated (and need to be at some
/// point).
template <typename T>
struct Dup
{
  /// Duplicate a value.
  virtual Ptr<T> dup() const = 0;
};

}

#endif /* end of include guard: DUP_HXX_SLXYIH1A */
