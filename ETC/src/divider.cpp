/**
 * @class Divider
 *
 * A hardware implementation of floating-point division and reciprocal functions.
 *
 * This class only supports 32-bit single precision values, as defined in the IEEE 754-2019
 * specification. All IEEE arithmetic and properties are respected.
 *
 * There is not a design document for this program, but the basic implementation is explained
 * below. It is assumed that the reader is familiar with IEEE floating-point arithmetic. In addition
 * to the 754 spec, this (https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html) article
 * provides a good knowledge base.
 *
 * Division can be (mathemtically) implemented as $\frac{a}{b} = a \times \frac{1}{b}$. Thus, the
 * primary goal of this class is to provide an implementation of floating-point reciprocal. The
 * reciprocal function $\text{rcp}(x) = \frac{1}{x}$ is defined under floating-point artihmetic
 * on $[-\infty, +\infty]$ (both infinities and zero are included in the domain). To reasonably
 * implement an approximation, we perform a process of range reduction by making the following
 * observations;
 *
 * Let $x$ be a floating-point number with an unbiased exponent $E$ and an explicit mantiss $M$.
 * Therefore, $x = 2^E \cdot M$.
 *
 * $\frac{1}{x}$
 * $= \frac{1}{2^E \cdot M}$
 * $= \frac{1}{2^E} \cdot \frac{1}{M}$
 * $= 2^{-E} \cdot \frac{1}{M}$
 * $= (1 >> E) \cdot \text{rcp}(M)$
 *
 * Applying rules of fractions and exponents, we have made two major simplifications:
 * 
 * 1) The term including $E$ has been rewritten using a left-shift, which is easy to perform.
 * 2) The term including $M$ is still a reciprocal, but because $M \in [1, 2)$ when normalized,
 *    the domain of the entire reciprocal function has been reduced to $[1, 2)$. This will be
 *    referred to as the "approximation domain" of the function.
 *
 * With a finite domain to approximate input values in, we now find a curve or curves that fit
 * the function in this domain. Because polynomial functions are very easy to compute (only
 * requiring multipliers and adders), they will be used for the approximation. Specifically,
 * Chebyshev polynomials are used, whose coefficients are generated using numpy from an external
 * script. The list of coefficients is our lookup table, which has a depth corresponding to the
 * number of intervals we choose to divide the approximation domain into, and a width respresenting
 * the order of each polynomial.
 *
 * There is a tradeoff (the "tablemaker's dilemma") between the accuracy of our function
 * approximation and the size of the lookup table. Floating-point arithmetic can be annoying
 * to work with at times because it does not obey many mathematical properties, but generally
 * $accuracy \propto tableDepth$.
 */


#include "inc/divider.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>


/**
 * Constructs a new `Divider` with the specified lookup table and parameters.
 *
 * @param lookup_table        the lookup table to use for approximations, must have `order = 2`.
 * @param constant_term_size  the implicit size of the mantissa of the constant coefficients.
 * @param linear_term_size    the implicit size of the mantissa of the linear coefficients.
 *
 * @throws std::illegal_argument  if the lookup table does not have at least two intervals.
 */
Divider::Divider(std::vector<std::vector<uint64_t>> lookup_table,
                 uint8_t constant_term_size,
                 uint8_t linear_term_size)
{
    this->lookup_table = lookup_table;
    this->constant_term_size = constant_term_size;
    this->linear_term_size = linear_term_size;
    this->table_depth = lookup_table.size();
}


/**
 * Calculates the approximate reciprocal of the argument.
 *
 * @param x_dec  the argument to compute the reciprocal of.
 *
 * @return the approximat reciprocal of the argument.
 */
float Divider::reciprocal(float x_dec) const {
    if (std::fpclassify(x_dec) == FP_INFINITE) {
        return std::copysign(0.0f, x_dec);
    }
    else if (std::fpclassify(x_dec) == FP_NAN) {
        return NAN;
    }
    else if (std::fpclassify(x_dec) != FP_NORMAL) {
        return std::copysign(INFINITY, x_dec);
    }

    uint8_t exponent;
    uint32_t mantissa;
    this->reduce_range(x_dec, exponent, mantissa);

    uint8_t lut_index;
    bool s_local;
    uint32_t x_local;
    this->read_mantissa(mantissa, lut_index, s_local, x_local);

    uint64_t y_local = this->approximate(lut_index, s_local, x_local);
    float abs_result = this->expand_range(exponent, y_local);
    float result = std::copysign(abs_result, x_dec);

    return result;
}


/**
 * Calculates the approximate quotient of the two arguments.
 *
 * @param numerator    the numerator of the division.
 * @param denominator  the denominator of the division.
 *
 * @return the approximate quotient of the two arguments.
 */
float Divider::operator()(float numerator, float denominator) const {
    return numerator * this->reciprocal(denominator);
}


/**
 * Returns the number of bits in the reciprocal input mantissa that encode the lookup table index.
 *
 * Excluding the implied bit, the top $\lceil \log_2(numIntervals) \rceil$ bits of the input's
 * mantissa represent the lookup table index.
 *
 * @return the number of bits used to encode the lookup table index.
 */
uint8_t Divider::get_num_interval_bits() const {
    return std::ceil(std::log2(this->table_depth));
}


/**
 * Returns the number of bits used in the Chebyshev argument "`x_local`".
 *
 * The value of `x_local` uses the remaining bits of the input's mantissa after the interval
 * index is determined. Thus, its width is the implicit width of the input mantissa minus the
 * number of interval index bits.
 *
 * @returns the number of bits used for `x_local`.
 */
uint8_t Divider::get_x_local_size() const {
    return DIVIDER_MANTISSA_SIZE - this->get_num_interval_bits();
}


/**
 * Performs the reciprocal range reduction process on the provided argument.
 *
 * @param x_dec     the argument to perform range reduction on.
 * @param exponent  a reference which will be set to the bits of the biased exponent of `x_dec`.
 * @param mantissa  a reference which will be set to the bits of the implicit mantissa of `x_dec`.
 */
void Divider::reduce_range(float x_dec, uint8_t& exponent, uint32_t& mantissa) const {
    uint32_t x;
    std::memcpy(&x, &x_dec, sizeof(float));

    uint32_t exponent_mask = (1U << DIVIDER_EXPONENT_SIZE) - 1U;
    uint32_t mantissa_mask = (1U << DIVIDER_MANTISSA_SIZE) - 1U;

    exponent = (x >> DIVIDER_MANTISSA_SIZE) & exponent_mask;
    mantissa = x & mantissa_mask;
}


/**
 * Performs the reciprocal range expansion process on the provided intermediate data.
 *
 * @param exponent  the biased exponent of the original reciprocal argument `x_dec`.
 * @param y_local   the binary result of function approximation on the approximation domain.
 *
 * @return the final reciprocal of the original argument `x_dec`.
 */
float Divider::expand_range(uint8_t exponent, uint64_t y_local) const {
    int8_t unbiased_exponent = exponent - 127;
    float exponent_contribution = std::exp2(-unbiased_exponent);
    float mantissa_contribution = static_cast<float>(y_local) / std::exp2(24.0f);  // MARK: 24 = c0 mant size + 1, when c0 mant is larger than c1 mant (generalization is explicit width of y_local - 1
    float result = exponent_contribution * mantissa_contribution;
    return result;
}


/**
 * Extracts values from the implicit mantissa of the reciprocal argument.
 *
 * @param mantissa   the implicit mantissa of the reciprocal argument.
 * @param lut_index  a reference which will be set to the lookup table index to use.
 * @param s_local    a reference which will be set to the sign bit of the `x_local` value.
 * @param x_local    a reference which will be set to the magnitude of the `x_local` value.
 */
void Divider::read_mantissa(uint32_t mantissa,
                            uint8_t& lut_index,
                            bool& s_local,
                            uint32_t& x_local) const
{
    uint8_t num_interval_bits = this->get_num_interval_bits();
    uint8_t x_local_size = this->get_x_local_size();
    uint32_t interval_bits_mask = (1U << num_interval_bits) - 1U;
    uint32_t x_local_mask = (1U << x_local_size) - 1U;

    lut_index = (mantissa >> x_local_size) & interval_bits_mask;
    x_local = mantissa & x_local_mask;

    uint32_t x_local_halfway_mask = 1U << (x_local_size - 1);
    bool x_local_is_negative = (x_local & x_local_halfway_mask) == 0;
    if (x_local_is_negative) {
        x_local |= x_local_halfway_mask;
        x_local = ~x_local & x_local_mask;
        x_local += 1U;
    }
    else {
        x_local &= ~x_local_halfway_mask;
    }

    s_local = x_local_is_negative;
}


/**
 * Extracts values from a coefficient in the lookup table.
 *
 * @param coefficient  the binary value of the coefficient.
 * @param term_size    the width of the coefficient's implicit mantissa.
 * @param sign         a reference which will be set to the sign of the coefficient.
 * @param exponent     a reference which will be set to biased exponent of the coefficient.
 * @param mantissa     a reference which will be set to explicit mantissa of the coefficient.
 */
void Divider::read_coefficient(uint64_t coefficient,
                               uint8_t term_size,
                               bool& sign,
                               int8_t& exponent,
                               uint64_t& mantissa) const
{
    uint64_t sign_mask = 1ULL;
    uint64_t exponent_mask = (1ULL << DIVIDER_EXPONENT_SIZE) - 1ULL;
    uint64_t mantissa_mask = (1ULL << term_size) - 1ULL;

    sign = (coefficient >> (DIVIDER_EXPONENT_SIZE + term_size)) & sign_mask;

    uint8_t biased_exponent = (coefficient >> term_size) & exponent_mask;
    exponent = biased_exponent - 127;

    uint64_t implied_bit = biased_exponent > 0;
    mantissa = coefficient & mantissa_mask;
    mantissa |= implied_bit << term_size;
}


/**
 * Aligns the provided mantissa based on a target common exponent and adjustment factor.
 *
 * @param mantissa    the mantissa to align the binary point of.
 * @param e_common    the common unbiased exponent of this mantissa and another mantissa it will
 *                    be added to.
 * @param adjustment  the adjustment required to align the mantissa assuming `e_common == 0`.
 *
 * @return the aligned mantissa.
 */
uint64_t Divider::align_mantissa(uint64_t mantissa, int8_t e_common, int8_t adjustment) const {
    int shift_amount = e_common - adjustment;
    return shift_amount > 0 ? mantissa >> shift_amount : mantissa << shift_amount;
}


/**
 * Performs the Chebyshev function approximation.
 *
 * @param lut_index  the 0-aligned index of the lookup table to use.
 * @param s_local    the sign of the `x_local` value.
 * @param x_local    the magnitude of the `x_local` value.
 *
 * @return the binary result of function approximation, `y_local`.
 */
uint64_t Divider::approximate(uint8_t lut_index, bool s_local, uint32_t x_local) const {
    uint8_t x_local_size = this->get_x_local_size();
    std::vector<uint64_t> coeffs = this->lookup_table[lut_index];
    assert(coeffs.size() == 2 && "lookup table has invalid order");

    bool s_constant, s_linear;
    int8_t e_constant, e_linear;
    uint64_t m_constant, m_linear;
    this->read_coefficient(coeffs[0], this->constant_term_size, s_constant, e_constant, m_constant);
    this->read_coefficient(coeffs[1], this->linear_term_size, s_linear, e_linear, m_linear);

    int s_m0 = s_constant ? -1 : 1;
    uint64_t m0 = m_constant;
    int s_m1 = s_linear != s_local ? -1 : 1;
    uint64_t m1 = m_linear * x_local;

    int8_t adjustment0 = e_constant;
    int8_t adjustment1 = e_linear - (x_local_size - 1);

    int8_t e_common = std::max(adjustment0, adjustment1);
    uint64_t m0_aligned = this->align_mantissa(m0, e_common, adjustment0);
    uint64_t m1_aligned = this->align_mantissa(m1, e_common, adjustment1);
    uint64_t y_local = (s_m0 * m0_aligned) + (s_m1 * m1_aligned);

    return y_local;
}
