local SpeedModP1_text = false
local SpeedModP2_text = false
local SpeedModP3_text = false
local SpeedModP4_text = false
local SpeedModP5_text = false
local SpeedModP6_text = false
local SpeedModP7_text = false
local SpeedModP8_text = false
local DiffP1_text = false
local DiffP2_text = false
local DiffP3_text = false
local DiffP4_text = false
local DiffP5_text = false
local DiffP6_text = false
local DiffP7_text = false
local DiffP8_text = false
local TableTrackRightPress = {0,0,0,0,0,0,0,0}
local TableTrackLeftPress = {0,0,0,0,0,0,0,0}
local Players = GAMESTATE:GetHumanPlayers()
local DoublePressDelay = 0.05 -- Time within two presses of the same button gets recognized as a double-tap. Values lover than ~0.05 sec don't seem to do anything - the delay is then still ~0.05 sec.

local modslevel = topscreen  == "ScreenEditOptions" and "ModsLevel_Stage" or "ModsLevel_Preferred"

local function set_button_text(text_actor, pn, button, press_type)
	text_actor:settext( ToEnumShortString(pn) .. " " .. button .. " " .. press_type )
end

local function set_SpeedModP1_text(text_actor1)
	text_actor1:settext( SL["P1"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP2_text(text_actor2)
	text_actor2:settext( SL["P2"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP3_text(text_actor3)
	text_actor3:settext( SL["P3"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP4_text(text_actor4)
	text_actor4:settext( SL["P4"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP5_text(text_actor9)
	text_actor9:settext( SL["P5"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP6_text(text_actor10)
	text_actor10:settext( SL["P6"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP7_text(text_actor11)
	text_actor11:settext( SL["P7"].ActiveModifiers.SpeedMod )
end
local function set_SpeedModP8_text(text_actor12)
	text_actor12:settext( SL["P8"].ActiveModifiers.SpeedMod )
end

local function set_DiffP1_text(text_actor5)
	text_actor5:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_1):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_1):GetMeter() )
end
local function set_DiffP2_text(text_actor6)
	text_actor6:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_2):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_2):GetMeter()  )
end
local function set_DiffP3_text(text_actor7)
	text_actor7:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_3):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_3):GetMeter()  )
end
local function set_DiffP4_text(text_actor8)
	text_actor8:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_4):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_4):GetMeter()  )
end
local function set_DiffP5_text(text_actor13)
	text_actor13:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_5):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_5):GetMeter() )
end
local function set_DiffP6_text(text_actor14)
	text_actor14:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_6):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_6):GetMeter()  )
end
local function set_DiffP7_text(text_actor15)
	text_actor15:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_7):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_7):GetMeter()  )
end
local function set_DiffP8_text(text_actor16)
	text_actor16:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_8):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_8):GetMeter()  )
end

local function InputHandler( event )
	if not event.PlayerNumber then return end
	local press_type= ToEnumShortString(event.type)

-- Changing speedmods depending on player input.
	for i=1,#Players do
		if event.button == "Right" and ToEnumShortString(event.type) == "FirstPress" and ToEnumShortString(event.PlayerNumber)=="P"..i then
			if TableTrackRightPress[i] == 0 then
				TableTrackRightPress[i] = 1
			elseif TableTrackRightPress[i] == 2 then
				SL[ToEnumShortString(event.PlayerNumber)].ActiveModifiers.SpeedMod = SL[ToEnumShortString(event.PlayerNumber)].ActiveModifiers.SpeedMod+50
			end
		elseif event.button == "Left" and ToEnumShortString(event.type) == "FirstPress" and ToEnumShortString(event.PlayerNumber)=="P"..i then
			if TableTrackLeftPress[i] == 0 then
				TableTrackLeftPress[i] = 1
			elseif TableTrackLeftPress[i] == 2 then
				SL[ToEnumShortString(event.PlayerNumber)].ActiveModifiers.SpeedMod = SL[ToEnumShortString(event.PlayerNumber)].ActiveModifiers.SpeedMod-50
			end
		end
	end
	
-- Activate Speedmods.
	for i=1,#Players do
		GAMESTATE:GetPlayerState("P"..i):GetPlayerOptions(modslevel):MMod(SL["P"..i].ActiveModifiers.SpeedMod)
	end
	
-- Setting speedmods text boxes.
	set_SpeedModP1_text(SpeedModP1_text, SL["P1"].ActiveModifiers.SpeedMod)
	if #Players > 1 then
		set_SpeedModP2_text(SpeedModP2_text, SL["P2"].ActiveModifiers.SpeedMod)
		if #Players > 2 then
			set_SpeedModP3_text(SpeedModP3_text, SL["P3"].ActiveModifiers.SpeedMod)
			if #Players > 3 then
				set_SpeedModP4_text(SpeedModP4_text, SL["P4"].ActiveModifiers.SpeedMod)
				if #Players > 4 then
					set_SpeedModP5_text(SpeedModP5_text, SL["P5"].ActiveModifiers.SpeedMod)
					if #Players > 5 then
						set_SpeedModP6_text(SpeedModP6_text, SL["P6"].ActiveModifiers.SpeedMod)
						if #Players > 6 then
							set_SpeedModP7_text(SpeedModP7_text, SL["P7"].ActiveModifiers.SpeedMod)
							if #Players > 7 then
								set_SpeedModP8_text(SpeedModP8_text, SL["P8"].ActiveModifiers.SpeedMod)
							end
						end
					end
				end
			end
		end
	end
	
-- Setting difficulty text boxes.
	set_DiffP1_text(DiffP1_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_1):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_1):GetMeter() )
	if #Players > 1 then
		set_DiffP2_text(DiffP2_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_2):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_2):GetMeter() )
		if #Players > 2 then
			set_DiffP3_text(DiffP3_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_3):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_3):GetMeter() )
			if #Players > 3 then
				set_DiffP4_text(DiffP4_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_4):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_4):GetMeter() )
				if #Players > 4 then
					set_DiffP5_text(DiffP5_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_5):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_5):GetMeter() )
					if #Players > 5 then
						set_DiffP6_text(DiffP6_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_6):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_6):GetMeter() )
						if #Players > 6 then
							set_DiffP7_text(DiffP7_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_7):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_7):GetMeter() )
							if #Players > 7 then
								set_DiffP8_text(DiffP8_text, ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_8):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_8):GetMeter() )
							end
						end
					end
				end
			end
		end
	end
	
end

local t = Def.ActorFrame{
	ChangeStepsMessageCommand=function(self, params)
		self:playcommand("StepsHaveChanged", {Direction=params.Direction, Player=params.Player})
	end,

	-- make the MusicWheel appear to cascade down
	LoadActor("./MusicWheelAnimation.lua"),
	-- Apply player modifiers from profile
	LoadActor("./PlayerModifiers.lua"),
	-- Difficulty Blocks (normal) or CourseContentsList (CourseMode)
	LoadActor("./StepsDisplayList/default.lua"),
	-- Graphical Banner
	LoadActor("./Banner.lua"),
	-- Song Artist, BPM, Duration (Referred to in other themes as "PaneDisplay")
	LoadActor("./SongDescription.lua"),
	-- a folder of Lua files to be loaded twice (once for each player)
	LoadActor("./PerPlayer/default.lua"),
	-- MenuTimer code for preserving SSM's timer value
	LoadActor("./MenuTimer.lua"),
	-- overlay for sorting the MusicWheel, hidden by default
	LoadActor("./SortMenu/default.lua"),
	
	-- Show SpeedMods
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.05, _screen.h*0.9)
					self:diffuse(DifficultyIndexColor(2) )
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.19, _screen.h*0.9)
					if #Players > 1 then
						self:diffuse(DifficultyIndexColor(5) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.33, _screen.h*0.9)
					if #Players > 2 then
						self:diffuse(DifficultyIndexColor(7) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.47, _screen.h*0.9)
					if #Players > 3 then
						self:diffuse(DifficultyIndexColor(9) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.61, _screen.h*0.9)
					if #Players > 4 then
						self:diffuse(DifficultyIndexColor(11) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.75, _screen.h*0.9)
					if #Players > 5 then
						self:diffuse(DifficultyIndexColor(13) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.89, _screen.h*0.9)
					if #Players > 6 then
						self:diffuse(DifficultyIndexColor(15) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*1.03, _screen.h*0.9)
					if #Players > 7 then
						self:diffuse(DifficultyIndexColor(17) )
					else
						self:visible(0)
					end
				end
			},

		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP1_text = self
				self:diffuse(color("#272150"))
				self:zoom(2):xy(_screen.w*0.05, _screen.h*0.9)
				self:settext( SL["P1"].ActiveModifiers.SpeedMod )
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP2_text = self
				if #Players > 1 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.19, _screen.h*0.9)
					self:settext( SL["P2"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP3_text = self
				if #Players > 2 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.33, _screen.h*0.9)
					self:settext( SL["P3"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP4_text = self
				if #Players > 3 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.47, _screen.h*0.9)
					self:settext( SL["P4"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP5_text = self
				if #Players > 4 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.61, _screen.h*0.9)
					self:settext( SL["P5"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP6_text = self
				if #Players > 5 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.75, _screen.h*0.9)
					self:settext( SL["P6"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP7_text = self
				if #Players > 6 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*0.89, _screen.h*0.9)
					self:settext( SL["P7"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			InitCommand=function(self)
				SpeedModP8_text = self
				if #Players > 7 then
					self:diffuse(color("#272150"))
					self:zoom(2):xy(_screen.w*1.03, _screen.h*0.9)
					self:settext( SL["P8"].ActiveModifiers.SpeedMod )
				end
			end,
		},
		
	-- Show Difficulties
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.05, _screen.h*0.7)
					self:diffuse(DifficultyIndexColor(2) )
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.19, _screen.h*0.7)
					if #Players > 1 then
						self:diffuse(DifficultyIndexColor(5) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.33, _screen.h*0.7)
					if #Players > 2 then
						self:diffuse(DifficultyIndexColor(7) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.47, _screen.h*0.7)
					if #Players > 3 then
						self:diffuse(DifficultyIndexColor(9) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.61, _screen.h*0.7)
					if #Players > 4 then
						self:diffuse(DifficultyIndexColor(11) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.75, _screen.h*0.7)
					if #Players > 5 then
						self:diffuse(DifficultyIndexColor(13) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*0.89, _screen.h*0.7)
					if #Players > 6 then
						self:diffuse(DifficultyIndexColor(15) )
					else
						self:visible(0)
					end
				end
			},
			
			Def.Quad{	
				InitCommand=function(self)
					self:zoomto(_screen.w*0.085, _screen.h*0.12)
					self:xy(_screen.w*1.03, _screen.h*0.7)
					if #Players > 7 then
						self:diffuse(DifficultyIndexColor(17) )
					else
						self:visible(0)
					end
				end
			},

		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP1_text = self
				self:diffuse(color("#272150"))
				self:zoom(0.9):xy(_screen.w*0.05, _screen.h*0.7)
				self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_1):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_1):GetMeter()  )
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP2_text = self
				if #Players > 1 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.19, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_2):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_2):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP3_text = self
				if #Players > 2 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.33, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_3):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_3):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP4_text = self
				if #Players > 3 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.47, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_4):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_4):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP5_text = self
				if #Players > 4 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.61, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_5):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_5):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP6_text = self
				if #Players > 5 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.75, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_6):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_6):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP7_text = self
				if #Players > 6 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*0.89, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_7):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_7):GetMeter()  )
				end
			end,
		},
		
		Def.BitmapText{
			Font="Common normal",
			OnCommand=function(self)
				DiffP8_text = self
				if #Players > 7 then
					self:diffuse(color("#272150"))
					self:zoom(0.9):xy(_screen.w*1.03, _screen.h*0.7)
					self:settext( ToEnumShortString(GAMESTATE:GetCurrentSteps(PLAYER_8):GetDifficulty()) .." "..GAMESTATE:GetCurrentSteps(PLAYER_8):GetMeter()  )
				end
			end,
		},
		
	OnCommand=function(self)
		SCREENMAN:GetTopScreen():AddInputCallback( InputHandler )
				StepsToDisplay =  GetStepsToDisplay(SongUtil.GetPlayableSteps( GAMESTATE:GetCurrentSong() ))
	end,

	-- Mumbo Jumbo to ensure only double-presses will change speedmods. Ugly level: Max300.
	
	Def.Quad{
		OnCommand=function(self)
			self:queuecommand("TrackP1RightPress")
			self:queuecommand("TrackP1LeftPress")
			if #Players > 1 then
				self:queuecommand("TrackP2RightPress")
				self:queuecommand("TrackP2LeftPress")
				if #Players > 2 then
					self:queuecommand("TrackP3RightPress")
					self:queuecommand("TrackP3LeftPress")
					if #Players > 3 then
						self:queuecommand("TrackP4RightPress")
						self:queuecommand("TrackP4LeftPress")
					end
				end
			end
		end,

		TrackP1RightPressCommand=function(self)
			if TableTrackRightPress[1] == 0 then
				self:sleep(0.0002)
			elseif TableTrackRightPress[1] == 2 then
				self:sleep(0.0002)
			else
				TableTrackRightPress[1] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP1RightPress")
			end
			self:queuecommand("TrackP1RightPress")
		end,
		ResetP1RightPressCommand=function(self)
			TableTrackRightPress[1] = 0
		end,
		TrackP1LeftPressCommand=function(self)
			if TableTrackLeftPress[1] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[1] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[1] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP1LeftPress")
			end
			self:queuecommand("TrackP1LeftPress")
		end,
		ResetP1LeftPressCommand=function(self)
			TableTrackLeftPress[1] = 0
		end,
		
		TrackP2RightPressCommand=function(self)
			if TableTrackRightPress[2] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[2] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[2] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP2RightPress")
			end
			self:queuecommand("TrackP2RightPress")
		end,
		ResetP2RightPressCommand=function(self)
			TableTrackRightPress[2] = 0
		end,
		TrackP2LeftPressCommand=function(self)
			if TableTrackLeftPress[2] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[2] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[2] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP2LeftPress")
			end
			self:queuecommand("TrackP2LeftPress")
		end,
		ResetP2LeftPressCommand=function(self)
			TableTrackLeftPress[2] = 0
		end,
		
		TrackP3RightPressCommand=function(self)
			if TableTrackRightPress[3] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[3] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[3] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP3RightPress")
			end
			self:queuecommand("TrackP3RightPress")
		end,
		ResetP3RightPressCommand=function(self)
			TableTrackRightPress[3] = 0
		end,
		TrackP3LeftPressCommand=function(self)
			if TableTrackLeftPress[3] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[3] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[3] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP3LeftPress")
			end
			self:queuecommand("TrackP3LeftPress")
		end,
		ResetP3LeftPressCommand=function(self)
			TableTrackLeftPress[3] = 0
		end,
		
		TrackP4RightPressCommand=function(self)
			if TableTrackRightPress[4] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[4] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[4] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP4RightPress")
			end
			self:queuecommand("TrackP4RightPress")
		end,
		ResetP4RightPressCommand=function(self)
			TableTrackRightPress[4] = 0
		end,
		TrackP4LeftPressCommand=function(self)
			if TableTrackLeftPress[4] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[4] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[4] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP4LeftPress")
			end
			self:queuecommand("TrackP4LeftPress")
		end,
		ResetP4LeftPressCommand=function(self)
			TableTrackLeftPress[4] = 0
		end,


		TrackP5RightPressCommand=function(self)
			if TableTrackRightPress[5] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[5] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[5] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP5RightPress")
			end
			self:queuecommand("TrackP5RightPress")
		end,
		ResetP5RightPressCommand=function(self)
			TableTrackRightPress[5] = 0
		end,
		TrackP5LeftPressCommand=function(self)
			if TableTrackLeftPress[5] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[5] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[5] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP5LeftPress")
			end
			self:queuecommand("TrackP5LeftPress")
		end,
		ResetP5LeftPressCommand=function(self)
			TableTrackLeftPress[5] = 0
		end,
	
		TrackP6RightPressCommand=function(self)
			if TableTrackRightPress[6] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[6] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[6] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP6RightPress")
			end
			self:queuecommand("TrackP6RightPress")
		end,
		ResetP6RightPressCommand=function(self)
			TableTrackRightPress[6] = 0
		end,
		TrackP6LeftPressCommand=function(self)
			if TableTrackLeftPress[6] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[6] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[6] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP6LeftPress")
			end
			self:queuecommand("TrackP6LeftPress")
		end,
		ResetP6LeftPressCommand=function(self)
			TableTrackLeftPress[6] = 0
		end,
	
		TrackP7RightPressCommand=function(self)
			if TableTrackRightPress[7] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[7] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[7] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP7RightPress")
			end
			self:queuecommand("TrackP7RightPress")
		end,
		ResetP7RightPressCommand=function(self)
			TableTrackRightPress[7] = 0
		end,
		TrackP7LeftPressCommand=function(self)
			if TableTrackLeftPress[7] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[7] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[7] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP7LeftPress")
			end
			self:queuecommand("TrackP7LeftPress")
		end,
		ResetP7LeftPressCommand=function(self)
			TableTrackLeftPress[7] = 0
		end,
	
		TrackP8RightPressCommand=function(self)
			if TableTrackRightPress[8] == 0 then
				self:sleep(0.02)
			elseif TableTrackRightPress[8] == 2 then
				self:sleep(0.02)
			else
				TableTrackRightPress[8] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP8RightPress")
			end
			self:queuecommand("TrackP8RightPress")
		end,
		ResetP8RightPressCommand=function(self)
			TableTrackRightPress[8] = 0
		end,
		TrackP8LeftPressCommand=function(self)
			if TableTrackLeftPress[8] == 0 then
				self:sleep(0.02)
			elseif TableTrackLeftPress[8] == 2 then
				self:sleep(0.02)
			else
				TableTrackLeftPress[8] = 2
				self:sleep(DoublePressDelay)
				self:queuecommand("ResetP8LeftPress")
			end
			self:queuecommand("TrackP8LeftPress")
		end,
		ResetP8LeftPressCommand=function(self)
			TableTrackLeftPress[8] = 0
		end,
	},
	
}

return t