#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;

#namespace acts;

function autoexec(0) __init__system__() {
    system::register("acts", &__pre_init__, undefined, undefined);
}

function __pre_init__() {
    callback::on_connect(&on_player_connect);
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