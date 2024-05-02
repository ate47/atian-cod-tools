
function control() {
    if (true) if (2) {
        return 1111111111;
    } else {
        return 4444444444;
    } else if (2) {
        return 2222222222;
    } else {
        return 3333333333;
    }
    while (isdefined(64)) {
        wait 2;
    }
    do {
        if (32) {
            continue;
        }
        wait 2;
        if (64) {
            break;
        }
    } while (isdefined(64));
    for (; 2 < 42; ) {
        wait 2;
    }
    for (;;) {
        if (32) {
            continue;
        }
        
        if (0) {
            switch (2) {
            default:
                continue;
            }
        }

        foreach (e in []) {
            test(e, 42);
        }
        
        foreach (e, z in []) {
            test(e + z, 42);
        }

    }
    if (1) {
        break;
    }
    return 52;
}
