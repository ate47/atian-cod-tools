#namespace optimizer_test;

function vector_test() {
    a = (1, 0, 0);
    a = (1, -1, 0);
    a = (0, -2.2, 0);
    a = (1, 12, 13);
    a = (1, 0, a[2]);
    a = (1, 42, 0);
    a = (42, 42, 0);

    while (true) {
        wait 1;
    }
    while (false) {
        wait 2;
    }
    while (a) {
        wait 3;
    }

    do {
        wait 4;
    } while (true);
    do {
        wait 5;
    } while (false);
    do {
        wait 6;
    } while (a);

    if (true) {
        wait 7;
    }
    if (false) {
        wait 8;
    }
    if (12) {
        wait 9;
    } else {
        wait 10;
    }
    if (0) {
        wait 11;
    } else {
        wait 12;
    }
    if (a) {
        wait 13;
    } else {
        wait 14;
    }

    if (a - 2) {
        wait 15;
    }
    if (2 - 2) {
        wait 16;
    }
    if (4 - 2) {
        wait 17;
    }

    return 12;
}

function numbers_test() {
    a = [
        0,
        0x7F,
        -0x80,
        0xFF,
        0x7FFF,
        -0x8000,
        0xFFFF,
        0x7FFFFFFF,
        -0x80000000,
        0xFFFFFFFF,
        0x12345678901,
        0x7FFFFFFFFFFFFFFF,
        -0x8000000000000000,
    ];
}