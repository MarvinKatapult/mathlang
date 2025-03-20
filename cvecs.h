/*
    MIT License

    Copyright (c) 2024 Marvin Katapult
    https://github.com/marvinkatapult/cvecs

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef __VEC_H__
#define __VEC_H__
#include <stdbool.h>
#include <stddef.h>

#define DEFAULT_CAP_VEC     69

/** BEGIN OF VEC **/
#define VEC_ENTRY_OTHER     0
#define VEC_ENTRY_NUM       1
#define VEC_ENTRY_DEC       2
#define VEC_ENTRY_STR       3

typedef struct VecEntry {
    void * val;
    unsigned int type;
} VecEntry;

typedef struct Vec {
    size_t count;
    size_t capacity;
    VecEntry * entries;
} Vec;

Vec createVec(void);              // Creates a Vec with default capacity
Vec createVecEx(size_t capacity); // Creates a Vec with capacity
void freeVec(Vec vec);            // Frees Memory of Vec

bool appendVecNum(Vec * vec, long val);               // Appends Number to Vec
bool appendVecStr(Vec * vec, const char * str);       // Appends String to Vec
bool appendVecDec(Vec * vec, double val);             // Appends Decimal Number to Vec
bool appendVec(Vec * vec, void * val);                // Appends generic to Vec
void * updateVec(Vec vec, void * val, size_t i);        // Updates Value to Vec
bool deleteVec(Vec * vec, void * ptr);                // Deletes Ptr from Vec
bool deleteVecRange(Vec * vec, size_t start, size_t end ); // Removes range(start, end) from vec
/** END OF VEC **/

/** BEGIN OF STRVEC **/
#ifndef __USE_CSTRING__ // There is another Library from me called CString https://github.com/MarvinKatapult/cstring
typedef struct StrVec {
    size_t count;
    size_t capacity;
    char ** vals;
} StrVec;

StrVec createStrVec(void);              // Creates a StrVec with capacity (Normally put DEFAULT_CAP_VEC)
StrVec createStrVecEx(size_t capacity); // Creates a StrVec with capacity (Normally put DEFAULT_CAP_VEC)
void freeStrVec(StrVec str_vec);        // Frees Memory of StrVec

bool appendStrVec(StrVec * str_vec, const char * str);           // Appends to StrVec
bool updateStrVec(StrVec str_vec, const char * str, size_t pos); // Updates Value at position
bool setStrVecCapacity(StrVec * str_vec, size_t cap);            // Sets Capacity and reallocs
/** END OF STRVEC **/

#else

/** BEGIN OF STRVEC WITH CSTRING **/

#include <cstring.h>
typedef struct StrVec {
    size_t count;
    size_t capacity;
    String * vals;
} StrVec;
StrVec createStrVec(void);              // Creates a StrVec with capacity (Normally put DEFAULT_CAP_VEC)
StrVec createStrVecEx(size_t capacity); // Creates a StrVec with capacity (Normally put DEFAULT_CAP_VEC)
void freeStrVec(StrVec str_vec);        // Frees Memory of StrVec

bool appendStrVec(StrVec * str_vec, const char * str);           // Appends to StrVec
bool insertStrVec(StrVec * str_vec, const char * str, size_t index);
bool updateStrVec(StrVec str_vec, const char * str, size_t pos); // Updates Value at position
bool setStrVecCapacity(StrVec * str_vec, size_t cap);            // Sets Capacity and reallocs

#endif // __USE_CSTRING__

/** END OF STRVEC **/

/** BEGIN OF INTVEC **/
typedef struct IntVec {
    size_t count;
    size_t capacity;
    long * vals;
} IntVec;

IntVec createIntVec(void);              // Creates a IntVec with default capacity
IntVec createIntVecEx(size_t capacity); // Creates a IntVec with capacity
void freeIntVec(IntVec int_vec);        // Frees Memory of IntVec

bool appendIntVec(IntVec * int_vec, long val);        // Appends to IntVec
bool setIntVecCapacity(IntVec * int_vec, size_t cap); // Sets Capacity and reallocs
/** END OF INTVEC **/

#ifndef __USE_CSTRING__
#define PRINT_STR_VEC(STR_VEC)    for (size_t i = 0; i < STR_VEC.count; i++) printf("%s\n", STR_VEC.vals[i])
#define PRINT_STR_VEC2(STR_VEC)   for (size_t i = 0; i < STR_VEC.count; i++) printf("%s;", STR_VEC.vals[i]); printf("\n")
#else
#define PRINT_STR_VEC(STR_VEC)    for (size_t i = 0; i < STR_VEC.count; i++) printf("%s\n", STR_VEC.vals[i].s)
#define PRINT_STR_VEC2(STR_VEC)   for (size_t i = 0; i < STR_VEC.count; i++) printf("%s;", STR_VEC.vals[i].s); printf("\n")
#endif // __USE_CSTRING__

#define PRINT_INT_VEC(INT_VEC)    for (size_t i = 0; i < INT_VEC.count; i++) printf("%ld\n", INT_VEC.vals[i])
#define PRINT_INT_VEC2(INT_VEC)   for (size_t i = 0; i < INT_VEC.count; i++) printf("%ld;", INT_VEC.vals[i]); printf("\n")

#define PRINT_VEC2(VEC, FORMAT)  for (size_t i = 0; i < VEC.count; i++) printf(FORMAT, VEC.entries[i].val)
#define PRINT_VEC(VEC) \
    for (size_t i = 0; i < VEC.count; i++) { \
        switch (VEC.entries[i].type) { \
            case VEC_ENTRY_NUM: \
                printf("%d\n", *(int *)VEC.entries[i].val); \
                break; \
            case VEC_ENTRY_DEC: \
                printf("%f\n", *(double *)VEC.entries[i].val); \
                break; \
            case VEC_ENTRY_STR: \
                printf("%s\n", (char *)VEC.entries[i].val); \
                break; \
            case VEC_ENTRY_OTHER: \
                printf("%p\n", VEC.entries[i].val); \
                break; \
        } \
    }

#endif // __VEC_H__
