#using scripts\core_common\array_shared.gsc;
#using scripts\core_common\system_shared.gsc;
#using scripts\core_common\callbacks_shared.gsc;

#namespace acts;

function autoexec __init__system__() {
    if (is_mp()) {
        setgametypesetting(#"drafttime", 2);
    }
    system::register(#"acts", &__pre_init__, undefined, undefined);
}

function __pre_init__() {
    callback::on_connect(&on_player_connect);
}

function on_player_connect() {
    self endon(#"disconnect", #"bled_out");
    level endon(#"end_game", #"game_ended");


    if (!self ishost()) {
        return;
    }

    wait 20;

    self.score = 500;

    i = 0;
    for (;;) {
        i++;
        self.score += 200;
        self iprintlnbold("test: #" + i + " score: " + self.score);
        wait 1;
    }
}


function is_wz() {
    return sessionmodeiswarzonegame();
}

function is_zm() {
    return sessionmodeiszombiesgame();
}

function is_mp() {
    return sessionmodeismultiplayergame();
}