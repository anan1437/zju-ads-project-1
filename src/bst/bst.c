#include "bst.h"
#include <cstdlib>
#include <stdio.h>



typedef struct bst_node{                //节点结构体
    tree_key_t val;
    struct bst_node* left, *right;
} bst_node;

typedef struct {                        //树结构体
    bst_node* root;
    int size;
} bst_tree;

void *bst_create(void) {                //创建树
    bst_tree *t = malloc(sizeof(*t));
    if (!t) return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

void recursive_destroy(bst_node* t){    //递归删除树，bst可能会爆栈但我懒得优化。
    if(!t) return ;
    recursive_destroy(t->left);       //递归删除左子
    recursive_destroy(t->right);      //柚子
    free(t);                      //释放空间
    return ;
}

void bst_destroy(void *tree){
    if(!tree) return ;
    bst_tree* t = tree;
    recursive_destroy(t->root);       //调用递归删除
    free(t);
    return ;
}

int bst_insert(void *tree, tree_key_t key){
    if(!tree ) return -1;
    bst_tree* t = tree;
    bst_node* root = t->root;
    bst_node* curr = root;
    if(!curr ){                         //插入第一个元素
        t->root = malloc(sizeof(*(t->root)));
        if(!t->root){
            return -1;
        }
        t->size++;
        t->root->val=key;
        t->root->left=NULL;
        t->root->right=NULL;
        return 0;
    }
    bst_node* prev;
    while(curr){                        //找到元素位置并记录其父节点
        prev = curr;
        if(key > curr->val){
            curr = curr->right;
        }
        else if(key < curr->val){
            curr = curr->left;
        }
        else{
            return 1;
        }
    }
    if(key > prev->val){
        prev->right = malloc(sizeof(*prev->right));
        if(!prev->right) return -1;
        prev->right->val = key;
        prev->right->left = NULL;
        prev->right->right = NULL;
    }
    else{
        prev->left = malloc(sizeof(*prev->right));
        if(!prev->left) return -1;
        prev->left->val = key;
        prev->left->left = NULL;
        prev->left->right = NULL;
    }
    t->size++;
    return 0;
}

int main(){
    int n;
}