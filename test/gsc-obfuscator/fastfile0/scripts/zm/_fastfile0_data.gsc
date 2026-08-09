
#namespace fastfile0_data;

function notify_player()
{
    self notify( #"fastfile0_do_something" );
}

function wait_player()
{
    self waittill( #"fastfile0_do_something" );
}