function init_menus() {
    self add_menu("tool_menu", "Tools", "start_menu", true);

    self add_menu_item_modswitch("tool_menu", "Fly", "fly");
    self add_menu_item_modswitch("tool_menu", "Ammos", "maxammo");

#ifdef _T9
    level.test = 0;

    test_ref = &level.test;
#endif
}