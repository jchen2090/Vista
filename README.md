# Vista

Vista is a small, toy programming language with a compiler written in C++ that targets LLVM IR and emits native object files.

The compiler pipeline is:

```
Lexer -> Parser -> AST -> TypeChecker -> Codegen (LLVM) -> Object file
```

## Requirements

- CMake 3.14+
- A C++17 compiler (GCC/Clang)
- LLVM (with `LLVMConfig.cmake` available)
- Ninja (optional, recommended)
- A C compiler (`cc`/`gcc`/`clang`) to link the emitted object file

## Building

```sh
cmake -B build -G Ninja
cmake --build build
```

This produces two executables in `build/`:

- `build/compiler` — the Vista compiler
- `build/tests` — the Catch2 unit test suite

### Running the tests

```sh
./build/tests
```

## Usage

Compile a `.vista` file to an object file:

```sh
./build/compiler hello.vista
```

The compiler always writes its output to `output.o` in the current directory.

Link the object file into a native executable and run it:

```sh
cc output.o -o hello
./hello
```

### Example

`hello.vista`:

```vista
int x = 6
int y = 2
str a = "hello world"

cout(a)
cout(x * y)
```

```sh
./build/compiler hello.vista
cc output.o -o hello
./hello
```

Output:

```
hello world
12
```

## Language Syntax

Vista is currently a minimal language. Every statement must be terminated by a newline.

### Data types

| Type    | Description            | Example         |
| ------- | ---------------------- | --------------- |
| `int`   | 32-bit integer         | `42`, `-3`      |
| `float` | Single-precision float | `3.14`, `2.5`   |
| `str`   | String                 | `"hello world"` |

### Variable declarations

```vista
int x = 5
float pi = 3.14
str greeting = "hello"
```

The type is required and must match the value's type. Variables must be declared before use, and a variable cannot be redeclared.

### Assignment to a new variable from an existing one

```vista
int a = 1
int b = a
```

### Printing

`cout` prints a value followed by a newline:

```vista
cout(42)            # prints 42
cout(3.14)          # prints 3.14
cout("hello world") # prints hello world
cout(x)             # prints the value of variable x
```

### Expressions

Binary arithmetic on numeric values is supported with `+`, `-`, `*`, and `/`:

```vista
int x = 6
int y = 2
cout(x * y)   # prints 12
cout(x / y)   # prints 3
cout(x + y)   # prints 8
cout(x - y)   # prints 4
```

Arithmetic is only defined between matching numeric types. Operations involving strings, or mixing `int` and `float`, are type errors.

## Current limitations

- Statements must end with a newline; a file ending without one is rejected.
- No comments, control flow, functions, or boolean types yet.
- `cout` only prints a single expression; the value must be a variable, literal, or binary expression.
- The compiler emits `output.o` with a fixed name; there is no CLI for choosing an output path.

## Project layout

```
src/
  lexer/     Tokenization (keywords, literals, operators)
  token/     Token type definitions
  parser/    Recursive-descent parser producing the AST
  ast/       AST node definitions and debug printing
  type/      Type checking and type definitions
  codegen/   LLVM IR generation and object file emission
  main.cpp   Driver: file -> lexer -> parser -> typechecker -> codegen
tests/       Catch2 unit tests for the lexer, parser, and type checker
```

## TODO

- Implement conditionals
- Implement loops
- Expand on binary operations (comparison operators, modulo)

