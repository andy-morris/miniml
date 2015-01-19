#ifndef DECL_HXX_TP5RLJSC
#define DECL_HXX_TP5RLJSC

#include "id.hxx"
#include "ast/expr.hxx"
#include "ast/type.hxx"
#include "ppr.hxx"
#include "pos.hxx"
#include "dup.hxx"

namespace miniml
{

enum class DeclType
{
  VAL,
};

class Decl: public Pretty, public HasPos, public Dup<Decl>
{
public:
  virtual ~Decl() {}
  /// Get the type of declaration this is.
  virtual DeclType type() const = 0;

protected:
  inline Decl(Pos start = Pos(), Pos end = Pos()): HasPos(start, end) {}
};

class ValDecl final: public Decl
{
public:
  ValDecl(Id name, Ptr<Expr> def, Pos start = Pos(), Pos end = Pos()):
    Decl(start, end), m_name(name), m_def(def)
  {}

  inline DeclType type() const { return DeclType::VAL; }

  Ptr<Ppr> ppr(unsigned=0, bool pos = false) const;

  inline Ptr<Decl> dup() const
  { return ptr<ValDecl>(name(), def(), start(), end()); }

  inline Id name() const { return m_name; }
  inline Ptr<Expr> def() const { return m_def; }

private:
  Id m_name;
  Ptr<Expr> m_def;
};

}

#endif /* end of include guard: DECL_HXX_TP5RLJSC */
