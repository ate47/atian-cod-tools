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

    level.acts = {
        #counter: 0

    };
}

function on_player_connect() {
    self endon(#"disconnect", #"bled_out");
    level endon(#"end_game", #"game_ended");


    if (!self ishost()) {
        return;
    }

    wait 20;

    i = 0;
test:
    i++;
    level.acts.counter++;
    if (self usebuttonpressed()) {
        


    } else if (self adsbuttonpressed()) {
        self iprintlnbold(
            "vector: " + vectorscale((i, 0, 2), 24)
            + "/" + i
        );
    } else {
        self iprintlnbold(
            "test: #" + level.acts.counter  + "/" + i
        );
    }
    wait 1;
    goto test;
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