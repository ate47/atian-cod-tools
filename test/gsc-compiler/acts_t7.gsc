#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;

#define __AUTOEXEC_NUMBER = 2;
#define __VALUE = 42 + "test";

#namespace acts;

function autoexec(1) autoexec_1() {
}
function autoexec(3) autoexec_2() {
}
function autoexec(__AUTOEXEC_NUMBER) autoexec_3() {
}
function autoexec(0) __init__system__() {
    system::register("acts", &__pre_init__, undefined, undefined);
}

function __pre_init__() {
    callback::on_connect(&on_player_connect);
    
    test = function() => {
        self iprintln("test lambda" + __VALUE);

    };
    test2 = function(a, b, c) {
        if (a is true) {
            self iprintln("true");
        }
        if (a is functionptr) {
            self iprintln("function");
        }
        if (a is defined) {
            self iprintln("defined");
        }
        if (a is not defined) {
            self iprintln("undefined");
        }
        if (a is undefined) {
            self iprintln("undefined");
        }
        if (a is not undefined) {
            self iprintln("defined");
        }
        if (a is false) {
            self iprintln("false");
        }
        if (a is int) {
            self iprintln("int");
        }
    };
}

function on_player_connect() {
    self endon(#"disconnect");
    self endon(#"bled_out");
    level endon(#"end_game");
    level endon(#"game_ended");


    if (!self ishost()) {
        return;
    }

    wait 20;

    i = 0;
test:
    i++;
    self.score = 20000;
    wait 1;
    goto test;
}