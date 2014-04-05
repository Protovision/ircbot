#include "array.h"

void array_init(array *a, size_t element_size) {
	a->base = a->dend = a->end = NULL;
	a->element_size = element_size;
}

void array_destroy(array *a) {
	if (a->base) {
		free(a->base);
		a->base = NULL;
	}
}

void array_clear(array *a) {
	a->dend = a->base;
}

void array_push(array *a, const void *val) {
	register size_t size = a->element_size;
	if (a->dend == a->end)
		_array_expand(a);
	memcpy(a->dend, val, size);
	a->dend += size;
}

int array_pop(array *a, void *val) {
	register size_t size = a->element_size;
	if (a->dend == a->base) return -1;
	a->dend -= size;
	memcpy(val, a->dend, size);
	return 0;
}

void array_unshift(array *a, const void *val) {
	register size_t size = a->element_size;
	if (a->end - a->dend < size)
		_array_expand(a);
	memmove(a->base + size, a->base, a->dend - a->base);
	memcpy(a->base, val, size);
}

int array_shift(array *a, void *val) {
	register size_t size = a->element_size;
	if (a->dend == a->base) return -1;
	memcpy(val, a->base, size);
	memmove(a->base, a->base + size, a->dend - (a->base + size));
	a->dend -= size;
	return 0;
}

int array_insert(array *a, ssize_t index, const void *val) {
	register size_t size = a->element_size;
	void *pos;
	if (a->end - a->dend < size)
		_array_expand(a);
	if (index >= 0) 
		pos = a->base + (index * size);
	else
		pos = a->dend + (index * size);
	if (pos >= a->dend) return -1;
	memmove(pos + size, pos, a->dend - pos);
	a->dend += size;
	memcpy(pos, val, size);
	return 0;
}

int array_remove(array *a, ssize_t index, size_t n) {
	register size_t size = a->element_size;
	void *pos;
	if (index >= 0) 
		pos = a->base + (index * size);
	else
		pos = a->dend + (index * size);
	if (pos >= a->dend) return -1;
	memmove(pos, pos + size, a->dend - (pos + size));	
	a->dend -= size;
	return 0;
}

void *array_at(array *a, ssize_t index) {
	if (index >= 0)
		return a->base + (index * a->element_size);
	else
		return a->dend + (index * a->element_size);
}

size_t array_count(array *a) {
	return (a->dend - a->base) / (a->element_size);
}

size_t array_capacity(array *a) {
	return (a->end - a->base) / (a->element_size);
}

void array_resize(array *a, size_t n) {
	size_t len = array_count(a);
	if (n < len) {
		a->dend = a->base + (n * a->element_size);
	} else if (n > len) {
		while (a->dend + (n * a->element_size) >= a->end)
			_array_expand(a);
		memset(a->dend, 0, n * a->element_size);
		a->dend += n * a->element_size;	
	}
}

int array_slice(array *a, ssize_t index, size_t n, void *buffer) {
	void *pos;
	size_t i;
	register size_t size = a->element_size;
	if (index < 0)
		pos = a->dend + (index * size);
	else
		pos = a->base + (index * size);
	if (pos + (n * size) >= a->dend) return -1;
	memcpy(buffer, pos, n * size);
	return 0; 
}

ssize_t array_find(array *a, const void *val) {
	void *p;
	for (p = a->base; p != a->dend; p += a->element_size) {
		if (memcmp(p, val, a->element_size) == 0)
			return (p - a->base) / a->element_size;
	}
	return -1;
}

void _array_expand(array *a) {
	long dend_off, end_off;
	dend_off = a->dend - a->base;
	end_off = a->end - a->base;
	a->base = realloc(a->base, 
		end_off ? (end_off *= 2) : (end_off = a->element_size * 4));
	a->dend = a->base + dend_off;
	a->end = a->base + end_off;
}
