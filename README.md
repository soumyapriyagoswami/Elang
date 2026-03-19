<div align="center">

<picture>
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=6,11,20&height=200&section=header&text=eLang&fontSize=80&fontAlignY=35&desc=The%20Language%20That%20Thinks%20Like%20You&descAlignY=58&descSize=20&fontColor=ffffff&animation=fadeIn" width="100%"/>
</picture>

<br>

<!-- Animated typing effect via readme-typing-svg -->
[![Typing SVG](https://readme-typing-svg.demolab.com?font=Fira+Code&size=18&pause=1000&color=58A6FF&center=true&vCenter=true&width=600&lines=Minimalist+%7C+Interpreted+%7C+Built+in+C;No+semicolons.+No+braces.+Just+ideas.;Prototype+algorithms+in+minutes%2C+not+hours.;English-like+syntax+%E2%80%94+anyone+can+read+it.)](https://github.com/soumyapriyagoswami/eLang)

<br>

[![Made with C99](https://img.shields.io/badge/Made%20with-C99-FF6B35?style=for-the-badge&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![License BSD 2-Clause](https://img.shields.io/badge/License-BSD%202--Clause-4A90D9?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-2ECC71?style=for-the-badge&logo=linux&logoColor=white)](https://github.com/soumyapriyagoswami/eLang)
[![PRs Welcome](https://img.shields.io/badge/PRs-Welcome-FF69B4?style=for-the-badge)](CONTRIBUTING.md)
[![Open Source](https://img.shields.io/badge/Open%20Source-❤-E74C3C?style=for-the-badge)](https://github.com/soumyapriyagoswami/eLang)
[![Star on GitHub](https://img.shields.io/github/stars/soumyapriyagoswami/eLang?style=for-the-badge&logo=github&color=yellow)](https://github.com/soumyapriyagoswami/eLang)

<br>

> ### *"Stop wrestling syntax. Start writing ideas."*

<br>

---

<sub>Crafted with ❤️ by <a href="https://github.com/soumyapriyagoswami"><b>Soumyapriya Goswami</b></a></sub>

---

</div>

---

<br>

## ◈ What is eLang?

**eLang** is a **minimalist, interpreted programming language** built in pure C — designed to make algorithmic thinking accessible and joyful for **students, hobbyists, and educators**.

No semicolons. No braces. No indentation rules.  
Just **clean, English-like code** that lets you prototype algorithms in minutes.

```elang
set name to "eLang"
if name == "eLang" then
    print "Hello, world. You found me."
end
```

<br>

---

## ◈ Feature Highlights

<br>

**✦ Ultra-Simple Syntax**
> No indentation rules · No semicolons · No braces  
> English-like constructs · Minimal punctuation — focus on *logic*, not ceremony.

**✦ Dynamic, Forgiving Typing**
> `set x to 5` — that's it. No declarations, no type annotations.  
> Mix strings and numbers. Switch types on the fly.

**✦ Blazing Fast & Tiny**
> ~500 lines of C · Under **1 MB** binary  
> Cross-platform: Linux, macOS, Windows (MinGW/WSL)

**✦ Rich Control Flow**
> `if–then–else–end` · `while–do–end`  
> Full comparisons: `== != < > <= >= and`  
> Arbitrarily nested logic — no limits.

**✦ Complete Function System**
> `function name(params) { body }`  
> Multiple parameters · Return values · Local scope · Native recursion

**✦ Educational Powerhouse**
> Zero setup · Classroom-ready · Recursion-first pedagogy  
> Ideal for teaching Tower of Hanoi, Fibonacci, GCD, Factorial

<br>

---

## ◈ Getting Started

### Prerequisites

- GCC or any C99-compliant compiler
- Linux / macOS / Windows (MinGW or WSL)

### Install in 30 seconds

```bash
git clone https://github.com/soumyapriyagoswami/eLang.git
cd eLang
./eLang
```

### Run your first program

Create `hello.elang`:
```elang
print("Hello, eLang!")
```

Execute it:
```bash
./eLang hello.elang
# → Hello, eLang!
```

<br>

---

## ◈ Code Examples

<br>

### Variables & Output

```elang
set name    to "eLang"
set version to 1.0
print "Welcome to " + name + " v" + version
```

<br>

### Conditional Logic

```elang
set age to 20
if age >= 18 then
    print "You are an adult"
else
    print "You are a minor"
end
```

<br>

### Functions

```elang
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
print "Circle area:   " + circle_area(5)
```

<br>

### Recursion — Classic Algorithms

```elang
# ── Fibonacci ────────────────────────────────────
function fib(n) {
    if n <= 1 then
        return n
    end
    return fib(n - 1) + fib(n - 2)
}

# ── Factorial ────────────────────────────────────
function fact(n) {
    if n <= 1 then
        return 1
    end
    return n * fact(n - 1)
}

# ── Tower of Hanoi ───────────────────────────────
function hanoi(n, from, to, aux) {
    if n == 1 then
        print "Move disk 1 from " + from + " to " + to
        return
    end
    hanoi(n - 1, from, aux, to)
    print "Move disk " + n + " from " + from + " to " + to
    hanoi(n - 1, aux, to, from)
}

print "Fibonacci(6):  " + fib(6)
print "Factorial(5):  " + fact(5)
print "Tower of Hanoi (3 disks):"
hanoi(3, "A", "C", "B")
```

<br>

---

## ◈ Documentation

Full language reference available as a PDF:

📄 **[Download eLang Documentation](documentation/eLang_documentation.pdf)**

<br>

---

## ◈ Contributing

eLang is open source and built by its community. Contributions of all kinds are welcome.

```
1. Fork  →  2. Branch  →  3. Commit  →  4. Pull Request
```

Whether it's fixing a bug, extending the language, or improving docs — every contribution matters. Let's make programming education more human together. ❤️

<br>

---

<div align="center">

<br>

**Start coding in minutes, not hours.**  
*Experience the simplicity of eLang today.* 🚀

<br>

[![Star on GitHub](https://img.shields.io/github/stars/soumyapriyagoswami/eLang?style=for-the-badge&logo=github&color=yellow)](https://github.com/soumyapriyagoswami/eLang)
[![Follow](https://img.shields.io/github/followers/soumyapriyagoswami?label=Follow%20Soumyapriya&style=for-the-badge&logo=github&color=58A6FF)](https://github.com/soumyapriyagoswami)

<br>

<picture>
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=6,11,20&height=100&section=footer&animation=fadeIn" width="100%"/>
</picture>

</div>
