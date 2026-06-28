#ifndef INDEX_H
#define INDEX_H
#include "RBtree.h"
#include "ix-vector.h"

#include <fstream>

namespace idx {
  inline std::string normalize(const std::string& raw) {
    std::string out;
    for (char ch : raw) {
      if (std::isalnum(static_cast<unsigned char>(ch)))
        out += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return out;
  }

  class Index {
  private:
    ivanov::RBtree<std::string, vector<int>> invIndex_;
    vector<std::string> wordOrder_;
    size_t totalWords_;

  public:
    class iterator;
    class const_iterator;

    Index() : totalWords_(0) {}

    explicit Index(std::istream& input) : totalWords_(0) {
      ivanov::RBtree<std::string, vector<int>> tmpInvIndex;
      vector<std::string> tmpWordOrder;
      size_t pos = 0;

      std::string line;
      while (std::getline(input, line)) {
        size_t start = 0;
        while (start < line.size()) {
          while (start < line.size() && std::isspace(static_cast<unsigned char>(line[start])))
            ++start;
          if (start == line.size()) break;

          size_t end = start;
          while (end < line.size() && !std::isspace(static_cast<unsigned char>(line[end])))
            ++end;

          std::string word = normalize(line.substr(start, end - start));
          if (!word.empty()) {
            tmpWordOrder.push_back(word);

            auto it = tmpInvIndex.find(word);
            if (it != tmpInvIndex.end()) {
              it->second.push_back(pos);
            } else {
              vector<int> tmp;
              tmp.push_back(pos);
              tmpInvIndex.insert(word, tmp);
            }
            ++pos;
          }
          start = end;
        }
      }
      if (input.bad()) {
        throw std::runtime_error("Error reading from stream");
      }
      std::swap(invIndex_, tmpInvIndex);
      std::swap(wordOrder_, tmpWordOrder);
      totalWords_ = pos;
    }

    void clear() {
      invIndex_.clear();
      wordOrder_.clear();
      totalWords_ = 0;
    }

    std::string reconstructText() const {
      if (totalWords_ == 0) return {};
      std::string result;
      for (size_t i = 0; i < wordOrder_.size(); ++i) {
        const std::string& w = wordOrder_[i];
        if (w == "\n") {
          result += '\n';
        } else {
          if (i > 0 && wordOrder_[i-1] != "\n")
            result += ' ';
          result += w;
        }
      }
      return result;
    }

    size_t totalWords() const { return totalWords_; }
    size_t uniqueWords() const { return invIndex_.size(); }

    template<typename Func>
    void forEachEntry(Func f) const {
      invIndex_.forEach(f);
    }

    void addEntry(const std::string& word, const vector<int>& positions) {
      invIndex_.insert(word, positions);
    }
    bool contains(const std::string& word) const {
      return invIndex_.contains(word);
    }

    void addWordToOrder(const std::string& word) {
      wordOrder_.push_back(word);
    }
    const vector<std::string>& getWordOrder() const {
      return wordOrder_;
    }
    void setTotalWords(int n) { totalWords_ = n; }

    int wordFrequency(const std::string& word) const {
      const_iterator it = find(word);
      return (it != end()) ? static_cast<int>(it->second.size()) : 0;
    }

    iterator find(const std::string& word);
    const_iterator find(const std::string& word) const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    friend class iterator;
    friend class const_iterator;
  };

  class Index::iterator {
    friend class Index;
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const std::string, vector<int>>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    iterator& operator++() {
      ++treeIt_;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    iterator& operator--() {
      --treeIt_;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const { return treeIt_ == other.treeIt_; }
    bool operator!=(const iterator& other) const { return treeIt_ != other.treeIt_; }

    reference operator*() const { return *treeIt_; }
    pointer operator->() const { return &(*treeIt_); }

  private:
    using TreeIterator = typename ivanov::RBtree<std::string, vector<int>>::iterator;
    TreeIterator treeIt_;

    explicit iterator(TreeIterator it) : treeIt_(it) {}
  };

  class Index::const_iterator {
    friend class Index;
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const std::pair<const std::string, vector<int>>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    const_iterator& operator++() {
      ++treeIt_;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    const_iterator& operator--() {
      --treeIt_;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const const_iterator& other) const { return treeIt_ == other.treeIt_; }
    bool operator!=(const const_iterator& other) const { return treeIt_ != other.treeIt_; }

    reference operator*() const { return *treeIt_; }
    pointer operator->() const { return &(*treeIt_); }

    const_iterator(const iterator& it) : treeIt_(it.treeIt_) {}

  private:
    using TreeConstIterator = typename ivanov::RBtree<std::string, vector<int>>::const_iterator;
    TreeConstIterator treeIt_;

    explicit const_iterator(TreeConstIterator it) : treeIt_(it) {}
  };

  inline Index::iterator Index::begin() {
    return iterator(invIndex_.begin());
  }
  inline Index::iterator Index::end() {
    return iterator(invIndex_.end());
  }
  inline Index::const_iterator Index::begin() const {
    return const_iterator(invIndex_.begin());
  }
  inline Index::const_iterator Index::end() const {
    return const_iterator(invIndex_.end());
  }
  inline Index::const_iterator Index::cbegin() const {
    return begin();
  }
  inline Index::const_iterator Index::cend() const {
    return end();
  }

  inline Index::iterator Index::find(const std::string& word) {
    auto it = invIndex_.find(word);
    return iterator(it);
  }
  inline Index::const_iterator Index::find(const std::string& word) const {
    auto it = invIndex_.find(word);
    return const_iterator(it);
  }
}

#endif