local player = ...

return Def.ActorFrame{
	InitCommand=function(self)
		self:xy( _screen.cx-(_screen.w*295/640), 65 )
		if player == PLAYER_2 then
			self:x( _screen.cx-(_screen.w*135/640) )
		elseif player == PLAYER_3 then
			self:x( _screen.cx+(_screen.w*25/640) )
		elseif player == PLAYER_4 then
			self:x( _screen.cx+(_screen.w*185/640) )
		-- elseif player == PLAYER_5 then
			-- self:x( _screen.w-WideScale(27,84) )
		-- elseif player == PLAYER_6 then
			-- self:x( _screen.w-WideScale(27,84) )
		end

		if SL.Global.GameMode == "StomperZ" then
			self:y( 20 )
		end
	end,


	-- colored background for player's chart's difficulty meter
	Def.Quad{
		InitCommand=function(self)
			self:zoomto(22, 22)
		end,
		OnCommand=function(self)
			local currentSteps = GAMESTATE:GetCurrentSteps(player)
			if currentSteps then
				local currentDifficulty = currentSteps:GetDifficulty()
				self:diffuse(DifficultyColor(currentDifficulty))
			end
		end
	},

	-- player's chart's difficulty meter
	LoadFont("_wendy small")..{
		InitCommand=function(self)
			self:diffuse( Color.Black )
			self:zoom( 0.32 )
		end,
		CurrentSongChangedMessageCommand=cmd(queuecommand,"Begin"),
		BeginCommand=function(self)
			local steps = GAMESTATE:GetCurrentSteps(player)
			local meter = steps:GetMeter()

			if meter then
				self:settext(meter)
			end
		end
	}
}