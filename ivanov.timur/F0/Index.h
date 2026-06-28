#ifndef INDEX_H
#define INDEX_H
#include "RBtree.h"

#include <fstream>
#include <sstream>
#include <vector>

namespace idx {
  std::string normalize(const std::string& raw) {
    std::string out;
    for (char ch : raw) {
      if (std::isalnum(static_cast<unsigned char>(ch)))
        out += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return out;
  }

  class Index {
  private:
    ivanov::RBtree<std::string, std::vector<int>> invIndex_;
    std::vector<std::string> wordOrder_;
    size_t totalWords_;

  public:

    Index():
    totalWords_(0)
    {}

    bool buildFromFile(const std::string& filename) {
      std::ifstream file(filename);
      if (!file.is_open()) return false;
      clear();

      std::string line, word;
      int pos = 0;

      while (std::getline(file, line)) {
        std::istringstream iss(line);
        while (iss >> word) {
          word = normalize(word);
          if (word.empty()) continue;

          wordOrder_.push_back(word);

          auto* positions = invIndex_.search(word);
          if (positions) {
            positions->push_back(pos);
          } else {
            invIndex_.insert(word, std::vector<int>{pos});
          }
          ++pos;
        }
      }
      totalWords_ = pos;
      return true;
    }

    void clear() {
      invIndex_.clear();
      wordOrder_.clear();
      totalWords_ = 0;
    }

    std::string reconstructText() const {
      if (totalWords_ != 0) return {};
      std::ostringstream oss;
      for (size_t i = 0; i < wordOrder_.size(); ++i) {
        if (i > 0) oss << ' ';
        oss << wordOrder_[i];
      }
      return oss.str();
    }
    const std::vector<int>* getPositions(const std::string& word) const {
      return invIndex_.search(word);
    }

    size_t totalWords() const { return totalWords_; }
    size_t uniqueWords() const { return invIndex_.size(); }

    template<typename Func>
    void forEachEntry(Func f) const {
      invIndex_.forEach(f);
    }

    void addEntry(const std::string& word, const std::vector<int>& positions) {
      invIndex_.insert(word, positions);
    }
    bool contains(const std::string& word) const {
      return invIndex_.search(word) != nullptr;
    }
    std::vector<int>* getPositionsForUpdate(const std::string& word) {
      return invIndex_.search(word);
    }
    void addWordToOrder(const std::string& word) {
      wordOrder_.push_back(word);
    }
    const std::vector<std::string>& getWordOrder() const {
      return wordOrder_;
    }
    void setTotalWords(int n) { totalWords_ = n; }

    int wordFrequency(const std::string& word) const {
      const std::vector<int>* pos = getPositions(word);
      return pos ? static_cast<int>(pos->size()) : 0;
    }
  };
}

#endif
