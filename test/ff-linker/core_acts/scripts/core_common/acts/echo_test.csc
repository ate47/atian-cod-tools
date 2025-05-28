#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

#namespace echo_test;

function private autoexec __init__system__()
{
    system::register( #"acts_echo_test", &__pre_init__, undefined );
}

function private __pre_init__()
{
    ActsLog( "Log from client" );
    ActsLog( "Test hash: " + ActsHashLookup( #"this is a hash" ) );

    // thread late_error();
}

function private late_error()
{
    f1();
}

function private f1()
{
    f2();
}

function private f2()
{
    f3();
}

function private f3()
{
    wait( 20 );
    a = "" + undefined;
}
