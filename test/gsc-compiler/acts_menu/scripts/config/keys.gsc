/*
 * Atian Menu Keys configurations
 *
 * These configurations are used to control the keys inside the menu.
 *
 */
function AtianMenuKeyConfig() {
    // key config, options:
    // "action", "actionslotfour", "actionslotone", "actionslotthree", "actionslottwo", "ads", "attack", 
    // "changeseat", "frag", "jump", "melee", "offhandspecial", "reload", "secondaryoffhand", "sprint", 
    // "stance", "throw", "use", "vehicleattack", "vehiclemoveup", "weaponswitch"

    // you can use self.opt = "your_key"; to use only one key 
    // or you can use + to use multiple key: self.opt = "your_key1+your_key2"

    // Open the menu, default "ads+melee"
    self.menu_open = "ads+melee";

    // Get back to the previous menu, close the menu if none, default "melee"
    self.parent_page = "melee";

    // last item in the menu, default "ads"
    self.last_item = "ads";

    // next item in the menu, default "attack"
    self.next_item = "attack";

    // select an item, default "use"
    self.select_item = "reload"; // use

    // fly in fly mode, cancel fly in rocketman mode, default "sprint"
    self.fly_fast_key = "sprint";
    self.fly_up_key = "jump";
    self.fly_down_key = "stance";

    // special tool in the menu
    self.special_weapon_primary = "attack";
    self.special_weapon_secondary = "reload";
    self.special_weapon_ternary = "use";
}
