#ifndef FUN_H
#define FUN_H
#include<string>
#include "Queue.h"
#include "Stack.h"
#include "List.h"
#include <stdexcept>
#include <cctype>

class Object {
public:
  std::string symbol;
  Object(std::string c) : symbol(c) {}
  virtual ~Object() = default;
  virtual bool isNumber() const { return false; }
};

class Integer : public Object {
  int data;

public:
  Integer(int d) : Object("\0"), data(d) {}
  virtual bool isNumber() const override { return true; }
  int getValue() const { return data; }
  ~Integer() = default;

  Integer& operator+(const Integer& other);
  Integer& operator-(const Integer& other) ;
  Integer& operator*(const Integer& other) ;
  Integer& operator/(const Integer& other) ;
  Integer& operator%(const Integer& other) ;

  Integer& pow(const Integer& other) ;
  Integer& also(const Integer& other);
  Integer& concatation(const Integer& other);

  Integer& gcd(const Integer& other);
  Integer& lcm(const Integer& other);
};

inline Integer& Integer::operator+(const Integer &other) {
  data += other.data;
  return *this;
}

inline Integer & Integer::operator-(const Integer &other) {
  data -= other.data;
  return *this;
}

inline Integer & Integer::operator*(const Integer &other) {
  data *= other.data;
  return *this;
}

inline Integer & Integer::operator/(const Integer &other) {
  if (other.data != 0) data /= other.data;
  return *this;
}

inline Integer & Integer::operator%(const Integer &other)  {
  if (other.data != 0) data = data%other.data;
  return *this;
}

inline Integer & Integer::pow(const Integer &other)  {
  for (int i = 0; i < other.data; ++i) data *= data;
  return *this;
}

inline Integer & Integer::also(const Integer &other) {
  data &= other.data;
  return *this;
}

inline Integer & Integer::concatation(const Integer &other) {
  std::string tmp = std::to_string(data) + std::to_string(other.data);
  data = stoi(tmp);
  return *this;
}

inline Integer & Integer::gcd(const Integer &other) {
  int a = data;
  int b = other.data;
  while (a%b != 0) {
    int tmp = a%b;
    a = b;
    b = tmp;
  }
  data = a;
  return *this;
}

inline Integer & Integer::lcm(const Integer &other) {
  data = (data*other.data)/gcd(other).data;
  return *this;
}

int getP(std::string op) {
  switch (op) {
    case "+":
    case "-": return 1;
    case "*":
    case "/":
    case "%": return 2;
    case "**": return 3;
    default: return 0;
  }
}

bool ira(std::string op) {
  return op == "**";
}

ivanov::List<Object*> infixToPostfix(const ivanov::List<Object*>& infix) {
  ivanov::List<Object*> stackList;
  ivanov::Stack<Object*> stack(&stackList);

  ivanov::List<Object*> queueList;
  ivanov::Queue<Object*> output(&queueList);

  for (Object* obj : infix) {
    if (obj->isNumber()) {
      output.push(obj);
    } else if (obj->symbol == "(") {
      stack.push(obj);
    } else if (obj->symbol == ")") {
      while (!stack.isEmpty() && stack.top()->symbol != "(") {
        output.push(stack.drop());
      }
      if (!stack.isEmpty() && stack.top()->symbol == "(") {
        delete stack.drop();
      } else {
        throw std::invalid_argument("Mismatched parentheses");
      }
    } else if (obj->symbol == "+" || obj->symbol == "-" || obj->symbol == "*" ||
               obj->symbol == "/" || obj->symbol == "%" || obj->symbol == "**" ||
               obj->symbol == "&" || obj->symbol == "&&") {
      int prec = getP(obj->symbol);
      bool rightAssoc = ira(obj->symbol);
      while (!stack.isEmpty() && stack.top()->symbol != "(") {
        std::string topSym = stack.top()->symbol;
        int topPrec = getP(topSym);
        if ((rightAssoc && prec < topPrec) || (!rightAssoc && prec <= topPrec)) {
          output.push(stack.drop());
        } else {
          break;
        }
      }
      stack.push(obj);
               } else {
                 throw std::invalid_argument("Unknown token: " + obj->symbol);
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
        case "+":
          resVal = (*left + *right).getValue();
          break;
        case "-":
          resVal = (*left - *right).getValue();
          break;
        case "*":
          resVal = (*left * *right).getValue();
          break;
        case "/":
          if (right->getValue() == 0) throw std::runtime_error("Division by zero");
          resVal = (*left / *right).getValue();
          break;
        case "%":
          if (right->getValue() == 0) throw std::runtime_error("Modulo by zero");
          resVal = (*left % *right).getValue();
          break;
        case "**":
          if (right->getValue() < 0) throw std::runtime_error("Negative exponent not supported");
          resVal = left->pow(*right).getValue();
          break;
        case "&":
          resVal = left->also(*right).getValue();
          break;
        case "&&":
          resVal = left->concatation(*right).getValue();
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

#endif
