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
expr(X) ::= FN LPAR id(I) COLON type(T) RPAR ARROW expr(R).
  { X = new LamExpr(ptr(I), ptr(T), ptr(R)); }
expr(X) ::= expr(A) COLON type(T).
  { X = new TypeExpr(ptr(A), ptr(T)); }
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
aexpr(X) ::= LPAR expr(A) RPAR.
  { X = A; }
%destructor aexpr {delete $$;}


%type type {Type*}
type(X) ::= id(A).
  { X = new IdType(ptr(A)); }
type(X) ::= type(L) TYARROW type(R).
  { X = new ArrowType(ptr(L), ptr(R)); }
%destructor type {delete $$;}


%type id {Id*}
id(X) ::= ID(I).
  { X = new Id(*get_id(I)); }
%destructor id {delete $$;}
