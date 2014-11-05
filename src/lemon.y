%include {
  #include "ast.hxx"
  #include "token.hxx"
  #include <assert.h>

  using namespace miniml;

  template <typename T>
  Ptr<T> ptr(T *t)
  { return Ptr<T>(t); }

  Id *get_id(Token *t)
  { return dynamic_cast<IdToken*>(t)->id; }

  long get_int(Token *t)
  { return dynamic_cast<IntToken*>(t)->val; }
}

%name MiniMLParser
%extra_argument {Expr **expr}

%token_prefix TOK_
%token_type {Token*}

%type start {Expr*}
start(X) ::= expr(A).
  { *expr = X = A; }
%destructor start {delete $$;}

%type expr {Expr*}
expr(X) ::= aexprs(A).
  { X = A; }
expr(X) ::= FN id(I) ARROW expr(R).
  { X = new LamExpr(ptr(I), ptr(R)); }
%destructor expr {delete $$;}

%type aexprs {Expr*}
aexprs(X) ::= aexprs(F) aexpr(E).
  { X = new AppExpr(ptr(F), ptr(E)); }
aexprs(X) ::= aexpr(A).
  { X = A; }
%destructor aexprs {delete $$;}

%type aexpr {Expr*}
aexpr(X) ::= id(I).
  { X = new IdExpr(ptr(I)); }
aexpr(X) ::= INT(I).
  { X = new IntExpr(get_int(I)); }
%destructor aexpr {delete $$;}

%type id {Id*}
id(X) ::= ID(I).
  { X = new Id(*get_id(I)); }
%destructor id {delete $$;}
