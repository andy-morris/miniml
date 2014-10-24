#include "expr.hxx"

namespace miniml
{

namespace
{
  Ptr<Ppr> parens_if(bool b, Ptr<Ppr> ppr)
  {
    if (b)
      return L'('_p * ppr * L')'_p;
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
                   L"fn"_p * +var()->ppr() * +L"->"_p +
                     (body()->ppr(0) >> 1));
}

}
