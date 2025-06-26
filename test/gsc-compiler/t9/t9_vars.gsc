#using scripts\core_common\struct;
#using scripts\core_common\array_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\clientfield_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\flag_shared;
#using scripts\core_common\values_shared;
#using scripts\core_common\util_shared;
#using scripts\core_common\player\player_stats;
#using scripts\core_common\spawner_shared;

#namespace t9_vars;

function private autoexec __init_system__()
{
    system::register(#"t9_vars", &__init__);
}

function private __init__()
{
    
}