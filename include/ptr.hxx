#ifndef PTR_HXX_OZMWHDZJ
#define PTR_HXX_OZMWHDZJ

#include <memory>

namespace miniml
{

/// Pointers used by syntax trees, etc.
template <typename T> using Ptr = std::shared_ptr<T>;

/// Allocates an object and makes a \ref Ptr to it.
template <typename T, typename... Args>
Ptr<T> ptr(Args... args)
{
  return std::make_shared<T>(args...);
}

}

#endif /* end of include guard: PTR_HXX_OZMWHDZJ */
