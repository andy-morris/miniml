#ifndef EXCEPTION_HXX_HZM9ECFD
#define EXCEPTION_HXX_HZM9ECFD

#include "../exception.hxx"

namespace miniml
{

struct TCException: public Exception
{
  virtual ~TCException() {}
};

struct NotInScope final: public TCException
{
  NotInScope(const Id id):
    msg("not in scope: " + *id.val())
  {}
  inline const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

struct Clash final: public TCException
{
  Clash(const Ptr<Type> expected, const Ptr<Type> actual)
  {
    auto ppr =
      ppr::vcat({"type clash: expected type"_p,
                 expected->ppr() >> 1,
                 "but got type"_p,
                 actual->ppr() >> 1});
    msg = *ppr->string();
  }
  inline const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

struct NotArrow final: public TCException
{
  NotArrow(const Ptr<Type> t)
  {
    msg = *ppr::vcat({"not an arrow type:"_p, t->ppr() >> 1})->string();
  }
  inline const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

}

#endif /* end of include guard: EXCEPTION_HXX_HZM9ECFD */
