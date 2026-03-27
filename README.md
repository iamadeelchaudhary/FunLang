# 🎢 FunLang: The Official Step-by-Step Tutorial

Welcome to **FunLang**! FunLang is a strict, minimalist, yet surprisingly powerful interpreted programming language. 

Before we start, you must understand the **Golden Rule of FunLang**:
> **Everything is a function call.**

There are no keywords, no stray operators (except for variable assignment), and no block brackets `{}`. Everything you do—from math to loops to if-statements—is done by calling a function and passing arguments inside parentheses `()`.

Let's dive in!

---

## Step 1: Hello World & Printing

To print to the screen, we use the `print()` and `println()` (print with newline) functions. Since everything is a function call, even raw text must be wrapped in the `string()` function.

` ` `
// This is a comment!
println(string("Hello, World!"))
print(string("Welcome to "))
println(string("FunLang!"))
` ` `

---

## Step 2: Variables and Data Types

To declare or use a variable, wrap its name in the `var()` function. FunLang supports four basic types: `string()`, `number()`, `bool()`, and `null()`. 

*Note: In FunLang, integers and decimals are both just `number()`.*

` ` `
var(name) = string("Alice")
var(age) = number(25)
var(pi) = number(3.14159)
var(is_programmer) = bool(true)
var(empty_thing) = null()

print(string("My name is "))
println(var(name))
` ` `

---

## Step 3: Math and Arithmetic

Since there are no operators like `+` or `-`, you use math functions. You can nest these function calls as deeply as you want!

Available functions: `add`, `sub`, `mul`, `div`, `mod`, `pow`.

` ` `
var(a) = number(10)
var(b) = number(3)

var(sum) = add(var(a), var(b))        // 10 + 3 = 13
var(diff) = sub(var(a), var(b))       // 10 - 3 = 7
var(rem) = mod(var(a), var(b))        // 10 % 3 = 1

// Nested math: (2 * 5) + 1
var(complex) = add(mul(number(2), number(5)), number(1))

println(var(complex)) // Prints 11
` ` `

---

## Step 4: Logic and Comparisons

Just like math, comparisons and boolean logic are handled via functions.

* **Comparisons:** `eq` (==), `neq` (!=), `gt` (>), `lt` (<), `gte` (>=), `lte` (<=)
* **Logic:** `and`, `or`, `not`

` ` `
var(x) = number(10)

// Check if x is greater than 5 AND less than 20
var(is_valid) = and(
    gt(var(x), number(5)),
    lt(var(x), number(20))
)

println(var(is_valid)) // Prints true
` ` `

---

## Step 5: Conditional Branching (If / Else)

FunLang uses the `iff()` function for if-statements. It takes three arguments:
1. The condition.
2. The `do()` block (what happens if true).
3. The `otherwise()` block (what happens if false).

*The `do()` block is a special function that executes multiple statements in order.*

` ` `
var(age) = number(18)

iff(
    gte(var(age), number(18)),
    do(
        println(string("You are an adult!")),
        println(string("Welcome to the club."))
    ),
    otherwise(
        println(string("You are too young."))
    )
)
` ` `

---

## Step 6: Loops

FunLang supports two types of loops: `loop_while()` and `loop_for()`.

### The While Loop
Runs as long as the condition is true.

` ` `
var(counter) = number(0)

loop_while(
    lt(var(counter), number(3)),
    do(
        print(string("Counter is: ")),
        println(var(counter)),
        // Increment the counter
        var(counter) = add(var(counter), number(1))
    )
)
` ` `

### The For Loop
Takes a variable, a start number (inclusive), an end number (exclusive), and a `do()` block.

` ` `
println(string("Counting to 5:"))

loop_for(var(i), number(1), number(6), do(
    println(var(i))
))
` ` `

---

## Step 7: Lists (Arrays)

You can group values together using `list()`.

` ` `
// Create a list
var(fruits) = list(string("apple"), string("banana"))

// Add a new item to the end
push(var(fruits), string("cherry"))

// Access an item by its index (0-based)
var(first_fruit) = item(var(fruits), number(0))
println(var(first_fruit)) // Prints "apple"

// Get the length of the list
var(len) = list_len(var(fruits))
println(var(len)) // Prints 3
` ` `

---

## Step 8: Creating Your Own Functions

In FunLang, you don't just "define" and "call" functions—you **cook** them and **serve** them!

* `cook(name, args(...), do(...))` defines the function.
* `serve(name, arg1, arg2)` executes the function.
* `give_back(value)` returns a value from the function.

` ` `
// 1. Cook the function
cook(multiply_by_two, args(x), do(
    var(result) = mul(var(x), number(2)),
    give_back(var(result))
))

// 2. Serve the function
var(ans) = serve(multiply_by_two, number(21))

print(string("The answer is: "))
println(var(ans)) // Prints 42
` ` `

---

## Step 9: Error Handling

Things go wrong. When they do, you want to catch the error instead of crashing your program. Use `attempt()` and `rescue()`. You can also throw your own custom errors using `explode()`.

` ` `
attempt(
    do(
        // This will trigger a MathError!
        var(impossible) = div(number(10), number(0)),
        println(string("This line will never run."))
    ),
    rescue(err, do(
        print(string("Uh oh! We caught an error: ")),
        println(var(err))
    ))
)

// Throwing a custom error:
// explode(string("Something went terribly wrong!"))
` ` `

---

## Step 10: File I/O (Reading and Writing)

FunLang can read from and write to text files seamlessly. 
Modes available: `"r"` (read), `"w"` (write), `"a"` (append).

` ` `
// Write to a file
var(fw) = file_open(string("secret.txt"), string("w"))
file_write(var(fw), string("FunLang is awesome!"))
file_close(var(fw))

// Read from the file
var(fr) = file_open(string("secret.txt"), string("r"))
var(content) = file_read(var(fr))
file_close(var(fr))

println(string("File contents:"))
println(var(content))
` ` `

---

## Conclusion

You now know everything there is to know about FunLang! 
Remember to wrap everything in functions, keep an eye on your parentheses, and have fun cooking up amazing logic! 🎉