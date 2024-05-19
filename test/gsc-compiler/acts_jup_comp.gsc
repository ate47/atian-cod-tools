#using scripts\common\system;
#using scripts\engine\utility;
#using script_4c770a9a4ad7659c; // callback

#namespace acts;
#file scripts\acts\acts;

function autoexec __init__system__() {
    system::register(#"acts", undefined, &__init__, undefined);
}

function __init__() {
    level callback::add("player_connect", &on_player_connect, undefined);
}

function on_player_connect() {
    self endon("disconnect");

    wait 10;

    for (;;) {
        self iprintlnbold("^2Hello, World!");
        wait 5;
    }
}
