local pn = ...

local stats = STATSMAN:GetCurStageStats():GetPlayerStageStats(pn)
local PercentDP = stats:GetPercentDancePoints()
local percent = FormatPercentScore(PercentDP)
-- Format the Percentage string, removing the % symbol
percent = percent:gsub("%%", "")

return Def.ActorFrame{
	Name="PercentageContainer"..ToEnumShortString(pn),
	OnCommand=function(self)
		self:y( _screen.cy+33 )
		self:x( (pn == PLAYER_1 and -335) or -335 )
	end,

	-- dark background quad behind player percent score
	-- Def.Quad{
		-- InitCommand=cmd(diffuse, color("#101519"); zoomto, 160,60 )
	-- },

	LoadFont("_ScreenEvaluation numbers")..{
		Text=percent,
		Name="Percent",
		InitCommand=cmd(valign, 1; horizalign, right; zoom,0.3 ),

	}
}