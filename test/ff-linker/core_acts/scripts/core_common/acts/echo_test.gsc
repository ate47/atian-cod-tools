#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

#namespace echo_test;

function private autoexec __init__system__() {
    system::register(#"acts_echo_test", &__pre_init__, undefined);
}

function private __pre_init__() {
    if (util::is_frontend_map()) {
        return; // only for ingame
    }
    
    callback::on_connect(&on_player_connect);
}

function private on_player_connect() {
    self endon("disconnect", #"bled_out");
    level endon(#"end_game", #"game_ended");

    for (i = 0; ; i++) {
        self iprintlnbold("^2Hello world " + i);

        wait 1;
    }
}