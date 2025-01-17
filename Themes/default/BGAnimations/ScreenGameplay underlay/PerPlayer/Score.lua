local player = ...

if SL[ ToEnumShortString(player) ].ActiveModifiers.HideScore then
	return false
else

	return Def.BitmapText{
		Font="_wendy monospace numbers",
		Text="0.00",

		Name=ToEnumShortString(player).."Score",
		InitCommand=function(self)
			self:valign(1):halign(1)

			if SL.Global.GameMode == "StomperZ" then
				self:zoom(0.4):x( WideScale(160, 214) ):y(20)
				if player == PLAYER_2 then
					self:x( _screen.w - WideScale(50, 104) )
				end
			else
				self:zoom(0.4):x( _screen.cx-(_screen.w*210/640) ):y(38)
				if player == PLAYER_2 then
					self:x( _screen.cx-(_screen.w*50/640) )
				elseif player == PLAYER_3 then
					self:x( _screen.cx+(_screen.w*110/640) )
				elseif player == PLAYER_4 then
					self:x( _screen.cx+(_screen.w*270/640) )
				-- if player == PLAYER_5 then
					-- self:x( _screen.cx + _screen.w/2.75 )
				-- if player == PLAYER_6 then
					-- self:x( _screen.cx + _screen.w/2.75 )
				end
			end
		end,
		JudgmentMessageCommand=function(self) self:queuecommand("RedrawScore") end,
		RedrawScoreCommand=function(self)
			local dp = STATSMAN:GetCurStageStats():GetPlayerStageStats(player):GetPercentDancePoints()
			local percent = FormatPercentScore( dp ):sub(1,-2)
			self:settext(percent)
		end
	}
end