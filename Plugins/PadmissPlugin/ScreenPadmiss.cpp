#include "global.h"
#include "ScreenPadmiss.h"
#include "PadmissClient.h"
#include "ProfileState.h"

#include "ProfileManager.h"
#include "OptionRowHandler.h"
#include "PluginManager.h"
#include "LocalizedString.h"
#include "GameState.h"
#include "ScreenManager.h"
#include "ScreenMiniMenu.h"
#include "ScreenDimensions.h"
#include "ScreenTextEntry.h"
#include "Preference.h"
#include "LuaManager.h"

#include "json/writer.h"

AutoScreenMessage(SM_BackFromContextMenu);
AutoScreenMessage(SM_BackFromCabName);
AutoScreenMessage(SM_BackFromLoginUsername);
AutoScreenMessage(SM_BackFromLoginPassword);
AutoScreenMessage(SM_BackFromDownloadProfile);

ScreenMessage afterLogin = SM_None;

RString lastUsername;
RString lastPassword;

// Global actions

static const char* PadmissGlobalActionNames[] =
{
	"Login",
	"Setup cab",
	"Download profile"
};

enum PadmissGlobalAction
{
	PadmissGlobalAction_Login,
	PadmissGlobalAction_SetupCab,
	PadmissGlobalAction_DownloadProfile,
	NUM_PadmissGlobalAction
};

AutoScreenMessage(SM_GlobalAction_Login);
AutoScreenMessage(SM_GlobalAction_SetupCab);
AutoScreenMessage(SM_GlobalAction_DownloadProfile);

ScreenMessage PadmissGlobalActionMessages[NUM_PadmissGlobalAction] =
{
	SM_GlobalAction_Login,
	SM_GlobalAction_SetupCab,
	SM_GlobalAction_DownloadProfile
};

XToString(PadmissGlobalAction);
#define FOREACH_PadmissGlobalAction( i ) FOREACH_ENUM( PadmissGlobalAction, i )

// Arcade cabs

struct ArcadeCab
{
	RString id;
	RString name;
	RString token;
};
vector<ArcadeCab> arcadeCabs;

void UpdateCabs()
{
	if (!PADMISS_CLIENT.IsLoggedIn())
		return;

	arcadeCabs.clear();

	Json::Value data;
	PADMISS_CLIENT.SendGraphql("{ArcadeCabs(where:{cabOwnerId:{eq:" + PADMISS_CLIENT.GetPlayerId() + "}}) {nodes {id name apiKey}}}", data);

	for (Json::Value& cab : data["ArcadeCabs"]["nodes"])
	{
		if (cab["apiKey"].asString().length() > 0)
		{
			arcadeCabs.push_back({
				cab["id"].asString(),
				cab["name"].asString(),
				cab["apiKey"].asString(),
			});
		}
	}
}

// Profile map
map<string, string> profileMap;
void UpdateProfileMap()
{
	if (PROFILEMAN->GetNumLocalProfiles() == 0)
		return;

	vector<RString>* vsProfileNames = APP_PTR(vector<RString>);
	PROFILEMAN->GetLocalProfileDisplayNames(*vsProfileNames);
	Json::Value jNames;

	int i = 0;
	for (auto& p : *vsProfileNames)
	{
		jNames.append(p);
		string padmissId = ProfileState::PadmissId(PROFILEMAN->GetLocalProfileIDFromIndex(i));
		if (padmissId.length() > 0)
		{
			profileMap.emplace(p, padmissId);
		}
		i++;
	}

	Json::FastWriter writer;
	string sNames = writer.write(jNames);

	Json::Value data;
	if (PADMISS_CLIENT.SendGraphql("{ Players(where:{ nickname: {in: " + sNames + "} }) { nodes { nickname id } } }", data) && data.isMember("Players"))
	{
		for (auto& p : data["Players"]["nodes"])
		{
			profileMap.emplace(p["nickname"].asString(), p["id"].asString());
		}
	}

	PLUGINMAN->AppDelete(vsProfileNames);
}

// Profile actions

enum PadmissProfileAction
{
	PadmissProfileAction_Link,
	NUM_PadmissProfileAction
};

static const char* PadmissProfileActionNames[] =
{
	"Link",
};

XToString(PadmissProfileAction);
#define FOREACH_PadmissProfileAction( i ) FOREACH_ENUM( PadmissProfileAction, i )

static MenuDef g_TempMenu(
	"ScreenMiniMenuContext"
);

static Preference<RString> g_padmissCabToken("PadmissCabToken", "");

ScreenPadmiss::ScreenPadmiss()
{

}

ScreenPadmiss::~ScreenPadmiss()
{
	for (OptionRowHandler* h : OptionRowHandlers)
	{
		//PLUGINMAN->AppDelete(h);
	}
}

void ScreenPadmiss::Init()
{
	ScreenOptions::Init();
}

void ScreenPadmiss::BeginScreen()
{
	//UpdateCabs();
	//UpdateProfileMap();
	
	FOREACH_PadmissGlobalAction(i)
	{
		if (i == PadmissGlobalAction_Login)
		{
			OptionRowHandler* pHand = OptionRowHandlerUtil::MakeNull();
			OptionRowDefinition& def = pHand->m_Def;
			def.m_sExplanationName = PadmissGlobalActionToString(i);

			def.m_bAllowThemeTitle = false;
			def.m_bAllowThemeItems = false;
			def.m_bOneChoiceForAllPlayers = true;
			def.m_sName = "Logged in";

			if (PADMISS_CLIENT.IsLoggedIn())
				def.m_vsChoices.push_back(PADMISS_CLIENT.GetPlayerName());
			else
				def.m_vsChoices.push_back("Login");

			OptionRowHandlers.push_back(pHand);
		}
		else if (i == PadmissGlobalAction_SetupCab)
		{
			OptionRowHandler* pHand = OptionRowHandlerUtil::MakeNull();
			OptionRowDefinition& def = pHand->m_Def;
			def.m_sName = PadmissGlobalActionToString(i);
			def.m_sExplanationName = PadmissGlobalActionToString(i);

			def.m_bAllowThemeTitle = false;
			def.m_bAllowThemeItems = false;
			def.m_bOneChoiceForAllPlayers = true;

			def.m_vsChoices.push_back("New");

			if (arcadeCabs.size() == 0 && g_padmissCabToken.Get().size() > 0)
			{
				def.m_vsChoices.push_back(PREFSMAN->m_sMachineName.Get());
			}
			else
			{
				for (auto& c : arcadeCabs)
				{
					def.m_vsChoices.push_back(c.name);
				}
			}

			OptionRowHandlers.push_back(pHand);
		}
		else
		{
			OptionRowHandler* pHand = OptionRowHandlerUtil::Make(ParseCommands(ssprintf("gamecommand;screen,%s;name,dummy", m_sName.c_str())));
			OptionRowDefinition& def = pHand->m_Def;

			def.m_bAllowThemeTitle = false;
			def.m_bAllowThemeItems = false;
			def.m_bOneChoiceForAllPlayers = true;

			def.m_sName = PadmissGlobalActionToString(i);
			def.m_sExplanationName = PadmissGlobalActionToString(i);
			OptionRowHandlers.push_back(pHand);
		}
	}

	vector<RString>* vsProfileNames = APP_PTR(vector<RString>);
	PROFILEMAN->GetLocalProfileIDs(*vsProfileNames);

	for (RString const& s : *vsProfileNames)
	{
		Profile* pProfile = PROFILEMAN->GetLocalProfile(s);
		ASSERT(pProfile != nullptr);

		//OptionRowHandler* pHand = OptionRowHandlerUtil::MakeNull();
		OptionRowHandler* pHand = OptionRowHandlerUtil::Make(ParseCommands(ssprintf("gamecommand;screen,%s;name,dummy", m_sName.c_str())));
		OptionRowDefinition& def = pHand->m_Def;

		def.m_bAllowThemeTitle = false;
		def.m_bAllowThemeItems = false;
		def.m_bOneChoiceForAllPlayers = true;

		def.m_sName = pProfile->m_sDisplayName;
		def.m_sExplanationName = "Link Profile";
		//def.m_vsChoices.push_back("Not available");

		if (profileMap.count(pProfile->m_sDisplayName))
		{
			//def.m_vsChoices.push_back("Unlinked");
			//def.m_vsChoices.push_back("Linked");
		}
		else
		{
			//def.m_vsChoices.push_back("Not available");
		}
		
		OptionRowHandlers.push_back(pHand);
	}

	ScreenOptions::InitMenu(OptionRowHandlers);
	ScreenOptions::BeginScreen();

	PLUGINMAN->AppDelete(vsProfileNames);
}

void ScreenPadmiss::ProcessMenuStart(const InputEventPlus& e)
{
	if (IsTransitioning())
		return;

	int iCurRow = m_iCurrentRow[GAMESTATE->GetMasterPlayerNumber()];
	OptionRow& row = *m_pRows[iCurRow];

	if (row.GetRowType() == OptionRow::RowType_Exit)
	{
		SCREENMAN->PlayStartSound();
		this->BeginFadingOut();
		return;
	}

	if (iCurRow < NUM_PadmissGlobalAction)
	{
		HandleScreenMessage(PadmissGlobalActionMessages[iCurRow]);
		return;
	}
	else
	{
		g_TempMenu.rows.clear();
		FOREACH_PadmissProfileAction(i)
			g_TempMenu.rows.push_back(MenuRowDef(i, PadmissProfileActionToString(i), true, EditMode_Home, false, false, 0, ""));

		int iWidth, iX, iY;
		this->GetWidthXY(PLAYER_1, iCurRow, 0, iWidth, iX, iY);
		ScreenMiniMenu::MiniMenu(&g_TempMenu, SM_BackFromContextMenu, SM_None, (float)iX, SCREEN_CENTER_Y);
	}
}

void ScreenPadmiss::HandleScreenMessage(const ScreenMessage SM)
{
	if (SM == SM_LoseFocus)
	{
		this->PlayCommand("ScreenLoseFocus");
	}
	else if (SM == SM_GainFocus)
	{
		this->PlayCommand("ScreenGainFocus");
	}
	else if (SM == SM_GlobalAction_Login)
	{
		ScreenTextEntry::TextEntry(
			SM_BackFromLoginUsername,
			"Padmiss username",
			"",
			200);
	}
	else if (SM == SM_GlobalAction_SetupCab)
	{
		if (!PADMISS_CLIENT.IsLoggedIn())
		{
			afterLogin = SM_GlobalAction_SetupCab;
			HandleScreenMessage(SM_GlobalAction_Login);
		}
		else
		{
			ScreenTextEntry::TextEntry(
				SM_BackFromCabName,
				"Cab name",
				"",
				200
			);
		}
	}
	else if (SM == SM_BackFromCabName)
	{
		if (ScreenTextEntry::s_sLastAnswer.length() > 0)
		{
			Json::Value req, resp;
			req["name"] = ScreenTextEntry::s_sLastAnswer;
			req["cabDescription"] = "Created from Padmiss Plugin";

			PADMISS_CLIENT.SendPost("/v2/v1/arcadeCabs", req, &resp);
			if (resp.isMember("id"))
			{
				Json::Value data;
				PADMISS_CLIENT.SendGraphql("{ArcadeCab(id:" + to_string(resp["id"].asInt()) + ") {name apiKey}}", data);
				UpdateCabs();
			}
			else
			{
				SCREENMAN->SystemMessage("Cab create failed");
			}
		}
	}
	else if (SM == SM_BackFromLoginUsername)
	{
		lastUsername = ScreenTextEntry::s_sLastAnswer;
		if (lastUsername.length())
		{
			ScreenTextEntry::TextEntry(
				SM_BackFromLoginPassword,
				"Padmiss password",
				"",
				200);
		}
	}
	else if (SM == SM_BackFromLoginPassword)
	{
		lastPassword = ScreenTextEntry::s_sLastAnswer;

		if (lastPassword.length())
		{
			if (PADMISS_CLIENT.Login(lastUsername, lastPassword))
			{

				SCREENMAN->SystemMessage("Logged in");
				if (afterLogin != SM_None)
				{
					HandleScreenMessage(afterLogin);
					afterLogin = SM_None;
				}
				else
				{
					BeginScreen();
					return;
				}
			}
			else
			{
				SCREENMAN->SystemMessage("Logged failed");
				afterLogin = SM_None;
			}
		}
	}
	else if (SM == SM_GlobalAction_DownloadProfile)
	{
		ScreenTextEntry::TextEntry(
			SM_BackFromDownloadProfile,
			"Player name of profile",
			"",
			200
		);
	}
	else if (SM == SM_BackFromDownloadProfile)
	{
		Json::Value player;
		RString profileId;
		string nickname = ScreenTextEntry::s_sLastAnswer;

		if (!ProfileState::GetPlayer(nickname, player, "nickname") || !ProfileState::InstallProfile(player, profileId))
		{
			SCREENMAN->SystemMessage("Profile download failed");
		}
		else
		{
			BeginScreen();
			return;
		}

	}
	
	ScreenOptions::HandleScreenMessage(SM);
}

void ScreenPadmiss::ImportOptions(int iRow, const vector<PlayerNumber>& /* vpns */)
{
	if (iRow == PadmissGlobalAction_SetupCab)
	{
		int i = -1;
		const char* token = g_padmissCabToken.Get().c_str();

		for (int c=0; c<arcadeCabs.size(); c++)
		{
			if (arcadeCabs.at(c).token.EqualsNoCase(token))
			{
				i = c;
				break;
			}
		}

		if (i != -1)
		{
			OptionRow& row = *m_pRows[iRow];
			row.SetSelected(PLAYER_1, i + 1, true);
			row.SetSelected(PLAYER_1, 0, false);
		}
	}
	else if (iRow >= NUM_PadmissGlobalAction)
	{
		int profileIndex = iRow - NUM_PadmissGlobalAction;
		if (profileIndex < PROFILEMAN->GetNumLocalProfiles())
		{
			OptionRow& row = *m_pRows[iRow];
			string padmissId = ProfileState::PadmissId(PROFILEMAN->GetLocalProfileIDFromIndex(profileIndex));
			
			if (padmissId.length() > 0 && profileMap.count(row.GetHandler()->m_Def.m_sName) > 0)
			{
				row.SetSelected(PLAYER_1, 1, true);
				row.SetSelected(PLAYER_1, 0, false);
			}
		}
	}
}

void ScreenPadmiss::ExportOptions(int iRow, const vector<PlayerNumber>& /* vpns */)
{
	if (iRow == PadmissGlobalAction_SetupCab)
	{
		OptionRow& row = *m_pRows[iRow];
		int c = row.GetChoiceInRowWithFocusShared();
		if (c > 0 && c <= arcadeCabs.size())
		{
			g_padmissCabToken.Set(arcadeCabs.at(c - 1).token);
			PREFSMAN->m_sMachineName.Set(arcadeCabs.at(c - 1).name);
		}
	}
	else if (iRow >= NUM_PadmissGlobalAction)
	{
		int profileIndex = iRow - NUM_PadmissGlobalAction;
		if (profileIndex < PROFILEMAN->GetNumLocalProfiles())
		{
			OptionRow& row = *m_pRows[iRow];
			Profile* profile = PROFILEMAN->GetLocalProfileFromIndex(profileIndex);
			int c = row.GetChoiceInRowWithFocusShared();

			if (c == 1)
			{
				if (ProfileState::PadmissId(profile).length() == 0 && profileMap.count(profile->m_sDisplayName))
				{
					ProfileState::SetPadmissId(profile, profileMap.at(profile->m_sDisplayName));
				}
			}
			else if(c == 0)
			{
				if (ProfileState::PadmissId(profile).length() > 0)
				{
					ProfileState::SetPadmissId(profile, "");
				}
			}
		}
	}
}
