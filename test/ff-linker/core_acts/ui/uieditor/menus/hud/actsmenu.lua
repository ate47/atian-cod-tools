
--[[
Prototype and notes for LUA

Engine.PrintInfo(0, "Fuck lua") -> PrintInfo should be XHASH instead of STRING

-- Engine[@"func"]
-- Enum[@"name"]
-- Enum[@"name"][@"name2"]
-- CoD[@"hash_8A9D1228888C8CB"]
-- CoD.BlackMarketTableUtility[@"name"]
-- Dvar[@"name"]


Hooks:
x64:2cfcd4cb80781b03.lua -> 27cac000f20ef647 (PC version)
x64:12dd4ff313102b79.lua -> 4a3eb94d551ddf71 (Frontend)
x64:66eee5926f63b60b.lua -> 4ed0bcba9d999e0f (Frontend 2)
x64:2cb5bf4e094259d8.lua -> 46f417a74d9ab424 (Zombies)
x64:8f9e828ebdeac4c.lua  -> 7f750379a77f3190 (Warzone)
x64:5be322a54ff91a9a.lua -> 4fb12287640e989a (Multiplayer/Warzone)


Hud factories:
ui/uieditor/menus/hud/t7hud_zm_factory.lua (Zombies / T7Hud_zm_factory) (2b23cf5ef446c848)
57d27ab8e2f7a9d0 (Others / Hud)

619241173 = Failed to allocate from element pool

LUI.UI????.new(leftAnchor, rightAnchor, left, right, topAnchor, bottomAnchor, top, bottom)

The hooks for blackout and the multiplayer is probably in one these files (dump required)
elseif CoD.isWarzone then
      require( "x64:8f9e828ebdeac4c" )
else
      require( "x64:eb491e34e330ae0" )
      require( "x64:6b7fc557f641386" )
      require( "x64:2f1b9c001eb57aa3" )
      require( "x64:37c614e146b5eea5" )
]]


Acts.@Log( "Hello from map  " .. Engine.@GetCurrentMap() .. " inject? " .. tostring(LUI.createMenu["T7Hud_" .. Engine.@GetCurrentMap()] ~= nil) )

-- Should be injected after ui/uieditor/menus/hud/t7hud_zm_factory.lua

LUI.createMenu[ "T7Hud_" .. Engine.@GetCurrentMap() ] = function ( controller ) 
    local self = LUI.createMenu.T7Hud_zm_factory( controller )
    self.luatest = {}
    
#ifdef LUA_DEV
#warning test preprocessor
#endif

    Acts.@Log( "Injecting custom menu" )

    local text = LUI.UIText.new( 0.5, 1, 0, -6, 0, 0, 6, 46 )

    self.luatest.text = text
    text:setText("test string")
    text:setTTF( "cascadiacode_bold" )
    text:setAlignment( Enum.@LuiAlignment.@LUI_ALIGNMENT_RIGHT )
    text:setAlignment( Enum.@LuiAlignment.@LUI_ALIGNMENT_TOP )
    self:addElement(text)
      
    return self
end
