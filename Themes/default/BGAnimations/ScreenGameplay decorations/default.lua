local t = LoadFallbackB();

--t[#t+1] = StandardDecorationFromFile( "StageFrame", "StageFrame" );

t[#t+1] = LoadActor("_warning")..{
	InitCommand=cmd(Center);
	OnCommand=cmd(diffusealpha,0);
	ShowDangerAllMessageCommand=cmd(stoptweening;accelerate,0.3;diffusealpha,1);
	HideDangerAllMessageCommand=cmd(stoptweening;accelerate,0.3;diffusealpha,0);
};

t[#t+1] = StandardDecorationFromFile( "NowPlaying", "NowPlaying" );
t[#t+1] = StandardDecorationFromFile( "StageDisplay", "StageDisplay" );

-- Song Meter Display
t[#t+1] = Def.ActorFrame{
	Name="SongMeterDisplayFrame";
	InitCommand=cmd(xy,SCREEN_CENTER_X,SCREEN_TOP+20);

	Def.Quad{
		Name="LineBG";
		InitCommand=cmd(zoomto,(SCREEN_WIDTH*0.8)-2,6;diffuse,color("0,0,0,0.375"););
	};
	Def.Quad{
		Name="LineMain";
		InitCommand=cmd(zoomto,SCREEN_WIDTH*0.8,1;diffusealpha,0.75;);
	};
	Def.Quad{
		Name="LineLeft";
		InitCommand=cmd(x,-(SCREEN_WIDTH*0.8)/2;zoomto,1,6;diffusealpha,0.75;);
	};
	Def.Quad{
		Name="LineRight";
		InitCommand=cmd(x,(SCREEN_WIDTH*0.8)/2;zoomto,1,6;diffusealpha,0.75;);
	};
	Def.SongMeterDisplay {
		InitCommand=cmd(SetStreamWidth,SCREEN_WIDTH*0.8);
		Stream=Def.Actor{};
		Tip=Def.ActorFrame{
			Def.Quad{
				InitCommand=cmd(zoomto,6,6;diffuse,HSV(48,0.8,0);rotationz,45;pulse;effectclock,'beatnooffset';effectmagnitude,1,0.75,1);
			};
			Def.Quad{
				InitCommand=cmd(zoomto,4,4;diffuse,HSV(48,0.8,1);rotationz,45;pulse;effectclock,'beatnooffset';effectmagnitude,1,0.75,1);
			};
		};
	};
	if theme_config:get_data().TimingDisplay then
		songMeterDisplay[#songMeterDisplay+1] = CreateSegments(pn);
	end
end

return t;
