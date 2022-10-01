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
    -cc1 run the cc1 function needs -cc1-input (-cc1-output optional) parameter
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
    -z <arg> Pass <arg> on to the linker.
    -soname <arg> Pass -soname <arg> on to the linker.
    --version-script <arg> Pass --version-script <arg> to the linker.
    -I<path> Pass path to the include directories
    -L<path> Pass path to the lib directories
    -D<macro> define macro example -DM13
    -U<macro> undefine macro example -UM13
    -s to strip all symbols during linkage phasis
    -M -MD -MP -MMD -MF <arg> -MT <arg> -MQ <arg> compiler write a list of input files to
        stdout in a format that "make" command can read. This feature is
        used to automate file dependency management
    -fpic or -fPIC Generate position-independent code (PIC)
    -fno-pic disables the generation of position-independent code with relative address references
    -fcommon is the default if not specified, it's mainly useful to enable legacy code to link without errors
    -fno-common specifies that the compiler places uninitialized global variables in the BSS section of the object file.
    -static  pass to the linker to link a program statically
    -shared pass to the linker to produce a shared object which can then be linked with other objects to form an executable.
    -hashmap-test to test the hashmap function
    -idirafter <dir> apply to lookup for both the #include "file" and #include <file> directives.
    -### to dump all commands executed by chibicc
    -debug to dump all commands executed by chibicc in a log file in /tmp/chibicc.log
    -E Stop after the preprocessing stage; do not run the compiler proper.
        The output is in the form of preprocessed source code, which is sent to the standard output.
        Input files that don’t require preprocessing are ignored.
    -rpath <dir> Add a directory to the runtime library search path this parameter is passed to the linker.
        This is used when linking an ELF executable with shared objects.
        All -rpath arguments are concatenated and passed to the runtime linker,
        which uses them to locate shared objects at runtime.
        The -rpath option is also used when locating shared objects
        which are needed by shared objects explicitly included in the link.
    -dumpmachine it's required by some projects returns x86_64-linux-gnu
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

By default the symbol tables is populated:

    chibicc -o ./test/mydefine ./test/define.c
    objdump -t ./test/mydefine
    ./test/mydefine:     file format elf64-x86-64
    SYMBOL TABLE:
    0000000000000000 l    df *ABS*  0000000000000000              crt1.o
    0000000000400320 l     O .note.ABI-tag  0000000000000020              \_\_abi_tag
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
    "-m32"
    "-pthread"
    "-pedantic"
    "-nostdinc"
    "-mno-red-zone"
    "-w"

## Dockerfile and devcontainer

Adding dockerfile suggested by 0xc0 https://github.com/0xc0/chibicc/tree/new
To build :

    docker build -t chibicc .

Adding also a devcontainer for those that want to use visual code inside a container (needs vs code extension remote-containers).

## Examples of C projects compiled using chibicc

tcc : tcc compiler (https://github.com/LuaDist/tcc.git)

    chibicc -o tcc tcc.c -DTCC_TARGET_X86_64 -O2 -g -Wall -Wno-pointer-sign -lm -ldl
    chibicc -o libtcc1.o -c lib/libtcc1.c -O2 -Wall
    ar rcs libtcc1.a libtcc1.o
    chibicc -o libtcc.o -c libtcc.c -DTCC_TARGET_X86_64 -O2 -g -Wall -Wno-pointer-sign
    ar rcs libtcc.a libtcc.o
    ./texi2pod.pl tcc-doc.texi tcc.pod
    pod2man --section=1 --center=" " --release=" " tcc.pod > tcc.1
    chibicc -o libtcc_test tests/libtcc_test.c libtcc.a -I. -O2 -g -Wall -Wno-pointer-sign -lm -ldl

for some projects you need to define CC=chibicc before executing ./configure.

curl : https://github.com/curl/curl.git

    CC=chibicc CFLAGS=-fpic LDFLAGS=-fpic ./configure --with-openssl
    make

        make[2]: Entering directory ...
        CC       libcurl_la-altsvc.lo
        CC       libcurl_la-amigaos.lo
        ...
        CC       ../lib/curl_multibyte.o
        CC       ../lib/version_win32.o
        CC       ../lib/dynbuf.o
        CCLD     curl

## Limits

Some C projects doesn't compile for now.

VLC

    VLC doesn't compile with chibicc because it has some extended assembly inline that are not managed yet. Even if for this part I'll try to use gcc it failed during linking with multiple definitions. If I use gcc to compile VLC it compiles fine. Perhaps mixing chibicc and gcc is not a great idea!

## TODO

- trying to compile other C projects from source to see what is missing or which bug we have with chibicc.
- trying to manage other assembly functions like \_\_asm\_\_("xchgb %b0,%h0": "=Q"(x):"0"(x));

## issues and pull requests fixed

    - Add dockerfile #23 pull request from 0xc0
    - Issue #30 from ludocode
    - Typecheck void type expressions in return and assignment #41 pull request from StrongerXi
    - Binary add segfaults when at least 1 arg is non-numeric, and both are non-pointer types #42 pull request from StrongerXi
    - It's UB to have a call to va_start without a corresponding call to va_end #51 pull request from camel-cdr
    - Fix bug in codegen.c if (ty->size == 4) instead of if (ty->size == 12) #52 pull request from mtsamis
    - difficulty in understanding priorities. #54 pull request from ihsinme
    - Fixed bug in preprocessor process extraneous tokens. #95 pull request from memleaker
    - Hashmap: Do not insert duplicate keys #98 pull request from boki1
    - issue #65 assembler error on larger than 32-bit bitfields from GabrielRavier
    - issue #28 bitfield validation from zamfofex
    - issue #48 asan issues with memcmp from math4tots
    - issue #36 assertion when using empty structs from edubart
    - issue #31 Anonymously named bitfield member segfaults compiler from Anonymously named bitfield member segfaults compiler fixed by zamfofex
    - issue #45 When anonymous union field is specified by designator, compiler crashe from vain0x fixed by zamfofex
    - issue #80 Size of string is wrong from tyfkda
    - issue #72 Internal error when initializing array of long with string literals from GabrielRavier
    - issue #47 postfix tails on compound literals from vain0x
    - issue #62 Nested designators error from sanxiyn
    - issue #37 Using goto inside statement expressions gives an error from edubart
    - issue #63 Function type parameter without identifier errs from sanxiyn
    - issue #69 Internal error on long double initializer from GabrielRavier
    - issue #71 Codegen error on _Atomic long double operation assignments from GabrielRavier
    - Fix atomic fetch operations #101 pull request from chjj
    - issue #107 string intialized by function-like failed during compilation
    - issue #108 if a macro ends a line and the next line starts by "#ifdef" the "#" is not recognized starting from beginning of the line.
    - issue #109 managing #warning as preprocess instruction
    - issue #110 union initialized by  "input_control_param_t it = { .id = p_input, .time.i_val = 1};" failed due to comma.
    - issue #113 depending where is _Atomic parsing failed
        ./issues/issue113.c:7:         char *_Atomic str; /**< Current value (if character string) */
                                     ^ expected ','

## release notes

1.0.11 Fixing issue #113 about \_Atomic when it's placed after the type. Fixing other issue like issue #108 sometimes some #ifdef are not recognized if a macro ends the previous line and the next line starting by a preprocessing instruction. Managing differently -soname and adding option -z, and --version-script. Adding -debug option to write commands in /tmp/chibicc.log (later I'll add some debugs info/values on this file to help to fix bugs). Adding 2 functions in stdatomic.h needed by VLC atomic_compare_exchange_strong_explicit(object, expected, desired, success, failure) that returns false for now and atomic_compare_exchange_weak_explicit(object, expected, desired, success, failure) that returns false too. Managing .lo files (libtool object). Adding generic path for Fix 'gcc library path is not found' on some platforms #108 by [Stardust8502](https://github.com/Stardust8502/chibicc).

## old release notes

1.0.0 Initial version

1.0.1 adding --version -v option and fixing the -cc1 parameter that caused segmentation fault if other mandatory parameters are missing.
trying to document cc1 and x options and adding a max length control parameter. Adding documentation for other parameters too.

1.0.2 fixing issue with cc1 parameter only -cc1-input parameter is mandatory. Finishing parameters documentation.

1.0.3 trying to add some sanitizing functions and check if arguments have valid allowed characters (probably too strict!).

1.0.4 Fixing the output directory for dependencies "xxx.d" when -MD option is set, the output "xxx.d" file is generated in the current directory if no -o parameter defined or in the directory defined by -o parameter (issue #30). Adding Dockerfile and a devcontainer for those who want to play with that (issue #23). Typecheck void type expressions in return and assignment (issue #41). Binary add segfaults when at least 1 arg is non-numeric, and both are non-pointer types (issue #42). It's UB to have a call to va_start without a corresponding call to va_end (issue #51). Fix bug in codegen.c (issue #52). fix issue in main.c (issue #54). Fixed bug in preprocessor process extraneous tokens (issue #95). Hashmap: Do not insert duplicate keys (issue #98).

1.0.5 Fixing assembler error on larger than 32-bit bitfields (issue #65) by GabrielRavier. Fixing bitfield validation by zamfofex (issue #28). Replacing memcmp by strncmp (issue #48) by math4tots. Fixing empty structs as one-byte object (issue #36) by edubart.

1.0.6 Anonymously named bitfield member segfaults compiler (issue #31). Fixing anonymous union field (issue #45) by zamfofex. Fixing wrong size of string initialized by braces (issue #80) by zamfofex.

1.0.7 Internal error when initializing array of long with string literals (issue #72) by GabrielRavier. Fixing postfix tails on compound literals (issue #47) by zamfofex. Trying to fix nested designators error(issue #62). Fixing Using goto inside statement expressions gives an error (issue #37) by zamfofex. Fixing function type parameter without identifier errs (issue #63) by zamfofex.

1.0.8 Internal error on long double initializer (issue #69). Codegen error on atomic long double operation assignments (issue #71). Fix atomic fetch operations (issue #101) by chjj. Adding -soname < arg > and -rpath < dir > parameters (needed to be able to compile curl from source). Soname is used to create a symbolic link and rpath is passed to the linker. Testing chibicc with some C projects (compiling fine tcc, curl).

1.0.9 Adding pthread and pedantic to omitted parameters list. Added -fno-pic parameter. Adding ignored parameter: m32, nostdinc. Adding -dumpmachine parameter. Fixing issue with \_\_has_attribute(diagnose_if). Adding lots of attributes that can be applied to function (for now ignored them) : "\_\_attribute\_\_((noreturn))", "\_\_attribute\_\_((returns_twice))",
"\_\_attribute\_\_((noinline))", "\_\_attribute\_\_((always_inline))", "\_\_attribute\_\_((flatten))", "\_\_attribute\_\_((pure))",
"\_\_attribute\_\_((nothrow))", "\_\_attribute\_\_((sentinel))", "\_\_attribute\_\_((format))", "\_\_attribute\_\_((format_arg))",
"\_\_attribute\_\_((no_instrument_function))", "\_\_attribute\_\_((section))", "\_\_attribute\_\_((constructor))",
"\_\_attribute\_\_((destructor))", "\_\_attribute\_\_((used))", "\_\_attribute\_\_((unused))", "\_\_attribute\_\_((deprecated))",
"\_\_attribute\_\_((weak))", "\_\_attribute\_\_((alias))", "\_\_attribute\_\_((malloc))",
"\_\_attribute\_\_((warn_unused_result))", "\_\_attribute\_\_((nonnull))", "\_\_attribute\_\_((externally_visible))",
"\_\_attribute\_\_((visibility(\"default\")))", "\_\_attribute\_\_((visibility(\"hidden\")))",
"\_\_attribute\_\_((visibility(\"protected\")))", "\_\_attribute\_\_((visibility(\"internal\")))"

1.0.10 Fixing issue about string initialized by function-like (issue #107). Fixing issue when a macro ends a line and the next line starts by "#ifdef" the "#" is not recognized starting from beginning of the line (issue #108). Managing \#warning as preprocessor instruction (issue #109). Fixing issue with union initializer when comma found like "input_control_param_t it = { .id = p_input, .time.i_val = 1};" (issue #110) and input_control_param_t it = { .id = p_input,} (issue #113). Removing fix for issue 106 (caused other issues with VLC when trying to compile).
