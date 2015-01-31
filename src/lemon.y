%include {
  #include "ast.hxx"
  #include "token.hxx"
  #include "parser.hxx"
  #include <cassert>
  #include <utility>

  using namespace miniml;

  using Decls = ModuleInput::Decls;

  namespace
  {
    struct Arg { Id name; Type *type; };

    template <typename T>
    inline Ptr<T> ptr(T *t)
    { return Ptr<T>(t); }

    inline Id *get_id(Token *t)
    { return dynamic_cast<IdToken*>(t)->id; }

    inline long get_int(Token *t)
    { return dynamic_cast<IntToken*>(t)->val; }

    inline String get_string(Token *t)
    { return dynamic_cast<StringToken*>(t)->val; }

    inline Type *id_type(Id *i)
    {
      if (*i == String("int")) {
        return new IntType(i->start(), i->end());
      } else if (*i == String("bool")) {
        return new BoolType(i->start(), i->end());
      } else if (*i == String("string")) {
        return new StringType(i->start(), i->end());
      } else {
        return new IdType(*i, i->start(), i->end());
      }
    }

    // NB: args in reverse order
    inline Ptr<Expr> make_lam(const std::deque<Ptr<Arg>> &args, Expr *body)
    {
      auto e = ptr(body);
      for (auto a: args) {
        e = ptr<LamExpr>(a->name, ptr(a->type), e);
      }
      return e;
    }

    // NB: args in reverse order, still
    inline Ptr<Type> make_lam_type(const std::deque<Ptr<Arg>> &args, Type *ret)
    {
      auto t = ptr(ret);
      for (auto a: args) {
        t = ptr<ArrowType>(ptr(a->type), t);
      }
      return t;
    }


    template <typename T>
    inline std::vector<T> *vec(std::deque<T> *deq)
    {
      auto v = new std::vector<T>;
      v->reserve(deq->size());
      for (auto x: *deq) { v->push_back(x); }
      delete deq;
      return v;
    }
  }

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
}

%name MiniMLParser
%extra_argument {Input **input}

%syntax_error {
  throw Parser::ParseFail(yyminor.yy0);
}

%token_prefix TOK_
%token_type {Token*}

%left HEADER.
%nonassoc EQ.
%right TYARROW.
%nonassoc COLON.
%left SEQ.
%left FN.
%right IFF.
%right OR.
%right AND.
%left LESS LEQ EQUAL GEQ GREATER NEQ.
%left PLUS MINUS.
%left TIMES DIVIDE.
%left DOT.

%type start {Input*}
start(X) ::= decl(A).
  { *input = X = new DeclInput(ptr(A)); }
start(X) ::= expr(A).
  { *input = X = new ExprInput(ptr(A)); }
start(X) ::= module(A).
  { *input = X = new ModuleInput(*A->first, ptr(A->second)); }
%destructor start {delete $$;}

%type module {std::pair<Id*, Decls*>*}
module(X) ::= header(I) decls(D).
  { X = new std::pair<Id*, Decls*>(I, vec(D)); }
%destructor module {delete $$;}

%type header {Id*}
header(X) ::= ID(I) ARROW. [HEADER]
  { X = get_id(I); }

%type decls {std::deque<Ptr<Decl>>*}
decls(X) ::= .
  { X = new std::deque<Ptr<Decl>>; }
decls(X) ::= decl(A) decls(B).
  { B->push_front(ptr(A)); X = B; }
%destructor decls {delete $$;}

%type expr {Expr*}
expr(X) ::= aexprs(A).
  { X = A; }
expr(X) ::= FN(L) LPAR id(I) COLON type(T) RPAR ARROW expr(R).
  { X = new LamExpr(*I, ptr(T), ptr(R),  L->start(), R->end()); }
expr(X) ::= IF(L) aexpr(B) aexpr(T) aexpr(E).
  { X = new IfExpr(ptr(B), ptr(T), ptr(E), L->start(), E->end()); }
expr(X) ::= expr(A) COLON type(T).
  { X = new TypeExpr(ptr(A), ptr(T),  A->start(), T->end()); }
expr(X) ::= expr(L) PLUS expr(R).
  { X = new BinOpExpr(BinOp::PLUS, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) MINUS expr(R).
  { X = new BinOpExpr(BinOp::MINUS, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) TIMES expr(R).
  { X = new BinOpExpr(BinOp::TIMES, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) DIVIDE expr(R).
  { X = new BinOpExpr(BinOp::DIVIDE, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) AND expr(R).
  { X = new BinOpExpr(BinOp::AND, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) OR expr(R).
  { X = new BinOpExpr(BinOp::OR, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) IFF expr(R).
  { X = new BinOpExpr(BinOp::IFF, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) LESS expr(R).
  { X = new BinOpExpr(BinOp::LESS, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) LEQ expr(R).
  { X = new BinOpExpr(BinOp::LEQ, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) EQUAL expr(R).
  { X = new BinOpExpr(BinOp::EQUAL, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) GEQ expr(R).
  { X = new BinOpExpr(BinOp::GEQ, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) GREATER expr(R).
  { X = new BinOpExpr(BinOp::GREATER, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) NEQ expr(R).
  { X = new BinOpExpr(BinOp::NEQ, ptr(L), ptr(R), L->start(), R->end()); }
expr(X) ::= expr(L) SEQ expr(R).
  { X = new BinOpExpr(BinOp::SEQ, ptr(L), ptr(R), L->start(), R->end()); }
%destructor expr {delete $$;}

%type aexprs {Expr*}
aexprs(X) ::= aexprs(F) aexpr(E).
  { X = new AppExpr(ptr(F), ptr(E),  F->start(), E->end()); }
aexprs(X) ::= aexpr(A).
  { X = A; }
%destructor aexprs {delete $$;}

%type aexpr {Expr*}
aexpr(X) ::= id(I).
  { X = new IdExpr(*I,  I->start(), I->end()); }
aexpr(X) ::= INT(I).
  { X = new IntExpr(get_int(I),  I->start(), I->end()); }
aexpr(X) ::= bool(B).
  { X = B; }
aexpr(X) ::= STRING(S).
  { X = new StringExpr(get_string(S), S->start(), S->end()); }
aexpr(X) ::= aexpr(E) DOT INT(I).
  { X = new DotExpr(ptr(E), get_int(I), E->start(), I->end()); }
aexpr(X) ::= LPAR expr(A) RPAR.
  { X = A; }
aexpr(X) ::= LPAR(L) RPAR(R).
  { X = new TupleExpr({}, L->start(), R->end()); }
aexpr(X) ::= LPAR(L) expr2(E) RPAR(R).
  { X = new TupleExpr(ptr(vec(E)), L->start(), R->end()); }
%destructor aexpr {delete $$;}

%type expr2 {std::deque<Ptr<Expr>>*}
expr2(X) ::= expr(A) COMMA expr(B).
  { X = new std::deque<Ptr<Expr>>({ptr(A), ptr(B)}); }
expr2(X) ::= expr(A) COMMA expr2(B).
  { B->push_front(ptr(A)); X = B; }
%destructor expr2 {delete $$;}

%type bool {Expr*}
bool(X) ::= TRUE(T).
  { X = new BoolExpr(true, T->start(), T->end()); }
bool(X) ::= FALSE(T).
  { X = new BoolExpr(false, T->start(), T->end()); }
%destructor bool {delete $$;}


%type type {Type*}
type(X) ::= id(I).
  { X = id_type(I); }
type(X) ::= type(L) TYARROW type(R).
  { X = new ArrowType(ptr(L), ptr(R),  L->start(), R->end()); }
type(X) ::= LPAR types(T) RPAR.
  { X = new TupleType(ptr(T), T->front()->start(), T->back()->end()); }
%destructor type {delete $$;}

%type types {TupleType::Types*}
types(X) ::= .
  { X = new TupleType::Types; }
types(X) ::= types1(T).
  { X = vec(T); }
%destructor types {delete $$;}

%type types1 {std::deque<Ptr<Type>>*}
types1(X) ::= type(T).
  { X = new std::deque<Ptr<Type>>({ptr(T)}); }
types1(X) ::= type(T) COMMA types1(U).
  { U->push_front(ptr(T)); X = U; }
%destructor types1 {delete $$;}

%type id {Id*}
id(X) ::= ID(I).
  { X = new Id(*get_id(I),  I->start(), I->end()); }
%destructor id {delete $$;}

%type decl {Decl*}
decl(X) ::= VAL(V) rec_opt(R) id(I) tyann_opt(T) EQ expr(D).
  { X = new ValDecl(*I, ptr(D), ptr(T), R, V->start(), D->end()); }
decl(X) ::= FUN(F) id(I) args(A) COLON type(T) EQ expr(D).
  { X = new ValDecl(*I, make_lam(*A, D), make_lam_type(*A, T), true,
                    F->start(), D->end()); }
%destructor decl {delete $$;}

%type rec_opt {bool}
rec_opt(X) ::= REC. { X = true; }
rec_opt(X) ::= .    { X = false; }
%destructor rec_opt {}

%type tyann_opt {Type*}
tyann_opt(X) ::= .              { X = nullptr; }
tyann_opt(X) ::= COLON type(T). { X = T; }
%destructor tyann_opt {delete $$;}

%type args {std::deque<Ptr<Arg>>*}
args(X) ::= arg(A).
  { X = new std::deque<Ptr<Arg>>({ptr(A)}); }
args(X) ::= arg(A) args(B).
  { B->push_back(ptr(A)); X = B; }
%destructor args {delete $$;}

%type arg {Arg*}
arg(X) ::= LPAR ID(I) COLON type(T) RPAR.
  { X = new Arg {*get_id(I), T}; }
