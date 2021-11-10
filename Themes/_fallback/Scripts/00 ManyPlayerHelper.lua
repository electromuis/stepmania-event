local playerNumberReverse = PlayerNumber:Reverse()

function GetNumPlayers()
	local style = GAMESTATE:GetCurrentStyle()
	if not style then return 0 end

	local NumPlayersAndSides = ToEnumShortString( style:GetStyleType() )
	
	if NumPlayersAndSides == "FourPlayersFourSides" then
		return 4
	end
	
	if NumPlayersAndSides == "SixPlayersSixSides" then
		return 6
	end
	
	if NumPlayersAndSides == "EightPlayersEightSides" then
		return 8
	end
	
	return 2
end

function GetPlayFieldZoom()
	local numPlayers = GetNumPlayers()
	
	if numPlayers == 4 then
		return 0.8
	end
	
	if numPlayers == 6 then
		return 0.7
	end
	
	if numPlayers == 8 then
		return 0.5
	end
	
	return 1
end

function PlayFieldWidth()
	return _screen.w / GetNumPlayers()
end

function GetPlayerPlayfieldX(player)
	local fieldWidth = PlayFieldWidth()
	
	return fieldWidth * player + (fieldWidth / 2)
end

function GetPlayerMetric(p1value, group, metric, player)
	local playerNumber = playerNumberReverse[player] + 1
	
	if metric == "PlayerP#FourPlayersFourSidesX" then
		return GetPlayerPlayfieldX(playerNumber - 1)
	end
	
	if metric == "PlayerP#SixPlayersSixSidesX" then
		return GetPlayerPlayfieldX(playerNumber - 1)
	end
	
	if metric == "PlayerP#EightPlayersEightSidesX" then
		return GetPlayerPlayfieldX(playerNumber - 1)
	end
	
	if playerNumber > 2 then
		playerNumber = 1
	end
	
	return p1value
end