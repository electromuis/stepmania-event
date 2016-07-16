local t = Def.ActorFrame{
	-- lol the things I have to hack in to fix StepMania's oversights (and yes,
	-- this fix applies to sm-ssc v1.0 beta 2 [also beta 3, likely] as well.)
	Def.Actor{
		Name="FixerUpper";
		CurrentSongChangedMessageCommand=function(self)
			local song = GAMESTATE:GetCurrentSong();
			for pn in ivalues(PlayerNumber) do
				local score = SCREENMAN:GetTopScreen():GetChild("Score"..ToEnumShortString(pn));
				if score and not song then
					score:settext("        0");
				end;
			end;
		end;
	};
};

t[#t+1] = StandardDecorationFromFile("ArtistAndGenre","ArtistAndGenre");
t[#t+1] = StandardDecorationFromFile("BPMDisplay","BPMDisplay");
t[#t+1] = StandardDecorationFromFileOptional("SortDisplay","SortDisplay");
t[#t+1] = StandardDecorationFromFileOptional("SelectionLength","SelectionLength");
t[#t+1] = StandardDecorationFromFileOptional("SongOptions","SongOptions");
t[#t+1] = StandardDecorationFromFileOptional("StageDisplay","StageDisplay");
t[#t+1] = StandardDecorationFromFileOptional("CourseContentsList","CourseContentsList");

local function LoadCursor(player)
    return Def.ActorFrame {
			BeginCommand=cmd(visible,true);
			StepsSelectedMessageCommand=function( self, param ) 
				if param.Player ~= player then return end;
				self:visible(false);
			end;
			children={
				LoadActor( "StepsDisplayList highlight" ) .. {
					InitCommand=cmd(addx,-12;diffusealpha,0.3);
					BeginCommand=cmd(player,player);
					OnCommand=cmd(playcommand,"UpdateAlpha");
                    
					CurrentStepsP1ChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
					CurrentStepsP2ChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
                    CurrentStepsP3ChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
                    CurrentStepsP4ChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
                    CurrentSongChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
                    CurrentCourseChangedMessageCommand=cmd(playcommand,"UpdateAlpha");
                    
					UpdateAlphaCommand=function(self)
                        if GAMESTATE:IsHumanPlayer(player)==false then return end;
                    
                        local humanpn = #GAMESTATE:GetHumanPlayers()
                    
						--local steps = GAMESTATE:GetCurrentSteps(player):GetDifficulty();
                        local maxdefuse=0.3
                        local defuseval=maxdefuse
                        
                        --for _, p in ipairs(GAMESTATE:GetHumanPlayers()) do
                        --    if GAMESTATE:GetCurrentSteps(p):GetDifficulty() == steps then
                        --        defuseval = defuseval - maxdefuse/humanpn
                        --    end
                        --end
                        
						self:stoptweening();
                        
                        self:linear(.08);
                        self:diffusealpha(defuseval);
                        
                        if TOURNAMENTMAN:IsActive() then
                            local steps = GAMESTATE:GetCurrentSteps(PLAYER_1)
                            for pn in ivalues(GAMESTATE:GetHumanPlayers()) do
                                if pn ~= PLAYER_1 and steps ~= GAMESTATE:GetCurrentSteps(pn) then  
                                    GAMESTATE:SetCurrentSteps(pn, steps)
                                end
                            end
                        end
					end;
					PlayerJoinedMessageCommand=function(self,param )
						if param.Player ~= player then return end;
						self:visible( true );
					end;
				};
				Def.ActorFrame {
					InitCommand=cmd(x,-(162 + PlayerNumber:Reverse()[player]*-16););
					children={
						Font("mentone","24px") .. {
							InitCommand=cmd(settext,pname(player);diffuse,PlayerColor(player);shadowlength,1;zoom,0.5;shadowcolor,color("#00000044");NoStroke);
							BeginCommand=cmd(player,player);
							PlayerJoinedMessageCommand=function(self,param )
								if param.Player ~= player then return end;
								self:visible( true );
							end;
						};
					}
				};
			};
    };
end

if not GAMESTATE:IsCourseMode() then
	t[#t+1] = Def.StepsDisplayList {
		Name="StepsDisplayList";
		InitCommand=cmd(xy,(SCREEN_CENTER_X*0.75/2)+28,SCREEN_CENTER_Y*0.575);
		OffCommand=cmd(bouncebegin,0.375;addx,-SCREEN_CENTER_X*1.25);
		CurrentSongChangedMessageCommand=function(self)
			self:visible(GAMESTATE:GetCurrentSong() ~= nil);
		end;
		CursorP1 = LoadCursor(PLAYER_1);
		CursorP2 = LoadCursor(PLAYER_2);
        CursorP3 = LoadCursor(PLAYER_3);
        CursorP4 = LoadCursor(PLAYER_4);
		CursorP1Frame = Def.Actor{ };
		CursorP2Frame = Def.Actor{ };
        CursorP3Frame = Def.Actor{ };
        CursorP4Frame = Def.Actor{ };
	};
end

return t;
