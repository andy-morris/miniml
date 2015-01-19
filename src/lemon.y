%include {
  #include "ast.hxx"
  #include "token.hxx"
  #include <cassert>

  using namespace miniml;

  namespace
  {
    template <typename T>
    inline Ptr<T> ptr(T *t)
    { return Ptr<T>(t); }

    inline Id *get_id(Token *t)
    { return dynamic_cast<IdToken*>(t)->id; }

    inline long get_int(Token *t)
    { return dynamic_cast<IntToken*>(t)->val; }

    inline Type *id_type(Id *i)
    {
      if (*i == String("int")) {
        return new IntType(i->start(), i->end());
      } else {
        return new IdType(*i, i->start(), i->end());
      }
    }
  }

#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
}

%name MiniMLParser
%extra_argument {Decl **decl}

%token_prefix TOK_
%token_type {Token*}

%nonassoc EQ.
%right TYARROW.
%nonassoc COLON.
%left FN.
%left PLUS MINUS.
%left TIMES DIVIDE.

%type start {Decl*}
start(X) ::= decl(A).
  { *decl = X = A; }
%destructor start {delete $$;}

%type expr {Expr*}
expr(X) ::= aexprs(A).
  { X = A; }
expr(X) ::= FN(L) LPAR id(I) COLON type(T) RPAR ARROW expr(R).
  { X = new LamExpr(*I, ptr(T), ptr(R),  L->start(), R->end()); }
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
aexpr(X) ::= LPAR expr(A) RPAR.
  { X = A; }
%destructor aexpr {delete $$;}


%type type {Type*}
type(X) ::= id(I).
  { X = id_type(I); }
type(X) ::= type(L) TYARROW type(R).
  { X = new ArrowType(ptr(L), ptr(R),  L->start(), R->end()); }
%destructor type {delete $$;}


%type id {Id*}
id(X) ::= ID(I).
  { X = new Id(*get_id(I),  I->start(), I->end()); }
%destructor id {delete $$;}

%type decl {Decl*}
decl(X) ::= VAL(V) id(I) EQ expr(D).
  { X = new ValDecl(*I, ptr(D), V->start(), D->end()); }
%destructor decl {delete $$;}
