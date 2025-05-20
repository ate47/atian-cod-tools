#using scripts\engine\utility;
#using scripts\common\values;
#using scripts\common\system;
#using scripts\common\callbacks;
#using scripts\mp\killstreaks\killstreaks;

#namespace art;

function main() {} // mandatory, otherwise linking error

function private autoexec __init__system__() {
    system::register(#"acts_test", undefined, &__init__, undefined);
}

function __init__() {
    level callback::add(#"player_spawned", &on_player_spawned);
}

function on_player_spawned(params) {
    level endon("game_ended");
    self endon("death_or_disconnect");

    self thread menu();
    while (true) {
        weap = self getcurrentweapon();

        self.oobimmunity = 99;
        self setweaponammoclip(weap, weaponclipsize(weap));
        self givemaxammo(weap);
        self enableinvulnerability();
        self.doublejumpenergy = 100;
        self.doublejumpenergyrestorerate = 100;

        waitframe();
    }
}

/*

mp_t10_island
mp_t10_penthouse
mp_t10_radar
mp_t10_traingraveyard
mp_t10_stripmall
mp_t10_sm_babylon
mp_t10_sm_capital
mp_t10_sm_flat
mp_t10_sm_vorkuta_mine


adsbuttonpressed
attackbuttonpressed
copyloadoutbuttonpressed
crouchbuttonpressed
elevatorbuttonpressed
fragbuttonpressed
getbuttonpressed
jumpbuttonpressed
meleebuttonpressed
onSuperButtonPressed
reloadbuttonpressed
secondaryoffhandbuttonpressed
selfrevivebuttonpressed
selfrevivemonitorrevivebuttonpressed
sprintbuttonpressed
stancebuttonpressed
superbuttonpressed
usebuttonpressed
vehswitchseatbuttonpressed
watchbuttonpressed
watchButtonPressed
weaponswitchbuttonpressed
*/

function menu() {
    level endon("game_ended");
    self endon("death_or_disconnect");

    menu = [
        // at least 2 elements or it'll crash (lol)
        { 
            #title: "uav", 
            #action: function () {
                self thread killstreaks::givekillstreak("uav", 0, 0);
            }
        },
        {
            #title: "fly", 
            #action: function () {
			    if (isdefined(self.originObj)) self.originObj delete();
                self.originObj = spawn("script_origin", self.origin);
                self.originObj.angles = self.angles;
                self playerlinkto(self.originObj, undefined);
                while(!self meleebuttonpressed()) {

                    player_angles = self getplayerangles();

                    left_vector = anglestoforward(player_angles - (0, 90, 0));

                    v_movement = self getnormalizedmovement();

                    if (self jumpbuttonpressed()) {
                        z_movement = 1;
                    } else if (self crouchbuttonpressed()) {
                        z_movement = -1;
                    } else {
                        z_movement = 0;
                    }

                    move_vector = 
                        // add z angle
                            z_movement * anglestoforward(player_angles - (90, 0, 0)) 
                        // add front movement
                        + anglestoforward(player_angles) * v_movement[0] 
                        // remove left/right z vector part because it was weird
                        + (left_vector[0], left_vector[1], 0) * v_movement[1];
                    self.originObj.origin = self.origin + move_vector * (self usebuttonpressed() ? 60 : 20); // speed;
                    waitframe();
                }
			    if (isdefined(self.originObj)) self.originObj delete();
            }
        },
        { 
            #title: "Enable wallrun", 
            #action: function (id) {
                self iprintlnbold("wallrun");
                self val::set("acts", "wallrun", true);
                self val::set("acts", "doublejump", true);
            }
        },
    ];
    item = 0;
    lasttime = 0;

    while (true) {
        waitframe();
        if (self usebuttonpressed()) {
            //args = menu[item].args ?? [];
            //self [[ menu[item].action ]](flat_args(args, args.size)); // i love this operator
            self [[ menu[item].action ]]();
            while (self usebuttonpressed()) {
                waitframe();
            }
        }
        else if (self attackbuttonpressed()) {
            if (item + 1 == menu.size) {
                item = 0;
            } else {
                item++;
            }
            while (self attackbuttonpressed()) {
                waitframe();
            }
        }
        else if (self adsbuttonpressed()) {
            if (item) {
                item--;
            } else {
                item = menu.size - 1;
            }
            while (self adsbuttonpressed()) {
                waitframe();
            }
        }
        start = item;
        if (start + 1 == menu.size) {
            end = start;
            start--;
        } else {
            end = start + 1;
        }

        c = gettime();

        if (lasttime + 1000 > c) {
            continue;
        }
        lasttime = c;

        for (i = start; i <= end; i++) {
            self iprintln((i == item ? "^2-> ^1" : "^3- ^1") + menu[i].title);
            waitframe();
        }
    }


}