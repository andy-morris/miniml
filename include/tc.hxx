#ifndef TC_HXX_HRAUMXNJ
#define TC_HXX_HRAUMXNJ

#include "ast.hxx"
#include "env.hxx"
#include "tc/exception.hxx"

namespace miniml
{

Ptr<Type> type_of(Ptr<Expr> expr, Ptr<Env<Type>> env);

}

#endif /* end of include guard: TC_HXX_HRAUMXNJ */
