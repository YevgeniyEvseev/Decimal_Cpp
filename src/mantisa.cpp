#include "mantisa_decimal.h"

Long_decimal::Long_decimal(Decimal_t& t) {
  exp_decimal = t.get_value_pow();
  sign = t.get_value_sign();
  for (int i = 0; i < 3; ++i) {
    bits[i] = t.get_value_mantissa(i);
  }
}

Long_decimal::Long_decimal(int m) {
  exp_decimal = 0;
  sign = 1;
  clear_long_decimal();
  if (m < 0) sign = -1;
  bits[0] = m;
}

void Long_decimal::clear_long_decimal() {
  for (int i = 0; i < 6; ++i) {
    bits[i] = 0;
  }
  sign = 1;
  exp_decimal = 0;
}

Long_decimal::Long_decimal() { clear_long_decimal(); }

Long_decimal Long_decimal::operator+(const Long_decimal& op2) {
  int tmp_bit = 0;
  unsigned val_1_tmp = 0, val_2_tmp = 0;
  Long_decimal res;
  for (int i = 0; i < 6; ++i) {
    unsigned val_1_bit = bits[i] >> 31;
    unsigned val_2_bit = op2.bits[i] >> 31;
    val_1_tmp = bits[i] & 0x7FFFFFFF;
    val_2_tmp = op2.bits[i] & 0x7FFFFFFF;
    res.bits[i] = val_1_tmp + val_2_tmp + tmp_bit;
    if (val_1_bit == 1 && val_2_bit == 1)
      tmp_bit = 1;
    else if (val_1_bit + val_2_bit == 1 && (res.bits[i] >> 31) == 1) {
      tmp_bit = 1;
      res.bits[i] = res.bits[i] & 0x7FFFFFFF;
    } else if (val_1_bit + val_2_bit == 1 && (res.bits[i] >> 31) == 0) {
      res.bits[i] = res.bits[i] | 0x80000000;
      tmp_bit = 0;
    } else if ((val_1_bit + val_2_bit) == 0) {
      tmp_bit = 0;
    }
  }
  res.sign = sign;
  res.exp_decimal = exp_decimal;
  return res;
}

Long_decimal Long_decimal::operator-(const Long_decimal& op2) {
  Long_decimal invert_val_2, res;
  for (int i = 0; i < 6; ++i) {
    invert_val_2.bits[i] = ~(op2.bits[i]);
  }
  invert_val_2.bits[0]++;
  res = *this + op2;
  return res;
}

bool Long_decimal::offset_mantissa_left(int count) {
  for (int j = 0; j < count; ++j) {
    if ((bits[5] & 0x80000000) != 0) return false;
    for (int i = 5; i > 0; i--) {
      bits[i] = (bits[i] << 1);
      if ((bits[i - 1] & 0x80000000) != 0) {
        bits[i] = (bits[i] | 1);
      }
    }
    bits[0] = (bits[0] << 1);
  }
  return true;
}

Long_decimal Long_decimal::operator*(const Long_decimal& op2) {
  int ofset_count = 0, i_max = 96;
  Long_decimal res, val_1_copy, val_2;
  val_1_copy = *this;
  val_2 = op2;
  for (int i = 2; i > 0; i--) {
    if (op2.bits[i] != 0) break;
    i_max -= 32;
  }
  for (int i = 0; i < i_max; ++i) {
    if (val_2.get_decimal_bit(i)) {
      if (val_1_copy.offset_mantissa_left(ofset_count) == false)
        std::cout << "mul_long is fail offset";
      res = res + val_1_copy;
      ofset_count = 1;
    } else {
      ofset_count++;
    }
  }
  res.sign = val_1_copy.sign * val_2.sign;
  res.exp_decimal = val_1_copy.exp_decimal + val_2.exp_decimal;
  return res;
}

bool Long_decimal::operator>=(const Long_decimal& op2) {
  for (int i = 5; i >= 0; i--) {
    if (bits[i] > op2.bits[i]) return true;
    if (bits[i] < op2.bits[i]) return false;
  }
  return true;
}

Long_decimal Long_decimal::div_long_Decimal(const Long_decimal& val2,
                                            int modif) {
  Long_decimal res;
  Long_decimal mod;
  int i_max = 96;
  for (int i = 2; i > 0; i--) {
    if (val2.bits[i] != 0) break;
    i_max -= 32;
  }
  for (int i = 0; i < i_max; ++i) {
    res.offset_mantissa_left(1);
    mod.offset_mantissa_left(1);
    int bit_delit = get_decimal_bit(i);
    if (bit_delit != 0) {
      mod.bits[0] = mod.bits[0] | 0x1;
    }
    if (mod >= val2) {
      mod = mod - val2;
      res.bits[0] = res.bits[0] | 0x1;
    }
  }
  res.sign = sign * val2.sign;
  res.exp_decimal = exp_decimal - val2.exp_decimal;
  mod.sign = res.sign;
  mod.exp_decimal = exp_decimal;
  return (modif == DIVIDED) ? res : mod;
}

Long_decimal Long_decimal::operator/(const Long_decimal& op2) {
  return div_long_Decimal(op2, DIVIDED);
}

Long_decimal Long_decimal::operator%(const Long_decimal& op2) {
  return div_long_Decimal(op2, MODULE);
}

int Long_decimal::high_order_bit() {
  for (int i = 191; i >= 0; i--) {
    if (get_decimal_bit(i)) return i;
  }
  return -1;
}

Long_decimal::operator Decimal_t() {
  int count_bit = high_order_bit();
  Decimal_t res;
  if (count_bit == -1) return res;  // res=0
  int tmp_pow = exp_decimal;
  if (exp_decimal > 28) {
    // error_decimal(ERR_EXPONENT_OVERFLOW);
    return res;
  }
  Long_decimal copy_src = *this;
  if (count_bit > 95) {
    Long_decimal ten(10);
    Long_decimal mod;
    if (count_bit - 95 >
        exp_decimal - 1) {  // error_decimal(ERR_MINIMUM_OVERFLOW);
      return res;
    }
    Long_decimal tmp;
    while (copy_src.bits[3] != 0) {
      tmp = copy_src / ten;
      mod = copy_src % ten;
      copy_src = tmp;
      tmp_pow--;
    }
    if (mod.bits[0] > 0) {
      if (copy_src.bits[0] % 2) copy_src.bits[0] += 1;
    }
  }
  res.set_value_sign(sign);
  res.set_value_pow(tmp_pow);
  for (int i = 0; i < 3; i++) {
    res[i] = copy_src.bits[i];
  }
  return res;
}