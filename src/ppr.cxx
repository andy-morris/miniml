#include "ppr.hxx"
#include <list>

namespace miniml
{

Ptr<String> Ppr::string() const
{
  SStream stream;
  stream << this;
  return ptr<String>(stream.str());
}


void PprString::output(OStream &out, unsigned indent) const
{
  out << String(indent, ' ') << m_val;
}


void PprIndent::output(OStream &out, unsigned indent) const
{
  m_child->output(out, indent + m_indent);
}


void PprHCat::output(OStream &out, unsigned indent) const
{
  for (auto child: *m_children) {
    child->output(out, indent);
  }
}


void PprVCat::output(OStream &out, unsigned indent) const
{
  for (auto child: *m_children) {
    child->output(out, indent);
    out << std::endl;
  }
}


namespace
{
  template <typename T>
  Ptr<T> cat(Ptr<Ppr> a, Ptr<Ppr> b)
  {
    return std::shared_ptr<T>(new T {a, b});
  }
}


namespace ppr
{
  Ptr<Ppr> string(const String &str)
  {
    return ptr<PprString>(str);
  }

  Ptr<Ppr> string(const Char *str)
  {
    return ptr<PprString>(String(str));
  }

  Ptr<Ppr> indent(Ptr<Ppr> ppr, unsigned ind)
  {
    return ptr<PprIndent>(ppr, ind);
  }
}


Ptr<Ppr> operator>>(Ptr<Ppr> ppr, unsigned ind)
{
  return ppr::indent(ppr, ind * Ppr::default_indent);
}

Ptr<Ppr> operator*(Ptr<Ppr> left, Ptr<Ppr> right)
{
  return cat<PprHCat>(left, right);
}

Ptr<Ppr> operator+(Ptr<Ppr> top, Ptr<Ppr> bottom)
{
  return cat<PprVCat>(top, bottom);
}

Ptr<Ppr> operator""_p(const Char *str, size_t len)
{
  return ptr<PprString>(String(str));
}

Ptr<Ppr> operator""_p(Char c)
{
  return ptr<PprString>(String(1, c));
}

Ptr<Ppr> operator+(Ptr<Ppr> p)
{
  return L' '_p * p;
}


}
