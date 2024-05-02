#namespace functiontest;

function functiontest() {
    self endon(#"dead");
    self endoncallback(#"dead", &functiontestcallback);
    waittillframeend();
    waitframe(1);
    self waittill(#"dead");
    self waittilltimeout(#"dead", 42);
    self waittillmatch(#"dead", spawnstruct());
    self waittillmatchtimeout(#"dead", spawnstruct(), 64);
    test0(self waittill(#"dead"));
    test0(self waittilltimeout(#"dead", 42));
    test0(self waittillmatch(#"dead", spawnstruct()));
    test0(self waittillmatchtimeout(#"dead", spawnstruct(), 64));
    wait(65.2);
    test0(vectorscale((42, undefined, 0), 2));
    self thread test1(2, 3, 4, 5);
    self test2();
    self childthread test3(2, 3, 4, 5);
    thread atiancodtools::test4(2, 3, 4, 5);
    self atiancodtools::test5(2, 3, 4, 5);
    childthread atiancodtools::test6(2, 3, 4, 5);
    [[ @atiancodtools<scripts\acts\test.gsc>::test6 ]](2, 3, 4, 5);
    thread [[ @atiancodtools<scripts\acts\test.gsc>::test6 ]](2, 3, 4, 5);
    childthread [[ @atiancodtools<scripts\acts\test.gsc>::test6 ]](2, 3, 4, 5);
    [[ undefined ]]->test7(2, 3, 4, 5);
    thread [[ undefined ]]->test7(2, 3, 4, 5);
    childthread [[ undefined ]]->test7(2, 3, 4, 5);
}

function functiontestcallback() {
}