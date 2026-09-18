#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int tree_key_t;         //按约定统一键

void *bst_create(void);
int   bst_insert(void *tree, tree_key_t key);
int   bst_erase (void *tree, tree_key_t key);
int   bst_find  (void *tree, tree_key_t key);
void  bst_destroy(void *tree);