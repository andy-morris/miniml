#include "ast/expr.hxx"
#include <sstream>

namespace miniml
{

using namespace ppr;

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
  return parens_if(prec > 10, left()->ppr(10) * +right()->ppr(11));
}


Ptr<Ppr> LamExpr::ppr(unsigned prec) const
{
  return parens_if(prec > 0,
      "fn"_p * +var()->ppr() * +"=>"_p + (body()->ppr(0) >> 1));
}


Ptr<Ppr> TypeExpr::ppr(unsigned prec) const
{
  return parens_if(prec > 0,
      expr()->ppr() * ":"_p * +ty()->ppr());
}

}
