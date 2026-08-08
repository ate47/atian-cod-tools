#using scripts/codescripts/struct;
#using scripts/shared/array_shared;
#using scripts/shared/callbacks_shared;
#using scripts/shared/scoreevents_shared;
#using scripts/shared/system_shared;
#using scripts/shared/table_shared;
#using scripts/shared/util_shared;
#using scripts/core/_test_acts2;

#namespace test_acts;

function autoexec auto()
{
    system::register( "test_acts", &__init__, undefined, undefined );
}

function private __init__()
{
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
        i = _test_acts2::add(i, 1);
        wait 1;
    }
}
