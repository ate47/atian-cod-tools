#using scripts\common\system;
#using scripts\engine\utility;
#using scripts\common\callbacks;

#namespace acts;
#file scripts\acts\acts;

function autoexec __init__system__() {
    // system::register(#"acts", undefined, &__init__, undefined);
    // no system to allow injecting in game
    thread wait_players();
}

function wait_players() {
    while (!isdefined(level.players) || level.players.size == 0) {
        wait (1);
    }

    for (i = 0; i < level.players.size; i++) {
        if (!isdefined(level.players[i])) continue;
        level.players[i] thread on_player_connect();
    }

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
