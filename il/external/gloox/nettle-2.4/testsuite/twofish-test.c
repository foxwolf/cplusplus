#include "testutils.h"
#include "twofish.h"

int
test_main(void)
{
  /* 128 bit key */
  test_cipher(&nettle_twofish128,
	      HL("0000000000000000 0000000000000000"),
	      HL("0000000000000000 0000000000000000"),
	      H("9F589F5CF6122C32 B6BFEC2F2AE8C35A"));

  /* 192 bit key */
  test_cipher(&nettle_twofish192,
	      HL("0123456789ABCDEF FEDCBA9876543210"
		 "0011223344556677"),
	      HL("0000000000000000 0000000000000000"),
	      H("CFD1D2E5A9BE9CDF 501F13B892BD2248"));

  /* 256 bit key */
  test_cipher(&nettle_twofish256,
	      HL("0123456789ABCDEF FEDCBA9876543210"
		 "0011223344556677 8899AABBCCDDEEFF"),
	      HL("0000000000000000 0000000000000000"),
	      H("37527BE0052334B8 9F0CFCCAE87CFA20"));

  SUCCESS();
}
