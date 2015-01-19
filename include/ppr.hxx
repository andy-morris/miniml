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

/// Abstract base class for pretty printed fragments.
class Ppr
{
public:
  virtual ~Ppr() {}

  /// Convert a pretty printed object to a string.
  Ptr<String> string() const;

  /// Default indent amount.
  static const unsigned default_indent = 4;

  /// Outputs the document to the given output stream.
  /// \param[in] indent The amount to indent, in characters.
  virtual void output(OStream&, unsigned indent = 0) const = 0;
};


class Pretty
{
public:
  /// \param prec [in] the surrounding precedence level for the
  //                   insertion of brackets.
  virtual Ptr<Ppr> ppr(unsigned prec = 0, bool pos = false) const = 0;
  Ptr<Ppr> ppr(bool pos) { return ppr(0, pos); }
};


/// Pretty printed string fragment.
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

/// Indents an existing document by a given amount.
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


/// Concatenated fragments.
/// \see PprVCat \see PprHCat
class PprCat: public Ppr
{
public:
  template <typename T> using List = std::list<T>;

protected:
  PprCat(std::initializer_list<Ptr<Ppr>> lst):
    m_children(ptr<List<Ptr<Ppr>>>(lst))
  {}
  virtual ~PprCat() {}

protected:
  Ptr<List<Ptr<Ppr>>> m_children;
};


/// Vertically concatenated fragments.
class PprVCat: public PprCat
{
public:
  PprVCat(std::initializer_list<Ptr<Ppr>> lst):
    PprCat(lst)
  {}

  void output(OStream&, unsigned indent = 0) const override;
};


/// Horizontally concatenated fragments.
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
  /// Constructs a \ref Ppr from a \ref String.
  Ptr<Ppr> string(const String&);
  /// Constructs a \ref Ppr from a C string.
  Ptr<Ppr> string(const Char*);
  /// Constructs a \ref Ppr from an numeric value.
  template <typename T>
  Ptr<Ppr> num(T);
  /// Indent a \ref Ppr by a given amount of characters.
  /// \see operator>>(Ptr<Ppr>, unsigned)
  Ptr<Ppr> indent(Ptr<Ppr>, unsigned indent = Ppr::default_indent);
  /// Add parentheses if the first argument is true.
  Ptr<Ppr> parens_if(bool b, const Ptr<Ppr> ppr);
  /// Concatenate vertically.
  Ptr<Ppr> vcat(std::initializer_list<Ptr<Ppr>> pprs);
  /// Concatenate horizontally.
  Ptr<Ppr> hcat(std::initializer_list<Ptr<Ppr>> pprs);
}

/// Indent a \ref Ppr by a multiple of \ref Ppr::default_indent.
Ptr<Ppr> operator>>(Ptr<Ppr>, unsigned);

/// Creates a document from a string literal.
Ptr<Ppr> operator""_p(const Char*, size_t);

/// Creates a (single-character) document from a \c char literal.
Ptr<Ppr> operator""_p(Char);

/// Inserts a space before a document.
/// \see operator*(Ptr<Ppr>, Ptr<Ppr>)
Ptr<Ppr> operator+(Ptr<Ppr>);


/// Outputs a document to an \ref OStream.
inline OStream &operator<<(OStream &out, const Ppr &p)
{
  p.output(out);
  return out;
}

/// Outputs a pretty-printable object to an \ref OStream.
inline OStream &operator<<(OStream &out, const Pretty &p)
{
  return out << p.ppr();
}


namespace ppr
{
  template <typename T>
  Ptr<Ppr> num(T x)
  {
    static_assert(std::is_integral<T>::value, "non-numeric type");

    auto str = std::to_string(x);
    if (str[0] == '-') str[0] = '~';
    return string(str);
  }
}


}

#endif /* end of include guard: PPR_HXX_MJEKHUYS */
