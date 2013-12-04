/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License along
with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */


#define GMP_LIMB_BITS 64
#define BYTES_PER_MP_LIMB 8


/* Generated by tuneup.c, 2004-02-10, gcc 3.2 & MIPSpro C 7.2.1 (R1x000) */

#define MUL_TOOM22_THRESHOLD             16
#define MUL_TOOM33_THRESHOLD             89

#define SQR_BASECASE_THRESHOLD            6
#define SQR_TOOM2_THRESHOLD              32
#define SQR_TOOM3_THRESHOLD              98

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 53
#define POWM_THRESHOLD                   61

#define HGCD_THRESHOLD                  116
#define GCD_ACCEL_THRESHOLD               3
#define GCD_DC_THRESHOLD                492
#define JACOBI_BASE_METHOD                2

#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always */

#define GET_STR_DC_THRESHOLD             21
#define GET_STR_PRECOMPUTE_THRESHOLD     26
#define SET_STR_THRESHOLD              3962

#define MUL_FFT_TABLE  { 368, 736, 1600, 3328, 7168, 20480, 49152, 0 }
#define MUL_FFT_MODF_THRESHOLD          264
#define MUL_FFT_THRESHOLD              1920

#define SQR_FFT_TABLE  { 368, 736, 1856, 3328, 7168, 20480, 49152, 0 }
#define SQR_FFT_MODF_THRESHOLD          280
#define SQR_FFT_THRESHOLD              1920
