#include "token.hxx"

namespace miniml
{

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

OStream &operator<<(OStream &out, const Pos &p)
{
  return out << p.line << ':' << p.col;
}


void IdToken::out(OStream &o) const
{
  o << "ID(" << *id << ")";
}

void IntToken::out(OStream &o) const
{
  o << "INT(" << val << ")";
}


#define ATOMIC_OUT(t, str) \
  template<> \
  void AtomicToken<Token::Type::t>::out(OStream &o) const \
  { o << str; }

ATOMIC_OUT(FN, "'fn'")
ATOMIC_OUT(ARROW, "'=>'")
ATOMIC_OUT(TYARROW, "'->'")
ATOMIC_OUT(LPAR, "'('")
ATOMIC_OUT(RPAR, "')'")
ATOMIC_OUT(COLON, "':'")
ATOMIC_OUT(PLUS, "'+'")
ATOMIC_OUT(MINUS, "'-'")
ATOMIC_OUT(TIMES, "'*'")
ATOMIC_OUT(DIVIDE, "'/'")
ATOMIC_OUT(VAL, "'val'")
ATOMIC_OUT(EQ, "'='")
#undef ATOMIC_OUT

OStream &operator<<(OStream &out, const Token &tok)
{
  tok.out(out);
  return out;
}

}
