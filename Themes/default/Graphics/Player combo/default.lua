-- todo: clean this the fuck up
local c;
local player = Var "Player";
local ShowComboAt = THEME:GetMetric("Combo", "ShowComboAt");
local Pulse = THEME:GetMetric("Combo", "PulseCommand");
local PulseLabel = THEME:GetMetric("Combo", "PulseLabelCommand");

local NumberMinZoom = 0.8;
local NumberMaxZoom = 1;
local NumberMaxZoomAt = 100;

local LabelMinZoom = THEME:GetMetric("Combo", "LabelMinZoom");
local LabelMaxZoom = THEME:GetMetric("Combo", "LabelMaxZoom");

local ShowFlashyCombo = player_config:get_data(player).FlashyCombo

local t = Def.ActorFrame {
	InitCommand= function(self)
		if player_config:get_data(player).ComboUnderField then
			self:draworder(notefield_draw_order.under_field)
		else
			self:draworder(notefield_draw_order.over_field)
		end
		c = self:GetChildren();
		cf = c.ComboFrame:GetChildren();
		cf.Number:visible(false);
		cf.ComboLabel:visible(false)
		cf.MissLabel:visible(false)
	end,
	-- flashy combo elements:
	LoadActor(THEME:GetPathG("Combo","100Milestone"), player) .. {
		Name="OneHundredMilestone";
		InitCommand=cmd(visible,ShowFlashyCombo);
		FiftyMilestoneCommand=cmd(playcommand,"Milestone");
	};
	LoadActor(THEME:GetPathG("Combo","1000Milestone")) .. {
		Name="OneThousandMilestone";
		InitCommand=cmd(visible,ShowFlashyCombo);
		ToastyAchievedMessageCommand=cmd(playcommand,"Milestone");
local t = Def.ActorFrame {
	InitCommand=cmd(vertalign,bottom);
	LoadFont( "Combo", "numbers" ) .. {
		Name="Number";
		OnCommand = THEME:GetMetric("Combo", "NumberOnCommand");
	};
	LoadFont("Common Normal") .. {
		Name="Label";
		OnCommand = THEME:GetMetric("Combo", "LabelOnCommand");
	};
	-- Milestones:
	-- 25,50,100,250,600 Multiples;
--[[ 		if (iCombo % 100) == 0 then
			c.OneHundredMilestone:playcommand("Milestone");
		elseif (iCombo % 250) == 0 then
			-- It should really be 1000 but thats slightly unattainable, since
			-- combo doesnt save over now.
			c.OneThousandMilestone:playcommand("Milestone");
		else
			return
		end; --]]
 	TwentyFiveMilestoneCommand=function(self,parent)
		if ShowFlashyCombo then
			(cmd(finishtweening;addy,-4;bounceend,0.125;addy,4))(self);
		end;

	InitCommand = function(self)
		c = self:GetChildren();
		c.Number:visible(false);
		c.Label:visible(false);
	end;
	ToastyAchievedMessageCommand=function(self,params)
		if params.PlayerNumber == player then
			(cmd(thump,2;effectclock,'beat'))(self);
		end;
	end;
	ComboCommand=function(self, param)
		local iCombo = param.Misses or param.Combo;
		if not iCombo or iCombo < ShowComboAt then
			c.Number:visible(false);
			c.Label:visible(false);
			return;
		end

		local labeltext = "";
		if param.Combo then
			labeltext = "Combo";
-- 			c.Number:playcommand("Reset");
		else
			labeltext = "Combo";
-- 			c.Number:playcommand("Miss");
		end
		c.Label:settext( labeltext );
		c.Label:visible(false);

		param.Zoom = scale( iCombo, 0, NumberMaxZoomAt, NumberMinZoom, NumberMaxZoom );
		param.Zoom = clamp( param.Zoom, NumberMinZoom, NumberMaxZoom );

		param.LabelZoom = scale( iCombo, 0, NumberMaxZoomAt, LabelMinZoom, LabelMaxZoom );
		param.LabelZoom = clamp( param.LabelZoom, LabelMinZoom, LabelMaxZoom );

		c.Number:visible(true);
		c.Label:visible(true);
		c.Label:diffusecolor(PlayerColor(player));
		c.Number:settext( string.format("%i", iCombo) );

		-- FullCombo Rewards
		if param.FullComboW1 then
			c.Number:diffuse(color("#00aeef"));
			c.Number:glowshift();
		elseif param.FullComboW2 then
			c.Number:diffuse(color("#fff568"));
			c.Number:glowshift();
		elseif param.FullComboW3 then
			c.Number:diffuse(color("#a4ff00"));
			c.Number:stopeffect();
		elseif param.Combo then
			c.Number:diffuse(PlayerColor(player));
			c.Number:strokecolor(Brightness(PlayerColor(player),0.5));
			c.Number:stopeffect();
			(cmd(diffuse,PlayerColor(player);))(c.Label);
		else
			c.Number:diffuse(color("#ff0000"));
			c.Number:strokecolor(color("#880000"));
			c.Number:stopeffect();
			(cmd(diffuse,Color("Red");))(c.Label);
		end

		-- Pulse
		Pulse( c.Number, param );
	end;
};

return t;
