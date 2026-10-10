#include "runner.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    struct pipeline p;
    if (parse_pipeline(argc, argv, &p) != 0) {
        fprintf(stderr, "usage: %s CMD [ARGS...] -- CMD [ARGS...]\n",
                argc > 0 ? argv[0] : "pipeline");
        return 2;
    }
    int rc = run_pipeline(&p);
    return rc == -1 ? 1 : rc;
}
