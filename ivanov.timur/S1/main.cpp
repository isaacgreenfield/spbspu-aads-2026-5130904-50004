#include <iostream>
#include <string>
#include <cctype>
#include "List.h"

using namespace ivanov;

int main() {
  List<std::string> names;
  List<List<unsigned long long>> nums;

  std::string token;
  char c;
  bool is_new_line = true;

  while (std::cin.get(c)) {
    if (std::isspace(static_cast<unsigned char>(c))) {
      if (!token.empty()) {
        if (is_new_line) {
          names.push_back(token);
          nums.push_back(List<unsigned long long>());
          is_new_line = false;
        } else {
          try {
            size_t pos;
            unsigned long long val = std::stoull(token, &pos);
            if (pos == token.length()) {
              nums.back().push_back(val);
            }
          } catch (const std::out_of_range&) {
            std::cout << "\n";
            std::cerr << "Overflow\n";
            return 1;
          } catch (const std::exception&) {
          }
        }
        token.clear();
      }
      if (c == '\n') {
        is_new_line = true;
      }
    } else {
      token += c;
    }
  }

  if (!token.empty()) {
    if (is_new_line) {
      names.push_back(token);
      nums.push_back(List<unsigned long long>());
    } else {
      try {
        size_t pos;
        unsigned long long val = std::stoull(token, &pos);
        if (pos == token.length()) {
          nums.back().push_back(val);
        }
      } catch (const std::out_of_range&) {
        std::cout << "\n";
        std::cerr << "Overflow\n";
        return 1;
      } catch (const std::exception&) {
      }
    }
  }

  if (names.empty()) {
    std::cout << "0\n";
    return 0;
  }

  bool first_name = true;
  for (auto it = names.cbegin(); it != names.cend(); ++it) {
    if (!first_name) std::cout << " ";
    std::cout << *it;
    first_name = false;
  }
  std::cout << "\n";

  size_t max_size = 0;
  for (auto it = nums.cbegin(); it != nums.cend(); ++it) {
    if (it->size() > max_size) {
      max_size = it->size();
    }
  }

  if (max_size == 0) {
    std::cout << "0\n";
    return 0;
  }

  List<unsigned long long> sums;
  List<CIter<unsigned long long>> iters;
  for (auto it = nums.cbegin(); it != nums.cend(); ++it) {
    iters.push_back(it->cbegin());
  }

  try {
    for (size_t col = 0; col < max_size; ++col) {
      unsigned long long current_sum = 0;
      bool first_in_row = true;

      if (!iters.empty()) {
        auto iters_it = iters.begin();
        auto nums_it = nums.cbegin();

        for (size_t i = 0; i < iters.size(); ++i) {
          if (*iters_it != nums_it->cend()) {
            if (!first_in_row) std::cout << " ";
            std::cout << **iters_it;
            ivanov::sum(current_sum, **iters_it);

            first_in_row = false;
            ++(*iters_it);
          }

          if (i < iters.size() - 1) {
            ++iters_it;
            ++nums_it;
          }
        }
      }

      if (!first_in_row) {
        std::cout << "\n";
        sums.push_back(current_sum);
      } else {
        break;
      }
    }
  } catch (const std::overflow_error&) {
    std::cout << "\n";
    std::cerr << "Overflow\n";
    return 1;
  } catch (const std::bad_alloc&) {
    std::cout << "\n";
    std::cerr << "Bad allocation failure\n";
    return 1;
  } catch (const std::exception&) {
    std::cout << "\n";
    std::cerr << "Error during sum processing\n";
    return 1;
  } catch (...) {
    std::cout << "\n";
    std::cerr << "Something went wrong\n";
    return 1;
  }

  bool first_sum = true;
  for (auto it = sums.cbegin(); it != sums.cend(); ++it) {
    if (!first_sum) std::cout << " ";
    std::cout << *it;
    first_sum = false;
  }
  std::cout << "\n";

  return 0;
}
