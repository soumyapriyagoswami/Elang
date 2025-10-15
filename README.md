# EasyLang

[![License](https://img.shields.io/badge/License-BSD%202--Clause-blue.svg)](LICENSE)

## Project Description

EasyLang is a minimalist, interpreted programming language crafted to make algorithmic thinking accessible and fun for students, hobbyists, and educators. Say goodbye to syntax overload—focus purely on ideas, loops, and logic. With its English-like constructs, EasyLang turns complex concepts into simple, readable code, empowering you to prototype algorithms in minutes rather than hours.
Why EasyLang? The Features That Shine

## Features

- **Ultra-Simple Syntax**:  
  No semicolons, braces, or indentation rules. Statements end naturally with newlines or dots—like writing a story. Perfect for beginners who want to code without fighting the language.

- **Dynamic and Forgiving Typing**:  
  Variables spring to life on first use (`set x to 5`). Auto-coercion handles number-string mixes seamlessly (e.g., `"Score: " + 100 → "Score: 100"`), reducing errors and boilerplate.

- **Blazing Performance in a Tiny Package**:  
  The single-file C interpreter (~500 LOC) compiles to a <1MB binary. Runs lightning-fast on desktops, laptops, or even embedded devices—ideal for Raspberry Pi or IoT prototypes.

- **Built-in Essentials, No Bloat**:  
  Core I/O (`print`/`read`), arithmetic (`+ - * / %`), comparisons (`== < >`), and controls (`if-then-else`, `while-do`) cover 80% of everyday needs. Logical conditions chain effortlessly.

- **Educational Powerhouse**:  
  Designed for teaching—mirror natural language (`if age > 18 then print "Adult" else print "Minor" end`). Great for classrooms, workshops, or self-study; no setup hassles.

- **Extensible and Hackable**:  
  Open-source under MIT. Tweak the lexer, parser, or evaluator in C for custom dialects. From prime checkers to mini-banks, EasyLang is your sandbox for language experiments.


---

## Installation

### Prerequisites
- GCC or any C compiler
- Linux / macOS / Windows (with MinGW or WSL for Windows)

### Steps
1. Clone this repository:
```bash
git clone https://github.com/soumyapriyagoswami/EasyLang.git
cd EasyLang
./easylang
```
Create a sample EasyLang script hello.easylang:

```bash
print("Hello, EasyLang!")
```

Run the script:
```bash
./easylang hello.elang
```

Expected output:
```bash
Hello, EasyLang!
```

You can experiment with variables, loops, and functions following EasyLang’s simple syntax. More examples will be added soon.

## Documentation

Full documentation is available as a PDF:

[Download EasyLang Documentation](docs/EasyLang_Documentation.pdf)

