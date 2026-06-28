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

    Index():
    totalWords_(0)
    {}

    bool buildFromFile(const std::string& filename) {
      std::ifstream file(filename);
      if (!file.is_open()) return false;
      clear();

      std::string line;
      int pos = 0;

      while (std::getline(file, line)) {
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
            wordOrder_.push_back(word);

            auto* positions = invIndex_.search(word);
            if (positions) {
              positions->push_back(pos);
            } else {
              vector<int> tmp;
              tmp.push_back(pos);
              invIndex_.insert(word, tmp);
            }
            ++pos;
          }
          start = end;
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
      if (totalWords_ == 0) return {};
      std::string result;
      size_t totalChars = 0;
      for (size_t i = 0; i < wordOrder_.size(); ++i)
        totalChars += wordOrder_[i].size();
      result.reserve(totalChars + wordOrder_.size() - 1);

      for (size_t i = 0; i < wordOrder_.size(); ++i) {
        if (i > 0) result += ' ';
        result += wordOrder_[i];
      }
      return result;
    }

    const vector<int>* getPositions(const std::string& word) const {
      return invIndex_.search(word);
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
      return invIndex_.search(word) != nullptr;
    }
    vector<int>* getPositionsForUpdate(const std::string& word) {
      return invIndex_.search(word);
    }
    void addWordToOrder(const std::string& word) {
      wordOrder_.push_back(word);
    }
    const vector<std::string>& getWordOrder() const {
      return wordOrder_;
    }
    void setTotalWords(int n) { totalWords_ = n; }

    int wordFrequency(const std::string& word) const {
      const vector<int>* pos = getPositions(word);
      return pos ? static_cast<int>(pos->size()) : 0;
    }
  };
}

#endif
