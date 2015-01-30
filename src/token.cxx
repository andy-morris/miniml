#include "token.hxx"
#include <string>
#include <limits>
#include <cassert>

namespace miniml
{

String unescaped(const String &str, Pos pos)
{
  String out; out.reserve(str.size());
  for (size_t i = 1; i < str.size() - 1; ++i) { // 1's because quotes
    char c = str[i];
    pos += c;
    if (c == '\\') {
      ++i; c = str[i];
      if (c >= '0' && c <= '9') {
        auto idx_ = str.substr(i, 3);
        pos += idx_;
        int idx = std::stoi(idx_);
        if (idx > std::numeric_limits<char>::max()) {
          throw InvalidEscape(idx, pos);
        }
        out.push_back((char) idx);
        i += 3;
      } else {
        switch (c) {
        case 'n': out.push_back('\n'); break;
        case 'r': out.push_back('\r'); break;
        case 't': out.push_back('\t'); break;
        case 'f': out.push_back('\f'); break;
        case 'v': out.push_back('\v'); break;
        case 'a': out.push_back('\a'); break;
        case 'e': out.push_back('\033'); break; // escape
        default:  abort(); // lexer should've rejected
        }
      }
    } else {
      out.push_back(c);
    }
  }
  out.shrink_to_fit();
  return out;
}

String escaped(const String &str)
{
  String out; out.reserve(str.size()*2); // generous guess
  for (char c: str) {
    if (std::isprint(c)) {
      out.push_back(c);
    } else {
      out.push_back('\\');
      switch (c) {
      case '\n':   out.push_back('n'); break;
      case '\r':   out.push_back('r'); break;
      case '\t':   out.push_back('t'); break;
      case '\f':   out.push_back('f'); break;
      case '\v':   out.push_back('v'); break;
      case '\a':   out.push_back('a'); break;
      case '\033': out.push_back('e'); break;
      default:     out += std::to_string((int) c); break;
      }
    }
  }
  out.shrink_to_fit();
  return out;
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

void StringToken::out(OStream &o) const
{
  o << "STRING(" << escaped(val) << ")";
}


#define ATOMIC_OUT(t, str) \
  template<> \
  void AtomicToken<Token::Type::t>::out(OStream &o) const \
  { o << str; }

ATOMIC_OUT(FN, "'fn'")
ATOMIC_OUT(IF, "'if'")
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
ATOMIC_OUT(REC, "'rec'")
ATOMIC_OUT(FUN, "'fun'")
ATOMIC_OUT(EQ, "'='")
ATOMIC_OUT(TRUE, "'true'")
ATOMIC_OUT(FALSE, "'false'")
ATOMIC_OUT(AND, "'&&'")
ATOMIC_OUT(OR, "'||'")
ATOMIC_OUT(IFF, "'<->'")
ATOMIC_OUT(LESS, "'<'")
ATOMIC_OUT(LEQ, "'<='")
ATOMIC_OUT(EQUAL, "'=='")
ATOMIC_OUT(GEQ, "'>='")
ATOMIC_OUT(GREATER, "'>'")
ATOMIC_OUT(NEQ, "'!='")
ATOMIC_OUT(SEQ, "';'")
ATOMIC_OUT(COMMA, "','")
ATOMIC_OUT(DOT, "'.'")
#undef ATOMIC_OUT

OStream &operator<<(OStream &out, const Token &tok)
{
  tok.out(out);
  return out;
}

}
