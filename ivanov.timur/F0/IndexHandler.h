#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H

#include "Index.h"
#include "RBtree.h"
#include <sstream>
#include <algorithm>
#include <cmath>

class IndexHandler {
private:
  ivanov::RBtree<std::string, Index*> indexesTree_;

  Index* findIndex(const std::string& name) const {
    Index** ptr = indexesTree_.search(name);
    return ptr ? *ptr : nullptr;
  }

  void addIndex(const std::string& name, Index* newIdx) {
    if (findIndex(name)) {
      indexesTree_.remove(name);
      delete findIndex(name);
    }
    indexesTree_.insert(name, newIdx);
  }

  Index* buildFromWords(const std::vector<std::string>& words, bool skipMarkers) {
    Index* idx = new Index();
    int pos = 0;
    for (const std::string& w : words) {
      idx->addWordToOrder(w);
      if (skipMarkers && w == "\n") {
        continue;
      }
      auto* vec = idx->getPositionsForUpdate(w);
      if (vec) {
        vec->push_back(pos);
      } else {
        idx->addEntry(w, std::vector<int>{pos});
      }
      ++pos;
    }
    idx->setTotalWords(pos);
    return idx;
  }

  std::vector<std::pair<std::string, Index*>> getAllIndexes() const {
    std::vector<std::pair<std::string, Index*>> result;
    indexesTree_.forEach([&](const std::string& name, Index* idx) {
      result.emplace_back(name, idx);
    });
    return result;
  }

  void searchTFIDF(const std::string& query) {
    std::istringstream iss(query);
    std::vector<std::string> queryWords;
    std::string w;
    while (iss >> w) {
      std::string norm = Index::normalize_(w);
      if (!norm.empty()) queryWords.push_back(norm);
    }
    if (queryWords.empty()) {
      std::cout << "Empty query\n";
      return;
    }

    auto indexes = getAllIndexes();
    int N = static_cast<int>(indexes.size());
    if (N == 0) {
      std::cout << "No active indexes\n";
      return;
    }

    ivanov::RBtree<std::string, double> idfTree;
    std::vector<std::string> uniqueWords;
    for (const auto& qw : queryWords) {
      bool found = false;
      for (const auto& uw : uniqueWords) {
        if (uw == qw) { found = true; break; }
      }
      if (!found) uniqueWords.push_back(qw);
    }

    for (const auto& word : uniqueWords) {
      int df = 0;
      for (const auto& p : indexes) {
        if (p.second->contains(word)) ++df;
      }
      double idf = (df > 0) ? std::log(static_cast<double>(N) / df) : 0.0;
      idfTree.insert(word, idf);
    }

    std::vector<std::pair<std::string, double>> scores;
    for (const auto& p : indexes) {
      double score = 0.0;
      int totalWords = p.second->totalWords();
      if (totalWords == 0) continue;
      idfTree.forEach([&](const std::string& word, double idf) {
        int tf = p.second->wordFrequency(word);
        if (tf > 0) {
          double tfNorm = static_cast<double>(tf) / totalWords;
          score += tfNorm * idf;
        }
      });
      if (score > 0.0) {
        scores.emplace_back(p.first, score);
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
    indexesTree_.forEach([this](const std::string& name, Index* idx) {
      delete idx;
    });
  }

  void search(const std::string& query) {
    searchTFIDF(query);
  }

  void readIndex(const std::string& name, const std::string& filename) {
    Index* existing = findIndex(name);
    if (existing) {
      indexesTree_.remove(name);
      delete existing;
    }

    Index* newIdx = new Index();
    if (!newIdx->buildFromFile(filename)) {
      std::cerr << "Error: cannot open file " << filename << "\n";
      delete newIdx;
      return;
    }
    indexesTree_.insert(name, newIdx);
    std::cout << "Index '" << name << "' created from " << filename << " (" << newIdx->totalWords() << " words)" << "\n";
  }

  void writeIndex(const std::string& filename, const std::string& indexName) {
    Index* idx = findIndex(indexName);
    if (!idx) {
      std::cerr << "Error: index '" << indexName << "' not found." << "\n";
      return;
    }
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
    Index* idx = findIndex(indexName);
    if (!idx) {
      std::cerr << "Error: index '" << indexName << "' not found.\n";
      return;
    }
    const std::vector<int>* pos = idx->getPositions(word);
    if (!pos || pos->empty()) {
      std::cout << "Word '" << word << "' not found in '" << indexName << "'\n";
    } else {
      std::cout << "Word '" << word << "' found at positions: ";
      for (size_t i = 0; i < pos->size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << (*pos)[i] + 1;
      }
      std::cout << "\n";
    }
  }

  void rightMerge(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    Index* idx1 = findIndex(idx1Name);
    Index* idx2 = findIndex(idx2Name);
    if (!idx1 || !idx2) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    std::vector<std::string> words = idx2->getWordOrder();
    const auto& w1 = idx1->getWordOrder();
    words.insert(words.end(), w1.begin(), w1.end());

    Index* newIdx = buildFromWords(words, false);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created from '" << idx1Name
              << "' and '" << idx2Name << "' (total " << newIdx->totalWords()
              << " words)\n";
  }

  void downMerge(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    Index* idx1 = findIndex(idx1Name);
    Index* idx2 = findIndex(idx2Name);
    if (!idx1 || !idx2) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    std::vector<std::string> words = idx2->getWordOrder();
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
    Index* idx = findIndex(name);
    if (!idx) {
      std::cerr << "Error: index '" << name << "' not found.\n";
      return;
    }
    indexesTree_.remove(name);
    delete idx;
    std::cout << "Index '" << name << "' deleted\n";
  }

  void listIndexes() const {
    std::cout << "Active indexes:\n";
    if (indexesTree_.empty()) {
      std::cout << "  (none)\n";
      return;
    }
    indexesTree_.forEach([](const std::string& name, Index* idx) {
      std::cout << "- " << name << " (" << idx->totalWords()
                << " words, " << idx->uniqueWords() << " unique)\n";
    });
  }

  void add(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name, int addPos) {
    Index* idx1 = findIndex(idx1Name);
    Index* idx2 = findIndex(idx2Name);
    if (!idx1 || !idx2) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }
    std::vector<std::string> words = idx1->getWordOrder();
    int size = static_cast<int>(words.size());
    if (addPos < 1) addPos = 1;
    if (addPos > size + 1) addPos = size + 1;
    const auto& w2 = idx2->getWordOrder();
    auto it = words.begin() + (addPos - 1);
    words.insert(it, w2.begin(), w2.end());

    Index* newIdx = buildFromWords(words, false);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with doc2_index merged into position "
              << addPos << " of doc1_index\n";
  }

  void intersect(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    Index* idx1 = findIndex(idx1Name);
    Index* idx2 = findIndex(idx2Name);
    if (!idx1 || !idx2) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }

    Index* newIdx = new Index();
    int total = 0;
    idx1->forEachEntry([&](const std::string& word, const std::vector<int>& pos1) {
      const std::vector<int>* pos2 = idx2->getPositions(word);
      if (pos2) {
        std::vector<int> combined = pos1;
        combined.insert(combined.end(), pos2->begin(), pos2->end());
        newIdx->addEntry(word, combined);
        total += combined.size();
      }
    });

    newIdx->setTotalWords(total);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with "
              << newIdx->uniqueWords() << " common words\n";
  }

  void subtract(const std::string& newName, const std::string& idx1Name, const std::string& idx2Name) {
    Index* idx1 = findIndex(idx1Name);
    Index* idx2 = findIndex(idx2Name);
    if (!idx1 || !idx2) {
      std::cerr << "Error: one of source indexes does not exist.\n";
      return;
    }

    Index* newIdx = new Index();
    int total = 0;
    idx1->forEachEntry([&](const std::string& word, const std::vector<int>& pos1) {
      if (!idx2->contains(word)) {
        newIdx->addEntry(word, pos1);
        total += pos1.size();
      }
    });

    newIdx->setTotalWords(total);
    addIndex(newName, newIdx);
    std::cout << "Index '" << newName << "' created with "
              << newIdx->uniqueWords() << " unique words from '"
              << idx1Name << "'\n";
  }
};

#endif