# Mini Pipeline Runner

Runs two commands connected by a pipe, like `cat file | grep text`.

## Build and run
    make
    ./pipeline cat file.txt -- grep apple

## Test and check
    make test       # shell tests: normal paths and edge cases
    make asan       # build with AddressSanitizer + UBSan
    valgrind --leak-check=full ./pipeline cat file.txt -- grep apple

## Design note
**Data structures.** `struct command` holds one NULL-terminated argv.
`struct pipeline` holds a left and a right command. Parsing splits the
argument list at `--` and points both commands into the original argv,
so no heap memory is allocated and nothing can leak.

**Modules.** `main.c` handles usage and the exit code. `runner.c` has
`parse_pipeline` (argument parsing) and `run_pipeline` (pipe, fork, dup2,
execvp, waitpid). `runner.h` is the shared interface.

**Error handling.** Every call (pipe, fork, dup2, execvp, waitpid) is
checked and reported with perror. Both pipe ends are closed in the parent
and the children on every path, so the right command sees EOF. Exit codes:
2 = bad usage, 127 = command not found, 1 = internal error, otherwise
the exit status of the right-hand command (like a shell pipeline).

**Edge cases tested.** Empty input, missing file, bad command, missing
`--`, empty left or right command, no arguments.
EOF 
