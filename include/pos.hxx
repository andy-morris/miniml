#ifndef POS_HXX_BITQRAWX
#define POS_HXX_BITQRAWX

#include "ppr.hxx"

namespace miniml
{

/// Source code position.
struct Pos final: public Pretty
{
  size_t line = 1,  ///< Line number
         col = 0,   ///< Column number
         pos = 0;   ///< File position

  Pos operator+(const char) const;
  /// Advance the position depending on what the character is.
  Pos &operator+=(const char);

  Pos operator+(const String&) const;
  /// Advance the position by the contents of a string.
  /// \sa #operator+=(const char)
  Pos &operator+=(const String&);

  inline Ptr<Ppr> ppr(unsigned=0, bool=false) const override
  { return ppr::hcat({ppr::num(line), ":"_p, ppr::num(col)}); }
};

OStream &operator<<(OStream&, const Pos&);


/// Span of positions. \sa Pos
struct Span final
{
  inline Ptr<Ppr> ppr() const
  {
    return ppr::hcat({"@("_p, start.ppr(), ";"_p, end.ppr(), ")"_p});
  }

  Pos start, end;
};

namespace ppr
{
  /// Maybe follow a Ppr fragment with a source span.
  Ptr<Ppr> pos_if(bool, Ptr<Ppr>, Pos start, Pos end);
}


/// Base class for values which have source code positions.
struct HasPos
{
  inline Pos start() const { return m_start; }
  inline Pos end()   const { return m_end; }

protected:
  inline HasPos(Pos start = Pos(), Pos end = Pos()): m_start(start), m_end(end)
  {}

private:
  Pos m_start, m_end;
};

}

#endif /* end of include guard: POS_HXX_BITQRAWX */
