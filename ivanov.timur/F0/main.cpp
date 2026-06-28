#include "IndexHandler.h"
#include <sstream>

int main() {
  IndexHandler handler;
  std::string line;

  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "read") {
      std::string idxName, filename;
      if (iss >> idxName >> filename) {
        handler.readIndex(idxName, filename);
      } else {
        std::cerr << "Usage: read <index_name> <input_file>\n";
      }
    }
    else if (cmd == "write") {
      std::string filename, idxName;
      if (iss >> filename >> idxName) {
        handler.writeIndex(filename, idxName);
      } else {
        std::cerr << "Usage: write <output_file> <index_name>\n";
      }
    }
    else if (cmd == "search") {
      std::string rest;
      std::getline(iss, rest);
      size_t start = rest.find_first_not_of(" \t");
      if (start == std::string::npos) {
        std::cout << "Empty query.\n";
        continue;
      }
      rest = rest.substr(start);

      std::istringstream iss2(rest);
      std::string w1, w2;
      if (iss2 >> w1 && iss2 >> w2) {
        std::string extra;
        if (!(iss2 >> extra) && handler.indexExists(w2)) {
          handler.searchWord(w1, w2);
        } else {
          handler.search(rest);
        }
      } else {
        handler.search(rest);
      }
    }
    else if (cmd == "right_merge") {
      std::string newIdx, idx1, idx2;
      if (iss >> newIdx >> idx1 >> idx2) {
        handler.rightMerge(newIdx, idx1, idx2);
      } else {
        std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
      }
    }
    else if (cmd == "down_merge") {
      std::string newIdx, idx1, idx2;
      if (iss >> newIdx >> idx1 >> idx2) {
        handler.downMerge(newIdx, idx1, idx2);
      } else {
        std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
      }
    }
    else if (cmd == "intersect") {
      std::string newIdx, idx1, idx2;
      if (iss >> newIdx >> idx1 >> idx2) {
        handler.intersect(newIdx, idx1, idx2);
      } else {
        std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
      }
    }
    else if (cmd == "subtract") {
      std::string newIdx, idx1, idx2;
      if (iss >> newIdx >> idx1 >> idx2) {
        handler.subtract(newIdx, idx1, idx2);
      } else {
        std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
      }
    }
    else if (cmd == "add") {
      std::string newIdx, idx1, idx2;
      int addPos;
      if (iss >> newIdx >> idx1 >> idx2 >> addPos) {
        handler.add(newIdx, idx1, idx2, addPos);
      } else {
        std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
      }
    }
    else if (cmd == "list") {
      handler.listIndexes();
    }
    else if (cmd == "delete") {
      std::string idxName;
      if (iss >> idxName) {
        handler.deleteIndex(idxName);
      } else {
        std::cerr << "Usage: delete <index_name>\n";
      }
    }
    else {
      std::cerr << "Unknown command: " << cmd << "\n";
    }
  }
  return 0;
}