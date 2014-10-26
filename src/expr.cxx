#include "expr.hxx"
#include <sstream>

namespace miniml
{

namespace
{
  Ptr<Ppr> parens_if(bool b, const Ptr<Ppr> ppr)
  {
    if (b)
      return '('_p * ppr * ')'_p;
    else
      return ppr;
  }
}


Ptr<Ppr> IdExpr::ppr(unsigned) const
{
  return id()->ppr();
}


Ptr<Ppr> IntExpr::ppr(unsigned) const
{
  std::stringstream str;
  str << val();
  return ptr<PprString>(str.str());
}


Ptr<Ppr> AppExpr::ppr(unsigned prec) const
{
  return parens_if(prec > 10, left()->ppr() * +right()->ppr());
}


Ptr<Ppr> LamExpr::ppr(unsigned prec) const
{
  return parens_if(prec > 0,
                   "fn"_p * +var()->ppr() * +"->"_p +
                     (body()->ppr(0) >> 1));
}

}
