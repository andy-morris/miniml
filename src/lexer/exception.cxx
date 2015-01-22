#include "lexer/exception.hxx"

namespace miniml
{

LexicalError::LexicalError(char c_, Pos pos_):
  c(c_)
{
  pos = pos_;
  std::stringstream s;
  s << pos << ": lexical error at char '" << c << "'";
  text = s.str();
}


InvalidEscape::InvalidEscape(int index_, Pos pos_):
  index(index_)
{
  pos = pos_;
  std::stringstream s;
  s << pos << ": numeric escape sequence '\\" << index << "' too high";
  text = s.str();
}


}
