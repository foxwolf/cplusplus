#include "testutils.h"
#include "md2.h"

int
test_main(void)
{
  /* Testcases from RFC 1319 */
  test_hash(&nettle_md2, 0, "",
	    H("8350e5a3e24c153df2275c9f80692773"));
  test_hash(&nettle_md2, LDATA("a"),
	    H("32ec01ec4a6dac72c0ab96fb34c0b5d1"));
  test_hash(&nettle_md2, LDATA("abc"),
	    H("da853b0d3f88d99b30283a69e6ded6bb"));
  test_hash(&nettle_md2, LDATA("message digest"),
	    H("ab4f496bfb2a530b219ff33031fe06b0"));
  test_hash(&nettle_md2, LDATA("abcdefghijklmnopqrstuvwxyz"),
	    H("4e8ddff3650292ab5a4108c3aa47940b"));
  test_hash(&nettle_md2,
	    LDATA("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		  "0123456789"),
	    H("da33def2a42df13975352846c30338cd"));
  test_hash(&nettle_md2, LDATA("1234567890123456789012345678901234567890"
			       "1234567890123456789012345678901234567890"),
	    H("d5976f79d83d3a0dc9806c3c66f3efd8"));

  SUCCESS();
}
