// import a script to use its function, the extension isn't mandatory (.gsc/.csc)
// it uses the one of the current script.
#using scripts\core_common\system;

// to insert a gsc header (gsh), the insert keyword can be used.
#insert scripts\core_common\shared.gsh;

// define a constant value
#define MY_CONSTANT 42

// in the fastfile linker context, the precache keyword can be 
// used to add an asset in the cache.
#precache( "string", "ACTS/DEMO_DESC" );

// if the vm allows it, an animtree can be used
#using_animtree( "demo_animtree" );

// define the current namespace, used when a function is imported
#namespace demo_acts;

// define a function that will automatically be executed by the game
function autoexec __init_system__()
{
    // call the function "register" with the namespace "system" included previously
    // undefined is like null in other language.
    // &__init__ is a function reference
    system::register("demo_acts", undefined, &__init__);
}

// define a private function
function private __init__()
{
    // values and types
    a = 42; // define a variable
    arr = [ 1, 45, 64 ]; // define an array (>bo4)
    stc = { #foo: "test", #bar: [] }; // define a structure (>bo4)
    vec = ( 1, 2, 3 ); // vector
    func = &__init__; // function ref
    func2 = &system::register; // function ref with namespace
    loc = &"ACTS/DEMO_DESC"; // localized reference (<bo3 or >mwiii)
    hash = #"hash"; // hash (>bo4)
    cst = MY_CONSTANT; // use constant defined previously
    atr = #animtree; // the used animtree (defined with the last #using_animtree)
    an = %acts_anim; // animation in the used animtree
    var_ref = &a; // var reference (only cw)
    b = self.value; // self is the function caller or undefined
    
    // hashes
    hashes = [
        #"hash", #x"hash", // default hash (>bo3)
        #scr_hash, #s"scr_hash", // scr hash
        %"asset hash", #a"asset hash", // asset hash (>mwiii)
        @"dvar hash", #d"dvar hash", // dvar hash (>mwiii)
        o"omnvar hash", #o"omnvar hash", // omnvar hash (>bo6)
        t"x32 hash", #x32"x32 hash", // x32 hash (mwiii)
    ];

    // control blocks
    if ( true ) {} // if block
    else if ( false ) {}  // else if block
    else {} // else block
    for ( i = 0; i < 42; i++ ) {} // for block
    while ( true ) {} // while block
    do {} while (false); // do while block
    
    // loop over array
    foreach ( v in arr ) {} // loop over values
    foreach ( k, v in arr ) {} // loop over keys/values
    
    // operators
    [[ func ]](); // call function reference
    [x, y, z] = arr; // unpack an array
    wait 1.2; // wait 1.2 second and put the func in sleep
    waitframe(11); // wait 11 frame (treyarch only) and put the func in sleep
    waitframe(); // wait 1 frame (iw only) and put the func in sleep
    self endon( "my_event" ); // stop this function in sleep when the event "my_event" is received on self
    self notify("my_event" ); // notify the event my_event on self
    b = stc.undd?.long?.path; // get a value that might not be defined
    c = stc.undd ?? 42; // get a value or another if not defined
    d = b is not defined; // is (not) defined -> same as isdefined/!isdefined
    e = d is true; // is true/false -> same as (isdefined(v) && (v))
    f = e is int; // type check -> same as calling the function isint(v)

    // dev
    /#
        // dev block, only executed when in dev mode, requires a dvar for bo3 modtool
        // otherwise requires an engine mod to enable it.
        // some functions can only be used inside dev blocks
        AddDebugCommand( "command" );
    #/

    // to call functions, multiple ways exist
    my_func(); // basic function call
    thread my_func(); // call a function, but continue the execution after first its end or a sleep
                      // my_func won't be stopped for our endons, same for the current function
    childthread my_func(); // (>bo4) same as thread, but copy our endons to the new func
    [[ func ]](); thread [[ func ]](); childthread [[ func ]](); // call a function pointer
    builtin [[ func ]](); // call a function pointer, but func needs to be a builtin (iw only)
}

// on >bo4 (treyarch vms) / >bo6 (iw vms) a callback can be defined to be called by the engine
// the event object contains the optional struct sent from the engine.
function event_handler[Test_Event] callback_test_event(event)
{
    // code when event
}

// the ... param creates a function with a variable params count
function vargs_func( ... )
{
    // if the last param of a func is "...", it is added a param "vararg" containing
    // all the params after the param
    // for >bo6, the param varargcount contains the size of vararg
    for ( i = 0; i < vararg.size; i++ )
    {
        println( "hello : " + vararg[i] );
    }
}

// it is possible to define a default value for a param, if the param is undefined or not given
// it'll use this param.
function default_param( a = 42 ) { return a; }
function test()
{
    default_param(); // returns 42
    default_param( undefined ); // same as no call, returns 42
    default_param( 34 ); // returns 34
}

// params can also have modifiers
function example_params(
    // array reference (>bo3)
    &array,
    // var reference (=cw)
    *a
) {}

// expression used in const contextes
#constexpr VALUE_2 = 2;

// autoexec can be ordered using an ordering number
function autoexec(1) ae1() {}
function autoexec(0) ae0() {}
function autoexec(VALUE_2) ae2() {}
// the order will be ae0(0), ae1(1) and ae2(VALUE_2=2)

// the function keyword isn't mandatory
test() {} // define func test

// a function doesn't need to have a name
() {} // define nameless func

()
{
    // inlined function definition (not suggested)
    inlined_func = function ( a, b )
    {
        return a + b;
    };

    // call function
    sum = [[ inlined_func ]]( 1, 2 ); // sum=3

    // lazy function reference, only computed at runtime, returns undefined
    // if the script isn't available or the function doesn't exist.
    // a modification of the engine is required to add the lazy references
    lazy = @test<scripts\core_common\test>::func;
}

// detour the function scripts\core_common\test.gsc test::func
// a modification of the engine is required to add the detours
detour test<scripts\core_common\test>::func()
{
    return true;
}