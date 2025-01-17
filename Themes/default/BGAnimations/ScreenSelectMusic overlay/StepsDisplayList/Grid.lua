local GridColumns = 14
local GridRows = 5
local GridZoomX = 0.5 --IsUsingWideScreen() and 0.435 or 0.39
local BlockZoomY = 0.275
local StepsToDisplay, SongOrCourse, StepsOrTrails

local t = Def.ActorFrame{
	Name="StepsDisplayList",
	InitCommand=cmd(vertalign, top; draworder, 1; xy, _screen.w*0.271, _screen.h*0.36),
	-- - - - - - - - - - - - - -

	OnCommand=cmd(queuecommand, "RedrawStepsDisplay"),
	CurrentSongChangedMessageCommand=cmd(queuecommand, "RedrawStepsDisplay"),
	CurrentCourseChangedMessageCommand=cmd(queuecommand, "RedrawStepsDisplay"),
	StepsHaveChangedCommand=cmd(queuecommand, "RedrawStepsDisplay"),

	-- - - - - - - - - - - - - -

	RedrawStepsDisplayCommand=function(self)

		SongOrCourse = (GAMESTATE:IsCourseMode() and GAMESTATE:GetCurrentCourse()) or GAMESTATE:GetCurrentSong()

		if SongOrCourse then
			StepsOrTrails = (GAMESTATE:IsCourseMode() and SongOrCourse:GetAllTrails()) or SongUtil.GetPlayableSteps( SongOrCourse )

			if StepsOrTrails then

				StepsToDisplay = GetStepsToDisplay(StepsOrTrails)

				for RowNumber=1,GridRows do
					if StepsToDisplay[RowNumber] then
						-- if this particular song has a stepchart for this row, update the Meter
						-- and BlockRow coloring appropriately
						local meter = StepsToDisplay[RowNumber]:GetMeter()
						local difficulty = StepsToDisplay[RowNumber]:GetDifficulty()
						self:GetChild("Grid"):GetChild("Meter_"..RowNumber):playcommand("Set", {Meter=meter, Difficulty=difficulty})
						self:GetChild("Grid"):GetChild("Blocks_"..RowNumber):playcommand("Set", {Meter=meter, Difficulty=difficulty})
					else
						-- otherwise, set the meter to "?" and hide this particular colored BlockRow
						self:GetChild("Grid"):GetChild("Meter_"..RowNumber):playcommand("Unset")
						self:GetChild("Grid"):GetChild("Blocks_"..RowNumber):playcommand("Unset")

					end
				end
				
			end
		else
			StepsOrTrails, StepsToDisplay = nil, nil
			self:playcommand("Unset")
		end
	end,

	-- - - - - - - - - - - - - -

	-- background
	Def.Quad{
		Name="Background",
		InitCommand=function(self)
			self:diffuse(color("#1e282f"))
			self:y(_screen.h*0.003)
			self:zoomto(_screen.w*0.525, _screen.h*0.19)
			if ThemePrefs.Get("RainbowMode") then
				self:diffusealpha(0.75)
			end
		end
	},
}


local Grid = Def.ActorFrame{
	Name="Grid",
	InitCommand=cmd(horizalign, left; vertalign, top; xy, 8, -52 ),
}


-- A grid of decorative faux-blocks that will exist
-- behind the changing difficulty blocks.
Grid[#Grid+1] = Def.Sprite{
	Name="BackgroundBlocks",
	Texture=THEME:GetPathB("ScreenSelectMusic", "overlay/StepsDisplayList/_block.png"),

	InitCommand=cmd(diffuse, color("#182025") ),
	OnCommand=function(self)
		local width = self:GetWidth()
		local height= self:GetHeight()
		self:zoomto(width * 1.3 * GridColumns * GridZoomX, height * GridRows * BlockZoomY)
		self:xy(_screen.w*0.07 , 3 * height * BlockZoomY )
		self:customtexturerect(0, 0, GridColumns, GridRows)
	end
}

for RowNumber=1,GridRows do

	Grid[#Grid+1] =	Def.Sprite{
		Name="Blocks_"..RowNumber,
		Texture=THEME:GetPathB("ScreenSelectMusic", "overlay/StepsDisplayList/_block.png"),

		InitCommand=cmd(diffusealpha,0),
		OnCommand=function(self)
			local width = self:GetWidth()
			local height= self:GetHeight()
			self:xy(_screen.w*0.07 , RowNumber * height * BlockZoomY)
			self:zoomto(width * 1.3 * GridColumns * GridZoomX, height * BlockZoomY)
		end,
		SetCommand=function(self, params)

			local meter = params.Meter

			-- our grid only supports charts with up to a 20-block difficulty meter
			-- but charts can have higher difficulties
			-- handle that here by setting a maximum number to worry about displaying
			if meter > GridColumns then
				meter = GridColumns
			end

			self:customtexturerect(0, 0, GridColumns, 1)
			self:cropright( 1 - (meter * (1/GridColumns)) )

			-- diffuse and set each chart's difficulty meter
			self:diffuse( DifficultyColor(params.Difficulty) )
		end,
		UnsetCommand=function(self)
			self:customtexturerect(0,0,0,0)
		end
	}

	Grid[#Grid+1] = Def.BitmapText{
		Name="Meter_"..RowNumber,
		Font="_wendy small",

		InitCommand=function(self)
			local height = self:GetParent():GetChild("Blocks_"..RowNumber):GetHeight()
			self:horizalign(right)
			self:y(RowNumber * height * BlockZoomY)
			self:x( _screen.w*-0.11 )
			self:zoom(0.4)
		end,
		SetCommand=function(self, params)

			-- diffuse and set each chart's difficulty meter
			self:diffuse( DifficultyColor(params.Difficulty) )
			self:settext(ToEnumShortString(params.Difficulty).."   "..params.Meter)
		end,
		UnsetCommand=cmd(settext, ""; diffuse,color("#182025")),
	}
end

t[#t+1] = Grid

return t