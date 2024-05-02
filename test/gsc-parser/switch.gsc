p() {
    if (3) {
        return #"test";
        return;
    }
    switch (4) {
        case 1:
            return 2;
        case 3:
        case 4:
            break;

        case 5:
            wait 42;
        default:
            return 5;
    }
    return "ok";
}