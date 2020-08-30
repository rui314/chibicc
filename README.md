# chibicc: A Teaching C Compiler

chibicc is a C compiler for educational purposes. I wrote it with the
following goals in mind:

- Simple: The compiler should be as simple as possible to help the
  reader understand how it works.

- Small: The compiler should be small enough to be covered in a
  semester.

- Demonstrating an incremental approach: It's git history should start
  from a minimal compiler implementation, and the compiler should gain
  one feature at a time with a small incremental patch. That should
  help the reader understand how to write a large program from
  scratch, which requires a different kind of skill set than writing a
  patch for an existing large project.

- Correctness: It should correctly capture the semantics of the major
  but obscure C language features, such as "usual arithmetic
  conversion" or "arrays decay into pointers".

- Completeness: While the compiler doesn't have to support all C
  language features, it should be able to compile nontrivial programs
  including itself.

I believe all the above goals are met. chibicc's source code is in my
opinion small and pretty easy to read, and not only the current state
of the code but _every commit_ was written with readability in
mind. The first commit is a minimalistic compiler that compiles an
integer to a program that exits with the given number as the exit
code. Then I added operators (e.g. `+` or `-`), local variable,
control structures (e.g. `if` or `while`), function call, function
definition, global variable, and other language features one at a
time. As the compiler gained features with a series of small patches,
the language the compiler accepts looked more and more like the real C
language.

When I found a bug in a previous commit, I edited the commit by
rewriting the git history instead of creating a new commit. This is an
unusual and undesirable development style for most projects, but for
my purpose, keeping clean commit history is more important than
avoiding git forced-pushes.

chibicc's internal design was carefully chosen to naturally support
the core C language semantics. It supports many C language features
including the preprocessor. chibicc is written in C and can compile
itself. I didn't try to avoid certain C features when writing this
compiler for ease of self-hosting, so I can say that it can compile at
least one ordinary C program.

Being said that, there are many missing features. They are left as an
exercise for the reader.

## Internals

chibicc consists of the following stages:

- Tokenize: A tokenizer takes a string as an input, breaks it into
  a list of tokens and returns them.

- Preprocess: A preprocessor takes as an input a list of tokens and
  output a new list of macro-expanded tokens. It interprets
  preprocessor directives while expanding macros.

- Parse: A recursive descendent parser constructs abstract syntax trees
  from the output of the preprocessor. It also adds a type to each
  AST node.

- Codegen: A code generator emits an assembly text for given AST nodes.

Currently, there's no optimization pass, but there's a plan to add one
to elimnate obvious inefficiencies in the chibicc's output.

Note that chibicc allocates memory using malloc() but never calls
free(). Once memory is allocated, it won't be released until the
process exits.  This may look like an odd design choice, and perhaps
it is, but in practice this memory management policy (or the lack of
thereof) works well for short-lived programs like chibicc. This design
eliminates all scaffolding and complexity of manual memory management
and makes the compiler much simpler than it would otherwise have been.
If the memory consumption becomes a real issue, you can plug in [Boehm
GC](https://en.wikipedia.org/wiki/Boehm_garbage_collector) for
automatic memory management.

## Book

I'm writing an online book about the C compiler. The draft is
available at https://www.sigbus.info/compilerbook, though currently it
is in Japanese. I have a plan to translate to English once it's
complete.

## About the Author

I'm Rui Ueyama. I'm the creator of [8cc](https://github.com/rui314/8cc),
which is a hobby C compiler, and also the original creator of the
current version of [LLVM lld](https://lld.llvm.org) linker, which is a
production-quality linker used by various operating systems and
large-scale build systems.

## References

- [tcc](https://bellard.org/tcc/): A small C compiler written by
  Fabrice Bellard. I learned a lot from this compiler, but the design
  of tcc and chibicc are largely different. In particular, tcc is a
  one-pass compiler, while chibicc is a multi-pass one.

- [lcc](https://github.com/drh/lcc): Another small C compiler. The
  creators wrote a
  [book](https://sites.google.com/site/lccretargetablecompiler/) about
  the internals of lcc, which I found a good resource to see how a
  compiler is implemented.

- [An Incremental Approach to Compiler
  Construction](http://scheme2006.cs.uchicago.edu/11-ghuloum.pdf)

## Project ideas

- Add missing features
- Port to different ISAs such as RISC-V
- Rewrite the compiler in a different language than C
- Use LLVM as a backend
- Add optimization passes
