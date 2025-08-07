Acts.@Log( "Load lua test for ACTS " .. Acts.@GetVersion() )

Acts.@Log( "acts_test_hash = " .. tostring( Acts.@HashLookup( @"acts_test_hash" ) ) )
Acts.@Log( "acts_test_hash2 = " .. tostring( Acts.@HashLookup( @"acts_test_hash2" ) ) )

#ifdef LUA_TEST_FAIL

--stack traceback:
--	lua/acts/test.lua:6: in function '(*stripped)'
--	lua/acts/test.lua:4: in function '(*stripped)'
--	lua/acts/test.lua:52: in function '(*stripped)'
--	[C]: in function 'require'
--	[C]: in function 'Lua loader'
--	[C]: in function 'require'

local obj = {}

function obj.test_acts2 ()
      Acts.@Log( "test fail " .. nil )
      return obj
end
function obj.test_acts ()
      obj.test_acts2()
end
obj.test_acts()

#endif