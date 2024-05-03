#namespace variables;

function variables(u = 42, e, zz = { #e : 42 }, &b, ...) {
    zz = self;
    i = 2;
    z = 4;

    c = 0;
    for (i = 0; i < vararg.size; i++) {
        c += vararg[i] * u;
    }
    evt = #"dead";
    self waittillmatchtimeout(evt, {#ev : "test"}, 64);

    funcref = &variables;
    test = [[ ([[ funcref ]]())[42].("test").z ]](vararg.size);

    val = {
        #a: "42",
        #z: [ 0, 1, 2, 3, 82:4 ],
    }.("z")[[1, 2, 3, 4][2]];
    i = i + 42;
    i += 874456;
    z -= 42 + val.z[2];
    i *= -44262;
    i /= z.("test" + 45 + z.("42").y).size * 2;
    i <<= i[42 + 23].test[67];
    i++;
    i--;
    z.y.u.a.b.c.d = 32;
    val[42] = 16;
    (i.y.u[16].a.b + 52)[65].c.d.("zdq") = 64;
    u = isdefined(zz) ? i : z + level.y + memory[42] + memory.("456") + memory.yyy + memory.zzz + level.r;

    for (i = 0; i < 42; i++) {
        wait 1;
    }

    return c;
}