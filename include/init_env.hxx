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


Ptr<Env<EnvEntry>> init_val_env();
}

#endif /* end of include guard: INIT_ENV_HXX_CE0WIC1R */
