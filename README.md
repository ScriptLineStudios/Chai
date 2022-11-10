# Untitled-Compiler
![Lines of code](https://img.shields.io/tokei/lines/github/ScriptLineStudios/Untitled-Compiler)

An untitled compiler written in C. So far there is is'nt a solid plan other than to make a simple language :)

# Compilation Targets

# In Development
- Intel x86_64 Linux - Being implemented by ScriptLine
- Intel x86_64 Windows - Being implmented by MousieDev

# Planned
- WASM

# Getting Started
```
1. git clone https://github.com/ScriptLineStudios/Untitled-Compiler
2. make
3. ./bin/comp.exe examples/strings.uc
```

# Language Reference

## Variables
```
    let int_var = 10;
    let string_var = "Hello World";
```

## Printing to standard output
```
    let var = "Hello, World!";
    stdout(var);
```

## If statements
```
    let x = 7;
    if (x != 8) {
        stdout(90);
    }
```
