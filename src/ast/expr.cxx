#include "ast/expr.hxx"
#include <sstream>

namespace miniml
{

using namespace ppr;

Ptr<Ppr> IdExpr::ppr(unsigned, bool pos) const
{
  auto p = id()->ppr();
  return pos? p * Span{start(), end()}.ppr() : p;
}


Ptr<Ppr> IntExpr::ppr(unsigned, bool pos) const
{
  auto p = num(val());
  return pos? p * Span{start(), end()}.ppr() : p;
}


Ptr<Ppr> AppExpr::ppr(unsigned prec, bool pos) const
{
  auto p = parens_if(prec > 10 || pos,
      left()->ppr(10, pos) * +right()->ppr(11, pos));
  return pos? p * Span{start(), end()}.ppr() : p;
}


Ptr<Ppr> LamExpr::ppr(unsigned prec, bool pos) const
{
  auto p = parens_if(prec > 0 || pos,
      "fn ("_p * var()->ppr(pos) * ':'_p * +ty()->ppr(pos) * ')'_p *
        +"=>"_p +
        (body()->ppr(pos) >> 1));
  return pos? p * Span{start(), end()}.ppr() : p;
}


Ptr<Ppr> TypeExpr::ppr(unsigned prec, bool pos) const
{
  auto p = parens_if(prec > 0 || pos,
      expr()->ppr(pos) * ":"_p * +ty()->ppr(pos));
  return pos? p * Span{start(), end()}.ppr() : p;
}

}
