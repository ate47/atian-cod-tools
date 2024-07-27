function init_menus() {
    self add_menu("tool_menu", "Tools", "start_menu", true);

    self add_menu_item_modswitch("tool_menu", "Fly", "fly");
    self add_menu_item_modswitch("tool_menu", "Ammos", "maxammo");

#ifdef _T9
    self add_menu("t9_ptr", "Test pointer", "start_menu", true);
    self add_menu_item("t9_ptr", "Test ptr", function (item) {
        test = 11;
        self iprintln("1: " + test);
        test_change(&test);
        self iprintln("2: " + test);
        test = 22;
        self iprintln("3: " + test);
        /*
            1: 11
            2: 44
            3: 22
         */
    });

    self add_menu_item("t9_ptr", "Test ptr 2", function (item) {
        test = 11;
        self iprintln("1: " + test);
        test2 = &test;
        test_change(test2);
        self iprintln("2: " + test);
        test3 = test2;
        test_change(test3);
        self iprintln("3: " + test);
        /*
            1: 11
            2: 11
            3: 11
         */
    });

    self add_menu_item("t9_ptr", "Test ptr 3", function (item) {
        test = 11;
        self iprintln("1: " + test);
        test2 = &test;
        self iprintln("2: " + test2);
        test = 44;
        self iprintln("3: " + test2);
        test3 = test2;
        self iprintln("4: " + test3);
        /*
            1: 11
         */
    });

    self add_menu_item("t9_ptr", "Test ptr 4", function (item) {
        level.acts = {
            #test: 11,
        };

        self iprintln("1: " + level.acts.test);
        test_change(&level.acts.test);
        self iprintln("2: " + level.acts.test);
        obj = level.acts;
        test_change(&obj.test);
        self iprintln("3: " + level.acts.test);
        /*
            1: 11
            2: 11
            3: 11
         */
    });
#endif
}

#ifdef _T9
function test_change(*val) {
    val = 44;
}
#endif