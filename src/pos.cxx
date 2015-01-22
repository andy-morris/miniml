#include "pos.hxx"

namespace miniml
{
namespace ppr
{

  Ptr<Ppr> pos_if(bool b, Ptr<Ppr> ppr, Pos start, Pos end)
  {
    return b? hcat({ppr, Span {start, end}.ppr()}) : ppr;
  }

}

Pos Pos::operator+(const char c) const
{
  Pos p2 = *this;
  p2 += c;
  return p2;
}

Pos &Pos::operator+=(const char c)
{
  ++pos;
  switch (c) {
    case '\n':
    case '\r':
    case '\f': // form feed
    case '\v': // vertical tab
      ++line;
      col = 0;
      break;
    case '\t':
      col += 8 - (col % 8);
      break;
    default:
      ++col;
  }
  return *this;
}


Pos Pos::operator+(const String &s) const
{
  Pos p2 = *this;
  p2 += s;
  return p2;
}

Pos &Pos::operator+=(const String &s)
{
  for (char c: s) {
    *this += c;
  }
  return *this;
}

}
