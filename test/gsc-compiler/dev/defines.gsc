#insert test\gsc-compiler\dev\defines_insert.gsh;

#define TEST_STRING "hello" // string val
#define TEST_STRING3 "haaaaaaaaaaaaaaaaaaa1"
#define AUTOEXEC_VAL 4  // Num autoexec


function autoexec(AUTOEXEC_VAL) ttt()
{
    a = TEST_STRING + TEST_STRING2 + TEST_STRING3;

}