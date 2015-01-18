#ifndef EXCEPTION_HXX_HZM9ECFD
#define EXCEPTION_HXX_HZM9ECFD

#include "../exception.hxx"

namespace miniml
{

struct TCException: public Exception {};

struct NotInScope: public TCException
{
  NotInScope(const Id id):
    msg("not in scope: " + *id.val())
  {}
  virtual const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

struct Clash: public TCException
{
  Clash(const Ptr<Type> expected, const Ptr<Type> actual)
  {
    auto ppr =
      "type clash: expected type"_p +
      (expected->ppr() >> 1) +
      "but got type"_p +
      (actual->ppr() >> 1);
    msg = *ppr->string();
  }
  virtual const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

struct NotArrow: public TCException
{
  NotArrow(const Ptr<Type> t)
  {
    auto ppr = "not an arrow type:"_p + (t->ppr() >> 1);
    msg = *ppr->string();
  }
  virtual const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

}

#endif /* end of include guard: EXCEPTION_HXX_HZM9ECFD */
