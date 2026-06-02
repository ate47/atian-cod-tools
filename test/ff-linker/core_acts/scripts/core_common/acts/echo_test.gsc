#using scripts\core_common\array_shared;
#using scripts\core_common\system_shared;
#using scripts\core_common\callbacks_shared;
#using scripts\core_common\util_shared;
#using scripts\core_common\values_shared;

#insert scripts\core_common\acts\test_defs.gsh;

#ifndef ACTS_DEFS_TEST
// should be part of the gsh file
#error Missing ACTS_DEFS_TEST
#endif

#namespace echo_test;

#precache( "string", "ACTS/SPRAY_DESC" );
#precache( "eventstring", "acts_core_info_data" );

function private autoexec __init__system__()
{
    system::register( #"acts_echo_test", &__pre_init__, undefined );
    setgametypesetting( #"drafttime", 2 );
}

function private __pre_init__()
{
    if ( !util::is_frontend_map() )
    {
        callback::on_connect( &on_player_connect );
    }
    // test localization
    ActsLog( #"ACTS/TEST_LOCALIZED_KEY" );

    // test script bundles

    sbtest = GetScriptBundle( #"acts_test" );

    ActsLog( "test ScriptBundle" );
    ActsLog( "bundle: " + sbtest.type + ":" + sbtest.name );
    ActsLog( sbtest.xhash );
    ActsLog( sbtest.weapon.name );
    ActsLog( sbtest.number );
    ActsLog( sbtest.array[ 0 ].item );
    ActsLog( sbtest.array[ 0 ].test );
    ActsLog( "-----------------" );

    // print test
    [[ &ActsLog ]]( "test func ptr" );
    ActsLog( "ActsLog =", &ActsLog );

    // test detour
    ActsLog( "safehouse =", util::is_safehouse() );
    ActsLog( "is_safehouse(lz) =", @util<scripts\core_common\util_shared.gsc>::is_safehouse );
    ActsLog( "is_safehouse(it) =", &util::is_safehouse );
    ActsLog( "register(lz) =", @system<scripts\core_common\system_shared.gsc>::register );
    ActsLog( "register(it) =", &system::register );

    // test xhashscr operations
    ActsLog( ActsGetDetour( #"scripts/core_common/util_shared.gsc", #util, #is_safehouse ) );
    ActsLog( ActsGetDetour( "scripts/core_common/util_shared.gsc", "util", "is_safehouse" ) );

    // test string tables

    st = #"acts/test.csv";
    ActsLog( "test StringTable" );
    num_rows = TableLookupRowCount( st );
    for (i = 0; i < num_rows; i++)
    {
        sv = TableLookupColumnForRow( st, i, 0 );
        iv = TableLookupColumnForRow( st, i, 1 );
        hv = TableLookupColumnForRow( st, i, 2 );

        ActsLog( "[" + i + "] " + sv + "/" + iv + "/" + hv );
    }
    ActsLog( "-----------------" );

    
    ActsLog( "test hud element" );

    hud = NewDebugHudElem();
    hud2 = NewDebugHudElem();
    hud3 = NewDebugHudElem();
    ActsLog( "hud", hud );
    ActsLog( "hud", hud2 );
    ActsLog( "hud", hud3 );
    hud.x = 42;
    hud.y = 15;
    hud.fontScale = 1;
    hud.color = ( 1, 0.2, 1 );
    hud.alpha = 0.8;
    hud2.x = 65;
    hud2.y = 25;
    ActsLog( "hud", hud );
    ActsLog( "hud", hud2 );
    ActsLog( "info", hud2.x, hud2.y, hud.fontScale, hud.color, hud.alpha );
    //hud destroy();
    ActsLog( "post hud destroy" );
}

function private on_player_connect()
{
    // ingame test
    self endon( #"disconnect", #"bled_out" );
    level endon( #"end_game", #"game_ended" );

    self val::set(#"acts_core", "ignoreme", true);

    if (!self ishost())
    {
        return; // we control that with the dvars so it's useless to give them to anyone else
    }

    self childthread max_ammo();
    self childthread gode_mode();
    self childthread fly_mode();
    self childthread player_cmd_think();
    self childthread info_runner();
}

function private player_cmd_think()
{
    setdvar( #"ac_score", 0 );
    setdvar( #"ac_tp", "" );
    setdvar( #"ac_fly", false );

    for (;;)
    {
        wait(1);

        score = getdvarint( #"ac_score", 0 );
        tp = getdvarstring( #"ac_tp", "" );
        self.acts_core_fly = getdvarint( #"ac_fly", false );

        if ( score > 0 )
        {
            self.score = score;
            self iprintlnbold( "Set score to " + score );
            setdvar( #"ac_score", 0 );
        }

        if ( tp != "" )
        {
            setdvar( #"ac_tp", "" );
            args = strtok( tp, " " );

            newOrigin = ( float( args[0] ), float( args[1] ), float( args[2] ) );
            
            if ( args.size != 3 )
            {
                self iprintlnbold( "Can't tp to " + tp );
            }
            else 
            {
                self iprintlnbold( "tp to " + tp );
                if ( isdefined( self.originObj ))
                {
                    self.originObj.futureOrigin = newOrigin;
                }
                else
                {
                    self setorigin( newOrigin );
                }
            }
        }
    }
}

function private max_ammo()
{
    for (;;)
    {
        self waittill( #"weapon_fired", #"grenade_fire", #"missile_fire", #"weapon_change", #"melee" );

        weapon = self getcurrentweapon();
        offhand = self getcurrentoffhand();

        if ( isdefined( weapon ) && weapon != level.weaponnone ) 
        {
            if ( isdefined( weapon.clipsize ) && weapon.clipsize > 0 )
            {
                self setweaponammoclip( weapon, 9999 );
            }
            self givemaxammo( weapon );
        }

        if ( isdefined( offhand ) && offhand != level.weaponnone )
        {
            if ( isdefined( offhand.clipsize ) && offhand.clipsize > 0 )
            {
                self setweaponammoclip( offhand, 9999 );
            }
            self givemaxammo( offhand );
        }

        if ( isdefined(self.acts_core_loc) && self.acts_core_loc )
        {
            self iprintlnbold( "Player location : " + self.origin );
        }

        self gadgetpowerset( 2, 100 );
        self gadgetpowerset( 1, 100 );
        self gadgetpowerset( 0, 100 );
    }
}


function private fly_mode()
{
	self unlink();
    if( isdefined(self.originObj) )
    {
        self.originObj delete();
        self.originObj = undefined;
    }
	for (;;)
    {
		if ( isdefined(self.acts_core_fly) && self.acts_core_fly ) {
			self.originObj = spawn( "script_origin", self.origin, 1 );
    		self.originObj.angles = self.angles;
			self playerlinkto( self.originObj, undefined );

			for (;;)
            {
				if (!self.acts_core_fly) 
                {
					self iprintlnbold( "^6Fly mode ^1disabled" );
					break;
				}

				if ( isdefined( self.originObj.futureOrigin ) )
                {
					self.originObj.origin = self.originObj.futureOrigin;
					self.originObj.futureOrigin = undefined;
					waitframe(1);
					continue;
				}
				
				fly_speed = self sprintbuttonpressed() ? 60 : 20;
				player_angles = self getplayerangles();

				// I'm too tired to remember my vector courses
				front_vector = anglestoforward( player_angles );
				left_vector = anglestoforward( player_angles - (0, 90, 0) );
				top_vector = anglestoforward( player_angles - (90, 0, 0) );

				v_movement = self getNormalizedMovement();

				if (self jumpbuttonpressed()) {
					z_movement = 1;
				} else if (self stancebuttonpressed()) {
					z_movement = -1;
				} else {
					z_movement = 0;
				}

				move_vector = 
					// add z angle
						z_movement * top_vector 
					// add front movement
					+ front_vector * v_movement[0] 
					// remove left/right z vector part because it was weird
					+ (left_vector[0], left_vector[1], 0) * v_movement[1];
				move_vector_scaled = vectorscale(move_vector, fly_speed);

				originpos = self.origin + move_vector_scaled;
				self.originObj.origin = originpos;

				waitframe(1);
			}
			self unlink();
			if (isdefined(self.originObj)) {
                self.originObj delete();
                self.originObj = undefined;
            }
			waitframe(1);
		}
		waitframe(1);
	}
}

function private gode_mode()
{
    self notify(#"stop_player_out_of_playable_area_monitor");

    for (;;)
    {
        waitframe(1);
        self freezecontrols( false );
        self enableInvulnerability();
		self enableweapons();
        self.var_f22c83f5 = true;
        self val::set( #"acts_core", "disable_oob", true );
    }
}


function get_look_trace()
{
    tag_origin = self geteye();
    look = anglestoforward( self getplayerangles() );
    return bullettrace( tag_origin, tag_origin + vectorscale(look, 10000), 1, self );
}

function private info_runner()
{
    for (;;)
    {
        wait(0.5);
        trace = self get_look_trace();

        msg = "loc: " + self.origin;
        ent = trace[#"entity"];
        if ( isdefined( ent ) )
        {
            msg += " e:" + ( isdefined( ent.model ) ? ent.model : "true" );
        }

        setdvar( #"ac_plr_nfo", msg );
        self luinotifyevent(#"acts_core_info_data", 1, 0);
    }
}

detour util<scripts\core_common\util_shared>::is_safehouse()
{
    return 42;
}