#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

struct Vector
{
    void** data;
    size_t size;
    size_t capacity;
};

void vectorInit(Vector* vec, size_t initialCapacity);
void vectorPush(Vector* vec, void* element);
void* vectorGet(Vector* vec, size_t index);
void vectorFree(Vector* vec);
void freeAllocatedVectorCells(Vector* vec);

#endif // VECTOR_H
