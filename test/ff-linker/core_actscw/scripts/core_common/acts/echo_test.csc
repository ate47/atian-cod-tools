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
    // client struff
}
