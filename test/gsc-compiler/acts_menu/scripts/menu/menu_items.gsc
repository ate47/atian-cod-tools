function init_menus() {
    profilestart();
    self add_menu("tool_menu", "Tools", "start_menu", true);

    self add_menu_item_modswitch("tool_menu", "Fly", "fly");
    self add_menu_item_modswitch("tool_menu", "Ammos", "maxammo");

    foreach (e in [1, 2, 3, 4, 5]) {
        self iprintlnbold(e);
    }
    profilestop();
}