#namespace unittest;

#define UNIT_TEST

/*
 * Print log message
 */
function test_print(msg) {
    // print message
    shieldlog("UNITTEST: " + msg);
}

/*
 * Run debug test
 */
function autoexec run_tests() {
    test_print("Begin");

    local();
    local_structs();
    level_object();
    func_calls();
    notif_tests();

    test_print("End");

    level notify(#"acts_end_test"); // notify end message to stop the thread in case of error
}

/*
 * Run local var tests
 */
function local() {
    test_print("-- local");
    a = 0;
    b = 1;
    c = "hi";
    d = "bye" + 0;
    test_print("constant: " + a + " " + b + " " + c + " " + d);
    a = 0;
    b = a;
    b = 2;
    c = b;
    a = a;
    test_print("assign: " + a + " " + b + " " + c + " " + d);
    c = 25;
    a = 0x10;
    b = 017;
    d = 0b11;
    test_print("constant 2: " + a + " " + b + " " + c + " " + d);
    v = (0, 0, 0);
    test_print("v0: (" + v[0] + ", " + v[1] + ", " + v[2] + " )");
    v = (1, 0, 0);
    test_print("v1: (" + v[0] + ", " + v[1] + ", " + v[2] + " )");
    v = (1, 2, 3.1);
    test_print("v2: (" + v[0] + ", " + v[1] + ", " + v[2] + " )");
    v = vectorscale((1, 0, 63), 65);
    test_print("v3: (" + v[0] + ", " + v[1] + ", " + v[2] + " )");
    x = v;
    test_print("x0: (" + x[0] + ", " + x[1] + ", " + x[2] + " )");
    x = (a, b, c);
    test_print("x1: (" + x[0] + ", " + x[1] + ", " + x[2] + " )");
    x = (a, 0, c);
    test_print("x2: (" + x[0] + ", " + x[1] + ", " + x[2] + " )");
    u = undefined;
    test_print("u: " + isdefined(u));
    s2 = {};
    s = s2;
    s = {};
    a0 = [];
    a1 = a0;
    a0 = [1, 2, 3];
}

/*
 * Run local structs tests
 */
function local_structs() {
    test_print("-- local structs");

    s = {
        #a: {
            #b: {
                #c: "val",
                #d: 42,
            },
            #arr: [
                5, 7, 6, 3
            ],
            #vect: (89, 7, 0),
        },
        #value: 37
    };

    test_print("s.a.b.c: " + s.a.b.c);
    test_print("s.a.b.d: " + s.a.b.d);
    test_print("s.a.vect: " + s.a.vect);
    test_print("s.value: " + s.value);
    test_print("s.a.arr: (it)");
    for (i = 0; i < s.a.arr.size; i++) {
        test_print(i + " - " + s.a.arr[i]);
    }
    test_print("s.a.arr: (foreach simple)");
    foreach (e in s.a.arr) {
        test_print(e);
    }
    test_print("s.a.arr: (foreach double)");
    foreach (key, e in s.a.arr) {
        test_print(key + " - " + e);
    }


}

/*
 * Run level object tests
 */
function level_object() {
    test_print("level object");
    level.acts = {};
    level.acts.a = 0;
    level.acts.b = 1;
    level.acts.c = "hi";
    test_print("level object constant: " + level.acts.a + " " + level.acts.b + " " + level.acts.c);
    level.acts.a++;
    level.acts.b = level.acts.a;
    level.acts.b = 2;
    level.acts.c = level.acts.b;
    level.acts.a = level.acts.a;
    test_print("level object assign: " + level.acts.a + " " + level.acts.b + " " + level.acts.c);
    level.acts.s = {};
    level.acts.s.a = 0;
    level.acts.s.b = 1;
    level.acts.s.c = "hi";
    test_print("level object nested: " + level.acts.s.a + " " + level.acts.s.b + " " + level.acts.s.c);
    test_print("level object clear before: isdefined: level.acts.s: " + isdefined(level.acts.s));
    level.acts.s = undefined;
    test_print("level object clear after: isdefined: level.acts.s: " + isdefined(level.acts.s));
    level.acts.a = [];
    level.acts.a[0] = 0;
    level.acts.a[1] = 2;
    level.acts.a[2] = "bye";
    test_print("level object array index: size: " + level.acts.a.size + " val: " + level.acts.a[0] + " " + level.acts.a[1] + " " + level.acts.a[2]);
    level.acts.a[0]++;
    level.acts.a["foo"] = 5;
    level.acts.a["bar"] = 6;
    level.acts.a["baz"] = "no";
    test_print("level object array string: size: " + level.acts.a.size + " val: " + level.acts.a["foo"] + " " + level.acts.a["bar"] + " " + level.acts.a["baz"]);
    test_print("level object array index(after): size: " + level.acts.a.size + " val: " + level.acts.a[0] + " " + level.acts.a[1] + " " + level.acts.a[2]);
    test_print("level object array foreach");
    foreach (v in level.acts.a) {
        test_print(v);
    }
    test_print("level object array foreach end");
    level.acts.a[2] = undefined;
    level.acts.a["bar"] = undefined;
    test_print("level object array remove index: size: " + level.acts.a.size);
    foreach (v in level.acts.a) {
        test_print(v);
    }
    level.acts.a = undefined;
    test_print("level object array clear: isdefined: " + isdefined(level.acts.a));
}

/*
 * Run func calls tests
 */
function func_calls() {
    test_print("func calls");

    s = {
        #name: "self_obj"
    };

    test_print("func");
    fc_f1(1, 4, 5, 6);
    thread fc_f1(1, 4, 5, 6);
    childthread fc_f1(1, 4, 5, 6);

    test_print("method");
    s fc_f1(1, 4.2, 5, 6);
    s thread fc_f1(1, 4.2, 5, 6);
    s childthread fc_f1(1, 4.2, 5, 6);

    f1 = &fc_f1;
    test_print("pointer func");
    [[ f1 ]](1, 4.2, 5, 6);
    thread [[ f1 ]](1, 4.2, 5, 6);
    childthread [[ f1 ]](1, 4.2, 5, 6);
    test_print("pointer method");
    s [[ f1 ]](1, 4.2, 5, 6);
    s thread [[ f1 ]](1, 4.2, 5, 6);
    s childthread [[ f1 ]](1, 4.2, 5, 6);

    test_print("va func");
    fc_fva(1, #"test", "test va", 0);
    thread fc_fva(1, #"test", "test va", 0);
    childthread fc_fva(1, #"test", "test va", 0);

    test_print("va method");
    s fc_fva(1, #"test", "test va", 0);
    s thread fc_fva(1, #"test", "test va", 0);
    s childthread fc_fva(1, #"test", "test va", 0);

    test_print("def param func");
    fc_fdefparam(12, 50);
    thread fc_fdefparam(12, 50);
    childthread fc_fdefparam(12, 50);

    test_print("def param method");
    s fc_fdefparam(12, 50);
    s thread fc_fdefparam(12, 50);
    s childthread fc_fdefparam(12, 50);
    s fc_fdefparam(12, 12, 25);
    s thread fc_fdefparam(12, 12, 25);
    s childthread fc_fdefparam(12, 12, 25);

    test_print("6! = " + fc_rec_fact(6) + " / " + (6 * 5 * 4 * 3 * 2 * 1));
}

function fc_f1(a, b, c, d) {
    test_print((isdefined(self) && isdefined(self.name) ? self.name : "") + " fc_f1: a:" + a + ",b:" + b + ",c:" + c + ",d:" + d);
}

function fc_fva(...) {
    test_print((isdefined(self) && isdefined(self.name) ? self.name : "") + " fc_fva:");
    for (i = 0; i < vararg.size; i++) {
        test_print(i + " - " + vararg[i]);
    }
}

function fc_fdefparam(a, b, c = 22, d = 64) {
    test_print("fc_f1: a:" + a + ",b:" + b + ",c:" + c + ",d:" + d);
}

function fc_rec_fact(n) {
    if (n <= 1) {
        return 1;
    }
    return fc_rec_fact(n - 1) * n;
}

function notif_waiter() {
    level endon(#"acts_end_test");
    self endon(#"acts_test_notif_end");

    test_print("wait notif 1");

    self waittill(#"acts_test_notif1");

    test_print("received notif 1, wait 2");

    self waittill(#"acts_test_notif2");

    test_print("received notif 2");
    
    self waittill(#"acts_test_notif3");
    
    test_print("received notif 3");
}

function ev_notif_waiter() {
    level endon(#"acts_end_test");

    for (;;) {
        waitresult = self waittill(#"acts_test_notif1", #"acts_test_notif2", #"acts_test_notif3");

        if (!isdefined(waitresult._notify)) {
            test_print("no _notify event received");
            return;
        }
        if (!isdefined(waitresult.data)) {
            test_print("no data event received");
            return;
        }
        switch (waitresult._notify) {
        case #"acts_test_notif1":
            test_print("acts_test_notif1: " + waitresult.data);
            break;
        case #"acts_test_notif2":
            test_print("acts_test_notif2: " + waitresult.data);
            break;
        case #"acts_test_notif3":
            test_print("acts_test_notif3: " + waitresult.data);
            break;
        default:
            test_print("invalid receive: " + waitresult._notify);
            continue;
        }
    }
}

/*
 * Run notif tests
 */
function notif_tests() {
    test_print("-- notif");
    s = {};
    s thread notif_waiter();

    test_print("send notif 1");
    s notify(#"acts_test_notif1");

    waitframe(1);

    test_print("send notif 2");
    s notify(#"acts_test_notif2");

    waitframe(1);

    test_print("send notif end");
    s notify(#"acts_test_notif_end");

    s thread ev_notif_waiter();

    test_print("send notif 1");
    s notify(#"acts_test_notif1", {#data: "test val 1"});

    waitframe(1);

    test_print("send notif 2");
    s notify(#"acts_test_notif2", {#data: "test val 2"});

    waitframe(1);
    
    test_print("send notif end");
    s notify(#"acts_test_notif_end");

}