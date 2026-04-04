#ifndef DECIMAL_CPP
#define DECIMAL_CPP

#define ERR_EXPONENT_OVERFLOW 1

#include <iostream>

/*     1000 0000
0001 |  000 0000
0010 |   00 0000
0100 |    0 0000
1000 |      0000    >4
+0011
1011 |      0000
0001 0110 |  000
+0011
0001 1001
0011 0010 |   00
0110 0100 |    0    >4
+0011
1001 0100 |    0
0001 0010 1000
*/

class Decimal_t {
 private:
  unsigned bit[4];
  struct BCD_t {
    unsigned bits[4];
    int sign;
    int exp;
  };
  BCD_t value;
  void clear_bcd();
  void decimal_to_bcd();
  void offset_bcd_left();
  void check_and_correct_decimal();
  int bcd_to_int();
  void clear_decimal();
  void set_bit_decimal(int number_bit);

  bool is_null();

 public:
  Decimal_t(std::string num);
  Decimal_t(int num = 0);
  Decimal_t(float num);
  Decimal_t operator+(const Decimal_t&);
  Decimal_t operator-(const Decimal_t&);
  Decimal_t operator*(const Decimal_t&);
  Decimal_t operator/(const Decimal_t&);
  Decimal_t operator%(const Decimal_t&);
  bool operator<(const Decimal_t&);
  bool operator<=(const Decimal_t&);
  bool operator>(const Decimal_t&);
  bool operator>=(const Decimal_t&);
  bool operator==(const Decimal_t&);
  bool operator!=(const Decimal_t&);
  operator int();
  operator float();
  friend Decimal_t& operator+(double num, Decimal_t& dec);
  friend Decimal_t& operator-(double num, Decimal_t& dec);
  friend Decimal_t& operator*(double num, Decimal_t& dec);
  friend Decimal_t& operator/(double num, Decimal_t& dec);
  friend std::istream& operator>>(std::istream& os, Decimal_t& t);
  friend std::ostream& operator<<(std::ostream& os, Decimal_t& t);
  int get_value_pow() { return ((value.bits[3] & 0x0000FF00) >> 8); }
  int get_value_sign() { return ((value.bits[3] & 0xA0000000)) ? -1 : 1; }
  int get_value_mantissa(int i) { return this->bit[i]; }
  void set_value_sign(int value) {
    if (value == -1) this->bit[3] |= (0x1 << 31);
  }  // correct argument (bool value)
  void set_value_pow(int val);
  unsigned& operator[](int i) { return bit[i]; }
};

// Decimal_t::Decimal_t() {}

#endif