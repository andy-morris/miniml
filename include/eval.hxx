#ifndef EVAL_HXX_F63P7CXN
#define EVAL_HXX_F63P7CXN

#include "ast.hxx"

namespace miniml
{

/// Evaluate an expression with the given value environment.
Ptr<Expr> eval(Ptr<Expr>, Ptr<Env<Expr>>);

}

#endif /* end of include guard: EVAL_HXX_F63P7CXN */
