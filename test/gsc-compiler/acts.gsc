#using scripts\core_common\array_shared.gsc;
#using scripts\core_common\system_shared.gsc;
#using scripts\core_common\callbacks_shared.gsc;

#namespace acts;

function autoexec __init__system__() {
    if (is_mp()) {
        setgametypesetting(#"drafttime", 2);
    }
    system::register(#"acts", &__pre_init__, undefined, undefined);
}

function __pre_init__() {
    callback::on_connect(&on_player_connect);

    level.acts = {
        #counter: 0

    };
}

function on_player_connect() {
    self endon(#"disconnect", #"bled_out");
    level endon(#"end_game", #"game_ended");


    if (!self ishost()) {
        return;
    }

    wait 20;

    i = 0;
test:
    i++;
    self.score = 20000;
    level.acts.counter++;
    if (self usebuttonpressed()) {
        


    } else if (self adsbuttonpressed()) {
        self iprintlnbold(
            "vector: " + vectorscale((i, 0, 2), 24)
            + "/" + i
        );
    } else {
        self iprintlnbold(
            "test: #" + level.acts.counter  + "/" + i
        );
    }
    wait 1;
    goto test;
}


function is_wz() {
    return sessionmodeiswarzonegame();
}

function is_zm() {
    return sessionmodeiszombiesgame();
}

function is_mp() {
    return sessionmodeismultiplayergame();
}

/#
function devblocktest() {
    a = 42 * 3;
    /#
        return "test" + a;    
    #/
}

function devblocktest2() {

    
}
#/

detour test<scripts\acts\test.gsc>::hello detoured() {
    return false;
}

// useless, but funny
() {
    detoured();
    // test nameless function
    test = "aaa";

    zqdzqd = @test<script\acts\test.gsc>::hello;
    zqd = @test<script\acts\test.gsc>::hello2;
    zdz = @test<script\acts\test.gsc>::hello;
    zqdqd = @test<script\acts\test.gsc>::hello2;
}

// ok I explain: to avoid having to add to much to the grammar, I consider a normal detour as a function without a name
// but with a detour modifier, so it gives automatically a fake name. It adds 2 useless features, nameless function and
// detour calls.