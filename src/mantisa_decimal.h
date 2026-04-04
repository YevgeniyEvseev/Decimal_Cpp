#ifndef MANTISA_DECIMAL_H
#define MANTISA_DECIMAL_H

#define DIVIDED 1
#define MODULE 2

#include "decimal.h"

class Long_decimal {
 private:
  unsigned bits[6];
  int sign;
  unsigned exp_decimal;
  void clear_long_decimal();
  bool offset_mantissa_left(int count);
  int get_decimal_bit(int index_bit) {
    return (bits[index_bit / 32] >> index_bit % 32) & 0x1;
  }
  int high_order_bit();
  // mod = MODUL or DIVIDED
  Long_decimal div_long_Decimal(const Long_decimal& val2, int mod);

 public:
  Long_decimal(int n);
  Long_decimal();
  Long_decimal(Decimal_t& t);
  Long_decimal operator+(const Long_decimal& op2);
  Long_decimal operator-(const Long_decimal& op2);
  Long_decimal operator*(const Long_decimal& op2);
  Long_decimal operator/(const Long_decimal& op2);
  Long_decimal operator%(const Long_decimal& op2);
  bool operator>=(const Long_decimal& op2);
  friend Long_decimal& operator+(int op1, Long_decimal& op2);
  friend Long_decimal& operator-(int op1, Long_decimal& op2);
  friend Long_decimal& operator*(int op1, Long_decimal& op2);
  friend Long_decimal& operator/(int op1, Long_decimal& op2);
  unsigned get_exp_decimal() { return exp_decimal; }
  void set_exp_decimal(unsigned value) { exp_decimal = value; }
  operator Decimal_t();
};

#endif
