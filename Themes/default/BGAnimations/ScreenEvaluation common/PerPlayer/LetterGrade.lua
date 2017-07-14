if SL.Global.GameMode ~= "StomperZ" then
	local pn = ...

	local playerStats = STATSMAN:GetCurStageStats():GetPlayerStageStats(pn)
	local grade = playerStats:GetGrade()

	-- "I passd with a q though."
	local title = GAMESTATE:GetCurrentSong():GetDisplayFullTitle()
	if title == "D" then grade = "Grade_Tier99" end

	return LoadActor(THEME:GetPathG("", "_grades/"..grade..".lua"), playerStats)..{
		InitCommand=cmd(xy, -300, _screen.cy+24),
		OnCommand=function(self)
			self:zoom(0.1)
		end
	}
end