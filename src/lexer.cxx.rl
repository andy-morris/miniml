#include "lexer.hxx"

namespace miniml
{

using TokType = Token::Type;

%%{
machine Lexer;
alphtype char;

idstart  = alpha | "_";
idletter = idstart | digit | "'";
id       = idstart idletter*;

int = ('+'|'-')? [0-9]+;

token := |*
  "fn"  => { push(Token::atomic<TokType::FN>()); };
  "=>"  => { push(Token::atomic<TokType::ARROW>()); };
  id    => {
    push(ptr<IdToken>(ts, te - ts));
  };
  int   => {
    std::string str(ts, te - ts);
    std::stringstream s(str); long x;
    s >> x;
    push(ptr<IntToken>(x));
  };
  space+;
  "//" . [^\n] . "\n";
*|;
}%%


%% write data;

Lexer::Lexer(const Ptr<String> str) throw(Lexer::LexicalError):
  data(str)
{
  p = begin = str->data();
  eof = pe = str->data() + str->size();
  %% write init;
  %% write exec;

  if (cs == Lexer_error)
    throw LexicalError(*p, p - begin);
}

Lexer::LexicalError::LexicalError(char c_, ptrdiff_t pos_):
  c(c_), pos(pos_)
{
  std::stringstream s;
  s << "Lexical error at '" << c << "', position " << pos;
  text = s.str();
}


OStream &operator<<(OStream &out, const Token &tok)
{
  using Ty = Token::Type;
  switch (tok.type()) {
  case Ty::ID:
    out << *dynamic_cast<const IdToken&>(tok).id;
    break;
  case Ty::INT:
    out << dynamic_cast<const IntToken&>(tok).val;
    break;
  case Ty::FN:
    out << "'fn'";
    break;
  case Ty::ARROW:
    out << "'=>'";
    break;
  }
  return out;
}


}
