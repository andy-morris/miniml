#include "eval.hxx"
#include <cassert>

namespace miniml
{

namespace
{
  using ENV = Ptr<Env<Expr>>;

  inline long INT(Ptr<Expr> e)
  {
    assert(e->type() == ExprType::INT);
    return dyn_cast<IntExpr>(e)->val();
  }

  struct Eval final: public ExprVisitor<Expr, ENV>
  {
    using ExprVisitor<Expr, ENV>::v;

    Ptr<Expr> v(Ptr<IdExpr> x, ENV env) override
    {
      auto e = env->lookup(x->id());
      assert(e);
      return v(e, env);
    }

    Ptr<Expr> v(Ptr<AppExpr> x, ENV env) override
    {
      auto l = v(x->left(),  env);
      auto r = v(x->right(), env);
      if (l->type() == ExprType::LAM) {
        auto ll = dyn_cast<LamExpr>(move(l));
        return v(ll->apply(r), ll->env());
      } else {
        return ptr<AppExpr>(l, r);
      }
    }

    inline Ptr<Expr> v(Ptr<IntExpr> x, ENV) override { return x; }

    inline Ptr<Expr> v(Ptr<BoolExpr> x, ENV) override { return x; }

    Ptr<Expr> v(Ptr<LamExpr> x, ENV env) override
    {
      x->set_env(env);
      return x;
    }

    Ptr<Expr> v(Ptr<BinOpExpr> x, ENV env) override
    {
      auto l = v(x->left(), env),
           r = v(x->right(), env);
      switch (x->op()) {
      case BinOp::PLUS:
        return ptr<IntExpr>(INT(l) + INT(r));
      case BinOp::MINUS:
        return ptr<IntExpr>(INT(l) - INT(r));
      case BinOp::TIMES:
        return ptr<IntExpr>(INT(l) * INT(r));
      case BinOp::DIVIDE:
        return ptr<IntExpr>(INT(l) / INT(r));
      }
    }

    inline Ptr<Expr> v(Ptr<TypeExpr> x, ENV env) override
    { return v(x->expr(), env); }
  };
}

Ptr<Expr> eval(Ptr<Expr> e, Ptr<Env<Expr>> env)
{
  Eval ev; return ev(e, env);
}

}
