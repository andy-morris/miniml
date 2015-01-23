#ifndef TC_HXX_HRAUMXNJ
#define TC_HXX_HRAUMXNJ

#include "ast.hxx"
#include "env.hxx"
#include "tc/exception.hxx"

namespace miniml
{

Ptr<Type> type_of(Ptr<Expr> expr, Ptr<Env<Type>> env);

/// If the types aren't equal throws a Clash exception with \a expr marked as
/// the problem expression.
void check_eq(Ptr<Type> t, Ptr<Type> u, Ptr<Expr> expr);

}

#endif /* end of include guard: TC_HXX_HRAUMXNJ */
