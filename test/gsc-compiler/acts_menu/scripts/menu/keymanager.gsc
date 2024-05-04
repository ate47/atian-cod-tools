function key_mgr_init() {
    if (isdefined(self.key_mgr)) {
        // ignore menu creation if already set
        return;
    }
    key_mgr = 
    {
        #key_config: spawnstruct(),
        #config: array(),
        #valid: array("action", "actionslotfour", "actionslotone", "actionslotthree", "actionslottwo", "ads", "attack", 
        "changeseat", "frag", "jump", "melee", "offhandspecial", "reload", "secondaryoffhand", "sprint", 
        "stance", "throw", "use", "vehicleattack", "vehiclemoveup", "weaponswitch")
    };

    self.key_mgr = key_mgr;

    // load the config
    key_config = key_mgr.key_config;
    key_config AtianMenuKeyConfig();

    self key_mgr_compile_key(#"open_menu", key_config.menu_open, array(#"ads", #"melee"));
    self key_mgr_compile_key(#"parent_page", key_config.parent_page, #"melee");
    self key_mgr_compile_key(#"last_item", key_config.last_item, #"ads");
    self key_mgr_compile_key(#"next_item", key_config.next_item, #"attack");
    self key_mgr_compile_key(#"select_item", key_config.select_item, #"use");
    self key_mgr_compile_key(#"fly_fast_key", key_config.fly_fast_key, #"sprint");
    self key_mgr_compile_key(#"fly_up_key", key_config.fly_up_key, #"jump");
    self key_mgr_compile_key(#"fly_down_key", key_config.fly_down_key, #"stance");
    self key_mgr_compile_key(#"special_weapon_primary", key_config.special_weapon_primary, #"attack");
    self key_mgr_compile_key(#"special_weapon_secondary", key_config.special_weapon_secondary, #"reload");
    self key_mgr_compile_key(#"special_weapon_ternary", key_config.special_weapon_ternary, #"use");
}

function key_mgr_is_valid(key) {
    key_mgr_init();
    return array::contains(self.key_mgr.valid, key);
}

function key_mgr_compile_key(id, config, default_config) {
    if (!isdefined(config)) {
        // no config, use default
        // force array
        if (!IsArray(default_config)) {
            default_config = array(default_config);
        }
        self.key_mgr.config[id] = default_config;
        return;
    }

    cfg_split = strtok(config, "+");

    cfg = array();

    for (i = 0; i < cfg_split.size; i++) {
        if (self key_mgr_is_valid(cfg_split[i])) {
            array::add(cfg, hash(cfg_split[i]));
        }
    }
    
    self.key_mgr.config[id] = cfg;
}

function key_mgr_get_key_str(id) {
    key_mgr_init();

    if (!isdefined(self.key_mgr.config[id])) {
        return; // bad config
    }
    
    key_cfg = self.key_mgr.config[id];
    if (key_cfg.size == 0) {
        return "";
    }

    s = key_mgr_get_key_str_id(key_cfg[0]);

    for (i = 1; i < key_cfg.size; i++) {
        s += "+" + key_mgr_get_key_str_id(key_cfg[i]);
    }

    return s;
}
function key_mgr_has_key_pressed(id, wait_release = false) {
    key_mgr_init();

    if (!isdefined(self.key_mgr.config[id])) {
        return; // bad config
    }
    
    key_cfg = self.key_mgr.config[id];

    for (i = 0; i < key_cfg.size; i++) {
        if (!self key_mgr_has_key_pressed_id(key_cfg[i])) {
            return false;
        }
    }
    if (!isdefined(wait_release) || !wait_release) {
        return true;
    }
    
    for (;;) {
        we_continue = false;
        for (i = 0; i < key_cfg.size; i++) {
            if (self key_mgr_has_key_pressed_id(key_cfg[i])) {
                we_continue = true;
            }
        }
        // wait for all the keys
        if (!we_continue) {
            break;
        }
        waitframe(1);
    }
    return true;
}

function key_mgr_get_key_str_id(id) {
    switch (id) {
        case #"action": return "[{+action}]";
        case #"actionslotfour": return "[{+actionslot 4}]";
        case #"actionslotone": return "[{+actionslot 1}]";
        case #"actionslotthree": return "[{+actionslot 3}]";
        case #"actionslottwo": return "[{+actionslot 2}]";
        case #"ads": return "[{+ads}]";
        case #"attack": return "[{+attack}]";
        case #"changeseat": return "[{+switchseat}]";
        case #"frag": return "[{+frag}]";
        case #"jump": return "[{+gostand}]";
        case #"melee": return "[{+melee}]";
        case #"offhandspecial": return "[{+offhandspecial}]";
        case #"reload": return "[{+reload}]";
        case #"secondaryoffhand": return "[{+smoke}]";
        case #"sprint": return "[{+sprint}]";
        case #"stance": return "[{+stance}]";
        case #"throw": return "[{+frag}]";
        case #"use": return "[{+use}]";
        case #"vehicleattack": return "[{+vehicleattack}]";
        case #"vehiclemoveup": return "[{+vehiclemoveup}]";
        case #"weaponswitch": return "[{+weapnext_inventory}]";
        default: return "??";
    }
}
function key_mgr_has_key_pressed_id(id) {
    switch (id) {
        case #"action":
            return self actionbuttonpressed();
        case #"actionslotfour":
            return self actionslotfourbuttonpressed();
        case #"actionslotone":
            return self actionslotonebuttonpressed();
        case #"actionslotthree":
            return self actionslotthreebuttonpressed();
        case #"actionslottwo":
            return self actionslottwobuttonpressed();
        case #"ads":
            return self adsbuttonpressed();
        case #"attack":
            return self attackbuttonpressed();
        case #"changeseat":
            return self changeseatbuttonpressed();
        case #"frag":
            return self fragbuttonpressed();
        case #"jump":
            return self jumpbuttonpressed();
        case #"melee":
            return self meleebuttonpressed();
        case #"offhandspecial":
            return self offhandspecialbuttonpressed();
        case #"reload":
            return self reloadbuttonpressed();
        case #"secondaryoffhand":
            return self secondaryoffhandbuttonpressed();
        case #"sprint":
            return self sprintbuttonpressed();
        case #"stance":
            return self stancebuttonpressed();
        case #"throw":
            return self throwbuttonpressed();
        case #"use":
            return self usebuttonpressed();
        case #"vehicleattack":
            return self vehicleattackbuttonpressed();
        case #"vehiclemoveup":
            return self vehiclemoveupbuttonpressed();
        case #"weaponswitch":
            return self weaponswitchbuttonpressed();
        default:
            return false;
    }
}