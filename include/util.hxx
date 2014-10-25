#ifndef UTIL_HXX_W8K0XBJR
#define UTIL_HXX_W8K0XBJR

#include "ptr.hxx"
#include <list>

namespace miniml
{

template <typename T>
Ptr<std::list<T>> singleton(T t)
{
  auto lst = ptr<std::list<T>>();
  lst->push_back(t);
  return lst;
}

}

#endif /* end of include guard: UTIL_HXX_W8K0XBJR */
