#include <stdlib.h>
typedef int tree_key_t;

typedef struct splay_node{
    tree_key_t key;
    struct splay_node* left;
    struct splay_node* right;
    struct splay_node* parent;
} Splay_Node;

typedef struct splay_tree{
    Splay_Node* root;
} Splay_Tree;

/* ---- 以下为内部辅助函数，不对外暴露，仅在本文件内使用 ----
 * 前置声明，使定义顺序不受限制（C99 下先用后定义必须声明） */
Splay_Node* Find (Splay_Node* Node, tree_key_t key);
Splay_Node* Find_insert (Splay_Node* Node, tree_key_t key);
Splay_Node* Find_Max (Splay_Node* Node);
void splay (void *tree, Splay_Node* Node);
void splay_for_two (Splay_Node* Node);
void left_roll (Splay_Node* parent, Splay_Node* kid);
void right_roll (Splay_Node* parent, Splay_Node* kid);
void Destroy (Splay_Node* Node);


void *splay_create(void)
{
        Splay_Tree*  tree;
        tree=malloc(sizeof(Splay_Tree));
        if(tree==NULL) return NULL;
        tree->root=NULL;
        return tree;
}
int   splay_find  (void *tree, tree_key_t key)
{
    if(tree==NULL) return -1;
    Splay_Tree* t=tree;
    Splay_Node* Node=t->root;
    Splay_Node* target=Find(Node,key);
    if(target==NULL) return 0;
    splay(tree,target);
    return 1;
}
Splay_Node* Find(Splay_Node* Node,tree_key_t key)
{
    if(Node==NULL) return NULL;
    if(Node->key<key) return Find(Node->right,key);
    if(Node->key>key) return Find(Node->left,key);
    return Node;
}




int   splay_insert(void *tree, tree_key_t key)
{
    if(tree==NULL) return -1;
    int Flag=splay_find(tree,key);
    if(Flag==1) return 1;
    Splay_Node* Node=malloc(sizeof(Splay_Node));
    if(Node==NULL) return -1;
    Node->key=key;
    Node->left=Node->right=Node->parent=NULL;
    Splay_Tree* t=tree;
    if(t->root==NULL) 
    {
        t->root=Node;
    }
    else
    {
        Splay_Node* target=Find_insert(t->root,key);
        if(target->key<key) 
        {
            target->right=Node;
            Node->parent=target;
        }
        else
        {
            target->left=Node;
            Node->parent=target;
        }
    }
    splay(tree,Node);
    return 0;
}

Splay_Node* Find_insert(Splay_Node* Node,tree_key_t key)
{
    if(Node!=NULL)
    {
        if(Node->key<key)
        {
            Splay_Node* result=Find_insert(Node->right,key);
            if (result==NULL) return Node;
            return result;
        }
        if(Node->key>key)
        {
            Splay_Node* result=Find_insert(Node->left,key);
            if (result==NULL) return Node;
            return result;
        }
    }
    return NULL;
}

int   splay_erase (void *tree, tree_key_t key)
{
    if(tree==NULL) return -1;
    Splay_Tree* t=tree;
    Splay_Node* place_node=Find(t->root,key);
    if(place_node==NULL) return 1;
    splay(tree,place_node);
    Splay_Node* left=place_node->left;
    Splay_Node* right=place_node->right;
    if(left!=NULL) left->parent=NULL;
    if(right!=NULL) right->parent=NULL;
    free(place_node);
    if(left!=NULL)
    {
        t->root=left;
        splay(tree,Find_Max(t->root));
        t->root->right=right;
        if(right!=NULL)
            right->parent=t->root;
    }
    else if(right!=NULL)
    {
        t->root=right;
        right->parent=NULL;
    }
    else
        t->root=NULL;
    return 0;
}
Splay_Node* Find_Max(Splay_Node* Node)
{
    if(Node->right!=NULL)
        return Find_Max(Node->right);
    return Node;
}



void  splay_destroy(void *tree)
{
    if(tree==NULL) return;
    Splay_Tree* t=tree;
    Destroy(t->root);
    free(t);
}
void Destroy(Splay_Node *Node)
{
    if(Node==NULL) return;
    Destroy(Node->left);
    Destroy(Node->right);
    free(Node);
}


void splay(void *tree,Splay_Node* Node)
{
    Splay_Tree* t=tree;
    while(Node->parent!=NULL)
        splay_for_two(Node);
    t->root=Node;
}
void splay_for_two(Splay_Node* Node)
{
    Splay_Node* grand;
    Splay_Node* parent;
    parent=Node->parent;
    if(parent==NULL) return;
    grand=parent->parent;
    if(grand==NULL)//zig or zag
    {
        if(Node==parent->left)  //zig
            right_roll(parent,Node);
        else
            left_roll(parent,Node);  //zag
    }
    else
    if(parent==grand->left&&Node==parent->left)  //zig-zig
    {
        right_roll(grand,parent);
        right_roll(parent,Node);
    }
    else
    if(parent==grand->right&&Node==parent->right)  //zag-zag
    {
        left_roll(grand,parent);
        left_roll(parent,Node);
    }
    else
    if(parent==grand->left&&Node==parent->right)  //zig-zag
    {
        left_roll(parent,Node);
        right_roll(grand,Node);
    }
    else
    if(parent==grand->right&&Node==parent->left)  //zag-zig
    {
        right_roll(parent,Node);
        left_roll(grand,Node);
    }
}
void left_roll(Splay_Node* parent,Splay_Node* kid)
{
    Splay_Node* left_grandkid=kid->left;
    kid->left=parent;
    parent->right=left_grandkid;
    kid->parent=parent->parent;
    if (left_grandkid!=NULL)
      left_grandkid->parent=parent;
    if(parent->parent==NULL) 
    {
        parent->parent=kid;
        return;
    }
    if(parent==parent->parent->left) 
        parent->parent->left=kid;
    else
        parent->parent->right=kid;
    parent->parent=kid;
}
void right_roll(Splay_Node* parent,Splay_Node* kid)
{
    Splay_Node* right_grandkid=kid->right;
    kid->right=parent;
    parent->left=right_grandkid;
    kid->parent=parent->parent;
    if (right_grandkid!=NULL)
        right_grandkid->parent=parent;
    if(parent->parent==NULL) 
    {
        parent->parent=kid;
        return;
    }
    if(parent==parent->parent->left) 
        parent->parent->left=kid;
    else
        parent->parent->right=kid;
    parent->parent=kid;
}