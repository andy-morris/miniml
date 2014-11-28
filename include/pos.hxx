#ifndef POS_HXX_BITQRAWX
#define POS_HXX_BITQRAWX

#include "ppr.hxx"

namespace miniml
{

struct Pos final: public Pretty
{
  size_t line = 1,  ///< Line number
         col = 0,   ///< Column number
         pos = 0;   ///< File position

  Pos operator+(const char c) const;
  Pos &operator+=(const char c);

  Ptr<Ppr> ppr(unsigned prec = 0, bool=false) const override
  { return ppr::num(line) * ":"_p * ppr::num(col); }
};

OStream &operator<<(OStream&, const Pos&);


struct Span final
{
  Ptr<Ppr> ppr() const
  { return "@("_p * start.ppr() * ";"_p * end.ppr() * ")"_p; }

  Pos start, end;
};


struct HasPos
{
  Pos start() const { return m_start; }
  Pos end()   const { return m_end; }

protected:
  HasPos(Pos start = Pos(), Pos end = Pos()): m_start(start), m_end(end) {}

private:
  Pos m_start, m_end;
};

}

#endif /* end of include guard: POS_HXX_BITQRAWX */
