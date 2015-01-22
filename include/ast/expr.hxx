#ifndef EXPR_HXX_SPN8H6I7
#define EXPR_HXX_SPN8H6I7

#include "pos.hxx"
#include "dup.hxx"
#include "ppr.hxx"
#include "id.hxx"
#include "ptr.hxx"
#include "ast/type.hxx"
#include "visitor.hxx"
#include <unordered_set>
#include <deque>
#include <functional>

namespace miniml
{

/// Type of expression.
enum class ExprType
{
  ID,      ///< Identifier
  APP,     ///< Application
  LAM,     ///< Lambda term
  INT,     ///< Integer literal
  BOOL,    ///< Boolean literal
  TYPE,    ///< Typed expression
  BINOP,   ///< Binary operator expression
  TUPLE,   ///< Tuple
  DOT,     ///< Dot expression (tuple indexing)
  BUILTIN, ///< Builtin expression
};

/// Abstract base class for expressions.
class Expr: public Pretty, public HasPos, public Dup<Expr>
{
public:
  virtual ~Expr() {}
  /// Get the (syntactic) type of the expression.
  /// \see ExprType
  virtual ExprType type() const = 0;

  // Substitutes \a expr for all occurrences of \a var in \a this.
  Ptr<Expr> subst(Id var, const Ptr<Expr> expr);
protected:
  inline Expr(Pos start = Pos(), Pos end = Pos()): HasPos(start, end) {}
};


/// Identifier expression.
class IdExpr final: public Expr
{
public:
  IdExpr(const IdExpr&) = default;
  IdExpr(IdExpr&&) = default;

  /// \param[in] id The identifier this expression represents.
  IdExpr(const Id id, Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_id(id)
  {}

  /// \return ExprType::ID
  inline ExprType type() const override { return ExprType::ID; }

  /// \param prec Ignored, since variables are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The identifier value.
  inline Id id() const { return m_id; }

  inline Ptr<Expr> dup() const override
  { return ptr<IdExpr>(id(), start(), end()); }

private:
  Id m_id;   ///< Identifier value.
};


/// Integer literals.
class IntExpr final: public Expr
{
public:
  IntExpr(const IntExpr&) = default;
  IntExpr(IntExpr&&) = default;

  /// \param[in] val The value of this literal.
  IntExpr(long val, Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_val(val)
  {}

  /// \return ExprType::INT
  inline ExprType type() const override { return ExprType::INT; }

  /// \param prec Ignored, since literals are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The value of this literal.
  inline long val() const { return m_val; }

  inline Ptr<Expr> dup() const override
  { return ptr<IntExpr>(val(), start(), end()); }

private:
  long m_val;
};


/// Boolean literals.
class BoolExpr final: public Expr
{
public:
  BoolExpr(const BoolExpr&) = default;
  BoolExpr(BoolExpr&&) = default;

  /// \param[in] val The value of this literal.
  BoolExpr(bool val, Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_val(val)
  {}

  /// \return ExprType::BOOL
  inline ExprType type() const override { return ExprType::BOOL; }

  /// \param prec Ignored, since literals are always atomic.
  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The value of this literal.
  inline bool val() const { return m_val; }

  inline Ptr<Expr> dup() const override
  { return ptr<BoolExpr>(val(), start(), end()); }

private:
  bool m_val;
};


/// Application expression.
class AppExpr final: public Expr
{
public:
  AppExpr(const AppExpr&) = default;
  AppExpr(AppExpr&&) = default;

  AppExpr(const Ptr<Expr> left, const Ptr<Expr> right,
          Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_left(left), m_right(right)
  {}

  /// \return ExprType::APP
  inline ExprType type() const override { return ExprType::APP; }

  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The left part (operator).
  inline Ptr<Expr> left() const { return m_left; }
  /// \return The right part (operand).
  inline Ptr<Expr> right() const { return m_right; }

  inline Ptr<Expr> dup() const override
  { return ptr<AppExpr>(left()->dup(), right()->dup(), start(), end()); }

private:
  Ptr<Expr> m_left;   ///< Operator.
  Ptr<Expr> m_right;  ///< Operand.
};


/// Lambda term (function) expression.
class LamExpr final: public Expr
{
public:
  LamExpr(const LamExpr&) = default;
  LamExpr(LamExpr&&) = default;

  LamExpr(const Id var, const Ptr<Type> ty, const Ptr<Expr> body,
          Pos start = Pos(), Pos end = Pos(),
          const Ptr<Env<Expr>> env = nullptr):
    Expr(start, end), m_var(var), m_ty(ty), m_body(body), m_env(env)
  {}

  /// \return ExprType::LAM
  inline ExprType type() const override { return ExprType::LAM; }

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The bound variable.
  inline Id var() const { return m_var; }
  /// \return The argument type.
  inline Ptr<Type> ty() const { return m_ty; }
  /// \return The body of the term.
  inline Ptr<Expr> body() const { return m_body; }

  Ptr<Expr> apply(const Ptr<Expr>) const;

  inline Ptr<Env<Expr>> env() { return m_env; }
  inline void set_env(Ptr<Env<Expr>> env) { m_env = env; }

  inline Ptr<Expr> dup() const override
  { return ptr<LamExpr>(var(), ty()->dup(), body()->dup(), start(), end()); }

private:
  Id m_var;           ///< Bound variable.
  Ptr<Type> m_ty;     ///< Argument type.
  Ptr<Expr> m_body;   ///< Body.

  /// Captured environment, or null if we're not evaluating yet.
  Ptr<Env<Expr>> m_env;
};


/// Expressions with type annotations.
class TypeExpr final: public Expr
{
public:
  TypeExpr(const TypeExpr&) = default;
  TypeExpr(TypeExpr&&) = default;

  TypeExpr(const Ptr<Expr> expr, const Ptr<Type> ty,
           Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_expr(expr), m_ty(ty)
  {}

  /// \return ExprType::TYPE
  inline ExprType type() const override { return ExprType::TYPE; }

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  /// \return The inner expression.
  inline Ptr<Expr> expr() const { return m_expr; }
  /// \return The assigned type.
  inline Ptr<Type> ty() const { return m_ty; }

  inline Ptr<Expr> dup() const override
  { return ptr<TypeExpr>(expr()->dup(), ty()->dup(), start(), end()); }

private:
  Ptr<Expr> m_expr; ///< Expression.
  Ptr<Type> m_ty;   ///< Assigned type.
};


class TupleExpr final: public Expr
{
public:
  using Exprs = std::vector<Ptr<Expr>>;

  TupleExpr(const TupleExpr&) = default;
  TupleExpr(TupleExpr&&) = default;

  TupleExpr(Ptr<Exprs> exprs,
            Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_exprs(exprs)
  {}

  TupleExpr(std::initializer_list<Ptr<Expr>> exprs,
            Pos start = Pos(), Pos end = Pos()):
    TupleExpr(ptr<Exprs>(exprs), start, end)
  {}

  inline ExprType type() const override { return ExprType::TUPLE; }

  Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Exprs> exprs() const { return m_exprs; }

  inline Ptr<Expr> dup() const override
  {
    auto es = ptr<Exprs>();
    es->reserve(exprs()->size());
    for (auto e: *exprs()) { es->push_back(e->dup()); }
    return ptr<TupleExpr>(es, start(), end());
  }

private:
  Ptr<Exprs> m_exprs;
};


class DotExpr final: public Expr
{
public:
  DotExpr(const DotExpr&) = default;
  DotExpr(DotExpr&&) = default;

  DotExpr(Ptr<Expr> expr, unsigned index,
              Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_expr(expr), m_index(index)
  {}

  inline ExprType type() const override { return ExprType::DOT; }

  Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override;

  inline Ptr<Expr> expr() const { return m_expr; }
  inline unsigned index() const { return m_index; }

  inline Ptr<Expr> dup() const override
  { return ptr<DotExpr>(expr()->dup(), index(), start(), end()); }

private:
  Ptr<Expr> m_expr;
  unsigned m_index;
};


/// Builtin expressions for side effects.
class BuiltinExpr final: public Expr
{
public:
  using Args = std::deque<Ptr<Expr>>;
  using Effect = std::function<Ptr<Expr>(Args&)>;

  BuiltinExpr(const BuiltinExpr&) = default;
  BuiltinExpr(BuiltinExpr&&) = default;

  BuiltinExpr(Ptr<Type> ty, Effect effect, unsigned arity,
              Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_effect(effect), m_ty(ty), m_arity(arity)
  {
    m_args = ptr<std::deque<Ptr<Expr>>>();
  }

  inline bool need_arg() const { return arity() > args()->size(); }

  void give_arg(Ptr<Expr> arg);

  Ptr<Expr> run();

  inline ExprType type() const override { return ExprType::BUILTIN; }

  Ptr<Expr> dup() const override;

  inline Ptr<Ppr> ppr(unsigned=0, bool pos = false) const override
  { return ppr::pos_if(pos, "<<builtin>>"_p, start(), end()); }

  inline Effect effect() const { return m_effect; }
  inline Ptr<std::deque<Ptr<Expr>>> args() const { return m_args; }
  inline Ptr<Type> ty() const { return m_ty; }
  inline unsigned arity() const { return m_arity; }

private:
  Effect m_effect;
  Ptr<std::deque<Ptr<Expr>>> m_args;
  Ptr<Type> m_ty;
  unsigned m_arity;
};


/// Operators. \see OpExpr
enum class BinOp
{
  PLUS, MINUS, TIMES, DIVIDE,
  LESS, LEQ, EQUAL, GEQ, GREATER, NEQ,
  AND, OR, IFF,
  SEQ,
};

enum class OpAssoc { LEFT, NONE, RIGHT };

inline unsigned prec(BinOp op)
{
  switch (op) {
  case BinOp::TIMES:
  case BinOp::DIVIDE:
    return 7;
  case BinOp::PLUS:
  case BinOp::MINUS:
    return 6;
  case BinOp::LESS:
  case BinOp::LEQ:
  case BinOp::EQUAL:
  case BinOp::GEQ:
  case BinOp::GREATER:
  case BinOp::NEQ:
    return 5;
  case BinOp::AND:
    return 4;
  case BinOp::OR:
    return 3;
  case BinOp::IFF:
    return 2;
  case BinOp::SEQ:
    return 1;
  }
}

inline Ptr<Ppr> name(BinOp op)
{
  switch (op) {
  case BinOp::PLUS:    return '+'_p;
  case BinOp::MINUS:   return '-'_p;
  case BinOp::TIMES:   return '*'_p;
  case BinOp::DIVIDE:  return '/'_p;
  case BinOp::LESS:    return '<'_p;
  case BinOp::LEQ:     return "<="_p;
  case BinOp::EQUAL:   return "=="_p;
  case BinOp::GEQ:     return ">="_p;
  case BinOp::GREATER: return '>'_p;
  case BinOp::NEQ:     return "!="_p;
  case BinOp::AND:     return "&&"_p;
  case BinOp::OR:      return "||"_p;
  case BinOp::IFF:     return "<->"_p;
  case BinOp::SEQ:     return ';'_p;
  }
}

inline OpAssoc assoc(BinOp op)
{
  switch (op) {
  case BinOp::PLUS:
  case BinOp::MINUS:
  case BinOp::TIMES:
  case BinOp::DIVIDE:
    return OpAssoc::LEFT;
  case BinOp::AND:
  case BinOp::OR:
  case BinOp::IFF:
    return OpAssoc::RIGHT;
  default:
    return OpAssoc::NONE;
  }
}

/// Binary operator expressions. \see OpType
class BinOpExpr final: public Expr
{
public:
  BinOpExpr(const BinOpExpr&) = default;
  BinOpExpr(BinOpExpr&&) = default;

  BinOpExpr(BinOp op, Ptr<Expr> left, Ptr<Expr> right,
            Pos start = Pos(), Pos end = Pos()):
    Expr(start, end), m_op(op), m_left(left), m_right(right)
  {}

  inline ExprType type() const override { return ExprType::BINOP; }

  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const override;

  inline Ptr<Expr> dup() const override
  { return ptr<BinOpExpr>(op(), left()->dup(), right()->dup()); }

  inline BinOp op() const { return m_op; }
  inline Ptr<Expr> left() const { return m_left; }
  inline Ptr<Expr> right() const { return m_right; }

private:
  BinOp m_op;
  Ptr<Expr> m_left, m_right;
};


template <typename T, typename... Args>
struct ExprVisitor
{
  inline Ptr<T> operator()(Ptr<Expr> e, Args... args)
  { return v(e, args...); }

  virtual Ptr<T> v(Ptr<Expr> e, Args... args)
  {
    switch (e->type()) {
#define CASE(x,t) \
      case ExprType::x: \
        return v(std::dynamic_pointer_cast<t>(e), args...);
      CASE(ID,      IdExpr)
      CASE(APP,     AppExpr)
      CASE(LAM,     LamExpr)
      CASE(INT,     IntExpr)
      CASE(BOOL,    BoolExpr)
      CASE(TYPE,    TypeExpr)
      CASE(BINOP,   BinOpExpr)
      CASE(TUPLE,   TupleExpr)
      CASE(DOT,     DotExpr)
      CASE(BUILTIN, BuiltinExpr)
#undef CASE
    }
  }

  virtual Ptr<T> v(Ptr<IdExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<AppExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<IntExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<BoolExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<LamExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<TypeExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<BinOpExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<TupleExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<DotExpr>, Args...) = 0;
  virtual Ptr<T> v(Ptr<BuiltinExpr>, Args...) = 0;
};

Ptr<std::unordered_set<Id>> fv(const Ptr<Expr> expr);

}

#endif /* end of include guard: EXPR_HXX_SPN8H6I7 */
