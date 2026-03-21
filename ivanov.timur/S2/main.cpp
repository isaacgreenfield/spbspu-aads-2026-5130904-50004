#include <stdexcept>
#include <string>
#include <fstream>
#include <cctype>
#include <iostream>
#include "fun.h"
#include "Queue.h"
#include "Stack.h"
#include "List.h"

int getP(char op) {
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
  ivanov::Stack<Object*> stack(&stackList);

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
      int prec = getP(obj->symbol);
      bool rightAssoc = ira(obj->symbol);
      while (!stack.isEmpty() && stack.top()->symbol != '(') {
        char topSym = stack.top()->symbol;
        int topPrec = getP(topSym);
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

Integer* eval(const ivanov::List<Object*>& line) {
  ivanov::List<Object*> stackList;
  ivanov::Stack<Object*> stack(&stackList);

  for (Object* obj : line) {
    if (obj->isNumber()) {
      stack.push(obj);
    } else {
      Integer* right = static_cast<Integer*>(stack.drop());
      Integer* left  = static_cast<Integer*>(stack.drop());
      int resVal = 0;
      switch (obj->symbol) {
        case '+':
          resVal = left->getValue() + right->getValue();
          break;
        case '-':
          resVal = left->getValue() - right->getValue();
          break;
        case '*':
          resVal = left->getValue() * right->getValue();
          break;
        case '/':
          if (right->getValue() == 0) throw std::runtime_error("Division by zero");
          resVal = left->getValue() / right->getValue();
          break;
        case '%':
          if (right->getValue() == 0) throw std::runtime_error("Modulo by zero");
          resVal = left->getValue() % right->getValue();
          break;
        case '^':
        {
          int base = left->getValue();
          int exp = right->getValue();
          if (exp < 0) throw std::runtime_error("Negative exponent not supported");
          int powVal = 1;
          for (int i = 0; i < exp; ++i) {
            powVal *= base;
          }
          resVal = powVal;
        }
          break;
        default:
          throw std::invalid_argument("Unknown operator");
      }
      Integer* result = new Integer(resVal);
      stack.push(result);
    }
  }

  if (stack.isEmpty()) throw std::runtime_error("No result");
  Integer* finalResult = static_cast<Integer*>(stack.drop());
  return finalResult;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;

  std::ifstream(argv[1]);

  return 0;
}