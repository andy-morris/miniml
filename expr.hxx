#ifndef EXPR_HXX_SPN8H6I7
#define EXPR_HXX_SPN8H6I7

#include "ppr.hxx"
#include "id.hxx"
#include "ptr.hxx"

namespace miniml
{

enum class ExprType
{
  ID,
  APP,
  LAM,
};

class Expr
{
public:
  virtual ~Expr() {}

  virtual ExprType type() = 0;
};


class IdExpr final: public Expr
{
public:
  IdExpr(const Ptr<Id> i): m_id(i) {}

  const Ptr<Id> id() const { return m_id; }

  ExprType type() { return ExprType::ID; }

private:
  const Ptr<Id> m_id;
};


class AppExpr final: public Expr
{
public:
  AppExpr(const Ptr<Expr> left, const Ptr<Expr> right):
    m_left(left), m_right(right)
  {}

  ExprType type() { return ExprType::APP; }

private:
  const Ptr<Expr> m_left, m_right;
};


class LamExpr final: public Expr
{
public:
  LamExpr(const Ptr<Id> var, const Ptr<Expr> body):
    m_var(var), m_body(body)
  {}

  ExprType type() { return ExprType::LAM; }

private:
  const Ptr<Id> m_var;
  const Ptr<Expr> m_body;
};

}

#endif /* end of include guard: EXPR_HXX_SPN8H6I7 */
