//# Constants.h: Mathematical and numerical constants
//# Copyright (C) 1993,1994,1995,1997,1998,1999,2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: casa-feedback@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#ifndef CASA_CONSTANTS_H
#define CASA_CONSTANTS_H

#include <casacore/casa/aips.h>

#if defined (sun) && ! defined (AIPS_SOLARIS) 
#   include <sys/limits.h>
#else
#   include <limits.h>
#endif

#include <float.h>
#if !defined(AIPS_DARWIN) && !defined(AIPS_BSD)
#include <values.h>
#endif

#if defined (AIPS_OSF)
#   define LN_MAXFLOAT (M_LN2 * FMAXEXP)
#   define LN_MINFLOAT (M_LN2 * (FMINEXP -1))
#endif

namespace casacore { //# NAMESPACE CASACORE - BEGIN

// <summary>Mathematical and numerical constants.</summary>

// <use visibility=export>

// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="tConstants" demos="">

// <synopsis>
// The constants and conversion factors are defined here as double precision
// values.  Where single precision calculations are done in a situation where
// processing speed is of concern, for example within the inner loop of an
// expensive algorithm, a separate single precision variable should be defined
// for use within the loop.
//
// </synopsis>
//
// <note role=warning> The following list is generated by hand, and may be incomplete.
// After future revision of ccx2html the real data will be displayed </note>

// <h3>Floating point limits</h3>
// <srcblock>
// flt_min		the minimum single precision floating point number, 
// 			excluding denormalised numbers
// minfloat		the minimum single precision floating point number, 
// 			including denormalised numbers
// dbl_min		the minimum double precision floating point number,
// 			excluding denormalised numbers
// mindouble		the minimum double precision floating point number,
// 			including denormalised numbers
// flt_max		the maximum single precision floating point number 
// dbl_max		the maximum double precision floating point number 
// flt_epsilon		Minimum single precision floating point number X
// 			such that 1+X does not equal X
// dbl_epsilon		Minimum double precision floating point number X
//			such that 1+X does not equal X
// </srcblock>
// <h3> Irrationals </h3>
// <srcblock>
// sqrt2		sqrt(2)
// sqrt3		sqrt(3)
// _1_sqrt2		1/sqrt(2)
// _1_sqrt3		1/sqrt(3)
// </srcblock>

// <h3>Pi and functions thereof</h3>
// <srcblock>
// pi                   pi
// _2pi                 2*pi
// pi_2                 pi/2
// pi_4                 pi/4
// _1_pi                1/pi
// _2_pi                2/pi
// _1_sqrtpi            1/sqrt(pi)
// _2_sqrtpi            2/sqrt(pi)
// </srcblock>

// <h3>e and functions thereof</h3>
// <srcblock>
// e                    e
// ln2                  ln(2)
// ln10                 ln(10)
// log2e                log2(e)
// log10e               log10(e)
// </srcblock>

// <h3>gamma and functions thereof</h3>
// <srcblock>
// gamma		gamma
// lngamma		ln(gamma)
// etogamma		e**gamma
// </srcblock>

// <h3>Fundamental physical constants (SI units)</h3>
// <note role=warning> Preserved for legacy reasons only.
// See <linkto class=QC>QC class</linkto> for other physical constants. </note>
// <srcblock>
// c			velocity of light (m/s)
// </srcblock>

// <h3>Numerical conversion factors</h3>
// <srcblock>
// quetta               e+30 (Q)
// ronna                e+27 (R)
// yotta                e+24 (Y)
// zetta                e+21 (Z)
// exa                  e+18 (E)
// peta                 e+15 (P)
// tera                 e+12 (T)
// giga                 e+09 (G)
// mega                 e+06 (M)
// kilo                 e+03 (k)
// hecto                e+02 (h)
// deka                 e+01 (da)
// deci                 e-01 (d)
// centi                e-02 (c)
// milli                e-03 (m)
// micro                e-06 (u)
// nano                 e-09 (n)
// pico                 e-12 (p)
// femto                e-15 (f)
// atto                 e-18 (a)
// zepto                e-21 (z)
// yocto                e-24 (y)
// ronto                e-27 (r)
// quecto               e-30 (q)
// </srcblock>

// <h3>Angular measure</h3>
// <srcblock>
// radian		radian
// circle		circle
// degree		degree
// arcmin		arcminute
// arcsec		arcsecond
// </srcblock>

// <h3>Solid angular measure</h3>
// <srcblock>
// steradian		steradian
// sphere		sphere
// square_degree	square degree
// square_arcmin	square arcminute
// square_arcsec	square arcsecond
// </srcblock>

// <h3>Time interval</h3>
// <srcblock>
// second               second
// minute               minute
// hour                 hour
// day                  day
// </srcblock>

// <h3> Machine constants </h3>
//
// Implementation-defined limits usually defined in <src><limits.h></src>,
// <src><float.h></src>, and <src><values.h></src> as preprocessor
// defines. They are 
// Inclusion of <src><casa/BasicSL/Constants.h</src> is
// sufficient to ensure that they are defined for any particular
// implementation, and the correct functioning of the <src>tConstants</src>
// test program guarantees this.
//
// In future use will be made of the (standard) <em>numeric_limits</em>
// template from the <src><limits></src> include file.

// <srcblock>
//
// Refer to Section 3.2c, pp28-30 of
// "The Annotated C++ Reference Manual",
// Ellis, M.A., and Stroustrup, B.,
// Addison-Wesley Publishing Company, 1990.
// IBSN 0-201-51459-1.
//
//    and
//
// Appendix B11, pp257-8 of
// "The C Programming Language", 2nd ed.,
// Kernighan, B.W., and Ritchie, D.M.,
// Prentice Hall Software Series, 1988.
// IBSN 0-13-110362-8.
//
// </srcblock>

// <h3> Constants defined in limits.h </h3>
// (these are part of the ANSI C and hence POSIX standards).
// Acceptable limits defined by the standard are quoted.
// <srcblock>
//
// CHAR_BIT              8  Maximum bits in a byte.
// CHAR_MIN           0 or  Minimum value of 'char'.
//               SCHAR_MIN
// CHAR_MAX   UCHAR_MAX or  Maximum value of 'char'.
//               SCHAR_MAX
// SCHAR_MIN          -127  Minimum value of 'signed char'.
// SCHAR_MAX          +127  Maximum value of 'signed char'.
// UCHAR_MAX           255  Maximum value of 'unsigned char'.
// MB_LEN_MAX               Maximum bytes in multibyte character.
//
// SHRT_MIN         -32767  Minimum value of 'short'.
// SHRT_MAX         +32767  Maximum value of 'short'.
// USHRT_MAX         65535  Maximum value of 'unsigned short'.
//
// INT_MIN          -32767  Minimum value of 'int'.
// INT_MAX          +32767  Maximum value of 'int'.
// UINT_MAX          65535  Maximum value of 'unsigned int'.
//
// LONG_MIN    -2147483647  Minimum value of 'long'.
// LONG_MAX    +2147483647  Maximum value of 'long'.
// ULONG_MAX    4294967295  Maximum value of 'unsigned long'.
//
// </srcblock>

// <h3> Constants defined in float.h </h3>
// (these are part of the ANSI C and hence POSIX standards).
// Acceptable limits defined by the standard are quoted.
// <srcblock>
//
// FLT_RADIX             2  Radix of exponent representation.
// FLT_ROUNDS               Floating point rounding mode for addition
//                            -1: indeterminate
//                             0: towards zero
//                             1: to nearest
//                             2: toward +infinity
//                             3: toward -infinity
//
// FLT_MIN_EXP              Minimum negative integer N such that FLT_RADIX
// DBL_MIN_EXP              raised to the Nth minus 1 is a normalized
// LDBL_MIN_EXP             floating point number.
//
// FLT_MAX_EXP              Maximum integer N such that FLT_RADIX raised to
// DBL_MAX_EXP              the Nth minus 1 is representable.
// LDBL_MAX_EXP
//
// FLT_MIN_10_EXP      -37  Minimum negative integer N such that 10 raised
// DBL_MIN_10_EXP      -37  to the Nth is in the range of normalized
// LDBL_MIN_10_EXP     -37  floating point numbers.
//
// FLT_MAX_10_EXP       37  Maximum integer N such that 10 raised to the
// DBL_MAX_10_EXP       37  Nth minus 1 is representable.
// LDBL_MAX_10_EXP      37
//
// FLT_MANT_DIG             Number of base FLT_RADIX digits in mantissa.
// DBL_MANT_DIG
// LDBL_MANT_DIG 
//
// FLT_DIG               6  Decimal digits of precision.
// DBL_DIG              10
// LDBL_DIG             10
//
// FLT_EPSILON        1E-5  Minimum floating point number X such  that
// (use C::flt_epsilon in preference to this)
// DBL_EPSILON        1E-9  1.0 + X does not equal 1.0.
// (use C::dbl_epsilon in preference to this)
// LDBL_EPSILON       1E-9
//
// FLT_MIN           1E-37  Minimum normalized positive floating point
// (use C::flt_min in preference to this)
// DBL_MIN           1E-37  number
// (use C::dbl_min in preference to this)
// LDBL_MIN          1E-37
//
// FLT_MAX           1E+37  Maximum representable floating point number.
// (use C::flt_max in preference to this)
// DBL_MAX           1E+37
// (use C::dbl_max in preference to this)
// LDBL_MAX          1E+37
//
// </srcblock>

// <h3> Constants defined in values.h </h3>
// (not part of the POSIX standard).
// <note role=warning> These constants will disappear in the near future.
// Do not use them in new code. </note>

// <srcblock>
//
// HIBITS            Value of a short integer with only the high-order
//                   bit set (in most implementations, 0x8000).
//
// HIBITL            Value of a long integer with only the high-order
//                   bit set (in most implementations, 0x80000000).
//
// MAXSHORT          Maximum value of a signed short integer (in most
//                   implementations, 0x7FFF = 32767).
//
// MAXLONG           Maximum value of a signed long integer (in most
//                   implementations, 0x7FFFFFFF = 2147483647).
//
// MAXINT            Maximum value of a signed regular integer (usually
//                   the same as MAXSHORT or MAXLONG).
//
// MINFLOAT          Minimum positive value of a single-precision
//                   floating-point number (use C::minfloat in preference 
//                   to this)
//
// MINDOUBLE         Minimum positive value of a double-precision
//                   floating-point number (use C::mindouble in preference 
//                   to this)
//
// MAXFLOAT          Maximum value of a single-precision floating-point number
//
// MAXDOUBLE         Maximum value of a double-precision floating-point number
//
// FSIGNIF           Number of significant bits in the mantissa of a
//                   single-precision floating-point number.
//
// DSIGNIF           Number of significant bits in the mantissa of a
//                   double-precision floating-point number.
// </srcblock>

namespace C {
  //#--------------------------------------------------------------------
  //  Mathematical constants
  //#--------------------------------------------------------------------
  //# <group>
  
  // Irrationals:
  // <group>
  // sqrt(2)
  [[deprecated("use M_SQRT2")]]
  inline constexpr double sqrt2 = 1.4142135623730950488;
  // sqrt(3)
  inline constexpr double sqrt3 = 1.7320508075688772935;
  // 1/sqrt(2)
  [[deprecated("use M_SQRT1_2")]]
  inline constexpr double _1_sqrt2 = 0.70710678118654752440;
  // 1/sqrt(3)
  inline constexpr double _1_sqrt3 = 0.57735026918962576451;
  // </group>

  // Pi and functions thereof:
  // <group>
  // pi
  [[deprecated("use M_PI")]]
  inline constexpr double pi = 3.141592653589793238462643;
  // 2*pi
  [[deprecated("use (2.0*M_PI)")]]
  inline constexpr double _2pi = 6.283185307179586476925286;
  // pi/2
  [[deprecated("use M_PI_2")]]
  inline constexpr double pi_2 = 1.570796326794896619231322;
  // pi/4
  [[deprecated("use M_PI_4")]]
  inline constexpr double pi_4 = 0.7853981633974483096156608;
  // 1/pi
  [[deprecated("use M_1_PI")]]
  inline constexpr double _1_pi = 0.3183098861837906715377675;
  // 2/pi
  [[deprecated("use M_2_PI")]]
  inline constexpr double _2_pi = 0.6366197723675813430755350;
  // 1/sqrt(pi)
  [[deprecated("use (0.5 * M_2_SQRTPI)")]]
  inline constexpr double _1_sqrtpi = 0.5641895835477562869480795;
  // 2/sqrt(pi)
  [[deprecated("use M_2_SQRTPI")]]
  inline constexpr double _2_sqrtpi = 1.1283791670955125738961590;
  // </group>
  
  // e and functions thereof:
  // <group>
  // e
  [[deprecated("use M_E")]]
  inline constexpr double e = 2.718281828459045235360287;
  // ln(2)
  [[deprecated("use M_LN2")]]
  inline constexpr double ln2 = 0.6931471805599453094172321;
  // ln(10)
  [[deprecated("use M_LN10")]]
  inline constexpr double ln10 = 2.3025850929940456840179915;
  // log2(e)
  [[deprecated("use M_LOG2E")]]
  inline constexpr double log2e = 1.4426950408889634074;
  // log10(e)
  [[deprecated("use M_LOG10E")]]
  inline constexpr double log10e = 0.4342944819032518276511289;
  // </group>
  
  // gamma and functions thereof:
  // <group>
  // gamma
  // With C++20, could be deprecated and use std::numbers::gamma instead
  inline constexpr double gamma = 0.577215664901532860606512;
  // ln(gamma)
  // With C++20, could be deprecated and use std::log(std::numbers::gamma) instead
  inline constexpr double lngamma = -0.549539312981644822337662;
  // e**gamma
  // With C++20, could be deprecated and use std::exp(std::numbers::gamma) instead
  inline constexpr double etogamma =  1.7810724179901979852;
  // </group>
 
  // statistics related
  // 1/(Phi^(-1)(3/4), see https://en.wikipedia.org/wiki/Median_absolute_deviation#Relation_to_standard_deviation
  inline constexpr double probit_3_4 = 1.482602218505602;

  //#--------------------------------------------------------------------
  //# Mathematical constants
  //#--------------------------------------------------------------------
  //# </group>
  
  
  //#--------------------------------------------------------------------
  //  Machine constants
  //#--------------------------------------------------------------------
  //# <group>
  // floating point limits
  // <group>
  // the minimum single precision floating point number, 
  // excluding denormalised numbers
  [[deprecated("Use FLT_MIN")]]
  inline constexpr double flt_min = FLT_MIN;
  // the minimum single precision floating point number, 
  // including denormalised numbers
  [[deprecated("Use FLT_MIN")]]
  inline constexpr double minfloat = FLT_MIN;
  // the minimum double precision floating point number,
  // excluding denormalised numbers
  [[deprecated("Use DBL_MIN")]]
  inline constexpr double dbl_min = DBL_MIN;
  // the minimum double precision floating point number,
  // including denormalised numbers
  [[deprecated("Use DBL_MIN")]]
  inline constexpr double mindouble = DBL_MIN;
  // the maximum single precision floating point number 
  [[deprecated("Use FLT_MAX")]]
  inline constexpr double  flt_max = FLT_MAX;
  // the maximum double precision floating point number 
  [[deprecated("Use DBL_MAX")]]
  inline constexpr double  dbl_max = DBL_MAX;
  // Minimum single precision floating point number X such that 1+X does not
  // equal X
  [[deprecated("Use FLT_EPSILON")]]
  inline constexpr double flt_epsilon = FLT_EPSILON;
  // Minimum double precision floating point number X such that 1+X does not
  // equal X
  [[deprecated("Use DBL_EPSILON")]]
  inline constexpr double dbl_epsilon = DBL_EPSILON;
  // </group>
  
  //#--------------------------------------------------------------------
  //# Machine constants
  //#--------------------------------------------------------------------
  //# </group>
  
  //#--------------------------------------------------------------------
  //#  Physical constants, and quantities
  //#--------------------------------------------------------------------
  //# <group>
  
  // Fundamental physical constants (SI units):
  // <group>
  // velocity of light -- for legacy reasons only -- refer to PC
  inline constexpr double c = 2.99792458e+08;
  // </group>
  
  //#--------------------------------------------------------------------
  //# Physical constants, and quantities
  //#--------------------------------------------------------------------
  //# </group>
  
  
  
  //#--------------------------------------------------------------------
  //#  Physical units
  //#--------------------------------------------------------------------
  //# <group>
  
  //#-----------------------------
  //#  Numerical conversion factors
  //#-----------------------------
  //# <group>
  
  //  Numerical conversion factors
  // <group>
  // e+30 (Q)
  inline constexpr double quetta = 1.0e+30;
  // e+27 (R)
  inline constexpr double ronna = 1.0e+27;
  // e+24 (Y)
  inline constexpr double yotta = 1.0e+24;
  // e+21 (Z)
  inline constexpr double zetta = 1.0e+21;
  // e+18 (E)
  inline constexpr double exa = 1.0e+18;
  // e+15 (P)
  inline constexpr double peta = 1.0e+15;
  // e+12 (T)
  inline constexpr double tera = 1.0e+12;
  // e+09 (G)
  inline constexpr double giga = 1.0e+09;
  // e+06 (M)
  inline constexpr double mega = 1.0e+06;
  // e+03 (k)
  inline constexpr double kilo = 1.0e+03;
  // e+02 (h)
  inline constexpr double hecto = 1.0e+02;
  // e+01 (da)
  inline constexpr double deka = 1.0e+01;
  // e-01 (d)
  inline constexpr double deci = 1.0e-01;
  // e-02 (c)
  inline constexpr double centi = 1.0e-02;
  // e-03 (m)
  inline constexpr double milli = 1.0e-03;
  // e-06 (u)
  inline constexpr double micro = 1.0e-06;
  // e-09 (n)
  inline constexpr double nano = 1.0e-09;
  // e-12 (p)
  inline constexpr double pico = 1.0e-12;
  // e-15 (f)
  inline constexpr double femto = 1.0e-15;
  // e-18 (a)
  inline constexpr double atto = 1.0e-18;
  // e-21 (z)
  inline constexpr double zepto = 1.0e-21;
  // e-24 (y)
  inline constexpr double yocto = 1.0e-24;
  // e-27 (r)
  inline constexpr double ronto = 1.0e-27;
  // e-30 (q)
  inline constexpr double quecto = 1.0e-30;
  // </group>
  
  // Angular measure:
  // <group>
  // radian
  inline constexpr double radian = 1.0;
  // circle
  inline constexpr double circle = 6.2831853071795864769252867;
  // degree
  inline constexpr double degree = 0.0174532925199432957692369;
  // arcminute
  inline constexpr double arcmin = 0.000290888208665721596153948459;
  // arcsecond
  inline constexpr double arcsec = 0.00000484813681109535993589914098765;
  // </group>
  
  // Solid angular measure:
  // <group>
  // steradian
  inline constexpr double steradian =  1.0;
  // sphere
  inline constexpr double sphere = 12.56637061435917295385057344;
  // square degree
  inline constexpr double square_degree =  0.30461741978670859934674354486e-3;
  // square arcminute
  inline constexpr double square_arcmin =  0.8461594994075238870742876246233e-7;
  // square arcsecond
  inline constexpr double square_arcsec=  0.235044305390978857520635451284e-10;
  // </group>
  
  //#-----------------------------
  //# Numerical conversion factors
  //#-----------------------------
  //# </group>
  
  
  //#----------------------------
  //#  Physical conversion factors
  //#----------------------------
  //# <group>
  
  // Time interval [T]:
  // <group>
  // second
  inline constexpr double second = 1.0;
  // minute
  inline constexpr double minute = 60.0;
  // hour
  inline constexpr double hour = 3600.0;
  // day
  inline constexpr double day = 86400.0;
  // Difference between Julian Day and Modified Julian Day
  inline constexpr double MJD0 = 2400000.5;
  // </group>
  
}


} //# NAMESPACE CASACORE - END

#endif
