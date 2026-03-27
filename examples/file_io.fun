// File writing and reading demo
var(fw) = file_open(string("data.txt"), string("w"))
file_write(var(fw), string("Hello File! This was written in FunLang.\n"))
file_close(var(fw))

var(fr) = file_open(string("data.txt"), string("r"))
var(content) = file_read(var(fr))
file_close(var(fr))

print(string("Read from file: "))
print(var(content))