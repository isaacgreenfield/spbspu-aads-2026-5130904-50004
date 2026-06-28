#ifndef RBTREE_H
#define RBTREE_H
#include <iostream>

namespace ivanov {
template <class Key, class Value>
class RBtree {
private:

  struct Node {
    Key key;
    Value val;
    bool color; //red - true, black - false
    Node *left;
    Node *right;
    Node *parent;

    Node(const Key& k, const Value& v, bool c = true)
        : key(k), val(v), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
  };

  Node *root;
  Node *NIL;

  void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL)
      y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == NIL)
      root = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;

    y->left = x;
    x->parent = y;
  }

  void rightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NIL)
      y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == NIL)
      root = y;
    else if (x == x->parent->right)
      x->parent->right = y;
    else
      x->parent->left = y;

    y->right = x;
    x->parent = y;
  }
  void insertFixUp(Node *z) {
    while (z->parent->color == true) {
      if (z->parent == z->parent->parent->left) {   // родитель — левый
        Node *y = z->parent->parent->right;
        //дядя красный
        if (y->color == true) {
          z->parent->color = false;
          y->color = false;
          z->parent->parent->color = true;
          z = z->parent->parent;
        } else {
          //дядя чёрный, z — правый потомок
          if (z == z->parent->right) {
            z = z->parent;
            leftRotate(z);
          }
          //дядя чёрный, z — левый потомок
          z->parent->color = false;
          z->parent->parent->color = true;
          rightRotate(z->parent->parent);
        }
      } else { // родитель — правый
        Node *y = z->parent->parent->left;
        if (y->color == true) {
          z->parent->color = false;
          y->color = false;
          z->parent->parent->color = true;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            rightRotate(z);
          }
          z->parent->color = false;
          z->parent->parent->color = true;
          leftRotate(z->parent->parent);
        }
      }
    }
    root->color = false;
  }

  int sizeRec(Node* node) const {
    if (node == NIL) return 0;
    return 1 + sizeRec(node->left) + sizeRec(node->right);
  }

  void transplant(Node *u, Node *v) {
    if (u->parent == NIL)
      root = v;
    else if (u == u->parent->left)
      u->parent->left = v;
    else
      u->parent->right = v;
    v->parent = u->parent;
  }
  Node* minimum(Node *node) {
    while (node->left != NIL)
      node = node->left;
    return node;
  }

  void deleteFixUp(Node *x) {
    while (x != root && x->color == false) {
      if (x == x->parent->left) {
        Node *w = x->parent->right;
        //брат красный
        if (w->color == true) {
          w->color = false;
          x->parent->color = true;
          leftRotate(x->parent);
          w = x->parent->right;
        }
        //брат чёрный, оба его ребёнка чёрные
        if (w->left->color == false && w->right->color == false) {
          w->color = true;
          x = x->parent;
        } else {
          //брат чёрный, правый ребёнок чёрный, левый красный
          if (w->right->color == false) {
            w->left->color = false;
            w->color = true;
            rightRotate(w);
            w = x->parent->right;
          }
          //брат чёрный, правый ребёнок красный
          w->color = x->parent->color;
          x->parent->color = false;
          w->right->color = false;
          leftRotate(x->parent);
          x = root;
        }
      } else {
        Node *w = x->parent->left;
        if (w->color == true) {
          w->color = false;
          x->parent->color = true;
          rightRotate(x->parent);
          w = x->parent->left;
        }
        if (w->right->color == false && w->left->color == false) {
          w->color = true;
          x = x->parent;
        } else {
          if (w->left->color == false) {
            w->right->color = false;
            w->color = true;
            leftRotate(w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = false;
          w->left->color = false;
          rightRotate(x->parent);
          x = root;
        }
      }
    }
    x->color = false;
  }
  void deleteNode(Node *z) {
    Node *y = z;
    Node *x;
    bool y_original_color = y->color;

    if (z->left == NIL) {
      x = z->right;
      transplant(z, z->right);
    } else if (z->right == NIL) {
      x = z->left;
      transplant(z, z->left);
    } else {
      y = minimum(z->right);
      y_original_color = y->color;
      x = y->right;
      if (y->parent == z) {
        x->parent = y;
      } else {
        transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }
      transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }
    if (y_original_color == false)
      deleteFixUp(x);
    delete z;
  }

  void clear(Node *node) {
    if (node != NIL) {
      clear(node->left);
      clear(node->right);
      delete node;
    }
  }
  void inorder(Node *node, std::ostream& os = std::cout) const {
    if (node != NIL) {
      inorder(node->left, os);
      os << node->key << " -> " << node->val
         << (node->color == true ? " (R)" : " (B)") << "\n";
      inorder(node->right, os);
    }
  }

  template<typename Func>
  void forEachNode(Node* node, Func f) const {
    if (node == NIL) return;
    forEachNode(node->left, f);
    f(node->key, node->val);
    forEachNode(node->right, f);
  }

  Node* searchNode(const Key& key) const {
    Node *curr = root;
    while (curr != NIL) {
      if (key < curr->key)
        curr = curr->left;
      else if (key > curr->key)
        curr = curr->right;
      else
        return curr;
    }
    return NIL;
  }

public:

  RBtree():
  NIL(new Node(Key(), Value(), false)),
  root(nullptr)
  {
    NIL->left = NIL;
    NIL->right = NIL;
    NIL->parent = NIL;
    root = NIL;
  }
  ~RBtree() {
    clear(root);
    delete NIL;
  }

  void remove(const Key& key) {
    Node *z = searchNode(key);
    if (z != NIL)
      deleteNode(z);
  }

  Value* search(const Key& key) {
    Node *node = searchNode(key);
    if (node != NIL)
      return &node->val;
    return nullptr;
  }

  bool contains(const Key& key) {
    return searchNode(key) != NIL;
  }

  void clear() {
    clear(root);
    root = NIL;
  }
  bool empty() const {
    return root == NIL;
  }
  void print() const {
    inorder(root);
  }

  template<typename Func>
  void forEach(Func f) const {
    forEachNode(root, f);
  }

  void insert(const Key& key, const Value& value) {
    Node *z = new Node(key, value);
    Node *y = NIL;
    Node *x = root;
    while (x != NIL) {
      y = x;
      if (key < x->key)
        x = x->left;
      else if (key > x->key)
        x = x->right;
      else {
        x->val = value;
        delete z;
        return;
      }
    }
    z->parent = y;
    if (y == NIL)
      root = z;
    else if (key < y->key)
      y->left = z;
    else
      y->right = z;
    z->left = NIL;
    z->right = NIL;
    z->color = true;
    insertFixUp(z);
  }

  int size() const { return sizeRec(root); }
};
}

#endif
