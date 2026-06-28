#ifndef RBTREE_H
#define RBTREE_H
#include <iostream>
#include <utility>
#include <iterator>

namespace ivanov {
template <class Key, class Value>
class RBtree
{
public:
  class iterator;
  class const_iterator;
private:
  struct Node
  {
    std::pair< const Key, Value > data;
    bool color;
    Node *left;
    Node *right;
    Node *parent;

    Node(const Key& k, const Value& v, bool c = true):
    data(k, v),
    color(c),
    left(nullptr),
    right(nullptr),
    parent(nullptr)
    {}
  };

  Node *root;
  Node *NIL;

  void leftRotate(Node *x)
  {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL) {
      y->left->parent = x;
    }

    y->parent = x->parent;
    if (x->parent == NIL) {
      root = y;
    }
    else if (x == x->parent->left) {
      x->parent->left = y;
    }
    else {
      x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
  }

  void rightRotate(Node *x)
  {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NIL) {
      y->right->parent = x;
    }

    y->parent = x->parent;
    if (x->parent == NIL) {
      root = y;
    }
    else if (x == x->parent->right) {
      x->parent->right = y;
    }
    else {
      x->parent->left = y;
    }

    y->right = x;
    x->parent = y;
  }
  void insertFixUp(Node *z)
  {
    while (z->parent->color == true) {
      if (z->parent == z->parent->parent->left) {
        Node *y = z->parent->parent->right;
        if (y->color == true) {
          z->parent->color = false;
          y->color = false;
          z->parent->parent->color = true;
          z = z->parent->parent;
        } else {
          if (z == z->parent->right) {
            z = z->parent;
            leftRotate(z);
          }
          z->parent->color = false;
          z->parent->parent->color = true;
          rightRotate(z->parent->parent);
        }
      } else {
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

  size_t sizeRec(Node* node) const
  {
    if (node == NIL) {
      return 0;
    }
    return 1 + sizeRec(node->left) + sizeRec(node->right);
  }

  void transplant(Node *u, Node *v)
  {
    if (u->parent == NIL) {
      root = v;
    }
    else if (u == u->parent->left) {
      u->parent->left = v;
    }
    else {
      u->parent->right = v;
    }
    v->parent = u->parent;
  }
  Node* minimum(Node *node) const
  {
    while (node->left != NIL) {
      node = node->left;
    }
    return node;
  }

  Node* maximum(Node *node) const
  {
    while (node->right != NIL) {
      node = node->right;
    }
    return node;
  }
  void deleteFixUp(Node *x)
  {
    while (x != root && x->color == false) {
      if (x == x->parent->left) {
        Node *w = x->parent->right;
        if (w->color == true) {
          w->color = false;
          x->parent->color = true;
          leftRotate(x->parent);
          w = x->parent->right;
        }
        if (w->left->color == false && w->right->color == false) {
          w->color = true;
          x = x->parent;
        } else {
          if (w->right->color == false) {
            w->left->color = false;
            w->color = true;
            rightRotate(w);
            w = x->parent->right;
          }
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
  void deleteNode(Node *z)
  {
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
    if (y_original_color == false) {
      deleteFixUp(x);
    }
    delete z;
  }

  void clear(Node *node)
  {
    if (node != NIL) {
      clear(node->left);
      clear(node->right);
      delete node;
    }
  }
  void inorder(Node *node, std::ostream& os = std::cout) const
  {
    if (node != NIL) {
      inorder(node->left, os);
      os << node->data.first << " -> " << node->data.second
         << (node->color == true ? " (R)" : " (B)") << "\n";
      inorder(node->right, os);
    }
  }

  template<typename Func>
  void forEachNode(Node* node, Func f) const
  {
    if (node == NIL) return;
    forEachNode(node->left, f);
    f(node->data.first, node->data.second);
    forEachNode(node->right, f);
  }

  Node* searchNode(const Key& key) const
  {
    Node *curr = root;
    while (curr != NIL) {
      if (key < curr->data.first) {
        curr = curr->left;
      }
      else if (key > curr->data.first) {
        curr = curr->right;
      }
      else {
        return curr;
      }
    }
    return NIL;
  }

public:
  RBtree():
  root(nullptr),
  NIL(new Node(Key(), Value(), false))
  {
    NIL->left = NIL;
    NIL->right = NIL;
    NIL->parent = NIL;
    root = NIL;
  }
  ~RBtree()
  {
    clear(root);
    delete NIL;
  }

  void remove(const Key& key)
  {
    Node *z = searchNode(key);
    if (z != NIL) {
      deleteNode(z);
    }
  }

  iterator find(const Key& key)
  {
    Node* node = searchNode(key);
    if (node != NIL) {
      return iterator(node, this);
    }
    return end();
  }
  const_iterator find(const Key& key) const
  {
    Node* node = searchNode(key);
    if (node != NIL) {
      return const_iterator(node, this);
    }
    return end();
  }

  bool contains(const Key& key) const
  {
    return searchNode(key) != NIL;
  }

  void clear()
  {
    clear(root);
    root = NIL;
  }
  bool empty() const
  {
    return root == NIL;
  }
  void print() const
  {
    inorder(root);
  }

  template<typename Func>
  void forEach(Func f) const
  {
    forEachNode(root, f);
  }

  void insert(const Key& key, const Value& value)
  {
    Node *z = new Node(key, value);
    Node *y = NIL;
    Node *x = root;
    while (x != NIL) {
      y = x;
      if (key < x->data.first) {
        x = x->left;
      }
      else if (key > x->data.first) {
        x = x->right;
      }
      else {
        x->data.second = value;
        delete z;
        return;
      }
    }
    z->parent = y;
    if (y == NIL) {
      root = z;
    }
    else if (key < y->data.first) {
      y->left = z;
    }
    else {
      y->right = z;
    }
    z->left = NIL;
    z->right = NIL;
    z->color = true;
    insertFixUp(z);
  }

  size_t size() const
  {
    return sizeRec(root);
  }

  class iterator {
    friend class RBtree;
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair< const Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    iterator():
    current(nullptr),
    tree(nullptr)
    {}

    reference operator*() const
    {
      return current->data;
    }
    pointer operator->() const
    {
      return &current->data;
    }

    iterator& operator++()
    {
      increment();
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      increment();
      return tmp;
    }
    iterator& operator--()
    {
      decrement();
      return *this;
    }
    iterator operator--(int)
    {
      iterator tmp = *this;
      decrement();
      return tmp;
    }

    bool operator==(const iterator& other) const
    {
      return current == other.current;
    }
    bool operator!=(const iterator& other) const
    {
      return !(*this == other);
    }

  private:
    Node* current;
    RBtree* tree;

    iterator(Node* node, RBtree* t):
    current(node),
    tree(t)
    {}

    void increment()
    {
      if (current == tree->NIL) {
        return;
      }
      if (current->right != tree->NIL) {
        current = current->right;
        while (current->left != tree->NIL)
          current = current->left;
      } else {
        Node* parent = current->parent;
        while (parent != tree->NIL && current == parent->right) {
          current = parent;
          parent = parent->parent;
        }
        current = parent;
      }
    }

    void decrement()
    {
      if (current == tree->NIL) {
        current = tree->root;
        if (current != tree->NIL) {
          while (current->right != tree->NIL)
            current = current->right;
        }
      } else {
        if (current->left != tree->NIL) {
          current = current->left;
          while (current->right != tree->NIL)
            current = current->right;
        } else {
          Node* parent = current->parent;
          while (parent != tree->NIL && current == parent->left) {
            current = parent;
            parent = parent->parent;
          }
          current = parent;
        }
      }
    }
  };

  class const_iterator {
    friend class RBtree;
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const std::pair< const Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    const_iterator():
    current(nullptr),
    tree(nullptr)
    {}

    const_iterator(const iterator& it):
    current(it.current),
    tree(it.tree)
    {}

    reference operator*() const
    {
      return current->data;
    }
    pointer operator->() const
    {
      return &current->data;
    }

    const_iterator& operator++()
    {
      increment();
      return *this;
    }
    const_iterator operator++(int)
    {
      const_iterator tmp = *this;
      increment();
      return tmp;
    }
    const_iterator& operator--()
    {
      decrement();
      return *this;
    }
    const_iterator operator--(int)
    {
      const_iterator tmp = *this;
      decrement();
      return tmp;
    }

    bool operator==(const const_iterator& other) const
    {
      return current == other.current;
    }
    bool operator!=(const const_iterator& other) const
    {
      return !(*this == other);
    }

  private:
    Node* current;
    const RBtree* tree;

    const_iterator(Node* node, const RBtree* t):
    current(node),
    tree(t)
    {}

    void increment()
    {
      if (current == tree->NIL) {
        return;
      }
      if (current->right != tree->NIL) {
        current = current->right;
        while (current->left != tree->NIL)
          current = current->left;
      } else {
        Node* parent = current->parent;
        while (parent != tree->NIL && current == parent->right) {
          current = parent;
          parent = parent->parent;
        }
        current = parent;
      }
    }

    void decrement()
    {
      if (current == tree->NIL) {
        current = tree->root;
        if (current != tree->NIL) {
          while (current->right != tree->NIL)
            current = current->right;
        }
      } else {
        if (current->left != tree->NIL) {
          current = current->left;
          while (current->right != tree->NIL)
            current = current->right;
        } else {
          Node* parent = current->parent;
          while (parent != tree->NIL && current == parent->left) {
            current = parent;
            parent = parent->parent;
          }
          current = parent;
        }
      }
    }
  };

  iterator begin()
  {
    return iterator(minimum(root), this);
  }
  iterator end()
  {
    return iterator(NIL, this);
  }
  const_iterator begin() const
  {
    return const_iterator(minimum(root), this);
  }
  const_iterator end() const
  {
    return const_iterator(NIL, this);
  }
  const_iterator cbegin() const
  {
    return begin();
  }
  const_iterator cend() const
  {
    return end();
  }
};

}
#endif