#include "decimal.h"

#include <cmath>
#include <cstring>
#include <string>

#include "mantisa_decimal.h"

Decimal_t::Decimal_t(int num) {
  clear_decimal();
  if (num < 0) set_value_sign(-1);
  bit[0] = abs(num);
}

Decimal_t::Decimal_t(std::string src) {
  char c;
  int count = 0, flag_pow = 0, i = 0;
  Long_decimal res;
  Long_decimal ten(10);
  while ((c = src[i++]) != '\0') {
    if (flag_pow == 1) count++;
    switch (c) {
      case '-':
        if (get_value_sign() < 0) {
          std::cerr << "input is uncorrect";
          return;
        }
        set_value_sign(-1);
        break;
      case '.':
        if (flag_pow == 1) {
          std::cerr << "input is uncorrect";
          return;
        }
        flag_pow = 1;
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        Long_decimal tmp(int(c - '0'));
        res = res * ten;
        res = res + tmp;
        break;
    }
  }
  res.set_exp_decimal(count);
  *this = res;
}

void Decimal_t::set_value_pow(int val) {
  if (val > 28) std::cerr << "pow is not correct";
  bit[3] |= (val << 8);
}

void Decimal_t::clear_bcd() {
  for (int i = 0; i < 4; ++i) {
    value.bits[i] = 0;
  }
}

void Decimal_t::offset_bcd_left() {
  for (int i = 3; i > 0; i--) {
    value.bits[i] = value.bits[i] << 1;
    if (value.bits[i - 1] & 0x80000000) value.bits[i] += 1;
  }
  value.bits[0] = value.bits[0] << 1;
}

void Decimal_t::check_and_correct_decimal() {
  for (int i = 31; i >= 0; i--) {
    int index_arr = i / 8;
    int index_bit = i % 8;

    int tmp = (value.bits[index_arr] >> (index_bit * 4)) & 0xf;
    if (tmp > 4) {
      tmp += 3;
      // clear need 1/2 bite
      value.bits[index_arr] =
          (~(0xf << (index_bit * 4))) & value.bits[index_arr];
      // set val tmp
      value.bits[index_arr] = (tmp << (index_bit * 4)) | value.bits[index_arr];
    }
  }
}

void Decimal_t::decimal_to_bcd() {
  int i = 95;
  clear_bcd();
  // skip_empty_zero
  while (!get_value_mantissa(i)) {
    i--;
  }
  for (; i >= 0; i--) {
    int index_arr = i / 32;
    int index_bit = i % 32;
    int val_arr = get_value_mantissa(index_arr);
    check_and_correct_decimal();
    int val_bit = (val_arr >> index_bit) & 0x1;
    offset_bcd_left();
    value.bits[0] += val_bit;
  }
  value.sign = get_value_sign();
  value.exp = get_value_pow();
}

bool Decimal_t::is_null() {
  for (int i = 0; i < 3; i++) {
    if (this->bit[i] != 0) return false;
  }
  return true;
}

std::ostream& operator<<(std::ostream& os, Decimal_t& t) {
  if (t.is_null()) {
    return (os << '0');
  }
  t.decimal_to_bcd();
  int flag_null = 0, exp = t.value.exp;
  std::string tmp;
  for (int i = 31; i >= 0; i--) {
    int arr = i / 8;
    int bit = i % 8;
    int c = (t.value.bits[arr] >> bit * 4) & 0xf;
    if (flag_null == 0 && c != 0) {
      flag_null = 1;
    }
    // пропускаем не значищушие нули
    if (c == 0 && flag_null == 0) continue;
    tmp += ('0' + c);
    exp--;
    if (exp == 0) tmp += '.';
  }
  if (exp > 0) {
    while (exp > 0) {
      tmp += '0';
      exp--;
    }
    tmp += "0.";
  }
  if (t.value.sign == -1) {
    tmp += '-';
  }
  os << tmp << std::endl;
  return os;
}

int Decimal_t::bcd_to_int() {
  unsigned res = 0, ten = 1;
  int exp_tmp = value.exp;

  for (int i = 0; i < 32; ++i) {
    int arr = i / 8;
    int i_bit = i % 8;
    int c = (value.bits[arr] >> i_bit * 4) & 0xf;
    if (exp_tmp > 0) {
      exp_tmp--;
    } else {
      if (c == 0) continue;
      c *= ten;
      ten *= 10;
      res += c;
    }
  }
  return res;
}

void Decimal_t::clear_decimal() {
  for (int i = 0; i < 4; ++i) {
    bit[i] = 0;
  }
}

int exp_from_float(unsigned d) {
  int res = d >> 23;
  res &= 0x000000ff;
  return res;
}

int mantisa_from_float(unsigned bit_float) {
  int res = 1;
  int bit_res = 1;
  bit_float = bit_float | (1 << 23);
  for (int i = 22; i > 0; i--, bit_res++) {
    if (bit_float & (1 << i)) {
      res |= (1 << bit_res);
    }
  }
  return res;
}

void Decimal_t::set_bit_decimal(int number_bit) {
  int index_arr = number_bit / 32;
  int num_bit = number_bit % 32;
  bit[index_arr] = (1 << num_bit) | bit[index_arr];
}

std::istream& operator>>(std::istream& is, Decimal_t& t) { return is; }