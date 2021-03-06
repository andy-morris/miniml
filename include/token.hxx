#ifndef TOKEN_HXX_LGIKOUIZ
#define TOKEN_HXX_LGIKOUIZ

#include "lexer/exception.hxx"
#include "id.hxx"
#include "pos.hxx"
#include "string.hxx"
#include <utility>
#include <cstddef>

namespace miniml
{

/// \\-escape a string.
String escaped(const String&);
/// Remove \\-escapes from a string.
String unescaped(const String&, Pos);

/// Abstract class for tokens.
struct Token: public HasPos
{
  enum class Type;                ///< Token types.
  virtual Type type() const = 0;  ///< What type of token is this?

  virtual ~Token() {}

  /// Make an atomic token.
  template <Type ty> static inline Ptr<Token> atomic(Pos start, Pos end);

  /// Whether a token type is atomic (needs no data).
  static constexpr bool is_atomic(Type);

  virtual void out(OStream &) const = 0;

protected:
  Token(Pos start, Pos end): HasPos(start, end) {}
};


enum class Token::Type
{
  ID,       ///< identifier
  INT,      ///< integer literal
  STRING,   ///< string literal
  FN,       ///< `fn`
  IF,       ///< `if`
  ARROW,    ///< `=>`
  TYARROW,  ///< `->`
  LPAR,     ///< `(`
  RPAR,     ///< `)`
  COLON,    ///< `:`
  PLUS,     ///< `+`
  MINUS,    ///< `-`
  TIMES,    ///< `*`
  DIVIDE,   ///< `/`

  VAL,      ///< `val`
  REC,      ///< `rec`
  FUN,      ///< `fun`
  EQ,       ///< `=`

  TRUE,     ///< `true`
  FALSE,    ///< `false`

  AND,      ///< `&&`
  OR,       ///< `||`
  IFF,      ///< `<->`

  LESS,     ///< `<`
  LEQ,      ///< `<=`
  EQUAL,    ///< `==`
  GEQ,      ///< `>=`
  GREATER,  ///< `>`
  NEQ,      ///< `!=`

  SEQ,      ///< `;`
  COMMA,    ///< `,`

  DOT,      ///< `.`
};


OStream &operator<<(OStream &out, const Token &tok);



constexpr bool Token::is_atomic(Token::Type ty)
{
  using Type = Token::Type;
  return !(ty == Type::ID || ty == Type::INT || ty == Type::STRING);
}


/// Identifiers.
struct IdToken final: public Token
{
  IdToken(Id *id_, Pos start, Pos end): Token(start, end), id(id_) {}
  IdToken(const Char *c, std::ptrdiff_t size, Pos start, Pos end):
    IdToken(new Id(ptr<String>(c, size)), start, end)
  {}

  ~IdToken() { delete id; }

  Id *id;

  inline Token::Type type() const override { return Token::Type::ID; }

  virtual void out(OStream &) const;
};


/// Integer literals.
struct IntToken final: public Token
{
  IntToken(long val_, Pos start, Pos end): Token(start, end), val(val_) {}
  long val;

  inline Token::Type type() const override { return Token::Type::INT; }

  virtual void out(OStream &) const;
};


/// String literals.
struct StringToken final: public Token
{
  StringToken(const String &val_, Pos start, Pos end):
    Token(start, end), val(val_)
  {}
  StringToken(String &&val_, Pos start, Pos end):
    Token(start, end), val(std::forward<String>(val_))
  {}
  const String val;

  inline Token::Type type() const override { return Token::Type::STRING; }

  virtual void out(OStream &) const;
};


/// Atomic tokens, i.e. ones with no value other than their Token::Type.
template <Token::Type Ty>
class AtomicToken final: public Token
{
public:
  AtomicToken(Pos start, Pos end): Token(start, end) {}

  inline Token::Type type() const { return Ty; }

static_assert(Token::is_atomic(Ty),
    "tried to make an atomic token of a nonatomic type");

  virtual void out(OStream &) const;
};


/// Make an AtomicToken.
template <Token::Type Ty>
inline Ptr<Token> Token::atomic(Pos start, Pos end)
{
  return ptr<AtomicToken<Ty>>(start, end);
}

}

#endif /* end of include guard: TOKEN_HXX_LGIKOUIZ */
