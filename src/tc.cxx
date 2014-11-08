#include "tc.hxx"

namespace miniml
{

namespace
{
  struct TypeOf final: public ExprVisitor<Type, Ptr<Env<Type>>>
  {
    using ExprVisitor<Type, Ptr<Env<Type>>>::operator();

    virtual Ptr<Type> operator()(Ptr<IdExpr> id, Ptr<Env<Type>> env) override
    {
      auto ty = env->lookup(*id->id());
      return ty? ty: throw NotInScope(id->id());
    }

    virtual Ptr<Type> operator()(Ptr<IntExpr>, Ptr<Env<Type>>) override
    {
      return ptr<IntType>();
    }

    virtual Ptr<Type> operator()(Ptr<AppExpr> e, Ptr<Env<Type>> env) override
    {
      auto ty_f0 = this->operator()(e->left(), env);
      auto ty_x = this->operator()(e->right(), env);

      if (ty_f0->type() == TypeType::ARROW) {
        auto ty_f = std::dynamic_pointer_cast<ArrowType>(ty_f0);
        check_eq(ty_f->left(), ty_x);
        return ty_f->right();
      } else {
        throw NotArrow(ty_f0);
      }
    }

    virtual Ptr<Type> operator()(Ptr<LamExpr> e, Ptr<Env<Type>> env) override
    {
      auto inner = ptr<Env<Type>>(env);
      inner->insert(*e->var(), e->ty());
      auto t = this->operator()(e->body(), inner);
      return ptr<ArrowType>(e->ty(), t);
    }

    virtual Ptr<Type> operator()(Ptr<TypeExpr> e, Ptr<Env<Type>> env) override
    {
      check_eq(this->operator()(e->expr(), env), type_nf(e->ty(), env));
      return e->ty();
    }

    void check_eq(Ptr<Type> s, Ptr<Type> t)
    { if (s != t) throw Clash(s, t); }
  };
}

Ptr<Type> type_of(Ptr<Expr> expr, Ptr<Env<Type>> env)
{ return TypeOf()(expr, env); }

}