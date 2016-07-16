-- this is only used for post-selection so far
local t = Def.ActorFrame{};

t[#t+1] = Def.Banner{
	InitCommand=cmd(x,SCREEN_CENTER_X;y,SCREEN_TOP-128;visible,false);
	SetCommand=function(self)
		if GAMESTATE:IsCourseMode() then
			if GAMESTATE:GetCurrentCourse() then
				self:LoadFromCourse(GAMESTATE:GetCurrentCourse());
			end;
		else
			if GAMESTATE:GetCurrentSong() then
				self:LoadFromSong(GAMESTATE:GetCurrentSong());
			end;
		end;

		local w, h = self:GetWidth(), self:GetHeight();
		local aspect = w/h;
		-- notable banner aspect ratios:
		-- 3.2 (256x80 [ddr]; 512x160 doublesized)
		-- 1.5 (300x200; 204x153 real [pump])
		-- 3.0 (300x100 [pump pro])
		-- 2.54878 (418x164 [itg])
		if h >= 128 then
			-- banner height may be too tall and obscure information, scale it
			local newZoomY = scale(h, 128,200, 80,100);
			local newZoomX = self:GetWidth() * newZoomY/self:GetHeight();
			self:zoomto(newZoomX,newZoomY);
		else
			self:zoomto(w,h)
		end;
	end;
	CurrentSongChangedMessageCommand=cmd(playcommand,"Set");
	CurrentCourseChangedMessageCommand=cmd(playcommand,"Set");
	ShowPressStartForOptionsCommand=cmd(visible,true;sleep,0.4;decelerate,1;y,SCREEN_CENTER_Y*0.75);
	OffCommand=cmd(sleep,0.75;bouncebegin,0.375;zoomx,0);
};

t[#t+1] = LoadFont("Common normal")..{
	Text=THEME:GetString("ScreenSelectMusic","OptionsMessage");
	InitCommand=cmd(x,SCREEN_CENTER_X;y,SCREEN_CENTER_Y*1.35;vertalign,bottom;NoStroke;shadowlength,1;shadowcolor,color("0,0,0,0.375"));
	OnCommand=cmd(visible,false);
	ShowPressStartForOptionsCommand=cmd(hibernate,0.5;visible,true;zoom,1.5;decelerate,1;zoom,1);
	ShowEnteringOptionsCommand=cmd(settext,THEME:GetString("ScreenSelectMusic","EnteringOptions"););
	OffCommand=cmd(sleep,0.8;bouncebegin,0.375;zoomy,0);
};

-- todo: add player information?

t[#t+1] = LoadFont("Common normal")..{
	Name="Title";
	InitCommand=cmd(x,SCREEN_CENTER_X;y,SCREEN_CENTER_Y+12;diffusealpha,0;zoom,1.25;valign,1;strokecolor,color("#00000000"));
	SetCommand=function(self)
		local SongOrCourse, text = nil, "";
		if GAMESTATE:IsCourseMode() then
			SongOrCourse = GAMESTATE:GetCurrentCourse();
		else
			SongOrCourse = GAMESTATE:GetCurrentSong();
		end;
		if SongOrCourse then
			text = SongOrCourse:GetDisplayFullTitle();
		end;
		self:settext(text);
	end;
	CurrentSongChangedMessageCommand=cmd(playcommand,"Set");
	CurrentCourseChangedMessageCommand=cmd(playcommand,"Set");
	ShowPressStartForOptionsCommand=cmd(linear,1;diffusealpha,1;zoom,1);
	OffCommand=cmd(sleep,1.5;bouncebegin,0.5;zoomy,0);
};

-- todo: localize stage text stuff
t[#t+1] = LoadFont("Common normal")..{
	Name="Secondary";
	InitCommand=cmd(x,SCREEN_CENTER_X;y,SCREEN_CENTER_Y+10;diffusealpha,0;zoom,1;valign,0;strokecolor,color("#00000000"));
	SetCommand=function(self)
		local SongOrCourse, text = nil, "";
		if GAMESTATE:IsCourseMode() then
			SongOrCourse = GAMESTATE:GetCurrentCourse();
			if SongOrCourse then
				local stages = SongOrCourse:GetEstimatedNumStages();
				if stages == 1 then
					text = string.format(ScreenString("%i stage"),stages)
				else
					text = string.format(ScreenString("%i stages"),stages)
				end;
			end;
		else
			SongOrCourse = GAMESTATE:GetCurrentSong();
			if SongOrCourse then
				text = SongOrCourse:GetDisplayArtist()
			end;
		end;
		self:settext(text);
	end;
	CurrentSongChangedMessageCommand=cmd(playcommand,"Set");
	CurrentCourseChangedMessageCommand=cmd(playcommand,"Set");
	ShowPressStartForOptionsCommand=cmd(linear,1;diffusealpha,1;zoom,0.8);
	OffCommand=cmd(sleep,1.55;bouncebegin,0.5;zoomy,0);
};

local function GetSpeedModName(player)
    local speed, mode= GetSpeedModeAndValueFromPoptions(player)
    if mode == "x" then
        return (speed/100) .. "x"
    else
        return mode .. speed
    end
end

local speedframe = Def.ActorFrame {
    InitCommand=cmd(xy,(SCREEN_CENTER_X*0.75/2)+28,SCREEN_CENTER_Y*1.50);
    OffCommand=cmd(bouncebegin,0.375;addx,-SCREEN_CENTER_X*1.25);
}

local function ReadSpeedModFile(path)
	local file = RageFileUtil.CreateRageFile()
	if not file:Open(path, 1) then
		file:destroy()
		return nil
	end

	local contents = file:Read()
	file:Close()
	file:destroy()
    
    local found={}
    
    for _, speed in ipairs(split(",", contents)) do
		found[#found+1]=speed
	end

	return found
end

local nextSpeeds = {}
local prevSpeeds = {}
local speeds = ReadSpeedModFile( "/Themes/"..THEME:GetCurThemeName().."/SpeedMods.txt")

for idex, speed in ipairs(speeds) do
    nextSpeeds[speed]=speeds[idex==#speeds and 1 or (idex+1)]
    prevSpeeds[speed]=speeds[idex==1 and (#speeds) or (idex-1)]
end

function has_value (tab, val)
    for index, value in ipairs (tab) do
        if value == val then
            return true
        end
    end

    return false
end

 --show player speedmods
for pn, p in ipairs(GAMESTATE:GetHumanPlayers()) do
    local nactor = Def.ActorFrame {
        InitCommand=cmd(xy,ScreenMetric('SpeedP'..pn..'X'), ScreenMetric('SpeedP'..pn..'Y'));
        Def.Quad {
            InitCommand=function(self)
                --if TOURNAMENTMAN:IsActive() then
                --    self:zoomto(80,70):diffuse(PlayerColor(p)):diffusealpha(0.4):y(5)
                --else
                    self:zoomto(80,60):diffuse(PlayerColor(p)):diffusealpha(0.4)
                --end
            end
        };
        Def.BitmapText {
            Font="Common normal";
            InitCommand=cmd(y,-22;settext,pname(p);diffuse,PlayerColor(p))
        };
        Def.BitmapText {
            Name="SpeedText";
            Font="Common normal";
            InitCommand=cmd(y,-2;queuecommand,"UpdateSpeed";zoom,0.7);
            CodeMessageCommand=function(self, param)
                if param.PlayerNumber == p then
                    if param.Name=="PreviousScrollSpeed" or param.Name=="NextScrollSpeed" then
                        local pSpeed = GetSpeedModName(p)
                        if not has_value(speeds, pSpeed) then
                            pSpeed = speeds[1]
                        end
                        
                        if param.Name=="PreviousScrollSpeed" then
                            GAMESTATE:GetPlayerState(p):SetPlayerOptions("ModsLevel_Preferred", prevSpeeds[pSpeed])
                        elseif param.Name=="NextScrollSpeed" then
                            GAMESTATE:GetPlayerState(p):SetPlayerOptions("ModsLevel_Preferred", nextSpeeds[pSpeed])
                        end
                        
                        self:queuecommand("UpdateSpeed")
                    end
                end
            end;
            UpdateSpeedCommand=function(self)
                self:settext(GetSpeedModName(p))
                self:GetParent():GetChild("BpmText"):queuecommand("UpdateBpm")
            end;
            CurrentSongChangedMessageCommand=cmd(playcommand,"UpdateSpeed");
            CurrentCourseChangedMessageCommand=cmd(playcommand,"UpdateSpeed");
        };
        Def.BitmapText {
            Name="BpmText";
            Font="Common normal";
            InitCommand=cmd(y,14;queuecommand,"UpdateBpm";zoom,0.7);
            UpdateBpmCommand=function(self)
                local speed, mode= GetSpeedModeAndValueFromPoptions(p)
                if GAMESTATE:GetCurrentSong() ~= nil then
                    self:settext(
                        (mode=='x' and
                            math.floor((GAMESTATE:GetCurrentSong():GetDisplayBpms()[2]*speed)/100) or
                            speed)
                    ..'bpm')
                else
                    self:settext('')
                end
            end;
        };
    }
    
    if TOURNAMENTMAN:IsActive() then
        nactor[#nactor+1] = Def.BitmapText {
            Name="PlayerText";
            Font="Common normal";
            InitCommand=cmd(y,32;queuecommand,"UpdateName";zoom,0.7);
            UpdateNameCommand=function(self)
                self:settext(TOURNAMENTMAN:GetPlayerName(p))
                --self:settext("apple")
            end;
            TournamentUpdateMessageCommand=cmd(queuecommand,"UpdateName");
        };
    end
    
    speedframe[#speedframe+1] = nactor
end

t[#t+1] = speedframe

return t;