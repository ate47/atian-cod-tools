#using scripts\core_common\array_shared.gsc;
#using scripts\core_common\system_shared.gsc;
#using scripts\core_common\callbacks_shared.gsc;

#namespace count_test;

function private autoexec __init__system__() {
    system::register(#"count_test", undefined, &__init__, undefined);
}

function private __init__() {}
