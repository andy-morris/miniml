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

template<>
void AtomicToken<Token::Type::FN>::out(OStream &o) const
{ o << "'fn'"; }

template<>
void AtomicToken<Token::Type::ARROW>::out(OStream &o) const
{ o << "'=>'"; }


OStream &operator<<(OStream &out, const Token &tok)
{
  tok.out(out);
  return out;
}

}
