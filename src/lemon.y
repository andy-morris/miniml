%include {
  #include "ast.hxx"
  #include "token.hxx"
  #include <cassert>

  using namespace miniml;

  template <typename T>
  Ptr<T> ptr(T *t)
  { return Ptr<T>(t); }

  Id *get_id(Token *t)
  { return dynamic_cast<IdToken*>(t)->id; }

  long get_int(Token *t)
  { return dynamic_cast<IntToken*>(t)->val; }

  Type *id_type(Id *i)
  {
    if (*i == String("int")) {
      return new IntType(i->start(), i->end());
    } else {
      return new IdType(*i, i->start(), i->end());
    }
  }

#pragma clang diagnostic ignored "-Wunused-variable"
}

%name MiniMLParser
%extra_argument {Expr **expr}

%token_prefix TOK_
%token_type {Token*}

%right TYARROW.
%nonassoc COLON.
%left FN.

%type start {Expr*}
start(X) ::= expr(A).
  { *expr = X = A; }
%destructor start {delete $$;}

%type expr {Expr*}
expr(X) ::= aexprs(A).
  { X = A; }
expr(X) ::= FN(L) LPAR id(I) COLON type(T) RPAR ARROW expr(R).
  { X = new LamExpr(*I, ptr(T), ptr(R),  L->start(), R->end()); }
expr(X) ::= expr(A) COLON type(T).
  { X = new TypeExpr(ptr(A), ptr(T),  A->start(), T->end()); }
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
