// Demonstrate attempt/rescue catching a div-by-zero
attempt(
do(
println(string("Trying division by zero...")),
var(x) = div(number(10), number(0)),
println(string("This won't print"))
),
rescue(err, do(
print(string("Caught error: ")),
println(var(err))
))
)

println(string("---"))

// Demonstrate custom error inside a cooked function
cook(double_it, args(x), do(
explode(string("I refuse to double anything!"))
))

attempt(
do(
serve(double_it, number(5))
),
rescue(err, do(
print(string("Caught custom error: ")),
println(var(err))
))
)