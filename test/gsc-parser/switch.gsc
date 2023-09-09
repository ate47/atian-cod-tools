p() {
    switch (test_thing) {
        case 1:
            return 2;
        case 3:
        case 4:
            break;
        default:
            return 5;
    }
}