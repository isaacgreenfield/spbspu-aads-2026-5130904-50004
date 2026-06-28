#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H

#include "Index.h"
#include "RBtree.h"
#include "ix-vector.h"
#include <algorithm>
#include <cmath>

using namespace idx;

class IndexHandler {
private:
  ivanov::RBtree<std::string, Index*> indexesTree_;

  using TreeIterator = typename ivanov::RBtree<std::string, Index*>::iterator;
  using ConstTreeIterator = typename ivanov::RBtree<std::string, Index*>::const_iterator;

  TreeIterator findIndex(const std::string& name) {
    return indexesTree_.find(name);
  }

  ConstTreeIterator findIndex(const std::string& name) const {
    return indexesTree_.find(name);
  }

  void addIndex(const std::string& name, Index* newIdx) {
    auto it = findIndex(name);
    if (it != indexesTree_.end()) {
      delete it->second;
      indexesTree_.remove(name);
    }
    indexesTree_.insert(name, newIdx);
  }

  Index* buildFromWords(const idx::vector<std::string>& words, bool skipMarkers) {
    Index* idx = new Index();
    int pos = 0;
    for (const std::string& w : words) {
      idx->addWordToOrder(w);
      if (skipMarkers && w == "\n") {
        continue;
      }
      auto it = idx->find(w);
      if (it != idx->end()) {
        it->second.push_back(pos);
      } else {
        idx::vector<int> vec{pos};
        idx->addEntry(w, vec);
      }
      ++pos;
    }
    idx->setTotalWords(pos);
    return idx;
  }

  void searchTFIDF(const std::string& query) {
    idx::vector<std::string> queryWords;
    size_t start = 0;
    while (start < query.size()) {
      while (start < query.size() && std::isspace(static_cast<unsigned char>(query[start])))
        ++start;
      if (start == query.size()) break;
      size_t end = start;
      while (end < query.size() && !std::isspace(static_cast<unsigned char>(query[end])))
        ++end;
      std::string word = query.substr(start, end - start);
      std::string norm = normalize(word);
      if (!norm.empty()) queryWords.push_back(norm);
      start = end;
    }

    if (queryWords.empty()) {
      std::cout << "Empty query\n";
      return;
    }

    int N = 0;
    for (auto it = indexesTree_.begin(); it != indexesTree_.end(); ++it) {
      ++N;
    }
    if (N == 0) {
      std::cout << "No active indexes\n";
      return;
    }

    idx::vector<std::string> uniqueWords;
    for (const auto& qw : queryWords) {
      bool found = false;
      for (const auto& uw : uniqueWords) {
        if (uw == qw) { found = true; break; }
      }
      if (!found) uniqueWords.push_back(qw);
    }

    ivanov::RBtree<std::string, double> idfTree;
    for (const auto& word : uniqueWords) {
      int df = 0;
      for (auto it = indexesTree_.begin(); it != indexesTree_.end(); ++it) {
        if (it->second->contains(word)) ++df;
      }
      double idf = (df > 0) ? std::log(static_cast<double>(N) / df) : 0.0;
      idfTree.insert(word, idf);
    }

    idx::vector<std::pair<std::string, double>> scores;
    for (auto it = indexesTree_.begin(); it != indexesTree_.end(); ++it) {
      const std::string& docName = it->first;
      Index* idx = it->second;
      double score = 0.0;
      int totalWords = idx->totalWords();
      if (totalWords == 0) continue;
      idfTree.forEach([&](const std::string& word, double idf) {
        int tf = idx->wordFrequency(word);
        if (tf > 0) {
          double tfNorm = static_cast<double>(tf) / totalWords;
          score += tfNorm * idf;
        }
      });
      if (score > 0.0) {
        scores.emplace_back(docName, score);
      }
    }

    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "Search results for '" << query << "':\n";
    if (scores.empty()) {
      std::cout << "  (no matches)\n";
    } else {
      for (const auto& sc : scores) {
        std::cout << "- " << sc.first << " (score: " << sc.second << ")\n";
      }
    }
  }

public:
  IndexHandler() = default;

  ~IndexHandler() {
    for (auto it = indexesTree_.begin(); it != indexesTree_.end(); ++it) {
      delete it->second;
    }
  }

  void search(const std::string& query) {
    searchTFIDF(query);
  }

  void readIndex(const std::string& name, const std::string& filename) {
    auto it = findIndex(name);
    if (it != indexesTree_.end()) {
      delete it->second;
      indexesTree_.remove(name);
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: cannot open file " << filename << "\n";
      return;
    }

    Index* newIdx = nullptr;
    try {
      newIdx = new Index(file);
    } catch (const std::exception& e) {
      std::cerr << "Error creating index: " << e.what() << "\n";
      return;
    }

    indexesTree_.insert(name, newIdx);
    std::cout << "Index '" << name << "' created from " << filename
              << " (" << newIdx->totalWords() << " words)\n";
  }

  void writeIndex(const std::string& filename, const std::string& indexName) {
    auto it = findIndex(indexName);
    if (it == indexesTree_.end()) {
      std::cerr << "Error: index '" << indexName << "' not found." << "\n";
      return;
    }
    Index* idx = it->second;
    if (idx->totalWords() == 0) {
      std::cerr << "Error: index '" << indexName << "' cannot be reconstructed." << "\n";
      return;
    }
    std::ofstream out(filename);
    if (!out) {
      std::cerr << "Error: cannot write to " << filename << "\n";
      return;
    }
    out << idx->reconstructText();
    std::cout << "Text restored from '" << indexName << "' and written to " << filename << "\n";
  }

  void searchWord(const std::string& word, const std::string& indexName) {
    auto it = findIndex(indexName);
    if (it == indexesTree_.end()) {
      std::cerr << "Error: index '" << indexName << "' not found.\n";
      return;
    }
    Index* idx = it->second;
    auto wordIt = idx->find(word);
    if (wordIt == idx->end() || wordIt->second.empty()) {
      std::cout << "Word '" << word << "' not found in '" << indexName << "'\n";
    } else {
      const auto& positions = wordIt->second;
      std::cout << "Word '" << word << "' found at positions: ";
      for (size_t i = 0; i < positions.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << positions[i] + 1;
      }
      std::cout << "\n";
    }
  }

  void rightMerge(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    auto it1 = findIndex(idx1Name);
    auto it2 = findIndex(idx2Name);
    if (it1 == indexesTree_.end() || it2 == indexesTree_.end()) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    Index* idx1 = it1->second;
    Index* idx2 = it2->second;
    idx::vector<std::string> words = idx2->getWordOrder();
    const auto& w1 = idx1->getWordOrder();
    words.insert(words.end(), w1.begin(), w1.end());

    Index* newIdx = buildFromWords(words, false);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created from '" << idx1Name
              << "' and '" << idx2Name << "' (total " << newIdx->totalWords()
              << " words)\n";
  }

  void downMerge(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    auto it1 = findIndex(idx1Name);
    auto it2 = findIndex(idx2Name);
    if (it1 == indexesTree_.end() || it2 == indexesTree_.end()) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    Index* idx1 = it1->second;
    Index* idx2 = it2->second;
    idx::vector<std::string> words = idx2->getWordOrder();
    words.push_back("\n");
    const auto& w1 = idx1->getWordOrder();
    words.insert(words.end(), w1.begin(), w1.end());

    Index* newIdx = buildFromWords(words, true);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created from '" << idx1Name
              << "' and '" << idx2Name << "' (total " << newIdx->totalWords()
              << " words)\n";
  }

  void deleteIndex(const std::string& name) {
    auto it = findIndex(name);
    if (it == indexesTree_.end()) {
      std::cerr << "Error: index '" << name << "' not found.\n";
      return;
    }
    delete it->second;
    indexesTree_.remove(name);
    std::cout << "Index '" << name << "' deleted\n";
  }

  void listIndexes() const {
    std::cout << "Active indexes:\n";
    if (indexesTree_.empty()) {
      std::cout << "  (none)\n";
      return;
    }
    for (auto it = indexesTree_.begin(); it != indexesTree_.end(); ++it) {
      std::cout << "- " << it->first << " (" << it->second->totalWords()
                << " words, " << it->second->uniqueWords() << " unique)\n";
    }
  }

  void add(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name, int addPos) {
    auto it1 = findIndex(idx1Name);
    auto it2 = findIndex(idx2Name);
    if (it1 == indexesTree_.end() || it2 == indexesTree_.end()) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    Index* idx1 = it1->second;
    Index* idx2 = it2->second;
    idx::vector<std::string> words = idx1->getWordOrder();
    int size = static_cast<int>(words.size());
    if (addPos < 1) addPos = 1;
    if (addPos > size + 1) addPos = size + 1;
    const auto& w2 = idx2->getWordOrder();
    auto insertIt = words.begin() + (addPos - 1);
    words.insert(insertIt, w2.begin(), w2.end());

    Index* newIdx = buildFromWords(words, false);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with doc2_index merged into position "
              << addPos << " of doc1_index\n";
  }

  void intersect(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    auto it1 = findIndex(idx1Name);
    auto it2 = findIndex(idx2Name);
    if (it1 == indexesTree_.end() || it2 == indexesTree_.end()) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    Index* idx1 = it1->second;
    Index* idx2 = it2->second;

    Index* newIdx = new Index();
    int total = 0;
    for (auto entryIt = idx1->begin(); entryIt != idx1->end(); ++entryIt) {
      const std::string& word = entryIt->first;
      const idx::vector<int>& pos1 = entryIt->second;
      auto foundIt = idx2->find(word);
      if (foundIt != idx2->end()) {
        idx::vector<int> combined = pos1;
        const auto& pos2 = foundIt->second;
        combined.insert(combined.end(), pos2.begin(), pos2.end());
        newIdx->addEntry(word, combined);
        total += combined.size();
      }
    }

    newIdx->setTotalWords(total);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with "
              << newIdx->uniqueWords() << " common words\n";
  }

  void subtract(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    auto it1 = findIndex(idx1Name);
    auto it2 = findIndex(idx2Name);
    if (it1 == indexesTree_.end() || it2 == indexesTree_.end()) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    Index* idx1 = it1->second;
    Index* idx2 = it2->second;

    Index* newIdx = new Index();
    int total = 0;
    for (auto entryIt = idx1->begin(); entryIt != idx1->end(); ++entryIt) {
      const std::string& word = entryIt->first;
      if (!idx2->contains(word)) {
        newIdx->addEntry(word, entryIt->second);
        total += entryIt->second.size();
      }
    }

    newIdx->setTotalWords(total);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with "
              << newIdx->uniqueWords() << " unique words from '"
              << idx1Name << "'\n";
  }
};

#endif