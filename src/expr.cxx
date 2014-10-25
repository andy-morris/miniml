#include "expr.hxx"

namespace miniml
{

namespace
{
  Ptr<Ppr> parens_if(bool b, Ptr<Ppr> ppr)
  {
    if (b)
      return '('_p * ppr * ')'_p;
    else
      return ppr;
  }
}


Ptr<Ppr> IdExpr::ppr(unsigned)
{
  return id()->ppr();
}


Ptr<Ppr> AppExpr::ppr(unsigned prec)
{
  return parens_if(prec > 10, left()->ppr() * +right()->ppr());
}


Ptr<Ppr> LamExpr::ppr(unsigned prec)
{
  return parens_if(prec > 0,
                   "fn"_p * +var()->ppr() * +"->"_p +
                     (body()->ppr(0) >> 1));
}

}
