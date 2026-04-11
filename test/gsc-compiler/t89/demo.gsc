#using scripts\core_common\struct;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\clientfield_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\util_shared;

#namespace demo_bo4;

function autoexec __init__system__()
{
	system::register( #"demo_bo4", &__init__, undefined, undefined );
}

function __init__()
{
	callback::on_connect( &on_player_connect );
}

function on_player_connect()
{
    self iprintln( "hello world" );
}