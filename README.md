# chibicc: A Small C Compiler from Rui Ueyama

This project has been forked from https://github.com/rui314/chibicc

I want to play with it and to add some other options like choosing other linker than default ld.
It's for learning purposes too, to try to understand existing code and to extend it.
Using PVS.studio to find some potential issues and fix them.

Trying to work on issues and pull requests done in the original repository.

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
    -x Specify the language of the following input files.
        Permissible languages include: c assembler none
        'none' means revert to the default behavior of
        guessing the language based on the file's extension.
    -S generate assembly file
    -o path to output executable if omitted a.out generated
    -c path to source to compile
    -Xlinker <arg> Pass <arg> on to the linker.
    -Wl,<options> Pass comma-separated <options> on to the linker.
    -I<path> Pass path to the include directories
    -L<path> Pass path to the lib directories
    -D<macro> define macro example -DM13
    -U<macro> undefine macro example -UM13
    -s to strip all symbols during linkage phasis
    -M -MD -MP -MMD -MF <arg> -MT <arg> -MQ <arg> compiler write a list of input files to
        stdout in a format that "make" command can read. This feature is
        used to automate file dependency management
    -fpic or -fPIC Generate position-independent code (PIC)
    -fcommon is the default if not specified, it's mainly useful to enable legacy code to link without errors
    -fno-common specifies that the compiler places uninitialized global variables in the BSS section of the object file.
    -static  pass to the linker to link a program statically
    -shared pass to the linker to produce a shared object which can then be linked with other objects to form an executable.
    -hashmap-test to test the hashmap function
    -idirafter <dir> apply to lookup for both the #include "file" and #include <file> directives.
    -### to dump all commands executed by chibicc
    -E Stop after the preprocessing stage; do not run the compiler proper.
        The output is in the form of preprocessed source code, which is sent to the standard output.
        Input files that don’t require preprocessing are ignored.
    chibicc [ -o <path> ] <file>

## Examples

Note that probably you need to define the include parameter and lib parameter to be able to execute these examples outside the chibicc directory. If you forgot you will have similar error message :

    /usr/include/stdio.h:33: #include <stddef.h>
                                    ^ stddef.h: cannot open file: No such file or directory

To solve it, add the include and lib parameter like below assuming that we are in the parent chibicc directory :

        chibicc pointerofpointers.c -I./chibicc/include -L./chibicc/lib

printing the help

    ./chibicc --help

    ./chibicc -h

printing the version

    ./chibicc --version

    ./chibicc -v

compiling and generating an executable

    ./chibicc -o ./test/hello ./test/hello.c

specifying another linker than the default one here lld :

    ./chibicc -o ./test/hello ./test/hello.c -fuse-ld ld.lld

specifying another linker than the default one here mold (https://github.com/rui314/mold)

    ./chibicc -o ./test/hello ./test/hello.c -fuse-ld mold

generating assembly file

    ./chibicc -S ./test/hello.c

generating a.out file if no parameter -o provided

    ./chibicc ./test/hello.c

generating the object only : it generates the pointerofpointers.o

    ./chibicc -c pointerofpointers.c -I./chibicc/include -L./chibicc/lib

generating executable from objects :

    ./chibicc pointerofpointers.o -o pointersofpointers

defining condition at compile time :

    ./chibicc -o ./test/mydefine ./test/define.c -DM13 -I../include -L../lib
    ./test/mydefine
    3

undefining condition at compile time :

    ./chibicc -o ./test/mydefine ./test/define.c -UM13 -I../include -L../lib
    ./test/mydefine
    4

By default the symbol tables is populated by default :

    chibicc -o ./test/mydefine ./test/define.c
    objdump -t ./test/mydefine
    ./test/mydefine:     file format elf64-x86-64
    SYMBOL TABLE:
    0000000000000000 l    df *ABS*  0000000000000000              crt1.o
    0000000000400320 l     O .note.ABI-tag  0000000000000020              __abi_tag
    0000000000000000 l    df *ABS*  0000000000000000              crtstuff.c

Stripping symbol tables during linkage phasis adding -s parameter :

    ./chibicc -o ./test/mydefine ./test/define.c -s
    objdump -t ./test/mydefine
    ./test/mydefine:     file format elf64-x86-64
    SYMBOL TABLE:
    no symbols

other way to generate assembly file

    ./chibicc ./test/define.c -cc1 -cc1-input ./test/define.c -cc1-output mydefine.s

generating dependencies in std output :

    ./chibicc  ./test/define.c -M
    define.o: \
    ./test/define.c \
    /usr/include/stdio.h \
    /usr/include/x86_64-linux-gnu/bits/libc-header-start.h \
    /usr/include/features.h \

generating dependencies in a output file with ".d" extension and generating executable a.out :

    ./chibicc  ./test/define.c -MD
    ls *.d
    define.d

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

## options ignored

List of options ignored :

    "-W"
    "-g"
    "-std="
    "-ffreestanding"
    "-fno-builtin"
    "-fno-omit-frame-pointer"
    "-fno-stack-protector"
    "-fno-strict-aliasing"
    "-m64"
    "-mno-red-zone"
    "-w"

## Dockerfile and devcontainer

Adding dockerfile suggested by 0xc0 https://github.com/0xc0/chibicc/tree/new
To build :

    docker build -t chibicc .

Adding also a devcontainer for those that want to use visual code inside a container (needs vs code extension remote-containers).

## TODO

- need to document all options by understanding the code!
- need to test all options to have a better understanding
- need to add some input validation to avoid calls to execvp with non-validated input (for now only limited the length of each parameter to 100 characters)

## issues and pull requests fixed

    - Add dockerfile #23 pull request from 0xc0
    - Issue #30 from ludocode
    - Typecheck void type expressions in return and assignment #41 pull request from StrongerXi
    - Binary add segfaults when at least 1 arg is non-numeric, and both are non-pointer types #42 pull request from StrongerXi
    - It's UB to have a call to va_start without a corresponding call to va_end #51 pull request from camel-cdr
    - Fix bug in codegen.c if (ty->size == 4) instead of if (ty->size == 12) #52 pull request from mtsamis
    - difficulty in understanding priorities. #54 pull request from ihsinme
    - Fixed bug in preprocessor process extraneous tokens. #95 pull request from memleaker

## release notes

1.0.0 Initial version

1.0.1 adding --version -v option and fixing the -cc1 parameter that caused segmentation fault if other mandatory parameters are missing.
trying to document cc1 and x options and adding a max length control parameter. Adding documentation for other parameters too.

1.0.2 fixing issue with cc1 parameter only -cc1-input parameter is mandatory. Finishing parameters documentation.

1.0.3 trying to add some sanitizing functions and check if arguments have valid allowed characters (probably too strict!).

1.0.4 Fixing the output directory for dependencies "xxx.d" when -MD option is set, the output "xxx.d" file is generated in the current directory if no -o parameter defined or in the directory defined by -o parameter (issue #30). Adding Dockerfile and a devcontainer for those who want to play with that (issue #23). Typecheck void type expressions in return and assignment (issue #41). Binary add segfaults when at least 1 arg is non-numeric, and both are non-pointer types (issue #42). It's UB to have a call to va_start without a corresponding call to va_end (issue #51). Fix bug in codegen.c (issue #52). fix issue in main.c (issue #54). Fixed bug in preprocessor process extraneous tokens (issue #95).
