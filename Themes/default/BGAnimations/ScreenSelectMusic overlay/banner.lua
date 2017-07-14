local banner_directory = ThemePrefs.Get("VisualTheme")

local t = Def.ActorFrame{
	OnCommand=function(self)
		self:zoom(0.7)
		self:xy(_screen.w*0.18, _screen.h*0.13)
	end,

	LoadActor("colored_banners/".. banner_directory .."/banner"..SL.Global.ActiveColorIndex.." (doubleres).png" )..{
		Name="FallbackBanner",
		OnCommand=cmd(rotationy,180; setsize,418,164; diffuseshift; effectoffset,3; effectperiod, 6; effectcolor1, 1,1,1,0; effectcolor2, 1,1,1,1)
	},

	LoadActor("colored_banners/".. banner_directory .."/banner"..SL.Global.ActiveColorIndex.." (doubleres).png" )..{
		Name="FallbackBanner",
		OnCommand=cmd(diffuseshift; effectperiod, 6; effectcolor1, 1,1,1,0; effectcolor2, 1,1,1,1; setsize, 418,164)
	},

	Def.ActorProxy{
		Name="BannerProxy",
		BeginCommand=function(self)
			local banner = SCREENMAN:GetTopScreen():GetChild('Banner')
			self:SetTarget(banner)
		end
	}
}

return t