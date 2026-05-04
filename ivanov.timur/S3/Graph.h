#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <functional>
#include "HashTable.h"

namespace ivanov {
  template<class Key>
  struct Edge {
    Key from, to;
    bool operator==(const Edge& other) const noexcept {
      return from == other.from && to == other.to;
    }
  };

  template<class Key>
  struct EdgeHash {
    size_t operator()(const Edge<Key>& e) const noexcept {
      return 1;
    }
  };

  template<class Key, class Weight = size_t>
  class Graph {
  public:
    std::vector<Key> vertices;
    HashTable<Edge<Key>,std::vector<Weight>, EdgeHash<Key>,std::equal_to<Edge<Key>>> edges;

    void createFrom(const std::vector<Key>& list) {
      vertices = list;
    }
    void addVertex(const Key& name) {
      vertices.push_back(name);
    }
    bool removeVertex(const Key& name);

    void addEdge(const Key& from, const Key& to, Weight w);
    bool removeEdge(const Key& from, const Key& to, Weight w);
  };

  template<class Key, class Weight>
  bool Graph<Key, Weight>::removeVertex(const Key &name) {
    auto it = std::find(vertices.begin(), vertices.end(), name);
    if (it == vertices.end()) return false;

    std::vector<Edge<Key>> toDelete;
    for (auto edgeIt = edges.begin(); edgeIt != edges.end(); ++edgeIt) {
      const auto& edge = (*edgeIt).first;
      if (edge.from == name || edge.to == name) {
        toDelete.push_back(edge);
      }
    }
    for (const auto& e : toDelete) {
      if (edges.has(e)) {
        edges.drop(e);
      }
    }
    vertices.erase(it);
    return true;
  }

  template<class Key, class Weight>
  void Graph<Key, Weight>::addEdge(const Key &from, const Key &to, Weight w) {
    Edge<Key> key{from, to};
    if (edges.has(key)) {
      for (auto it = edges.begin(); it != edges.end(); ++it) {
        auto& pair = *it;
        if (pair.first == key) {
          pair.second.push_back(w);
          return;
        }
      }
    } else {
      edges.add(key, {w});
    }
  }

  template<class Key, class Weight>
  bool Graph<Key, Weight>::removeEdge(const Key &from, const Key &to, Weight w) {
    Edge<Key> key{from, to};
    if (!edges.has(key)) return false;
    for (auto it = edges.begin(); it != edges.end(); ++it) {
      auto& pair = *it;
      if (pair.first == key) {
        auto& weights = pair.second;
        auto wIt = std::find(weights.begin(), weights.end(), w);
        if (wIt == weights.end()) return false;
        weights.erase(wIt);
        if (weights.empty()) {
          edges.drop(key);
        }
        return true;
      }
    }
    return false;
  }
}

#endif
