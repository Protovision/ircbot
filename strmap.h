/*
 * Key/Value Map of strings
 * Implemented as a binary search tree
 */
#ifndef STRMAP_H
#define STRMAP_H

#include <stdlib.h>
#include <string.h>

struct _strmap_node {
	struct _strmap_node *left, *right;
	char data[0];
};

typedef struct {
	size_t count;
	struct _strmap_node *root;
} strmap_t;

#define strmap_count(M) ((M)->count)
#define strmap_clear(M) (strmap_destroy((M)))

void	strmap_init(strmap_t *m);
void	strmap_destroy(strmap_t *m);

int	strmap_remove(strmap_t *m, const char *key);
int	strmap_iterate(strmap_t *m, int (*callback)(const char *key, const char *data));
const char *strmap_insert(strmap_t *m, const char *key, const char *data);
const char *strmap_find(strmap_t *m, const char *key);

#endif
