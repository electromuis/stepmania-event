local player = ...
local pn = ToEnumShortString(player)

local mode = ""
if SL.Global.GameMode == "StomperZ" then mode = "StomperZ" end
if SL.Global.GameMode == "ECFA" then mode = "ECFA" end

-- tap note types
-- Iterating through the enum isn't worthwhile because the sequencing is so bizarre...
local TNSNames = {
	THEME:GetString("TapNoteScore" .. mode, "W1"),
	THEME:GetString("TapNoteScore" .. mode, "W2"),
	THEME:GetString("TapNoteScore" .. mode, "W3"),
	THEME:GetString("TapNoteScore" .. mode, "W4"),
	THEME:GetString("TapNoteScore" .. mode, "W5"),
	THEME:GetString("TapNoteScore" .. mode, "Miss"),
	THEME:GetString("ScreenEvaluation", 'Holds'),
	THEME:GetString("ScreenEvaluation", 'Mines')
}

local StomperZColors = {
	color("#FFFFFF"),	-- white
	color("#e29c18"),	-- gold
	color("#66c955"),	-- green
	color("#21CCE8"),	-- blue
	color("#000000"),	-- black
	color("#ff0000")	-- red
}

local ECFAColors = {
	color("#21CCE8"),	-- blue
	color("#FFFFFF"),	-- white
	color("#e29c18"),	-- gold
	color("#66c955"),	-- green
	color("#9e00f7"),	-- purple
	color("#ff0000")	-- red
}


local t = Def.ActorFrame{
	InitCommand=cmd(xy, -50, _screen.cy-24),
	OnCommand=function(self)
		 if player ~= PLAYER_1 then
			 self:visible(false)
		 end
	end
}


--  labels: W1 ---> Miss
for index, label in ipairs(TNSNames) do
	t[#t+1] = LoadFont("_miso")..{
		Text=label:upper(),
		InitCommand=cmd(zoom,0.75; horizalign,right ),
		BeginCommand=function(self)
			self:y( (player == PLAYER_1 and 25) or 25 )
			self:x((index-1)*80 -118)
			self:diffuse(color("#000000"))

			local gmods = SL.Global.ActiveModifiers
			local mode = SL.Global.GameMode
			if (mode == "Casual" or mode == "Competitive") then mode = "" end

			-- if Way Offs were turned off
			if gmods.DecentsWayOffs == "Decents Only" and label == THEME:GetString("TapNoteScore" .. mode, "W5") then
				self:visible(false)

			-- if both Decents and WayOffs were turned off
			elseif gmods.DecentsWayOffs == "Off" and (label == THEME:GetString("TapNoteScore" .. mode, "W4") or label == THEME:GetString("TapNoteScore" .. mode, "W5")) then
				self:visible(false)
			end
		end
	}
end

return t