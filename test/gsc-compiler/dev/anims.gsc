#namespace anims;

// test, but no direct usage

function test()
{
    level endon(#test);
    at2 = #test2;
    at = $test_anim_tree;
    at = $test_anim_tree;
    anim1 = %test_anim_tree::test;
    anim2 = %test_anim_tree::test2;
    anim1 = %test_anim_tree::test;
    anim2 = %test_anim_tree::test2;
}

function test2()
{
    at = $test_anim_tree2;
    at = $test_anim_tree2;
    anim1 = %test_anim_tree2::test;
    anim2 = %test_anim_tree2::test2;
    anim1 = %test_anim_tree2::test;
    anim2 = %test_anim_tree::test2;
}


#using_animtree( "test_anim_tree3" );

function test3()
{
    at = #animtree;
    an = %test_anim_tree;
}