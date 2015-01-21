#include "ppr.hxx"
#include <list>

namespace miniml
{

Ptr<String> Ppr::string() const
{
  SStream stream;
  stream << *this;
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
  out << String(indent, ' ');
  for (auto child: *m_children) {
    child->output(out, 0);
  }
}


void PprVCat::output(OStream &out, unsigned indent) const
{
  bool first = true;
  for (auto child: *m_children) {
    if (first) {
      first = false;
    } else {
      out << std::endl;
    }
    child->output(out, indent);
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

  // num: see header file
  
  Ptr<Ppr> bool_(bool b)
  {
    return b? "true"_p : "false"_p;
  }

  Ptr<Ppr> indent(Ptr<Ppr> ppr, unsigned ind)
  {
    return ptr<PprIndent>(ppr, ind);
  }

  Ptr<Ppr> parens_if(bool b, const Ptr<Ppr> ppr)
  {
    if (b)
      return hcat({'('_p, ppr, ')'_p});
    else
      return ppr;
  }

  Ptr<Ppr> vcat(std::initializer_list<Ptr<Ppr>> pprs)
  { return ptr<PprVCat>(pprs); }

  Ptr<Ppr> vcat(Ptr<std::list<Ptr<Ppr>>> pprs)
  { return ptr<PprVCat>(pprs); }

  Ptr<Ppr> hcat(std::initializer_list<Ptr<Ppr>> pprs)
  { return ptr<PprHCat>(pprs); }

  Ptr<Ppr> hcat(Ptr<std::list<Ptr<Ppr>>> pprs)
  { return ptr<PprHCat>(pprs); }
}


Ptr<Ppr> operator>>(Ptr<Ppr> ppr, unsigned ind)
{
  return ppr::indent(ppr, ind * Ppr::default_indent);
}

Ptr<Ppr> operator""_p(const Char *str, size_t len)
{
  return ptr<PprString>(String(str, len));
}

Ptr<Ppr> operator""_p(Char c)
{
  return ptr<PprString>(String(1, c));
}

Ptr<Ppr> operator+(Ptr<Ppr> p)
{
  return ppr::hcat({' '_p, p});
}


}
