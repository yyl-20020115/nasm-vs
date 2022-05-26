#ifndef INSN_DUMP_H
#define INSN_DUMP_H
#include <nasm.h>

struct ldump {
    void (*init)(const char* fname);
    void (*cleanup)(void);
    void (*dump)(int64_t lineno, char* line, insn* i);
};

const struct ldump* ldump;

#endif