# MiniML—A tiny functional language

## Installation

Needs a C++11 compiler, [lemon], and [ragel] to build. (The [release] contains
pre-made versions of the output files.) Then `make` should Just Work (tested
with GCC 4.9 on Debian and Xcode's version of clang on OS X).

[lemon]: http://www.hwaci.com/sw/lemon/
[ragel]: http://www.colm.net/open-source/ragel/
[release]: https://github.com/andy-morris/miniml/releases


## Quick language guide

- Has types `int`, `bool`, `string`, tuples (written `(type₁, type₂, ...)`, and
  (higher-order) functions (written `type₁ -> type₂`).

- Operators: `<-> || && < <= > >= == != + - * /` with what is hopefully the
  obvious precedence.
    - `<->` is "iff", i.e. equality on booleans, with the lowest precedence.
      Writing this, I just realised `==` doesn't work on strings, only ints.
      Sorry about that.

- Other expressions:
    - `if branch then_case else_case`
    - `fn (x: type) => expr`
    - `expr: type` (type ascription, though not actually useful…)
    - `expr₁; expr₂` (sequencing)
    - `(expr₁, expr₂, ...)` (tuples, also `()`)
    - `tuple.0`, `tuple.1`, etc (0-based projection)
        - I don't mean to say it needs to be a tuple literal, just something
          of some tuple type with enough elements.

- REPL inputs terminated with `;;`.

- Declarations:

  ~~~
  val x: type = def       // type optional
  val rec x: type = def   // recursive; type not optional
  fun x (y: type₁) (...): type₂ = def
      // abbreviation for:
      // val rec x: type₁ -> … -> type₂ =
      //     fn (y: type₁) => ... => def
  ~~~

- Builtins (defined in `src/init_env.cxx` for magic functions or `prelude.mml`
  for nonmagic ones):

  ~~~
  // magic:
  print: string -> ()
  app: string -> string -> string
  string_int: int -> string
  newline: () -> ()
  use: string -> ()
  // in prelude:
  println: string -> ()
  print_int: int -> ()
  even, odd: int -> bool
  ~~~

  Since `use` needs a reference to the environment, it's defined in `Repl`'s
  constructor instead (`src/repl.cxx`).

- “Modules” (well, files) have syntax `name => decl₁ decl₂ ...`. Note that `;;`
  isn't used in files, only interactively.

- As you probably notced by now, comments are of the `//` variety.


## Notes

- GCC [doesn't check exhaustiveness of `switch`][gcc_switch], which is what the
  `#ifdef __GNUC__` business is all about.
- `#pragma GCC diagnostic` is understood by Clang too.

[gcc_switch]: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=28236
