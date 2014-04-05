#include "strmap.h"

void strmap_init(strmap_t *m) {
	m->count = 0;
	m->root = NULL;
}

void _strmap_destroy_rec(struct _strmap_node *node) {
	if (node == NULL) return;
	_strmap_destroy_rec(node->left);
	_strmap_destroy_rec(node->right);
	free(node);
}

void strmap_destroy(strmap_t *m) {
	_strmap_destroy_rec(m->root);
	m->root = NULL;
	m->count = 0;
}

const char *strmap_find(strmap_t *m, const char *key) {
	int i;
	struct _strmap_node *node;
	for (node = m->root; node != NULL; ) {
		i = strcmp(key, node->data);
		if (i < 0) node = node->left;
		else if (i > 0) node = node->right;
		else return node->data + strlen(node->data) + 1;	
	}
	return NULL;
}

const char *_strmap_insert_rec(struct _strmap_node *parent, struct _strmap_node *node, const char *key, const char *data) {
	int i;
	struct _strmap_node *new_node;
	i = strcmp(key, node->data);
	if (i < 0) {
		if (node->left == NULL) {
			new_node = (struct _strmap_node*)malloc(sizeof(struct _strmap_node) + strlen(key) + strlen(data) + 2);
			node->left = new_node;
			new_node->left = NULL;
			new_node->right = NULL;
			strcpy(new_node->data, key);
			return strcpy(new_node->data + strlen(new_node->data) + 1, data);
			
		} else return _strmap_insert_rec(node, node->left, key, data);
	} else if (i > 0) {
		if (node->right == NULL) {
			new_node = (struct _strmap_node*)malloc(sizeof(struct _strmap_node) + strlen(key) + strlen(data) + 2);
			node->right = new_node;
			new_node->left = NULL;
			new_node->right = NULL;
			strcpy(new_node->data, key);
			return strcpy(new_node->data + strlen(new_node->data) + 1, data);	
		} else return _strmap_insert_rec(node, node->right, key, data);
	} else {	
		new_node = (struct _strmap_node*)realloc(node, sizeof(struct _strmap_node) + strlen(key) + strlen(data) + 2);
		if (parent) {
			if (parent->left == node) parent->left = new_node;
			else parent->right = new_node;
		}
		return strcpy(new_node->data + strlen(key) + 1, data);	
	}	
}

const char *strmap_insert(strmap_t *m, const char *key, const char *data) {
	if (m->root == NULL) {
		m->root = (struct _strmap_node*)malloc(sizeof(struct _strmap_node) + strlen(key) + strlen(data) + 2);
		m->root->left = NULL;
		m->root->right = NULL;
		strcpy(m->root->data, key);
		return strcpy(m->root->data + strlen(key) + 1, data);
	}
	return _strmap_insert_rec(NULL, m->root, key, data);
}

void _strmap_replace_node_in_parent(
	struct _strmap_node *parent,
	struct _strmap_node *oldnode,
	struct _strmap_node *newnode
) {
	if (parent != NULL) {
		if (parent->left == oldnode)
			parent->left = newnode;
		else
			parent->right = newnode;
	}
	free(oldnode);
}

int _strmap_remove_rec(struct _strmap_node *parent, struct _strmap_node *node, const char *key) {
	int i;
	struct _strmap_node *succ;
	i = strcmp(key, node->data);
	if (i < 0) {
		if (node->left == NULL) return -1;
		return _strmap_remove_rec(node, node->left, key);
	} else if (i > 0) {
		if (node->right == NULL) return -1;
		return _strmap_remove_rec(node, node->right, key);
	} else {
		if (node->left && node->right) {
			parent = node;
			succ = node->right;
			while (succ->left) { parent = succ; succ = succ->left; }
			_strmap_remove_rec(parent, succ, key);
		} else if (node->left) {
			_strmap_replace_node_in_parent(parent, node, node->left);
		} else if (node->right) {
			_strmap_replace_node_in_parent(parent, node, node->right);
		} else {
			_strmap_replace_node_in_parent(parent, node, NULL);
		}
	}
	return 0;
}

int strmap_remove(strmap_t *m, const char *key) {
	if (m->root == NULL) return -1;
	return _strmap_remove_rec(NULL, m->root, key);
}

int _strmap_iterate_rec(struct _strmap_node *node, int (*callback)(const char*, const char*)) {
	int i;
	if (node == NULL) return 0;
	i = _strmap_iterate_rec(node->left, callback);
	if (i < 0) return i;
	i = callback(node->data, node->data + strlen(node->data) + 1);
	if (i < 0) return i;
	i = _strmap_iterate_rec(node->right, callback);
	return i;
}

int strmap_iterate(strmap_t *m, int (*callback)(const char *key, const char *value)) {
	return _strmap_iterate_rec(m->root, callback);
}
