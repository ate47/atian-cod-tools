
#namespace fastfile0_utils;

function test()
{
    return true;
}

function add( a, b )
{
    return a + b;
}

/#
    function dev_call_test()
    {
        return 42;
    }

    function dev_call_logic()
    {
        iprintln("I like trains");
        e = 2 + 2;
        self.val = dev_call_test();

        return 12;
    }

#/