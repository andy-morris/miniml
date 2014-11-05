#ifndef TOKEN_HXX_LGIKOUIZ
#define TOKEN_HXX_LGIKOUIZ

#include "id.hxx"
#include "string.hxx"

namespace miniml
{

/// Abstract class for tokens.
struct Token
{
  enum class Type;                ///< Token types.
  virtual Type type() const = 0;  ///< What type of token is this?

  virtual ~Token() {}

  /// Make an atomic token.
  template <Type ty> static Ptr<Token> atomic();

  /// Whether a token type is atomic (needs no data).
  static constexpr bool is_atomic(Type);

  virtual void out(OStream &) const = 0;
};


enum class Token::Type
{
  ID,     ///< identifier
  INT,    ///< integer literal
  FN,     ///< `fn`
  ARROW,  ///< `=>`
};


OStream &operator<<(OStream &out, const Token &tok);



constexpr bool Token::is_atomic(Token::Type ty)
{
  using Type = Token::Type;
  return !(ty == Type::ID || ty == Type::INT);
}


struct IdToken final: public Token
{
  IdToken(Id *id_): id(id_) {}
  IdToken(const Char *start, ptrdiff_t size):
    IdToken(new Id(ptr<String>(start, size)))
  {}

  virtual ~IdToken() { delete id; }

  Id *id;

  virtual Token::Type type() const override { return Token::Type::ID; }

  virtual void out(OStream &) const;
};


struct IntToken final: public Token
{
  IntToken(long val_): val(val_) {}
  long val;

  virtual Token::Type type() const override { return Token::Type::INT; }

  virtual void out(OStream &) const;
};


template <Token::Type Ty>
class AtomicToken final: public Token
{
public:
  virtual Token::Type type() const { return Ty; }

static_assert(Token::is_atomic(Ty),
    "tried to make an atomic token of a nonatomic type");

  virtual void out(OStream &) const;
};


template <Token::Type Ty>
Ptr<Token> Token::atomic()
{
  return ptr<AtomicToken<Ty>>();
}

}

#endif /* end of include guard: TOKEN_HXX_LGIKOUIZ */
