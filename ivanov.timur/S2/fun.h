#ifndef FUN_H
#define FUN_H
#include<string>

class Object {
public:
  char symbol;
  Object(char c) : symbol(c) {}
  virtual ~Object() = default;
  virtual bool isNumber() const { return false; }
};

class Integer : public Object {
  int data;

public:
  Integer(int d) : Object('\0'), data(d) {}
  virtual bool isNumber() const override { return true; }
  int getValue() const { return data; }
  ~Integer() = default;

  Integer& operator+(const Integer& other) noexcept;
  Integer& operator-(const Integer& other) noexcept;
  Integer& operator*(const Integer& other) noexcept;
  Integer& operator/(const Integer& other) noexcept;
  Integer& operator%(const Integer& other) noexcept;

  Integer& pow(const Integer& other) noexcept;
  Integer& also(const Integer& other);
  Integer& reverse();
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

inline Integer & Integer::operator%(const Integer &other) noexcept {
  if (other.data != 0) data = data%other.data;
  return *this;
}

inline Integer & Integer::pow(const Integer &other) noexcept {
  for (int i = 0; i < other.data; ++i) data *= data;
  return *this;
}

inline Integer & Integer::also(const Integer &other) {
  data &= other.data;
  return *this;
}

inline Integer & Integer::reverse() {
  std::string rs = std::to_string(data);
  std::string ns = "";
  for (int i = rs.size() - 1; i >= 0; ++i) ns+=rs[i];
  data = stoi(ns);
  return *this;
}

inline Integer & Integer::concatation(const Integer &other) {
  data = data*10 + other.data;
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

#endif
