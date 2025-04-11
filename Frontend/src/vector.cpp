#include "vector.h"

#include <assert.h>
#include <stdlib.h>

#include "debug.h"

void vectorInit(Vector* vec, size_t initialCapacity)
{
    vec->size = 0;
    vec->capacity = initialCapacity;

    vec->data = (void**)calloc(vec->capacity, sizeof(void*));
    assert(vec->data);
}

void vectorPush(Vector* vec, void* element)
{
    if (vec->size >= vec->capacity)
    {
        vec->capacity *= 2;

        vec->data = (void**)realloc(vec->data, sizeof(void*) * vec->capacity);
        assert(vec->data);
    }

    vec->data[vec->size++] = element;
}

void* vectorGet(Vector* vec, size_t index)
{
    if (index < vec->capacity)
    {
        return vec->data[index];
    }

    fprintf(stderr, "Error: vector index out of bounds\n");
    assert(0);
}

void vectorFree(Vector* vec)
{
    free(vec->data);
    vec->data = nullptr;
}

void freeAllocatedVectorCells(Vector* vec)
{
    for (size_t i = 0; i < vec->size; i++)
    {
        free(vectorGet(vec, i));
    }
}
