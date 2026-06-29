#ifndef BSTMANAGER_H
#define BSTMANAGER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "idx-vector.h"
#include "BSTtree.h"

namespace ivanov {
  inline void invalid()
  {
    std::cout << "<INVALID COMMAND>\n";
  }

  class BSTManager {
  public:
    using tree = BSTtree< int, std::string, std::less<int> >;

    void loadFromFile(const std::string &filename);

    void execute(const std::string &line, bool &isAnything, bool silent);

  private:
    idx::vector< std::pair<std::string, tree> > datasets;

    tree *getDataset(const std::string &name);

    const tree *getDatasetConst(const std::string &name) const;

    bool contains(const tree &tr, int key);
  };

  inline void BSTManager::loadFromFile(const std::string &filename)
  {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "File opening error " << filename << "\n";
      std::exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
      if (line.empty()) {
        continue;
      }
      std::istringstream iss(line);
      std::string dname;
      if (!(iss >> dname)) {
        continue;
      }

      tree *tr = getDataset(dname);
      if (!tr) {
        datasets.emplace_back(dname, tree());
        tr = &datasets[datasets.size() - 1].second;
      }

      int key;
      std::string val;
      while (iss >> key >> val) {
        tr->push(key, val);
      }
    }
  }

  inline void BSTManager::execute(const std::string &line, bool &isAnything, bool silent)
  {
    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd)) {
      return;
    }

    if (cmd == "print") {
      std::string dname;
      if (!(iss >> dname)) {
        if (!silent) {
          invalid();
        }
        return;
      }

      const tree *tr = getDatasetConst(dname);
      if (!tr) {
        if (!silent) {
          invalid();
        }
        return;
      }

      tree::const_iterator begin = tr->begin();
      tree::const_iterator end = tr->end();

      if (begin == end) {
        if (!silent) {
          std::cout << "<EMPTY>\n";
        }
        else isAnything = false;
        return;
      }
      if (silent) {
        isAnything = true;
        return;
      }

      std::cout << dname;
      for (tree::const_iterator it = begin; it != end; ++it) {
        std::pair< const int &, const std::string & > kv = *it;
        std::cout << ' ' << kv.first << ' ' << kv.second;
      }
      std::cout << '\n';
    } else if (cmd == "complement" || cmd == "intersect" || cmd == "union") {
      std::string name, name1, name2;
      if (!(iss >> name >> name1 >> name2)) {
        if (!silent) {
          invalid();
        }
        return;
      }

      const tree *tr1 = getDatasetConst(name1);
      const tree *tr2 = getDatasetConst(name2);
      if (!tr1 || !tr2) {
        if (!silent) {
          invalid();
        }
        return;
      }

      tree newtree;

      if (cmd == "complement") {
        for (tree::const_iterator it = tr1->begin(); it != tr1->end(); ++it) {
          std::pair< const int &, const std::string & > kv = *it;
          if (!contains(*tr2, kv.first)) {
            newtree.push(kv.first, kv.second);
          }
        }
      } else if (cmd == "intersect") {
        for (tree::const_iterator it = tr1->begin(); it != tr1->end(); ++it) {
          std::pair< const int &, const std::string & > kv = *it;
          if (contains(*tr2, kv.first)) {
            newtree.push(kv.first, kv.second);
          }
        }
      } else {
        for (tree::const_iterator it = tr1->begin(); it != tr1->end(); ++it) {
          std::pair< const int &, const std::string & > kv = *it;
          newtree.push(kv.first, kv.second);
        }
        for (tree::const_iterator it = tr2->begin(); it != tr2->end(); ++it) {
          std::pair< const int &, const std::string & > kv = *it;
          if (!contains(*tr1, kv.first)) {
            newtree.push(kv.first, kv.second);
          }
        }
      }

      tree *existing = getDataset(name);
      if (existing) {
        *existing = std::move(newtree);
      }
      else {
        datasets.emplace_back(name, std::move(newtree));
      }

      if (silent) {
        isAnything = false;
      }
    } else if (!silent) {
      invalid();
    }
  }

  inline BSTManager::tree *BSTManager::getDataset(const std::string &name)
  {
    for (size_t i = 0; i < datasets.size(); ++i) {
      if (datasets[i].first == name) {
        return &datasets[i].second;
      }
    }
    return nullptr;
  }

  inline const BSTManager::tree *BSTManager::getDatasetConst(const std::string &name) const
  {
    for (size_t i = 0; i < datasets.size(); ++i) {
      if (datasets[i].first == name) {
        return &datasets[i].second;
      }
    }
    return nullptr;
  }

  inline bool BSTManager::contains(const tree &tr, int key)
  {
    try {
      tr.get(key);
      return true;
    } catch (...) {
      return false;
    }
  }
}
#endif
