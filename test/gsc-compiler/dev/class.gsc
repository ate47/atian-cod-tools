#include scripts/core_common/array_shared;

#namespace cls;

class ctest_class {



}

function autoexec test() {
    level iprintln("test");
    test = new ctest_class();
    a = [[ test ]]->test(level);
}