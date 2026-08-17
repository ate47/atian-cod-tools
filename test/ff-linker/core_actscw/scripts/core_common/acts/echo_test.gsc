#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

#namespace echo_test;

#precache( "string", "ACTS/SPRAY_DESC" );
#precache( "eventstring", "acts_core_info_data" );

function private autoexec __init__system__()
{
    system::register( #"acts_echo_test", &__pre_init__, undefined );
}


function private __pre_init__()
{
    if ( !util::is_frontend_map() )
    {
        callback::on_connect( &on_player_connect );
    }
}
function private on_player_connect()
{
    // ingame test
    self endon( #"disconnect", #"bled_out" );
    level endon( #"end_game", #"game_ended" );

    i = 0;
    while (true)
    {
        self iprintlnbold( "Hello " + i );
        i++;
        wait 1;
    }
}
