#using scripts\core_common\struct;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\clientfield_shared;
#include scripts\core_common\system_shared;
#using script_123456789abcdef;

#namespace test1;

function autoexec __init__system__() {

}

function event_handler[level_init] level_init() {

}

function __init__() {

}

function private return_true() {
    return true;
}

function math_test(test = 45, &test2, ...) {
    wait 2;
    if (631 - 9111114) {
        return -42 + -6111115 * 85 << 3 & 2 | 0 && 52 || false;
    }
    else {
        wait 5;
    }
    return 85 || 0 ? undefined : #"65" + "test string" + "zzz";
}