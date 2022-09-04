# chibicc: A Small C Compiler from Rui Ueyama

This project has been forked from https://github.com/rui314/chibicc

I want to play with it and to add some other options like choosing other linker than default ld.
It's for learning purposes too, to try to understand existing code and to extend it.
Using PVS.studio to find some potential issues and fix them.

## usage

./chibicc --help
or
./chibicc -h

    chibicc is a C compiler based on chibicc created by Rui Ueyama.
    See original project https://github.com/rui314/chibicc for more information
    this chibicc contains only some differences for now like new parameters
    chibicc usage :
    --help or -h print the help
    --version or -v print the version of chibicc
    -cc1 run the cc1 function needs -cc1-input and -cc1-output parameters
    -fuse-ld to specify other linker than ld used by default
    -x followed by one of the following values c, assembler, none
    -S generate assembly file
    chibicc [ -o <path> ] <file>

## Examples

printing the help

    ./chibicc --help

    ./chibicc -h

printing the version

    ./chibicc --version

    ./chibicc -v

compiling and generating an executable

    ./chibicc -o ./test/hello ./test/hello.c

specifying another linker than the default one

    ./chibicc -o ./test/hello ./test/hello.c -fuse-ld ld.lld

generating assembly file

    ./chibicc -S ./test/hello.c

## options always passed to the linker

it means that if you don't use the ld linker or ld.lld probably some options should be conditionned depending your linker

lld-link: warning: ignoring unknown argument '-o'
lld-link: warning: ignoring unknown argument '-m'
lld-link: warning: ignoring unknown argument '-L/usr/lib/gcc/x86_64-linux-gnu/9'
lld-link: warning: ignoring unknown argument '-L/usr/lib/x86_64-linux-gnu'
lld-link: warning: ignoring unknown argument '-L/usr/lib64'
lld-link: warning: ignoring unknown argument '-L/lib64'
lld-link: warning: ignoring unknown argument '-L/usr/lib/x86_64-linux-gnu'
lld-link: warning: ignoring unknown argument '-L/usr/lib/x86_64-pc-linux-gnu'
lld-link: warning: ignoring unknown argument '-L/usr/lib/x86_64-redhat-linux'
lld-link: warning: ignoring unknown argument '-L/usr/lib'
lld-link: warning: ignoring unknown argument '-L/lib'
lld-link: warning: ignoring unknown argument '-dynamic-linker'
lld-link: warning: ignoring unknown argument '-lc'
lld-link: warning: ignoring unknown argument '-lgcc'
lld-link: warning: ignoring unknown argument '--as-needed'
lld-link: warning: ignoring unknown argument '-lgcc_s'
lld-link: warning: ignoring unknown argument '--no-as-needed'

## TODO

- need to document all options by understanding the code!
- need to test all options to have a better understanding
- need to add some input validation to avoid calls to execvp with non-validated input (for now only limited the length of each parameter to 100 characters)

## release notes

1.0.0 Initial version

1.0.1 adding --version -v option and fixing the -cc1 parameter that caused segmentation fault if other mandatory parameters are missing.
trying to document cc1 and x options and adding a max length control parameter.
