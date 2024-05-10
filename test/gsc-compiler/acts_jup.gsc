#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;

#namespace acts;
#file scripts\test\acts;

function autoexec __init__system__() {
    level.acts = {
        #a: 42,
        #b: [1,2,3,4],
        #c: { #d: "bye", #e: "bye" }
    };
    test3 = "hello";
    test = 42;
    test2 = test;
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

function test() {
    self endon("test plr");

    while (true) {
        self waittill("test waittill", v1, v2, v3);

        self iprintln("hello: " + v1 + v2 + v3);
    }
}

function sum(array, count = array.size) {
    acc = 0;

    for (i = 0; i < count; i++) {
        acc += array[i];
    }
    
    return acc;
}

function get_name() {
    if (!isdefined(self.name)) {
        return "invalid name";
    }
    return self.name;
}