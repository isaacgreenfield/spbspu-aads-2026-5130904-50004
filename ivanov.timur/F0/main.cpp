#include "IndexHandler.h"
#include "S3Hashtable.h"
#include <iostream>
#include <string>

struct StringHash {
  size_t operator()(const std::string& s) const
  {
    size_t h = 0;
    for (char c : s) h = h * 31 + static_cast<size_t>(c);
    return h;
  }
};

struct StringEqual {
  bool operator()(const std::string& a, const std::string& b) const
  {
    return a == b;
  }
};

using CommandFunc = void (*)(IndexHandler&, const std::string& args);

void cmd_read(IndexHandler& handler, const std::string& args)
{
  std::string idxName, filename;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: read <index_name> <input_file>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  idxName = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: read <index_name> <input_file>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: read <index_name> <input_file>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  filename = args.substr(p, q - p);
  handler.readIndex(idxName, filename);
}

void cmd_write(IndexHandler& handler, const std::string& args)
{
  std::string filename, idxName;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: write <output_file> <index_name>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  filename = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: write <output_file> <index_name>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: write <output_file> <index_name>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idxName = args.substr(p, q - p);
  handler.writeIndex(filename, idxName);
}

void cmd_search(IndexHandler& handler, const std::string& args)
{
  if (args.empty()) {
    std::cout << "Empty query.\n";
    return;
  }
  std::string w1, w2;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    handler.search(args);
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  w1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    handler.search(args);
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    handler.search(args);
    return;
  }
  q = args.find_first_of(" \t", p);
  w2 = args.substr(p, q - p);
  if (q == std::string::npos) {
    if (handler.indexExists(w2)) {
      handler.searchWord(w1, w2);
    } else {
      handler.search(args);
    }
    return;
  }
  handler.search(args);
}

void cmd_right_merge(IndexHandler& handler, const std::string& args)
{
  std::string newIdx, idx1, idx2;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  newIdx = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: right_merge <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx2 = args.substr(p, q - p);
  handler.rightMerge(newIdx, idx1, idx2);
}

void cmd_down_merge(IndexHandler& handler, const std::string& args)
{
  std::string newIdx, idx1, idx2;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  newIdx = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: down_merge <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx2 = args.substr(p, q - p);
  handler.downMerge(newIdx, idx1, idx2);
}

void cmd_intersect(IndexHandler& handler, const std::string& args)
{
  std::string newIdx, idx1, idx2;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  newIdx = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: intersect <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx2 = args.substr(p, q - p);
  handler.intersect(newIdx, idx1, idx2);
}

void cmd_subtract(IndexHandler& handler, const std::string& args)
{
  std::string newIdx, idx1, idx2;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  newIdx = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: subtract <new_index> <index1> <index2>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx2 = args.substr(p, q - p);
  handler.subtract(newIdx, idx1, idx2);
}

void cmd_add(IndexHandler& handler, const std::string& args)
{
  std::string newIdx, idx1, idx2;
  int addPos = 0;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  newIdx = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx1 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  idx2 = args.substr(p, q - p);
  if (q == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  p = args.find_first_not_of(" \t", q);
  if (p == std::string::npos) {
    std::cerr << "Usage: add <new_index> <index1> <index2> <add_pos>\n";
    return;
  }
  q = args.find_first_of(" \t", p);
  std::string posStr = args.substr(p, q - p);
  addPos = std::stoi(posStr);
  handler.add(newIdx, idx1, idx2, addPos);
}

void cmd_list(IndexHandler& handler, const std::string&)
{
  handler.listIndexes();
}

void cmd_delete(IndexHandler& handler, const std::string& args)
{
  std::string idxName;
  size_t p = args.find_first_not_of(" \t");
  if (p == std::string::npos) {
    std::cerr << "Usage: delete <index_name>\n";
    return;
  }
  size_t q = args.find_first_of(" \t", p);
  idxName = args.substr(p, q - p);
  handler.deleteIndex(idxName);
}

int main() {
  IndexHandler handler;
  ivanov::HashTable<std::string, CommandFunc, StringHash, StringEqual> commands;

  commands.add("read", cmd_read);
  commands.add("write", cmd_write);
  commands.add("search", cmd_search);
  commands.add("right_merge", cmd_right_merge);
  commands.add("down_merge", cmd_down_merge);
  commands.add("intersect", cmd_intersect);
  commands.add("subtract", cmd_subtract);
  commands.add("add", cmd_add);
  commands.add("list", cmd_list);
  commands.add("delete", cmd_delete);

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;

    size_t cmdEnd = line.find_first_of(" \t");
    std::string cmd = (cmdEnd == std::string::npos) ? line : line.substr(0, cmdEnd);
    std::string args;
    if (cmdEnd != std::string::npos) {
      size_t argStart = line.find_first_not_of(" \t", cmdEnd);
      if (argStart != std::string::npos) {
        args = line.substr(argStart);
      }
    }

    if (commands.has(cmd)) {
      commands.at(cmd)(handler, args);
    } else {
      std::cerr << "Unknown command: " << cmd << "\n";
    }
  }
  return 0;
}