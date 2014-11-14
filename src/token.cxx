#include "token.hxx"

namespace miniml
{

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
#undef ATOMIC_OUT

OStream &operator<<(OStream &out, const Token &tok)
{
  tok.out(out);
  return out;
}

}
