#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

#insert scripts\core_common\acts\test_defs.gsh;

#ifndef ACTS_DEFS_TEST
// should be part of the gsh file
#error Missing ACTS_DEFS_TEST
#endif

#namespace echo_test;

#precache( "string", "ACTS_SPRAY_DESC" );

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

    // test script bundles

    sbtest = GetScriptBundle( #"acts_test" );

    ActsLog( "test ScriptBundle" );
    ActsLog( "bundle: " + sbtest.type + ":" + sbtest.name );
    ActsLog( sbtest.xhash );
    ActsLog( sbtest.weapon.name );
    ActsLog( sbtest.number );
    ActsLog( sbtest.array[ 0 ].item );
    ActsLog( sbtest.array[ 0 ].test );
    ActsLog( "-----------------" );

    // test detour
    ActsLog( "safehouse: " + util::is_safehouse() );

    // test string tables

    st = #"acts/test.csv";
    ActsLog( "test StringTable" );
    num_rows = TableLookupRowCount( st );
    for (i = 0; i < num_rows; i++)
    {
        sv = TableLookupColumnForRow( st, i, 0 );
        iv = TableLookupColumnForRow( st, i, 1 );
        hv = TableLookupColumnForRow( st, i, 2 );

        ActsLog( "[" + i + "] " + sv + "/" + iv + "/" + hv );
    }
    ActsLog( "-----------------" );
}

function private on_player_connect()
{
    // ingame test
    self endon( #"disconnect", #"bled_out" );
    level endon( #"end_game", #"game_ended" );

    for (i = 0; ; i++)
    {
        self IPrintLnBold( "^2Hello world " + i );

        wait(1);
    }
}

detour util<scripts\core_common\util_shared>::is_safehouse()
{
    return 42;
}