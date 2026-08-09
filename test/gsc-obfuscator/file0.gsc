#using scripts/codescripts/struct;
#using scripts/shared/array_shared;
#using scripts/shared/callbacks_shared;
#using scripts/shared/scoreevents_shared;
#using scripts/shared/system_shared;
#using scripts/shared/table_shared;
#using scripts/shared/util_shared;

#namespace t7_obf_test;

function autoexec auto()
{
    system::register( "t7_obf_test", &__init__, undefined, undefined );
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
    my_name = "gsc obfuscator";
    wait 5;
    i = 0;
    while ( true )
    {
        self iprintlnbold( "Hello " + i + ", my name is " + my_name );
        i = add( i, 1 );
        wait 1;
    }
    /#
        iprintln( "test dev block" );
        iprintln( "test dev block" + 42 );
    #/
}

function private add( a, b )
{
    return a + b;
}