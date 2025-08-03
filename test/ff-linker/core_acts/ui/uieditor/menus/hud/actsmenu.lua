Acts.@Log( "Hello from map  " .. Engine.@GetCurrentMap() .. " inject? " .. tostring(LUI.createMenu["T7Hud_" .. Engine.@GetCurrentMap()] ~= nil) )

-- Should be injected after ui/uieditor/menus/hud/t7hud_zm_factory.lua

LUI.createMenu[ "T7Hud_" .. Engine.@GetCurrentMap() ] = function ( controller ) 
    local self = LUI.createMenu.T7Hud_zm_factory( controller )
    Acts.@Log( "Injecting custom menu" )
    self.luatest = {}


    local text = LUI.UIText.new( 0.5, 1, 0, -6, 0, 0, 6, 46 )

    self.luatest.text = text
    text:setText("test string")
    text:setTTF( "cascadiacode_bold" )
    text:setAlignment( Enum.@LuiAlignment.@LUI_ALIGNMENT_RIGHT )
    text:setAlignment( Enum.@LuiAlignment.@LUI_ALIGNMENT_TOP )
    self:addElement(text)
      
    return self
end
