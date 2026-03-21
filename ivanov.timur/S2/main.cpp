#include <stdexcept>
#include <string>
#include <fstream>
#include <cctype>
#include <iostream>
#include "fun.h"
#include "Queue.h"
#include "Stack.h"
#include "List.h"

int getPrecedence(char op) {
  switch (op) {
    case '+':
    case '-': return 1;
    case '*':
    case '/':
    case '%': return 2;
    case '^': return 3;
    default: return 0;
  }
}

bool ira(char op) {
  return op == '^';
}

ivanov::List<Object*> infixToPostfix(const ivanov::List<Object*>& infix) {
  ivanov::List<Object*> stackList;
  Stack<Object*> stack(&stackList);

  ivanov::List<Object*> queueList;
  ivanov::Queue<Object*> output(&queueList);

  for (Object* obj : infix) {
    if (obj->isNumber()) {
      output.push(obj);
    } else if (obj->symbol == '(') {
      stack.push(obj);
    } else if (obj->symbol == ')') {
      while (!stack.isEmpty() && stack.top()->symbol != '(') {
        output.push(stack.drop());
      }
      if (!stack.isEmpty() && stack.top()->symbol == '(') {
        delete stack.drop();
      } else {
        throw std::invalid_argument("Mismatched parentheses");
      }
    } else if (obj->symbol == '+' || obj->symbol == '-' || obj->symbol == '*' ||
               obj->symbol == '/' || obj->symbol == '%' || obj->symbol == '^') {
      int prec = getPrecedence(obj->symbol);
      bool rightAssoc = ira(obj->symbol);
      while (!stack.isEmpty() && stack.top()->symbol != '(') {
        char topSym = stack.top()->symbol;
        int topPrec = getPrecedence(topSym);
        if ((rightAssoc && prec < topPrec) || (!rightAssoc && prec <= topPrec)) {
          output.push(stack.drop());
        } else {
          break;
        }
      }
      stack.push(obj);
    }
  }

  while (!stack.isEmpty()) {
    output.push(stack.drop());
  }

  ivanov::List<Object*> outputList;
  while (!output.isEmpty()) {
    outputList.push_back(output.drop());
  }
  return outputList;
}

ivanov::List<Object*> stringToInfixList(const std::string& expr) {
  ivanov::List<Object*> result;
  for (size_t i = 0; i < expr.size(); ++i) {
    char c = expr[i];
    if (std::isdigit(c)) {
      int num = 0;
      while (i < expr.size() && std::isdigit(expr[i])) {
        num = num * 10 + (expr[i] - '0');
        ++i;
      }
      --i;
      result.push_back(new Integer(num));
    } else if (!std::isspace(c)) {
      result.push_back(new Object(c));
    }
  }
  return result;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;

  std::ifstream(argv[1]);

  return 0;
}