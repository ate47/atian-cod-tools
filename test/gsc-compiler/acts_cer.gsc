#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;

#namespace acts;
// #file scripts\test\acts; // not a feature anymore (good)

function event_handler[sprint_begin] on_sprint_begin(params) {

    println("test " + &"ok" + t"ok2" + #"ok3" + @"ok4");
}

function autoexec test_ev() {
    assert(dev::func_dev(2) === 2);
    level.acts = {
        #a: 42,
        #b: [1,2,3,4],
        #c: { #d: "bye", #e: "bye" }
    };
    test3 = "hello";
    test = 42 >== 23 <== 64 === 22 !== false;
    test2 = test ?? 23;
    test3 = getthread();
    test3 = istrue(true);
    waitframe();

    level notify("notify calls", 42, 52);

    self iprintln("builtin method call");
    self iprintln("builtin method call");
    iprintln("builtin function call");
    iprintln("builtin function call");

    while (!isdefined(level.players) && !isdefined(level.players[0])) {
        waitframe();
    }

    f = &test;

    level.players[0] thread [[ f ]]();
}

function test2(..., params) {
    // todo: find name of params
    return self iprintln(flat_args(vararg, params));
}

function test() {
    self endon("test plr");

    while (true) {
        self waittill("test waittill", v1, v2, v3);

        self iprintln("hello: " + v1 + v2 + v3);
    }
}