// Calculate the first 10 Fibonacci numbers
var(a) = number(0)
var(b) = number(1)

println(string("Fibonacci Sequence:"))

loop_for(var(i), number(0), number(10), do(
println(var(a)),
var(temp) = add(var(a), var(b)),
var(a) = var(b),
var(b) = var(temp)
))