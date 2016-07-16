--Reset all player mods when entering screen
if ThemePrefs.Get("OpenEvent") then
    for pn, p in ipairs(GAMESTATE:GetHumanPlayers()) do
        GAMESTATE:GetPlayerState(p):SetPlayerOptions('ModsLevel_Preferred', 'clearall')
    end
end

if TOURNAMENTMAN:IsActive() then
    local steps = GAMESTATE:GetCurrentSteps(PLAYER_1)
    for pn, p in ipairs(GAMESTATE:GetHumanPlayers()) do
        GAMESTATE:SetCurrentSteps(p, steps)
    end
end

return LoadActor(THEME:GetPathB("","_wipeIn"),0.5);