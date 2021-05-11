The code in this directory allows you to create interprocedural control flow graphs from
a program written in C, and then check if the model verify a specific LTL formula.

## Artchitecture

The clang_cfg directory contains everything to get a .dot representation of the
program's cfg.

The cfg_linker directory contains a code that creates a graph composed of
linked .dot graphs into an interprocedural graph.

The automaton_check folder contains code regarding transforming the cfg into
a spot twa-automaton and applying model checking algorithms on it.

## Quick try out

To try it out, just type
$ make

You can probably tweak the Makefile to try out your own programs, otherwise follow next step.

## Try it out on your own program

To try this program and verify properties on your own program:
- Type $ make exe
- Dump all your source code (.c and .h files) into a folder called ./prg/
- Type $ ./launch_on_prg "formula_to_verify"

## Dependencies

To run it, you need (at least!) to install not too old versions of the following:
- clang
- llvm
- the boost library
- sed
- gawk
- dot
- spot

- - - - - - - - - - -

## TODO:
- Have a lazy insertion of functions
- Manage cases where we have function calls inside parenthesis of another function call
