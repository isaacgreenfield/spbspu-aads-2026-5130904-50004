#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <functional>
#include <sstream>
#include <algorithm>
#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/blake2.hpp>
#include <boost/hash2/flavor.hpp>
#include "HashTable.h"

namespace ivanov {
  template<class Key>
  struct Edge {
    Key from, to;

    bool operator==(const Edge &other) const noexcept {
      return from == other.from && to == other.to;
    }
  };

  template<class Key>
  struct EdgeHash {
    size_t operator()(const Edge<Key> &e) const noexcept {
      boost::hash2::blake2b_512 hasher;

      boost::hash2::hash_append(hasher, boost::hash2::default_flavor{}, e.from);
      boost::hash2::hash_append(hasher, boost::hash2::default_flavor{}, e.to);

      auto digest = hasher.result();
      size_t result = 0;
      std::memcpy(&result, digest.data(), sizeof(result));
      return result;
    }
  };

  template<class Key, class Weight = size_t>
  class Graph {
  public:
    std::vector<Key> vertices;
    HashTable<Edge<Key>, std::vector<Weight>, EdgeHash<Key>, std::equal_to<Edge<Key> > > edges;

    void createFrom(const std::vector<Key> &list) {
      vertices = list;
    }

    void addVertex(const Key &name) {
      vertices.push_back(name);
    }

    bool removeVertex(const Key &name);

    void addEdge(const Key &from, const Key &to, Weight w);

    bool removeEdge(const Key &from, const Key &to, Weight w);
  };

  template<class Key, class Weight>
  bool Graph<Key, Weight>::removeVertex(const Key &name) {
    auto it = std::find(vertices.begin(), vertices.end(), name);
    if (it == vertices.end()) return false;

    std::vector<Edge<Key> > toDelete;
    for (auto edgeIt = edges.begin(); edgeIt != edges.end(); ++edgeIt) {
      auto &edge = (*edgeIt).first;
      if (edge.from == name || edge.to == name) {
        toDelete.push_back(edge);
      }
    }
    for (auto &e: toDelete) {
      if (edges.has(e)) {
        edges.drop(e);
      }
    }
    vertices.erase(it);
    return true;
  }

  template<class Key, class Weight>
  void Graph<Key, Weight>::addEdge(const Key &from, const Key &to, Weight w) {
    if (std::find(vertices.begin(), vertices.end(), from) == vertices.end())
        vertices.push_back(from);
    if (std::find(vertices.begin(), vertices.end(), to) == vertices.end())
        vertices.push_back(to);
    Edge<Key> key{from, to};
    if (edges.has(key)) {
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            auto pair = *it;
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
      auto pair = *it;
      if (pair.first == key) {
        auto &weights = pair.second;
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

  class GraphManager {
  public:
    void execute(const std::string& line, bool& isAnything, bool silent = false);
    void loadFromFile(const std::string& filename);

  private:
    HashTable<std::string, Graph<std::string, int>, std::hash<std::string>, std::equal_to<std::string>> graphs;

    Graph<std::string, int>& getGraph(const std::string& name) {return graphs.at(name);}

    bool hasVertex(const Graph<std::string, int>& g, const std::string& v) const {
      return std::find(g.vertices.begin(), g.vertices.end(), v) != g.vertices.end();
    }

    static void addUnique(std::vector<std::string>& vec, const std::string& val) {
      if (std::find(vec.begin(), vec.end(), val) == vec.end())
        vec.push_back(val);
    }

    static bool contains(const std::vector<std::string>& vec, const std::string& val) {
      return std::find(vec.begin(), vec.end(), val) != vec.end();
    }

    static Graph<std::string, int> mergeGraphs(const Graph<std::string, int>& g1, const Graph<std::string, int>& g2);
    static Graph<std::string, int> extractGraph(const Graph<std::string, int>& g, const std::vector<std::string>& keep);
  };

  inline void GraphManager::execute(const std::string& line, bool& isAnything, bool silent) {
    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd)) return;

    if (cmd == "graphs") {
        if (silent) { isAnything = true; return; }
        std::vector<std::string> names;
        for (auto it = graphs.begin(); it != graphs.end(); ++it)
            names.push_back((*it).first);
        std::sort(names.begin(), names.end());
        bool flag = false;
        for (auto& name : names) {
            flag = true;
            std::cout << name << '\n';
        }
        if(!flag) std::cout << "\n";
    }
    else if (cmd == "vertexes") {
        std::string gname;
        if (!(iss >> gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (!graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (silent) {isAnything = true; return; }
        Graph<std::string, int>& g = getGraph(gname);
        std::vector<std::string> sverts = g.vertices;
        std::sort(sverts.begin(), sverts.end());
        sverts.erase(std::unique(sverts.begin(), sverts.end()), sverts.end());
        for (auto& v : sverts)
            std::cout << v << '\n';
    }
    else if (cmd == "outbound") {
        std::string gname, v;
        if (!(iss >> gname >> v)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (!graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        Graph<std::string, int>& g = getGraph(gname);
        if (!hasVertex(g, v)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (silent) { isAnything = true; return; }

        HashTable<std::string, std::vector<int>,std::hash<std::string>, std::equal_to<std::string>> out;
        for (auto it = g.edges.begin(); it != g.edges.end(); ++it) {
            auto& edge = (*it).first;
            if (edge.from == v) {
                const std::string& target = edge.to;
                if (out.has(target)) {
                    auto& wv = out.at(target);
                    auto& src = (*it).second;
                    wv.insert(wv.end(), src.begin(), src.end());
                } else {
                    out.add(target, (*it).second); // создаём копию вектора весов
                }
            }
        }

        std::vector<std::string> keys;
        for (auto it = out.begin(); it != out.end(); ++it)
            keys.push_back((*it).first);
        std::sort(keys.begin(), keys.end());

        for (auto& key : keys) {
            auto& weights = out.at(key);
            std::sort(weights.begin(), weights.end());
            std::cout << key;
            for (int w : weights) std::cout << ' ' << w;
            std::cout << '\n';
        }
    }
    else if (cmd == "inbound") {
        std::string gname, v;
        if (!(iss >> gname >> v)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (!graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        Graph<std::string, int>& g = getGraph(gname);
        if (!hasVertex(g, v)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (silent) {isAnything = true; return; }

        HashTable<std::string, std::vector<int>,std::hash<std::string>, std::equal_to<std::string>> in;
        for (auto it = g.edges.begin(); it != g.edges.end(); ++it) {
            auto& edge = (*it).first;
            if (edge.to == v) {
                const std::string& source = edge.from;
                if (in.has(source)) {
                    auto& wv = in.at(source);
                    auto& src = (*it).second;
                    wv.insert(wv.end(), src.begin(), src.end());
                } else {
                    in.add(source, (*it).second);
                }
            }
        }

        std::vector<std::string> keys;
        for (auto it = in.begin(); it != in.end(); ++it)
            keys.push_back((*it).first);
        std::sort(keys.begin(), keys.end());

        for (auto& key : keys) {
            auto& weights = in.at(key);
            std::sort(weights.begin(), weights.end());
            std::cout << key;
            for (int w : weights) std::cout << ' ' << w;
            std::cout << '\n';
        }
    }
    else if (cmd == "bind") {
        std::string gname, from, to;
        int weight;
        if (!(iss >> gname >> from >> to >> weight)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (!graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        Graph<std::string, int>& g = getGraph(gname);
        g.addEdge(from, to, weight);
    }
    else if (cmd == "cut") {
        std::string gname, from, to;
        int weight;
        if (!(iss >> gname >> from >> to >> weight)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (!graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        Graph<std::string, int>& g = getGraph(gname);
        if (!hasVertex(g, from) || !hasVertex(g, to)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        bool removed = g.removeEdge(from, to, weight);
        if (!removed && !silent) std::cout << "<INVALID COMMAND>\n";
    }
    else if (cmd == "create") {
        std::string gname;
        size_t count;
        if (!(iss >> gname >> count)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (graphs.has(gname)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        std::vector<std::string> verts(count);
        for (size_t i = 0; i < count; ++i) {
            if (!(iss >> verts[i])) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        }
        Graph<std::string, int> g;
        g.createFrom(verts);
        graphs.add(gname, g);
    }
    else if (cmd == "merge") {
        std::string newName, old1, old2;
        if (!(iss >> newName >> old1 >> old2)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (graphs.has(newName) || !graphs.has(old1) || !graphs.has(old2)) {
            if (!silent){ std::cout << "<INVALID COMMAND>\n"; }
            return;
        }
        Graph<std::string, int>& g1 = getGraph(old1);
        Graph<std::string, int>& g2 = getGraph(old2);
        Graph<std::string, int> merged = mergeGraphs(g1, g2);
        graphs.add(newName, merged);
    }
    else if (cmd == "extract") {
        std::string newName, oldName;
        size_t count;
        if (!(iss >> newName >> oldName >> count)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        if (graphs.has(newName) || !graphs.has(oldName)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        Graph<std::string, int>& oldG = getGraph(oldName);
        std::vector<std::string> keep(count);
        for (size_t i = 0; i < count; ++i) {
            if (!(iss >> keep[i])) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        }
        for (auto& v : keep) {
            if (!hasVertex(oldG, v)) { if (!silent) std::cout << "<INVALID COMMAND>\n"; return; }
        }
        Graph<std::string, int> extracted = extractGraph(oldG, keep);
        graphs.add(newName, extracted);
    }
    else {
        if (!silent) std::cout << "<INVALID COMMAND>\n";
    }
}

  inline void GraphManager::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
      std::cerr << "Error opening file: " << filename << std::endl;
      std::exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
      if (line.empty()) continue;

      std::istringstream iss(line);
      std::string graphName;
      std::size_t edgesCount;
      if (!(iss >> graphName >> edgesCount)) continue;

      Graph<std::string, int> g;
      for (std::size_t i = 0; i < edgesCount; ++i) {
        if (!std::getline(file, line) || line.empty()) {
          break;
        }
        std::istringstream edgeIss(line);
        std::string from, to;
        int weight;
        if (edgeIss >> from >> to >> weight) {
          g.addEdge(from, to, weight);
        } else {
        }
      }
      graphs.add(graphName, g);
    }
  }

  inline Graph<std::string, int> GraphManager::mergeGraphs(const Graph<std::string, int> &g1, const Graph<std::string, int> &g2) {
    Graph<std::string, int> res;
    for (auto& v : g1.vertices) addUnique(res.vertices, v);
    for (auto& v : g2.vertices) addUnique(res.vertices, v);

    auto addAllEdges = [&](const Graph<std::string, int>& src) {
      for (auto it = src.edges.begin(); it != src.edges.end(); ++it) {
        auto& e = (*it).first;
        for (size_t w : (*it).second) res.addEdge(e.from, e.to, w);
      }
    };
    addAllEdges(g1);
    addAllEdges(g2);
    return res;
  }
  inline Graph<std::string, int> GraphManager::extractGraph(const Graph<std::string, int> &g, const std::vector<std::string> &keep) {
    Graph<std::string, int> res;
    res.vertices = keep;

    for (auto it = g.edges.begin(); it != g.edges.end(); ++it) {
      auto& e = (*it).first;
      if (contains(keep, e.from) && contains(keep, e.to)) {
        for (size_t w : (*it).second) res.addEdge(e.from, e.to, w);
      }
    }
    return res;
  }
}

#endif
