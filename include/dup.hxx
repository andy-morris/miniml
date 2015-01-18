#ifndef DUP_HXX_SLXYIH1A
#define DUP_HXX_SLXYIH1A

#include "ptr.hxx"

namespace miniml
{

template <typename T>
struct Dup
{
  virtual Ptr<T> dup() const = 0;
};

}

#endif /* end of include guard: DUP_HXX_SLXYIH1A */
