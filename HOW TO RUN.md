# 🚀 How to Run FunLang Code

Once you have written your FunLang scripts (like the examples in the tutorial), you need to execute them using the compiled FunLang interpreter. Follow these simple steps to run your code.

## Prerequisites
Ensure you have successfully compiled the FunLang interpreter. If you haven't done this yet, open your terminal in the project folder and compile it using GCC:

**Mac / Linux / Windows (Git Bash or PowerShell):**
```bash
gcc -std=c11 -Wall -Wextra -O2 src/*.c -o funlang -lm
```

This command generates an executable file named `funlang` (or `funlang.exe` on Windows).

---

## Step 1: Save Your Script
Write your FunLang code and save it in a plain text file with a `.fun` extension. For example, create a file named `my_script.fun` and add your code:

```fun
// my_script.fun
println(string("Running my first FunLang script!"))
```

## Step 2: Open Your Terminal
Open your command prompt, PowerShell, or terminal and navigate to the folder where your `funlang` executable is located.

## Step 3: Execute the Code
Run the interpreter from the command line, passing the path to your `.fun` script as an argument.

### On Linux or macOS:
```bash
./funlang my_script.fun
```

### On Windows (Command Prompt or PowerShell):
```powershell
.\funlang.exe my_script.fun
```

*(Note: If your script is inside a folder, for example `examples/`, you would run `./funlang examples/my_script.fun` or `.\funlang.exe examples\my_script.fun`)*

---

## Troubleshooting

* **Command not found:** Ensure you are in the correct directory where the `funlang` executable was built, and that you are using `./` before the executable name on Mac/Linux or `.\` on Windows PowerShell.
* **File not found:** Double-check the path to your `.fun` file. If there are spaces in the folder names, wrap the path in quotes (e.g., `./funlang "my scripts/test.fun"`).
* **Parse/Lexer Errors:** FunLang is very strict! Make sure every statement is a proper function call and you haven't forgotten any closing parentheses `)`.