#include "bst.h"


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

static void recursive_destroy(bst_node* t){    //递归删除树，bst可能会爆栈但我懒得优化。
    if(!t) return ;
    recursive_destroy(t->left);       //递归删除左子
    recursive_destroy(t->right);      //右子
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
        prev->left = malloc(sizeof(*prev->left));
        if(!prev->left) return -1;
        prev->left->val = key;
        prev->left->left = NULL;
        prev->left->right = NULL;
    }
    t->size++;
    return 0;
}

static bst_node* find_min_node(bst_node* t){    //找子树中的最小节点
    while(t->left){
        t = t->left;
    }
    return t;
}

static bst_node* recursive_erase(bst_node* t, tree_key_t key, int* deleted){    //在子树中删除节点，返回删除后的子树根
    if(!t) return NULL;
    if(key < t->val){                   //待删键在左子树
        t->left = recursive_erase(t->left, key, deleted);
    }
    else if(key > t->val){              //待删键在右子树
        t->right = recursive_erase(t->right, key, deleted);
    }
    else{
        *deleted = 1;                   //记录已找到并删除
        if(!t->left){                   //无左孩子：右孩子直接顶替
            bst_node* r = t->right;
            free(t);
            return r;
        }
        if(!t->right){                  //无右孩子：左孩子直接顶替
            bst_node* l = t->left;
            free(t);
            return l;
        }
        bst_node* suc = find_min_node(t->right);    //双孩子：用后继的值覆盖，再去右子树删后继
        t->val = suc->val;
        t->right = recursive_erase(t->right, suc->val, deleted);
    }
    return t;
}

int bst_erase(void *tree, tree_key_t key){
    if(!tree) return -1;
    bst_tree* t = tree;
    int deleted = 0;
    t->root = recursive_erase(t->root, key, &deleted);    //用返回的新子树根接回
    if(!deleted) return 1;              //键不存在，无副作用
    t->size--;
    return 0;
}

int bst_find(void *tree, tree_key_t key){
    if(!tree) return -1;
    bst_tree* t = tree;
    bst_node* curr = t->root;
    while(curr){                        //沿查找路径下探
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
    return 0;                           //不存在
}