#ifndef INIT_ENV_HXX_CE0WIC1R
#define INIT_ENV_HXX_CE0WIC1R

#include "ptr.hxx"
#include "env.hxx"
#include "ast.hxx"

namespace miniml
{
struct EnvEntry final
{
  inline EnvEntry(Ptr<Type> t, Ptr<Expr> v): type(t), value(v) {}
  Ptr<Type> type;
  Ptr<Expr> value;

  Ptr<Ppr> ppr()
  { return ppr::hcat({value->ppr(), ": "_p, type->ppr()}); }
};


Ptr<Type> arr(Ptr<Type> l, Ptr<Type> r);

extern Ptr<Type> int_;
extern Ptr<Type> string_;
extern Ptr<Type> bool_;
extern Ptr<Type> unit;

extern Ptr<Expr> UNIT;

long INT(Ptr<Expr> e);
String STRING(Ptr<Expr> e);
Ptr<Expr> STRING(String &&e);

Ptr<EnvEntry> builtin(Ptr<Type> ty, unsigned arity, BuiltinExpr::Effect eff);
Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<Ptr<Expr>(Ptr<Expr>)> f);
Ptr<EnvEntry> builtin_v(Ptr<Type> ty, std::function<void(Ptr<Expr>)> f);
Ptr<EnvEntry> builtin(Ptr<Type> ty,
                      std::function<Ptr<Expr>(Ptr<Expr>,Ptr<Expr>)> f);
Ptr<EnvEntry> builtin(Ptr<Type> ty, std::function<void()> f);

Ptr<Env<EnvEntry>> init_val_env();
}

#endif /* end of include guard: INIT_ENV_HXX_CE0WIC1R */
