#ifndef PPR_HXX_MJEKHUYS
#define PPR_HXX_MJEKHUYS

#include "string.hxx"
#include "ptr.hxx"

#include <string>
#include <list>
#include <initializer_list>
#include <ostream>
#include <sstream>

namespace miniml
{

class Ppr
{
public:
  virtual ~Ppr() {}

  Ptr<String> string() const;

  static const unsigned default_indent = 4;

  friend OStream &operator<<(OStream&, const Ppr&);

  virtual void output(OStream&, unsigned indent = 0) const = 0;
};


class PprString final: public Ppr
{
public:
  PprString(const String val):
    m_val(val)
  {}

  void output(OStream&, unsigned indent = 0) const override;

private:
  const String m_val;
};

class PprIndent final: public Ppr
{
public:
  PprIndent(Ptr<Ppr> child,
            unsigned indent = Ppr::default_indent):
    m_child(child),
    m_indent(indent)
  {}

  void output(OStream&, unsigned indent = 0) const override;

private:
  Ptr<Ppr> m_child;
  unsigned m_indent;
};

class PprCat: public Ppr
{
protected:
  PprCat(std::initializer_list<Ptr<Ppr>> lst):
    m_children(ptr<List<Ptr<Ppr>>>(lst))
  {}
  virtual ~PprCat() {}

protected:
  Ptr<List<Ptr<Ppr>>> m_children;
};


class PprVCat: public PprCat
{
public:
  PprVCat(std::initializer_list<Ptr<Ppr>> lst):
    PprCat(lst)
  {}

  void output(OStream&, unsigned indent = 0) const override;
};


class PprHCat: public PprCat
{
public:
  PprHCat(std::initializer_list<Ptr<Ppr>> lst):
    PprCat(lst)
  {}

  void output(OStream&, unsigned indent = 0) const override;
};


namespace ppr
{
  Ptr<Ppr> string(const String&);
  Ptr<Ppr> string(const Char*);
  Ptr<Ppr> indent(Ptr<Ppr>, unsigned indent = Ppr::default_indent);
}

Ptr<Ppr> operator>>(Ptr<Ppr>, unsigned);
Ptr<Ppr> operator*(Ptr<Ppr>, Ptr<Ppr>);
Ptr<Ppr> operator+(Ptr<Ppr>, Ptr<Ppr>);
Ptr<Ppr> operator""_p(const Char*, size_t);
Ptr<Ppr> operator""_p(Char);
Ptr<Ppr> operator+(Ptr<Ppr>);


inline OStream &operator<<(OStream &out, const Ppr &p)
{
  p.output(out);
  return out;
}

}

#endif /* end of include guard: PPR_HXX_MJEKHUYS */