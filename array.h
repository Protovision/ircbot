#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>
#include <string.h>

struct _array {
	void *base, *dend, *end;
	size_t element_size;
};

typedef struct _array array;

void	array_init(array *a, size_t element_size);
void	array_destroy(array *a);
void	array_clear(array *a);

void	array_push(array *a, const void *val);
int	array_pop(array *a, void *val);
void	array_unshift(array *a, const void *val);
int	array_shift(array *a, void *val);

int	array_insert(array *a, ssize_t index, const void *val);
int	array_remove(array *a, ssize_t index, size_t n);

void* array_at(array *a, ssize_t index);
size_t array_count(array *a);
size_t array_capacity(array *a);
void	array_resize(array *a, size_t n);
int	array_slice(array *a, ssize_t first, size_t n, void *buffer);

ssize_t array_find(array *a, const void *val);

void _array_expand(array *a);

#endif
