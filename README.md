# BasicLisp

This is a Lisp interpreter I wrote as a learning exercise in the summer of 2014
to learn more about parsing and interpreters. Note that it is pretty unstable
and can't read files reliably, but the REPL works pretty well. To build and run:

```
mkdir build
cmake ../ && make
./basic_lisp
```

This interpreter supports functions, basic arithmetic, conditional statements,
loops, and builtin functions. See files beginning with builtins_ for
implementations of these.

## TODO

* Test and fix bugs in file loading
* Better error messages for unexpected end of file
