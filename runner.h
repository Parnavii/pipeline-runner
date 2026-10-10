#ifndef RUNNER_H
#define RUNNER_H

struct command {
    char **argv;
};

struct pipeline {
    struct command left;
    struct command right;
};

int parse_pipeline(int argc, char **argv, struct pipeline *p);
int run_pipeline(const struct pipeline *p);

#endif
