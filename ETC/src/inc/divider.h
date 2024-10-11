#ifndef _DIVIDER_H_
#define _DIVIDER_H_

#include <cstdint>
#include <vector>

#define DIVIDER_EXPONENT_SIZE 8
#define DIVIDER_MANTISSA_SIZE 23

#define DIVIDER_APPROX_DOMAIN_MIN   1.0f
#define DIVIDER_APPROX_DOMAIN_MAX   2.0f
#define DIVIDER_TABLE_WIDTH         2


class Divider {

public:
    Divider(std::vector<std::vector<uint64_t>> lookup_table,
            uint8_t constant_term_size,
            uint8_t linear_term_size);

    float reciprocal(float x_dec) const;
    float operator()(float numerator, float denominator) const;

private:
    std::vector<std::vector<uint64_t>> lookup_table;
    uint8_t constant_term_size;
    uint8_t linear_term_size;
    uint8_t table_depth;

    uint8_t get_num_interval_bits() const;
    uint8_t get_x_local_size() const;

    void reduce_range(float x_dec, uint8_t& exponent, uint32_t& mantissa) const;
    float expand_range(uint8_t exponent, uint64_t y_local) const;
    void read_mantissa(uint32_t mantissa, uint8_t& lut_index, bool& sign, uint32_t& x_local) const;
    void read_coefficient(uint64_t coefficient,
                      uint8_t term_width,
                      bool& sign,
                      int8_t& exponent,
                      uint64_t& mantissa) const;
    uint64_t align_mantissa(uint64_t mantissa, int8_t e_common, int8_t adjustment) const;
    uint64_t approximate(uint8_t lut_index, bool s_local, uint32_t x_local) const;

};

#endif  // _DIVIDER_H_
