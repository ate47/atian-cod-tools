#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;

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

    sbtest = GetScriptBundle( #"acts_test" );

    ActsLog( "test ScriptBundle" );
    ActsLog( "bundle: " + sbtest.type + ":" + sbtest.name );
    ActsLog( sbtest.xhash );
    ActsLog( sbtest.weapon.name );
    ActsLog( sbtest.number );
    ActsLog( sbtest.array[ 0 ].item );
    ActsLog( sbtest.array[ 0 ].test );
    ActsLog( "-----------------" );

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
    self endon( #"disconnect", #"bled_out" );
    level endon( #"end_game", #"game_ended" );

    for (i = 0; ; i++)
    {
        self IPrintLnBold( "^2Hello world " + i );

        wait(1);
    }
}