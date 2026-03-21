#include <stdexcept>
#include "fun.h"
#include "Queue.h"
#include "Stack.h"
#include "List.h"

int getPrecedence(char op) {
  switch (op) {
    case '+': case '-': return 1;
    case '*': case '/': case '%': return 2;
    case '^': return 3;
    default: return 0;
  }
}

bool ira(char op) {
    return op == '^';
}

ivanov::List<char> infixToPostfix(const ivanov::List<char>& infix) {
    ivanov::List<char> output;
    ivanov::List<char> stackList;
    Stack<char> stack(&stackList);

    auto it = infix.begin();
    while (it != infix.end()) {
        char c = *it;
        if (std::isdigit(c)) {
            if (!output.empty()) {
                output.push_back(' ');
            }
            while (it != infix.end() && std::isdigit(*it)) {
                output.push_back(*it);
                ++it;
            }
        }
        else if (c == '(') {
            stack.push(c);
            ++it;
        }
        else if (c == ')') {
            while (!stack.isEmpty() && stack.top() != '(') {
                if (!output.empty()) output.push_back(' ');
                output.push_back(stack.drop());
            }
            if (!stack.isEmpty() && stack.top() == '(') {
                stack.drop();
            } else {
                throw std::invalid_argument("Mismatched parentheses");
            }
            ++it;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^') {
            int prec = getPrecedence(c);
            bool rightAssoc = ira(c);

            while (!stack.isEmpty() && stack.top() != '(') {
                char topOp = stack.top();
                int topPrec = getPrecedence(topOp);
                if ((rightAssoc && prec < topPrec) || (!rightAssoc && prec <= topPrec)) {
                    if (!output.empty()) output.push_back(' ');
                    output.push_back(stack.drop());
                } else {
                    break;
                }
            }
            stack.push(c);
            ++it;
        }
        else if (std::isspace(c)) {
            ++it;
        }
        else {
            ++it;
        }
    }
    while (!stack.isEmpty()) {
        if (!output.empty()) output.push_back(' ');
        output.push_back(stack.drop());
    }
    return output;
}

int main() {
  return 0;
}