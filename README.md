 <h1 align="center" style="font-weight:700;">
  🌈 <span style="color:#6a5acd;">EasyLang</span> — 
  <i>The Language That Thinks Like You</i>
</h1>


[![License](https://img.shields.io/badge/License-BSD%202--Clause-blue.svg)](LICENSE)
[![Made with C](https://img.shields.io/badge/Made%20with-C99-orange.svg)](#)
[![Platform](https://img.shields.io/badge/Platform-Linux%20|%20macOS%20|%20Windows-green.svg)](#)
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](#)

---

## 🧠 **Project Description**

**EasyLang** is a **minimalist, interpreted programming language** designed to make algorithmic thinking accessible and fun for **students, hobbyists, and educators**.  

Say goodbye to syntax clutter—focus purely on **ideas, loops, and logic**.  
With **English-like constructs**, EasyLang turns complex code into human-friendly expression, letting you **prototype algorithms in minutes, not hours.**

---

## 🌟 **Why EasyLang? – The Features That Shine**

### 🚀 **Ultra-Simple Syntax**
- ❌ **No Indentation Rules:** Write code freely—no tabs or spaces stress.  
- ❌ **No Semicolons or Braces:** Statements end naturally.  
- 🗣️ **English-like Constructs:** Code that reads like plain English.  
- 🎯 **Minimal Punctuation:** Focus on logic, not syntax memorization.  

### 🔥 **Dynamic and Forgiving Typing**
- ✨ `set x to 5` creates a variable instantly.  
- 🔁 Auto type handling: mix strings and numbers seamlessly.  
- 🚫 No declarations—just start coding.  
- 🧩 Variables are flexible: switch types dynamically.  

### ⚡ **Blazing Performance in a Tiny Package**
- 💾 Single-file interpreter (~500 lines of C).  
- 📦 Under **1 MB binary**, runs anywhere.  
- 🧠 Optimized for quick prototyping and execution.  
- 💻 Cross-platform: Linux, macOS, Windows (MinGW/WSL).  

### 🎯 **Rich Control Structures**
- 🔄 `if-then-else-end` with natural English flow.  
- 🔁 `while-do-end` for looping.  
- ⚖️ Full comparison set (`==`, `!=`, `<`, `>`, `<=`, `>=`, `and`).  
- 🧩 Nested control flow for complex logic.  

### 🧮 **Complete Function System**
- 🧠 Define with `function name(params) { body }`.  
- 📬 Pass multiple parameters, return values easily.  
- 🧍‍♂️ Local scope for each function call.  
- 🔁 Recursion supported natively.  

### 🔄 **Advanced Recursion Capabilities**
- 🏗️ Solve **Tower of Hanoi**, compute **Fibonacci**, **GCD**, **Factorial**.  
- 🧩 Perfect for teaching recursion and scope handling.  

### 📚 **Built-in Essentials**
- 🖨️ `print`, `read` for I/O.  
- ➕ `+ - * / %` arithmetic with precedence.  
- 🧵 String operations and concatenation.  
- 💬 `#` for single-line comments.  

### 🎓 **Educational Powerhouse**
- 🏫 Ideal for **classrooms and workshops**.  
- ⚙️ Quick prototyping environment.  
- 🧠 Focused on **algorithmic clarity** over syntax.  
- 💡 Zero setup: just download and run.  

### 🔧 **Extensible and Hackable**
- 🔓 Open Source (BSD 2-Clause).  
- 🧩 Easy to extend lexer, parser, and evaluator.  
- 🧬 Create your own language dialects.  
- 🌍 Built by and for the programming education community.  




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

[Download EasyLang Documentation](documentation/easylang_documentation.pdf)

## 🚀 **Quick Start Examples**

### 🧾 Basic Variables and Output
```easylang
set name to "EasyLang"
set version to 1.0
print "Welcome to " + name + " v" + version
```

### 🎯 Conditional Logic
```easylang
set age to 20
if age >= 18 then
    print "You are an adult"
else
    print "You are a minor"
end
```
###  🧮 Functions Without Recursion
```easylang
function max(a, b) {
    if a > b then
        return a
    else
        return b
    end
}

function circle_area(radius) {
    return 3.14159 * radius * radius
}

print "Larger number: " + max(5, 3)
print "Circle area: " + circle_area(5)
```

### 🔂 Advanced Recursive Functions
```easylang
# Fibonacci sequence
function fib(n) {
    if n <= 1 then
        return n
    end
    return fib(n - 1) + fib(n - 2)
}

# Factorial
function fact(n) {
    if n <= 1 then
        return 1
    end
    return n * fact(n - 1)
}

# Tower of Hanoi
function hanoi(n, from, to, aux) {
    if n == 1 then
        print "Move disk 1 from " + from + " to " + to
        return
    end
    hanoi(n - 1, from, aux, to)
    print "Move disk " + n + " from " + from + " to " + to
    hanoi(n - 1, aux, to, from)
}

print "Fibonacci(6): " + fib(6)
print "Factorial(5): " + fact(5)
print "Tower of Hanoi with 3 disks:"
hanoi(3, "A", "C", "B")

```
## 🤝 Contributing

We welcome contributions from everyone! 🚀

Here’s how you can help improve **EasyLang**:

1. **Fork** the repository  
2. **Create** a feature branch  
3. **Commit** your changes  
4. **Submit** a pull request  

Let’s make coding education easier together ❤️  

---

### 🌟 Start coding in minutes, not hours.  
**Experience the simplicity of EasyLang today! 🚀**
