#include scripts\utility;

#namespace acts_dev;

function add( a, b )
{
    return a + b;
}

function log( msg )
{
    print( "::log(" + msg + ")" );
}

autoexec main()
{
    print( "hello world " + add( 1, 2 ) + ", " + utility::add( 3, 4 ) + ".");

}


function test()
{
    a = undefined;
    print( "demo function call" );
    log( "demo script function call" );
    a print( "demo method call" );
    a log( "demo script method call" );
}