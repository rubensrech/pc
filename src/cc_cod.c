#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int getSizeOf(int type) {
    switch (type) {
        case DATATYPE_FLOAT: return 4;
        case DATATYPE_INT: return 4;
        case DATATYPE_BOOL: return 0;       // ??
        case DATATYPE_STRING: return 0;     // ??
        case DATATYPE_CHAR: return 1;       // ??
        default: return 0;
    }
}