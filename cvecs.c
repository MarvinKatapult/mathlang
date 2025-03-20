#include "cvecs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t calcNewVecCap(size_t capacity) {
    return (size_t)((float)capacity + ((float)capacity / DEFAULT_CAP_VEC) * DEFAULT_CAP_VEC);
}

static bool setVecCapacity(void ** start, size_t cap, size_t size) {
    if (!start) return false;
    
    void * res = realloc(*start, size * cap);
    if (!res) {
        fprintf(stderr, "Realloc failed: %s %d\n", __FILE__, __LINE__);
        return false;
    }
    *start = res;

    return true;
}

static char * copyCString(const char * str) {
    char * ret = malloc(sizeof(char) * strlen(str) + 1);
    if (ret) {
        strcpy(ret, str);
    }

    return ret;
}

#ifndef __USE_CSTRING__
StrVec createStrVec(void) {
    char ** vals = calloc(DEFAULT_CAP_VEC, sizeof(char *));

    return (StrVec) {
        .count = 0,
        .capacity = DEFAULT_CAP_VEC,
        .vals = vals
    };
}

StrVec createStrVecEx(size_t capacity) {
    char ** vals = calloc(capacity, sizeof(char *));

    return (StrVec) {
        .count = 0,
        .capacity = capacity,
        .vals = vals
    };
}

void freeStrVec(StrVec str_vec) {
    for (size_t i = 0; i < str_vec.count; i++) {
        free(str_vec.vals[i]);
    }
    free(str_vec.vals);
}

bool appendStrVec(StrVec * str_vec, const char * str) {
    if (!str_vec) return false;
    char * new_str = copyCString(str);

    str_vec->count++;
    if (str_vec->capacity < str_vec->count) {
        setStrVecCapacity(str_vec, calcNewVecCap(str_vec->capacity));
    }

    str_vec->vals[str_vec->count-1] = new_str;
    return true;
}

bool updateStrVec(StrVec str_vec, const char * str, size_t pos) {
    if (pos >= str_vec.count) return false;
    free(str_vec.vals[pos]);
    str_vec.vals[pos] = copyCString(str);
    return true;
}

bool setStrVecCapacity(StrVec * str_vec, size_t cap) {
    if (!str_vec) return false;
    
    setVecCapacity((void **)&str_vec->vals, cap, sizeof(char *));
    str_vec->capacity = cap;

    return true;
}
#else
StrVec createStrVec() {
    String * vals = calloc(DEFAULT_CAP_VEC, sizeof(String));

    return (StrVec) {
        .count = 0,
        .capacity = DEFAULT_CAP_VEC,
        .vals = vals
    };
}

StrVec createStrVecEx(size_t capacity) {
    String * vals = calloc(capacity, sizeof(String));

    return (StrVec) {
        .count = 0,
        .capacity = capacity,
        .vals = vals
    };
}

void freeStrVec(StrVec str_vec) {
    for (size_t i = 0; i < str_vec.count; i++) {
        freeString(str_vec.vals[i]);
    }
    free(str_vec.vals);
}

bool appendStrVec(StrVec * str_vec, const char * str) {
    if (!str_vec) return false;

    str_vec->count++;
    if (str_vec->capacity < str_vec->count) {
        setStrVecCapacity(str_vec, calcNewVecCap(str_vec->capacity));
    }

    str_vec->vals[str_vec->count-1] = createStringExt(str);
    return true;
}

bool insertStrVec(StrVec * str_vec, const char * str, size_t index) {
    if (!str_vec || index > str_vec->count) return false;

    if (index - 1 == str_vec->count) return appendStrVec(str_vec, str);

    str_vec->count++;
    if (str_vec->capacity < str_vec->count) {
        setStrVecCapacity(str_vec, calcNewVecCap(str_vec->capacity));
    }

    String * to_move = &str_vec->vals[index];
    const size_t num_to_move = str_vec->vals + str_vec->count - 1 - to_move;
    printf("ToMove:%ld\n", num_to_move);
    memmove(to_move + 1, to_move, sizeof(String) * num_to_move);
    str_vec->vals[index] = createStringExt(str);

    return true;
}

bool updateStrVec(StrVec str_vec, const char * str, size_t pos) {
    if (pos >= str_vec.count) return false;
    setString(str_vec.vals + pos, str);
    return true;
}

bool setStrVecCapacity(StrVec * str_vec, size_t cap) {
    if (!str_vec) return false;
    
    setVecCapacity((void **)&str_vec->vals, cap, sizeof(String));
    str_vec->capacity = cap;

    return true;
}
#endif // __USE_CSTRING__

IntVec createIntVec() {
    long * vals = calloc(DEFAULT_CAP_VEC, sizeof(long));

    return (IntVec) {
        .count = 0,
        .capacity = DEFAULT_CAP_VEC,
        .vals = vals
    };
}

IntVec createIntVecEx(size_t capacity) {
    long * vals = calloc(capacity, sizeof(long));

    return (IntVec) {
        .count = 0,
        .capacity = capacity,
        .vals = vals
    };
}

void freeIntVec(IntVec int_vec) {
    if (int_vec.vals) free(int_vec.vals);
}

bool appendIntVec(IntVec * int_vec, long val) {
    if (!int_vec) return false;

    int_vec->count++;
    if (int_vec->capacity < int_vec->count) {
        if (!setIntVecCapacity(int_vec, calcNewVecCap(int_vec->capacity))) return false;
    }
    int_vec->vals[int_vec->count-1] = val;

    return true;
}

bool setIntVecCapacity(IntVec * int_vec, size_t cap) {
    if (!int_vec) return false;
    
    if (!setVecCapacity((void **)&int_vec->vals, cap, sizeof(int))) return false;
    int_vec->capacity = cap;

    return true;
}

Vec createVec() {
    VecEntry * entries = calloc(DEFAULT_CAP_VEC, sizeof(VecEntry));

    return (Vec) {
        .count = 0,
        .capacity = DEFAULT_CAP_VEC,
        .entries = entries,
    };
}

Vec createVecEx(size_t capacity) {
    VecEntry * entries = calloc(capacity, sizeof(VecEntry));

    return (Vec) {
        .count = 0,
        .capacity = capacity,
        .entries = entries,
    };
}
void freeVec(Vec vec) {
    for (size_t i = 0; i < vec.count; i++) {
        if (vec.entries[i].type != VEC_ENTRY_OTHER) free((vec.entries + i)->val);
    }
    free(vec.entries);
}

bool appendVecNum(Vec * vec, long val) {
    if (!vec) return false;

    vec->count++;
    if (vec->capacity < vec->count) {
        if (!setVecCapacity((void **)&vec->entries, calcNewVecCap(vec->capacity), sizeof(VecEntry))) return false;
    }

    long * new_val = malloc(sizeof(long));
    if (!new_val) {
        fprintf(stderr, "Malloc failed %s %d\n", __FILE__, __LINE__);
        return false;
    }

    *new_val = val;
    vec->entries[vec->count-1].val = new_val;
    vec->entries[vec->count-1].type = VEC_ENTRY_NUM;

    return true;
}

bool appendVecStr(Vec * vec, const char * str) {
    if (!vec) return false;

    vec->count++;
    if (vec->capacity < vec->count) {
        if (!setVecCapacity((void **)&vec->entries, calcNewVecCap(vec->capacity), sizeof(VecEntry))) return false;
    }

    char * new_val = copyCString(str);
    if (!new_val) {
        fprintf(stderr, "Malloc failed %s %d\n", __FILE__, __LINE__);
        return false;
    }
    vec->entries[vec->count-1].val = new_val;
    vec->entries[vec->count-1].type = VEC_ENTRY_STR;

    return true;
}

bool appendVecDec(Vec * vec, double val) {
    if (!vec) return false;

    vec->count++;
    if (vec->capacity < vec->count) {
        if (!setVecCapacity((void **)&vec->entries, calcNewVecCap(vec->capacity), sizeof(VecEntry))) return false;
    }

    double * new_val = malloc(sizeof(double));
    if (!new_val) {
        fprintf(stderr, "Malloc failed %s %d\n", __FILE__, __LINE__);
        return false;
    }
    *new_val = val;
    vec->entries[vec->count-1].val = new_val;
    vec->entries[vec->count-1].type = VEC_ENTRY_DEC;

    return true;
}

bool appendVec(Vec * vec, void * val) {
    if (!vec) return false;

    vec->count++;
    if (vec->capacity < vec->count) {
        if (!setVecCapacity((void **)&vec->entries, calcNewVecCap(vec->capacity), sizeof(VecEntry))) return false;
    }

    vec->entries[vec->count-1].val = val;
    vec->entries[vec->count-1].type = VEC_ENTRY_OTHER;

    return true;
}

void * updateVec(Vec vec, void * val, size_t i) {
    
    VecEntry * e = &vec.entries[i];
    void * old = e->val;
    if (e->type != VEC_ENTRY_OTHER) {
        free(old);
        old = NULL;
    }
    
    e->val = val;
    e->type = VEC_ENTRY_OTHER;
    return old;
}

bool deleteVec(Vec * vec, void * ptr) {
    for (size_t i = vec->count - 1;; i--) {
        if (ptr == vec->entries[i].val) {
            if (!deleteVecRange(vec, i, i)) return false;
        }

        if (i == 0) break;
    }
    return true;
}

bool deleteVecRange(Vec * vec, size_t start, size_t end ) {
    if (!vec) return false;
    if (start > end) {
        // Swap
        size_t t = start;
        start = end;
        end = t;
    }

    if (end > vec->count - 1) return false;

    for (size_t i = start; i <= end; i++) {
        VecEntry * entry = &vec->entries[i];
        if (entry->type != VEC_ENTRY_OTHER) {
            free(entry->val);
        }
        entry->type = VEC_ENTRY_OTHER;
        entry->val = NULL;
    }

    size_t n = vec->count - end - 1;
    vec->count -= end - start + 1;
    memmove(vec->entries + start, vec->entries + end + 1, sizeof(VecEntry) * n);

    return true;
}
