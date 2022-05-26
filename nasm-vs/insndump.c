#include "insndump.h"

FILE* dump_fp = 0;
static void dump_cleanup() {
    if (dump_fp != 0) {
        fclose(dump_fp);
        dump_fp = 0;
    }
}
static void dump_init(const char* p) {
    dump_cleanup();
    const char* fn = filename_append_extension(p, ".yml");
    if (fn != 0)
    {
        dump_fp = nasm_open_write(fn, NF_TEXT);

        nasm_free((void*)fn);
    }
}
static const char* const condition_name[32] = {
    "a","ae","b","be","c","ce","g","ge","l","le","na","nae",
    "nb","nbe","nc","ne","ng","nge","nl","nle","no","np",
    "ns","nz","o","p","pe","po","s","z","",""
};


static void dump_dump(int64_t lineno, char* line, const insn* instruction) {
    if (instruction != 0) {
        fprintf(dump_fp, "- instruction:\n");
        fprintf(dump_fp, "  line: %lld\n", lineno);
        fprintf(dump_fp, "  text: \"%s\"\n", line);
        if (instruction->label != 0) {
            fprintf(dump_fp, "  label: \"%s\"\n", instruction->label);
        }
        for (int i = 0; i < MAXPREFIX; i++) {
            int prx = instruction->prefixes[i];
            if (prx <= 0) {
                break;
            }
            else {
                const char* pn = prefix_name(prx);
                if (pn != 0) {
                    fprintf(dump_fp, "  prefix: \"%s\"\n", pn);
                }
            }
        }
        if (instruction->addr_size > 0) {
            fprintf(dump_fp, "  addr_size: %d\n", instruction->addr_size);
        }
        int opi = (int)instruction->opcode;
        if (opi >= 0) {
            if (opi >= FIRST_COND_OPCODE)
            {
                fprintf(dump_fp, "  opcode: \"%s %s\"\n", nasm_insn_names[opi],
                    condition_name[instruction->condition & 0x1f]);
            }
            else {
                fprintf(dump_fp, "  opcode: \"%s\"\n", nasm_insn_names[opi]);
            }   
            fprintf(dump_fp, "  operands:\n");

            for (int i = 0; i < instruction->operands && i< MAX_OPERANDS; i++) {
                operand* opr = instruction->oprs + i;
                if (opr != 0) {
                    fprintf(dump_fp, "  - operand:\n");
                    //fprintf(dump_fp, "    index: %d\n",i);
                    fprintf(dump_fp, "    type: 0x%016llx\n", opr->type);
                    if (opr->disp_size > 0) {
                        fprintf(dump_fp, "    disp_size: %d\n", opr->disp_size);
                    }
                    if (opr->basereg > 0) {
                        fprintf(dump_fp, "    base: %s\n", nasm_reg_names[(int)opr->basereg- EXPR_REG_START]);
                    }
                    if (opr->indexreg > 0) {
                        fprintf(dump_fp, "    index: %s\n", nasm_reg_names[(int)opr->indexreg - EXPR_REG_START]);
                    }
                    if (opr->scale > 0) {
                        fprintf(dump_fp, "    scale: %d\n", opr->scale);
                    }
                    if (opr->offset > 0) {
                        fprintf(dump_fp, "    offset: 0x%016llx\n", opr->offset);
                    }
                    if (opr->segment >= 0) {
                        fprintf(dump_fp, "    segment: 0x%08lx\n", opr->segment);
                    }
                }
            }

        }

    }
    else {
        fprintf(dump_fp, "- directive:\n");
        fprintf(dump_fp, "  line: %lld\n", lineno);
        fprintf(dump_fp, "  text: \"%s\"\n", line);
    }
}

static const struct ldump nasm_dump = {
    dump_init,
    dump_cleanup,
    dump_dump,
};

const struct ldump* ldump = &nasm_dump;
