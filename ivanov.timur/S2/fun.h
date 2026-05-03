#ifndef FUN_H
#define FUN_H
#include <string>
#include <stdexcept>
#include <cctype>
#include <climits>
#include <cstdlib>
#include "Queue.h"
#include "Stack.h"
#include "List.h"

class Object {
public:
    std::string symbol;
    Object(std::string c) : symbol(c) {}
    virtual ~Object() = default;
    virtual bool isNumber() const { return false; }
    virtual long long getValue() const { return 0; }
};

class Integer : public Object {
    long long data;
public:
    Integer(long long d) : Object(""), data(d) {}
    bool isNumber() const override { return true; }
    long long getValue() const override { return data; }

    Integer& operator+(const Integer& other);
    Integer& operator-(const Integer& other);
    Integer& operator*(const Integer& other);
    Integer& operator/(const Integer& other);
    Integer& operator%(const Integer& other);
    Integer& pow(const Integer& other);
    Integer& also(const Integer& other);
    Integer& concatation(const Integer& other);
    Integer& gcd(const Integer& other);
    Integer& lcm(const Integer& other);
};

inline bool addOverflow(long long a, long long b, long long& res) {
    if ((b > 0 && a > LLONG_MAX - b) ||
        (b < 0 && a < LLONG_MIN - b))
        return true;
    res = a + b;
    return false;
}

inline bool subOverflow(long long a, long long b, long long& res) {
    if ((b < 0 && a > LLONG_MAX + b) ||
        (b > 0 && a < LLONG_MIN + b))
        return true;
    res = a - b;
    return false;
}

inline bool mulOverflow(long long a, long long b, long long& res) {
    if (a == 0 || b == 0) {
        res = 0;
        return false;
    }
    if (a > 0) {
        if (b > 0) {
            if (a > LLONG_MAX / b) return true;
        } else {
            if (b < LLONG_MIN / a) return true;
        }
    } else {
        if (b > 0) {
            if (a < LLONG_MIN / b) return true;
        } else {
            if (a < LLONG_MAX / b) return true;
        }
    }
    res = a * b;
    return false;
}

inline Integer& Integer::operator+(const Integer& other) {
    long long res;
    if (addOverflow(data, other.data, res))
        throw std::overflow_error("Addition overflow");
    data = res;
    return *this;
}

inline Integer& Integer::operator-(const Integer& other) {
    long long res;
    if (subOverflow(data, other.data, res))
        throw std::overflow_error("Subtraction overflow");
    data = res;
    return *this;
}

inline Integer& Integer::operator*(const Integer& other) {
    long long res;
    if (mulOverflow(data, other.data, res))
        throw std::overflow_error("Multiplication overflow");
    data = res;
    return *this;
}

inline Integer& Integer::operator/(const Integer& other) {
    if (other.data == 0)
        throw std::runtime_error("Division by zero");
    data /= other.data;
    return *this;
}

inline Integer& Integer::operator%(const Integer& other) {
    if (other.data == 0)
        throw std::runtime_error("Modulo by zero");
    data %= other.data;
    if (data < 0) data += std::abs(other.data);
    return *this;
}

inline Integer& Integer::pow(const Integer& other) {
    if (other.data < 0)
        throw std::runtime_error("Negative exponent not supported");
    long long base = data;
    long long exp = other.data;
    long long res = 1;
    while (exp > 0) {
        if (exp & 1) {
            if (mulOverflow(res, base, res))
                throw std::overflow_error("Power overflow");
        }
        exp >>= 1;
        if (exp > 0) {
            if (mulOverflow(base, base, base))
                throw std::overflow_error("Power overflow");
        }
    }
    data = res;
    return *this;
}

inline Integer& Integer::also(const Integer& other) {
    data &= other.data;
    return *this;
}

inline Integer& Integer::concatation(const Integer& other) {
    std::string tmp = std::to_string(data) + std::to_string(other.data);
    long long val = std::stoll(tmp);
    data = val;
    return *this;
}

inline Integer& Integer::gcd(const Integer& other) {
    long long a = std::abs(data);
    long long b = std::abs(other.data);
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    data = a;
    return *this;
}

inline Integer& Integer::lcm(const Integer& other) {
    long long g = std::abs(data);
    long long o = std::abs(other.data);
    data = (data / g) * o;
    return *this;
}

inline int getP(std::string op) {
    switch (op[0]) {
        case '+': return 1;
        case '*': return (op == "**") ? 3 : 1;
        case '-': return 1;
        case '/': return 1;
        case '%': return 2;
        default: return 0;
    }
}

inline bool ira(std::string op) {
    return op == "**";
}

inline ivanov::List<Object*> infixToPostfix(const ivanov::List<Object*>& infix) {
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
            delete obj;   // удаляем саму ')'
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

inline ivanov::List<Object*> stringToInfixList(const std::string& expr) {
    ivanov::List<Object*> result;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (std::isdigit(c)) {
            long long num = 0;
            while (i < expr.size() && std::isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                ++i;
            }
            --i;
            result.push_back(new Integer(num));
        } else if (!std::isspace(c)) {
            result.push_back(new Object(std::string(1, c)));
        }
    }
    return result;
}

inline Integer* eval(const ivanov::List<Object*>& line) {
    ivanov::List<Object*> stackList;
    ivanov::Stack<Object*> stack(&stackList);

    for (Object* obj : line) {
        if (obj->isNumber()) {
            stack.push(new Integer(obj->getValue()));
        } else {
            Integer* right = static_cast<Integer*>(stack.drop());
            Integer* left  = static_cast<Integer*>(stack.drop());

            try {
                switch (obj->symbol[0]) {
                    case '+': left->operator+(*right); break;
                    case '-': left->operator-(*right); break;
                    case '*':
                        if (obj->symbol == "**") {
                            left->pow(*right);
                        } else {
                            left->operator*(*right);
                        }
                        break;
                    case '/': left->operator/(*right); break;
                    case '%': left->operator%(*right); break;
                    case '&':
                        if (obj->symbol == "&&") {
                            left->concatation(*right);
                        } else {
                            left->also(*right);
                        }
                        break;
                    default:
                        throw std::invalid_argument("Unknown operator");
                }
            } catch (...) {
                delete left;
                delete right;
                throw;
            }

            delete right;
            stack.push(left);
        }
    }

    if (stack.isEmpty())
        throw std::runtime_error("No result");
    Integer* finalResult = static_cast<Integer*>(stack.drop());
    while (!stack.isEmpty()) {
        delete stack.drop();
    }
    return finalResult;
}

#endif
