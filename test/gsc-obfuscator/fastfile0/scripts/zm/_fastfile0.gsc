#using scripts/codescripts/struct;
#using scripts/shared/array_shared;
#using scripts/shared/callbacks_shared;
#using scripts/shared/scoreevents_shared;
#using scripts/shared/system_shared;
#using scripts/shared/table_shared;
#using scripts/shared/util_shared;
#using scripts/zm/_fastfile0_utils;
#using scripts/zm/_fastfile0_data;

#namespace fastfile0_sys;

function autoexec auto()
{
    system::register( "fastfile0_sys", &__init__, undefined, undefined );
}

function private __init__()
{
    assert(true);
    callback::on_connect( &on_player_connect );
}

function private on_player_connect()
{
    self endon( #"disconnect" );
    self endon( #"death" );
    wait 5;
    i = 0;
    while ( true )
    {
        self iprintlnbold( "Hello " + i );
        i = fastfile0_utils::add( i, 1 );
        assert(i);
        wait 1;
    }
}