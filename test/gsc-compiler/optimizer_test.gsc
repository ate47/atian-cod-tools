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