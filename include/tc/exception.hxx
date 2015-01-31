#ifndef EXCEPTION_HXX_HZM9ECFD
#define EXCEPTION_HXX_HZM9ECFD

#include "../ast.hxx"
#include "../exception.hxx"
#include "../ppr.hxx"

namespace miniml
{

/// Exceptions that might occur during type checking.
struct TCException: public Exception
{
  virtual ~TCException() noexcept {}
  inline const char *what() const noexcept override { return msg.c_str(); }
  String msg;
};

/// A definition of an identifier couldn't be found.
struct NotInScope final: public TCException
{
  NotInScope(const Id id)
  {
    msg = "not in scope: " + *id.val();
  }
};

/// Two types should be equal but aren't.
struct Clash final: public TCException
{
  Clash(const Ptr<Type> expected, const Ptr<Type> actual, const Ptr<Expr> expr)
  {
    auto ppr =
      ppr::vcat({"type clash: expected type"_p,
                 expected->ppr() >> 1,
                 "but got type"_p,
                 actual->ppr() >> 1,
                 "in the expression"_p,
                 expr->ppr() >> 1});
    msg = *ppr->string();
  }
};

/// An arrow type `a -> b` was expected but something else was found.
struct NotArrow final: public TCException
{
  NotArrow(const Ptr<Type> t)
  {
    msg = *ppr::vcat({"not an arrow type:"_p, t->ppr() >> 1})->string();
  }
};

/// A projection was either applied to a tuple that was too short or something
/// that wasn't a tuple at all.
struct CannotProject final: public TCException
{
  CannotProject(Ptr<Expr> expr, Ptr<Type> ty, unsigned index)
  {
    msg = *ppr::vcat({ppr::hcat({"cannot project element "_p, ppr::num(index),
                                 "from the expression"_p}),
                      expr->ppr() >> 1,
                      "which has type"_p,
                      ty->ppr() >> 1})->string();
  }
};

/// A recursive value doesn't have a type.
struct UntypedRec final: public TCException
{
  UntypedRec(Ptr<ValDecl> decl)
  {
    msg = *ppr::hcat({"declaration "_p, decl->name().ppr(),
                      " is recursive but has no type annotation"_p})->string();
  }
};

}

#endif /* end of include guard: EXCEPTION_HXX_HZM9ECFD */
