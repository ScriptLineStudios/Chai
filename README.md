# Chai
## Currently being rewritten here: https://github.com/ScriptLineStudios/Chai/tree/complete-rewrite

Compiler for the Chai programming language.
Chai is a fast, compiled dynamically typed language.

# Compilation Targets

## In Development
- Intel x86_64 Linux - Being implemented by ScriptLine
- Intel x86_64 Windows - Being implmented by MousieDev

## Planned
- WASM

# Getting Started
```
1. git clone https://github.com/ScriptLineStudios/Untitled-Compiler
2. make
3. ./bin/comp.exe examples/strings.uc
```

# Language Reference

## Variables
```rust
let int_var = 10;
let string_var = "Hello World";
```

## Printing to standard output
```rust
let var = "Hello, World!";
stdout(var);
```

## If statements
```rust
let x = 7;
if (x != 8) {
    stdout(90);
}
```

## While loops
```rust
let foo = 0;
while (foo != 100) {
    stdout(foo);
    foo = foo + 1;
}
```

## Lists
```rust
let lst = [1, 2, 3, 4, 5];
let i = 0;
while (i != 5) {
    stdout(lst[i]);
    i = i + 1;
}
```

## Functions
```rust
func foo(bar) {
    if (bar != 200) {
        stdout("The value of bar is not 200!");
    }
}

foo(201);
```
