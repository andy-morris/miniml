#ifndef PTR_HXX_OZMWHDZJ
#define PTR_HXX_OZMWHDZJ

#include <memory>

namespace miniml
{

template <typename T> using Ptr = std::shared_ptr<T>;

template <typename T, typename... Args>
Ptr<T> ptr(Args... args)
{
  return std::make_shared<T>(args...);
}

}

#endif /* end of include guard: PTR_HXX_OZMWHDZJ */
