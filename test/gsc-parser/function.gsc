function functiontest() {
    self endon(#"dead");

    self thread test(2, 3, 4, 5);
    self test();
    self childthread test(2, 3, 4, 5);
    thread atiancodtools::test(2, 3, 4, 5);
    self atiancodtools::test(2, 3, 4, 5);
    childthread atiancodtools::test(2, 3, 4, 5);
    childthread [[ test ]](2, 3, 4, 5);
    thread [[ test ]]->test(2, 3, 4, 5);
}