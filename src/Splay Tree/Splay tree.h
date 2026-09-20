#ifndef SPLAY_H
#define SPLAY_H

/*
 * Splay 树（自调整二叉搜索树）对外接口
 *
 * 与 bst.h / avl.h 保持完全一致的五个函数签名与返回值约定，
 * 由 bench/bench.py 通过 ctypes 以 void * 句柄加载调用。
 * 节点结构体与树的内部表示不对外暴露。
 *
 * 返回值约定（见 docs/接口约定.md §4.1）：
 *   create   成功返回树句柄；失败返回 NULL
 *   insert   0 成功 / 1 键已存在（不做修改） / -1 内存分配失败
 *   erase    0 成功 / 1 键不存在
 *   find     1 存在 / 0 不存在
 *   destroy  无返回值，调用后句柄不可再用
 *
 * 句柄为 NULL 时，除 create 外一律返回 -1。
 */

#include <stdlib.h>

typedef int tree_key_t;

void *splay_create(void);
int   splay_insert(void *tree, tree_key_t key);
int   splay_erase (void *tree, tree_key_t key);
int   splay_find  (void *tree, tree_key_t key);
void  splay_destroy(void *tree);

#endif /* SPLAY_H */
