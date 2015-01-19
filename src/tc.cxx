#include "tc.hxx"

namespace miniml
{

namespace
{
  struct TypeOf final: public ExprVisitor<Type, Ptr<Env<Type>>>
  {
    using ExprVisitor<Type, Ptr<Env<Type>>>::v;

    virtual Ptr<Type> v(Ptr<IdExpr> id, Ptr<Env<Type>> env) override
    {
      auto ty = env->lookup(id->id());
      return ty? ty: throw NotInScope(id->id());
    }

    virtual Ptr<Type> v(Ptr<IntExpr>, Ptr<Env<Type>>) override
    {
      return ptr<IntType>();
    }

    virtual Ptr<Type> v(Ptr<AppExpr> e, Ptr<Env<Type>> env) override
    {
      auto ty_f0 = v(e->left(), env);
      auto ty_x = v(e->right(), env);

      if (ty_f0->type() == TypeType::ARROW) {
        auto ty_f = std::dynamic_pointer_cast<ArrowType>(ty_f0);
        check_eq(ty_f->left(), ty_x);
        return ty_f->right();
      } else {
        throw NotArrow(ty_f0);
      }
    }

    virtual Ptr<Type> v(Ptr<LamExpr> e, Ptr<Env<Type>> env) override
    {
      auto inner = ptr<Env<Type>>(env);
      inner->insert(e->var(), e->ty());
      auto t = v(e->body(), inner);
      return ptr<ArrowType>(e->ty(), t);
    }

    virtual Ptr<Type> v(Ptr<TypeExpr> e, Ptr<Env<Type>> env) override
    {
      check_eq(v(e->expr(), env), type_nf(e->ty(), env));
      return e->ty();
    }

    virtual Ptr<Type> v(Ptr<BinOpExpr> e, Ptr<Env<Type>> env) override
    {
      switch (e->op()) {
      case BinOp::PLUS:
      case BinOp::MINUS:
      case BinOp::TIMES:
      case BinOp::DIVIDE:
        auto int_ = ptr<IntType>();
        check_eq(v(e->left(), env), int_);
        check_eq(v(e->right(), env), int_);
        return int_;
      }
    }

    void check_eq(Ptr<Type> s, Ptr<Type> t)
    { if (*s != *t) throw Clash(s, t); }
  };
}

Ptr<Type> type_of(Ptr<Expr> expr, Ptr<Env<Type>> env)
{ return TypeOf()(expr, env); }

}
