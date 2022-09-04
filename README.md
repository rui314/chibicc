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
    chibicc [ -o <path> ] <file>

## release notes

1.0.0 Initial version

1.0.1 adding --version -v option and fixing the -cc1 parameter that caused segmentation fault if other mandatory parameters are missing.
