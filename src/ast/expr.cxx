#include "ast/expr.hxx"
#include <sstream>

namespace miniml
{

using namespace ppr;
using std::unordered_set;

Ptr<Ppr> IdExpr::ppr(unsigned, bool pos) const
{
  auto p = id().ppr();
  return pos ? hcat({p, Span{start(), end()}.ppr()}) : p;
}


Ptr<Ppr> IntExpr::ppr(unsigned, bool pos) const
{
  auto p = num(val());
  return pos ? hcat({p, Span{start(), end()}.ppr()}) : p;
}


Ptr<Ppr> AppExpr::ppr(unsigned prec, bool pos) const
{
  auto p = parens_if(prec > 10 || pos,
                     hcat({left()->ppr(10, pos), +right()->ppr(11, pos)}));
  return pos? hcat({p, Span{start(), end()}.ppr()}) : p;
}


Ptr<Ppr> LamExpr::ppr(unsigned prec, bool pos) const
{
  auto p =
      parens_if(prec > 0 || pos,
                vcat({hcat({"fn ("_p, var().ppr(pos), ':'_p,
                            +ty()->ppr(pos), ')'_p, +"=>"_p}),
                      (body()->ppr(pos) >> 1)}));
  return pos ? hcat({p, Span{start(), end()}.ppr()}) : p;
}


Ptr<Ppr> TypeExpr::ppr(unsigned prec, bool pos) const
{
  auto p = parens_if(prec > 0 || pos,
      hcat({expr()->ppr(pos), ':'_p, +ty()->ppr(pos)}));
  return pos? hcat({p, Span{start(), end()}.ppr()}) : p;
}

Ptr<Ppr> BinOpExpr::ppr(unsigned pr, bool pos) const
{
  unsigned oprec = prec(op());
  unsigned lprec = assoc(op()) == OpAssoc::LEFT?  oprec : oprec + 1;
  unsigned rprec = assoc(op()) == OpAssoc::RIGHT? oprec : oprec + 1;
  auto p = parens_if(pr > oprec || pos,
                     hcat({left()->ppr(lprec, pos),
                           +name(op()),
                           +right()->ppr(rprec, pos)}));
  return pos? hcat({p, Span{start(), end()}.ppr()}) : p;
}


namespace
{
  /// Free variables
  struct FV: public ExprVisitor<unordered_set<Id>>
  {
    using ExprVisitor<unordered_set<Id>>::v;

    typedef Ptr<unordered_set<Id>> Ret;

    Ret v(Ptr<IdExpr> i) override { return single(i->id()); }
    Ret v(Ptr<IntExpr>) override { return Ret(); }

    Ret v(Ptr<AppExpr> e) override
    {
      auto s1 = v(e->left()), s2 = v(e->right());
      s1->insert(s2->begin(), s2->end());
      return s1;
    }

    Ret v(Ptr<LamExpr> e) override
    {
      auto s = v(e->body());
      s->erase(e->var());
      return s;
    }

    Ret v(Ptr<TypeExpr> e) override
    {
      return v(e->expr());
    }

    Ret v(Ptr<BinOpExpr> e) override
    {
      auto s1 = v(e->left()), s2 = v(e->right());
      s1->insert(s2->begin(), s2->end());
      return s1;
    }

    static Ret single(const Id x)
    {
      auto set = ptr<unordered_set<Id>>();
      set->insert(x);
      return set;
    }
  };

  struct Subst:
    public ExprVisitor<Expr, const Id, Ptr<Expr>, Ptr<unordered_set<Id>>>
  {
    using ExprVisitor<Expr, const Id, Ptr<Expr>, Ptr<unordered_set<Id>>>::v;

    Ptr<Expr> v(Ptr<IdExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return e->id() == x? arg : e;
    }

    Ptr<Expr> v(Ptr<AppExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return ptr<AppExpr>(v(e->left(), x, arg, fv),
                          v(e->right(), x, arg, fv));
    }

    Ptr<Expr> v(Ptr<IntExpr> e, const Id, Ptr<Expr>, FV::Ret) override
    { return e; }

    Ptr<Expr> v(Ptr<LamExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      // capture-avoiding substitution (λx.e)[y:=e']:
      //   - if x = y, do nothing
      //   - else
      //     - if x ∈ FV(e'), use (λx'.e[x:=x']) instead for fresh x'
      //     - subst body as normal

      if (x == e->var()) return e;

      // id = x, id2 = x'
      auto id = e->var(), id2 = e->var();
      if (fv->find(x) != fv->end()) {
        // find fresh variable
        unsigned i = 0;
        while (fv->find(id) != fv->end())
          id = id.suffix(i++);
      }

      auto body =
        id == id2?
          e->body()
        : v(e->body(), id, ptr<IdExpr>(id2), FV::single(id2));

      return ptr<LamExpr>(id2, e->ty(), v(body, x, arg, fv));
    }

    Ptr<Expr> v(Ptr<TypeExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv) override
    {
      return ptr<TypeExpr>(v(e->expr(), x, arg, fv), e->ty());
    }

    Ptr<Expr> v(Ptr<BinOpExpr> e, const Id x, Ptr<Expr> arg, FV::Ret fv)
      override
    {
      return ptr<BinOpExpr>(e->op(),
                            v(e->left(), x, arg, fv),
                            v(e->right(), x, arg, fv));
    }
  };
}

Ptr<unordered_set<Id>> fv(const Ptr<Expr> expr)
{ return FV()(expr); }

Ptr<Expr> Expr::subst(const Id var, const Ptr<Expr> expr)
{
  return Subst()(dup(), var, expr, fv(expr));
}

Ptr<Expr> LamExpr::apply(const Ptr<Expr> arg) const
{
  auto fv = FV()(arg);
  return Subst()(body(), var(), arg, fv);
}

}
