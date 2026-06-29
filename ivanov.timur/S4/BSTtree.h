#ifndef BSTTREE_H
#define BSTTREE_H
#include <stdexcept>
#include <utility>

template<class Key, class Value, class Compare>
class BSTtree;
template<class Key, class Value, class Compare>
class BSTConstIterator;

template<class Key, class Value, class Compare>
class BSTIterator {
private:
  friend class BSTtree< Key, Value, Compare >;

public:
  using tree = BSTtree< Key, Value, Compare >;

private:
  tree *ptr;

  explicit BSTIterator(tree *p):
  ptr(p)
  {}

public:
  BSTIterator():
  ptr(nullptr)
  {}

  explicit operator BSTConstIterator< Key, Value, Compare >() const
  {
    return BSTConstIterator< Key, Value, Compare >(ptr);
  }

  bool operator==(const BSTIterator &other) const
  {
    return ptr == other.ptr;
  }

  bool operator!=(const BSTIterator &other) const
  {
    return !operator==(other);
  }

  std::pair< const Key &, Value & > operator*() const
  {
    return {ptr->key, ptr->val};
  }

  BSTIterator &operator++()
  {
    if (!(ptr->isFakeRoot) && ptr->right != &tree::nil) {
      ptr = ptr->right;
      while (ptr->left != &tree::nil) ptr = ptr->left;
    } else {
      tree *parent = ptr->parent;
      while (parent != &tree::nil && ptr == parent->right) {
        ptr = parent;
        parent = parent->parent;
      }
      ptr = parent;
    }
    return *this;
  }

  BSTIterator operator++(int)
  {
    BSTIterator tmp = *this;
    ++*this;
    return tmp;
  }
};

template<class Key, class Value, class Compare>
class BSTConstIterator {
private:
  friend class BSTtree< Key, Value, Compare >;

public:
  using tree = BSTtree< Key, Value, Compare >;

private:
  const tree *ptr;

  explicit BSTConstIterator(const tree *p):
  ptr(p)
  {}

public:
  BSTConstIterator():
  ptr(nullptr)
  {}

  bool operator==(const BSTConstIterator &other) const
  {
    return ptr == other.ptr;
  }

  bool operator!=(const BSTConstIterator &other) const
  {
    return !operator==(other);
  }

  std::pair< const Key &, const Value & > operator*() const
  {
    return {ptr->key, ptr->val};
  }

  BSTConstIterator &operator++()
  {
    if (!(ptr->isFakeRoot) && ptr->right != &tree::nil) {
      ptr = ptr->right;
      while (ptr->left != &tree::nil) ptr = ptr->left;
    } else {
      const tree *parent = ptr->parent;
      while (parent != &tree::nil && ptr == parent->right) {
        ptr = parent;
        parent = parent->parent;
      }
      ptr = parent;
    }
    return *this;
  }

  BSTConstIterator operator++(int)
  {
    BSTConstIterator tmp = *this;
    ++*this;
    return tmp;
  }
};

template<class Key, class Value, class Compare>
class BSTtree {
public:
  using const_iterator = BSTConstIterator< Key, Value, Compare >;
  using iterator = BSTIterator< Key, Value, Compare >;

private:
  friend class BSTIterator< Key, Value, Compare >;
  friend class BSTConstIterator< Key, Value, Compare >;

  Value val;
  Key key;
  Compare comp;
  BSTtree *right, *left;
  BSTtree *parent;
  bool isFakeRoot;

  static BSTtree nil;

  size_t subHg(const BSTtree *tree) const
  {
    if (tree == &nil) {
      return 0;
    }
    size_t leftH = subHg(tree->left);
    size_t rightH = subHg(tree->right);
    return 1 + std::max(leftH, rightH);
  }

  static void clearSubtree(BSTtree *node)
  {
    if (node == &nil) {
      return;
    }
    clearSubtree(node->left);
    clearSubtree(node->right);
    delete node;
  }

public:
  BSTtree():
  val(),
  key(),
  right(&nil),
  left(&nil),
  parent(&nil),
  isFakeRoot(true)
  {}

  explicit BSTtree(std::pair< Value, Key > init):
  val(init.first),
  key(init.second),
  right(&nil),
  left(&nil),
  parent(&nil),
  isFakeRoot(false)
  {}

  explicit BSTtree(std::pair< Value, Key > init, BSTtree *parnt = nullptr):
  val(init.first),
  key(init.second),
  right(&nil),
  left(&nil),
  parent(parnt),
  isFakeRoot(false)
  {}

  ~BSTtree();

  BSTtree(const BSTtree &) = delete;

  BSTtree &operator=(const BSTtree &) = delete;

  BSTtree(BSTtree &&other) noexcept:
  val(std::move(other.val)),
  key(std::move(other.key)),
  comp(std::move(other.comp)),
  right(other.right),
  left(other.left),
  parent(other.parent),
  isFakeRoot(other.isFakeRoot)
  {
    if (left != &nil) {
      left->parent = this;
    }
    if (right != &nil) {
      right->parent = this;
    }
    other.left = other.right = &nil;
    other.isFakeRoot = false;
  }

  BSTtree &operator=(BSTtree &&other) noexcept
  {
    if (this != &other) {
      if (isFakeRoot) {
        clearSubtree(left);
      }
      val = std::move(other.val);
      key = std::move(other.key);
      comp = std::move(other.comp);
      right = other.right;
      left = other.left;
      parent = other.parent;
      isFakeRoot = other.isFakeRoot;
      if (left != &nil) {
        left->parent = this;
      }
      if (right != &nil) {
        right->parent = this;
      }
      other.left = other.right = &nil;
      other.isFakeRoot = false;
    }
    return *this;
  }


  void push(const Key &k, const Value &v)
  {
    BSTtree *p = this;
    BSTtree *curr = left;
    while (curr != &nil) {
      p = curr;
      if (comp(k, curr->key)) {
        curr = curr->left;
      }
      else if (comp(curr->key, k)) {
        curr = curr->right;
      }
      else {
        curr->val = v;
        return;
      }
    }
    BSTtree *nw = new BSTtree({v, k}, p);
    if (p == this || comp(k, p->key)) {
      p->left = nw;
    }
    else p->right = nw;
  }

  Value get(Key k) const
  {
    BSTtree *curr = left;
    while (curr != &nil) {
      if (comp(k, curr->key)) {
        curr = curr->left;
      }
      else if (comp(curr->key, k)) {
        curr = curr->right;
      }
      else {
        return curr->val;
      }
    }
    throw std::out_of_range("No such element here");
  }

  Value drop(Key k)
  {
    BSTtree *curr = left;
    while (curr != &nil) {
      if (comp(k, curr->key)) {
        curr = curr->left;
      }
      else if (comp(curr->key, k)) {
        curr = curr->right;
      }
      else {
        break;
      }
    }
    if (curr == &nil) {
      throw std::out_of_range("No such element here");
    }

    Value rmv = curr->val;

    if (curr->left != &nil && curr->right != &nil) {
      BSTtree *mRight = curr->right;
      while (mRight->left != &nil) mRight = mRight->left;
      curr->val = mRight->val;
      curr->key = mRight->key;
      curr = mRight;
      rmv = curr->val;
    }

    BSTtree *child = (curr->left != &nil) ? curr->left : curr->right;
    if (child != &nil) {
      child->parent = curr->parent;
    }
    if (curr->parent->left == curr) {
      curr->parent->left = child;
    }
    else {
      curr->parent->right = child;
    }

    delete curr;
    return rmv;
  }

  iterator begin()
  {
    if (left == &nil) {
      return iterator(this);
    }
    BSTtree *cur = left;
    while (cur->left != &nil) cur = cur->left;
    return iterator(cur);
  }

  const_iterator begin() const
  {
    if (left == &nil) {
      return const_iterator(this);
    }
    const BSTtree *cur = left;
    while (cur->left != &nil) cur = cur->left;
    return const_iterator(cur);
  }

  iterator end()
  {
    return iterator(this);
  }

  const_iterator end() const
  {
    return const_iterator(this);
  }

  const_iterator cbegin() const
  {
    return begin();
  }
  const_iterator cend() const
  {
    return end();
  }

  const_iterator rotateLeft(const_iterator it)
  {
    BSTtree *y = const_cast< BSTtree * >(it.ptr);
    if (y->isFakeRoot) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *x = y->parent;
    if (x->isFakeRoot || x == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    if (x->right != y) {
      throw std::logic_error("Your tree is broken, cannot be roatated");
    }
    x->right = y->left;

    if (y->left != &nil) {
      y->left->parent = x;
    }
    y->parent = x->parent;

    if (x->parent->isFakeRoot) {
      x->parent->left = y;
    }
    else if (x == x->parent->left) {
      x->parent->left = y;
    }
    else {
      x->parent->right = y;
    }

    y->left = x;
    x->parent = y;

    return const_iterator(y);
  }

  const_iterator rotateRight(const_iterator it)
  {
    BSTtree *y = const_cast<BSTtree *>(it.ptr);
    if (y->isFakeRoot) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *x = y->parent;
    if (x->isFakeRoot || x == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    if (x->left != y) {
      throw std::logic_error("Your tree is broken, cannot be roatated");
    }
    x->left = y->right;

    if (y->right != &nil) {
      y->right->parent = x;
    }
    y->parent = x->parent;

    if (x->parent->isFakeRoot) {
      x->parent->left = y;
    }
    else if (x == x->parent->left) {
      x->parent->left = y;
    }
    else {
      x->parent->right = y;
    }

    y->right = x;
    x->parent = y;

    return const_iterator(y);
  }

  const_iterator rotateLargeLeft(const_iterator it)
  {
    BSTtree *y = const_cast< BSTtree * >(it.ptr);
    if (y->isFakeRoot) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *z = y->parent;
    if (z->isFakeRoot || z == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *x = z->parent;
    if (x->isFakeRoot || x == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    if (z->left != y || x->right != z) {
      throw std::logic_error("Your tree is broken, cannot be rotated");
    }

    const_iterator y_it(y);
    rotateRight(y_it);
    return rotateLeft(const_iterator(y));
  }

  const_iterator rotateLargeRight(const_iterator it)
  {
    BSTtree *y = const_cast< BSTtree * >(it.ptr);
    if (y->isFakeRoot) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *z = y->parent;
    if (z->isFakeRoot || z == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    BSTtree *x = z->parent;
    if (x->isFakeRoot || x == &nil) {
      throw std::invalid_argument("Cannot parse fake part");
    }
    if (z->right != y || x->left != z) {
      throw std::logic_error("Your tree is broken, cannot be rotated");
    }

    const_iterator y_it(y);
    rotateLeft(y_it);
    return rotateRight(const_iterator(y));
  }

  size_t height(const_iterator it)
  {
    if (it.ptr->isFakeRoot) {
      return height();
    }
    return subHg(it.ptr);
  }

  size_t height() const
  {
    return subHg(left);
  }
};

template< class Key, class Value, class Compare >
BSTtree< Key, Value, Compare >::~BSTtree()
{
  if (isFakeRoot) {
    clearSubtree(left);
  }
}

template< class Key, class Value, class Compare >
BSTtree< Key, Value, Compare > BSTtree< Key, Value, Compare >::nil;
#endif
