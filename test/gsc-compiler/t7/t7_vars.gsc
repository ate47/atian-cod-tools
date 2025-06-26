#using scripts/shared/array_shared;
#using scripts/shared/flag_shared;
#using scripts/shared/flagsys_shared;
#using scripts/shared/util_shared;

#namespace t7_vars;

#using_animtree( "acts_animtree_demo" );

function demo()
{
    // varaiable test
    level endon( #t7varendon );
    istrings = &"test/acts_istring";
    atr = #animtree;
    an = %acts_anim;
    hash = #hello;
    str = "my string";

    // function calls
    t = Array();
    array::add( t, 42 );
    array::add( t, 24 );

    // struct test
    s = SpawnStruct();
    s.a = 2;
    s.b = SpawnStruct();
    s.b.c = 4;

    if ( s.a )
    {
        compiler::nprintln( "test" );
        compiler::nprintln(isprofilebuild());
    }
    isprofilebuild(#nprintln, "test2");

    // va test
    PrintLn( "sum() = " + sumva( 1, 3, 5, 7, 9 ) );
}

function sumva( ... )
{
    c = 0;
    for ( i = 0 ; i < vararg.size ; i++ ) {
        c += vararg[i];
    }
    return c;
}
