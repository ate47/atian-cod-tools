#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

#namespace no_linked_test;

function private autoexec __init__system__()
{
    // crash because it shoundn't be linked
    ActsLog( "THIS MESSAGE SHOULDN'T BE THERE" );
    ErrorMsg( "ERROR LINKED" );
}