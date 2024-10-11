"""
Generates a lookup table header file to be used by `divider.cpp`.

Author: Jonathan Uhler
"""


import struct
import sys
from typing import Final
import numpy
from numpy.polynomial import chebyshev


APPROX_DOMAIN_MIN: Final = 1.0
APPROX_DOMAIN_MAX: Final = 2.0
TABLE_WIDTH: Final = 2


def reciprocal(x: float, interval_min: float, interval_max: float) -> float:
    """
    Computes the reciprocal of the Chebyshev local argument for a given interval.

    Args:
     x (float):            the non-local argument to the interval
     interval_min (float): the left-hand limit of the interval
     interval_max (float): the right-hand limit of the interval

    Returns:
     float: the Chebyshev reciprocal of the provided argument.
    """

    interval_midpoint: float = (interval_max + interval_min) / 2.0
    interval_width = interval_max - interval_min
    x_local: float = x * (interval_width / 2.0) + interval_midpoint
    return 1.0 / x_local


def build_approximation(degree: int, num_intervals: int) -> list:
    """
    Creates a lookup table of the specified width and depth.

    Values in the returned table are integers which represent the binary values of 32-bit
    floating-point coefficients.

    Args:
     degree (int)         the degree of each polynomial in the lookup table (width - 1).
     num_intervals (int): the number of polynomials in the lookup table (depth).

    Returns:
     list: the lookup table.
    """

    polynomials: list = []

    approx_domain_width: float = APPROX_DOMAIN_MAX - APPROX_DOMAIN_MIN
    interval_width: float = approx_domain_width / num_intervals
    for interval_index in range(num_intervals):
        interval_min: float = APPROX_DOMAIN_MIN + interval_index * interval_width
        interval_max: float = interval_min + interval_width
        limits: list = [interval_min, interval_max]
        interpolation: numpy.ndarray = chebyshev.chebinterpolate(func = reciprocal,
                                                                 deg = degree,
                                                                 args = limits)
        polynomial: numpy.ndarray = chebyshev.cheb2poly(interpolation)
        polynomial = polynomial.astype(numpy.float32)
        polynomial_bits: list = []
        for coefficient in polynomial:
            polynomial_bits.append(int.from_bytes(struct.pack("<f", coefficient), "little"))
        polynomials.append(polynomial_bits)

    return polynomials


def build_lut(polynomials: list) -> str:
    """
    Creates the content of a header file for the provided table values.

    Args:
     polynomials (list): the result of `build_approximation`.

    Returns:
     str: the content of a lookup table header file with the provided values.
    """

    lut: str =                                                       \
        "#ifndef _DIVIDER_LUT_H_\n" +                                \
        "#define _DIVIDER_LUT_H_\n" +                                \
        "#include <cstdint>\n" +                                     \
        "#include <vector>\n" +                                      \
        "const std::vector<std::vector<uint64_t>> DIVIDER_LUT = {\n"

    for i, polynomial in enumerate(polynomials):
        lut_entry: str = ", ".join(map(str, polynomial))
        lut += f"\t{{{lut_entry}}}" + (",\n" if i < len(polynomials) - 1 else "\n")

    lut +=                             \
        "};\n" +                       \
        "#endif  // _DIVIDER_LUT_H_\n"

    return lut


def main() -> None:
    """
    Program entry point
    """

    ##################################
    # Can be changed if desired
    num_intervals: int = 8
    output_file: str = "divider_lut.h"
    ##################################

    polynomials: list = build_approximation(TABLE_WIDTH - 1, num_intervals)
    header_file: str = build_lut(polynomials)
    try:
        with open(output_file, "w+", encoding = "utf-8") as f:
            f.write(header_file)
    except OSError as ose:
        print(f"error: cannot write output: {ose}")
        sys.exit(1)


if (__name__ == "__main__"):
    main()
