-- if the MenuTimer is enabled, we should reset SSM's MenuTimer now that we've reached Gameplay
if PREFSMAN:GetPreference("MenuTimer") then
	SL.Global.MenuTimer.ScreenSelectMusic = ThemePrefs.Get("ScreenSelectMusicMenuTimer")
end

local Players = GAMESTATE:GetHumanPlayers()
local t = Def.ActorFrame{ Name="GameplayUnderlay" }

-- underlay stuff like BackgroundFilter, ColumnFlash, and MeasureCounter
for player in ivalues(Players) do
	t[#t+1] = LoadActor("./PerPlayer/Danger.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/Filter.lua", player)
end

-- semi-transparent quad at the top of ScreenGameplay
t[#t+1] = LoadActor("./Shared/Header.lua")

-- Song title and progress bar for how much song remains
t[#t+1] = LoadActor("./Shared/SongInfoBar.lua")

-- More per-player stuff
for player in ivalues(Players) do
	t[#t+1] = LoadActor("./PerPlayer/Score.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/LifeMeter/default.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/ColumnFlashOnMiss.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/MeasureCounter.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/DifficultyMeter.lua", player)
	t[#t+1] = LoadActor("./PerPlayer/TargetScore/default.lua", player)
end


if GAMESTATE:IsPlayerEnabled(PLAYER_1) and GAMESTATE:IsPlayerEnabled(PLAYER_2) then
	t[#t+1] = LoadActor("./Shared/WhoIsCurrentlyWinning.lua")
end

t[#t+1] = LoadActor("./Shared/BPMDisplay.lua")

--Player column seperators
t[#t+1] = Def.Quad{InitCommand=cmd(diffusealpha,0;xy,_screen.w*0.25,_screen.h*0.6;zoomto,_screen.w*0.015,_screen.h*0.867;linear,1;diffuse,0,0,0,0.20)}
t[#t+1] = Def.Quad{InitCommand=cmd(diffusealpha,0;xy,_screen.w*0.5,_screen.h*0.6;zoomto,_screen.w*0.015,_screen.h*0.867;linear,1;diffuse,0,0,0,0.20)}
t[#t+1] = Def.Quad{InitCommand=cmd(diffusealpha,0;xy,_screen.w*0.75,_screen.h*0.6;zoomto,_screen.w*0.015,_screen.h*0.867;linear,1;diffuse,0,0,0,0.20)}

--Squishing the player actors so that they wont strech on dual monitor
-- local players = {}
-- t[#t+1] = Def.ActorFrame{
	-- OnCommand=function(self)
		-- for player in ivalues(GAMESTATE:GetHumanPlayers()) do
			-- local player = SCREENMAN:GetTopScreen():GetChild('Player'..ToEnumShortString(player))
			-- players[#players+1] = player
		-- end
		-- for player in ivalues(players) do
			-- player["linear"](player, 0):zoomx(0.5)
		-- end
	-- end,
	-- }

return t