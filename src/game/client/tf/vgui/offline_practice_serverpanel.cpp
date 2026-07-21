//========= Copyright Preconomy Team, 2026. All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "offline_practice_serverpanel.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/RichText.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Slider.h"
#include "vgui_controls/CheckButton.h"
#include "vgui/ISurface.h"
#include "filesystem.h"
#include "KeyValues.h"
#include "tier1/convar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

static const char* s_pDifficultyNames[] =
{
	"easy",
	"normal",
	"hard",
};

static const char* s_pDifficultyTickCaptions[] =
{
	"#TF_Easy",
	"#TF_Normal",
	"#TF_Hard",
};

COfflinePracticeServerPanel::COfflinePracticeServerPanel(vgui::Panel* pParent, const char* pName)
	: BaseClass(pParent, pName)
{
	m_pMapList = NULL;
	m_pMapImage = NULL;
	m_pGametypeLabel = NULL;
	m_pMapDescription = NULL;
	m_pSuggestedPlayersLabel = NULL;
	m_pPlayerCountEntry = NULL;
	m_pDifficultySlider = NULL;
	m_pRequestCoachCheckbox = NULL;

	m_nMinPlayers = 0;
	m_nMaxPlayers = 32;
	m_bWaitingForServer = false;

	m_pOfflinePracticeConfig = NULL;
	m_pOfflinePracticeRes = NULL;

	m_pMapList = new ComboBox(this, "MapList", 12, false);
	m_pMapImageContainer = new Panel(this, "MapImageContainer");
	m_pGametypeLabel = new Label(this, "MapTypeLabel", "");
	m_pMapDescription = new RichText(this, "MapDescription");
	m_pSuggestedPlayersLabel = new Label(this, "MapRecommendedNumPlayersLabel", "");
	m_pPlayerCountEntry = new TextEntry(this, "BotQuotaCombo");
	m_pDifficultySlider = new Slider(this, "BotDifficultySlider");
	m_pDifficultySlider->SetRange(0, ARRAYSIZE(s_pDifficultyNames) - 1);
	m_pDifficultySlider->SetNumTicks(ARRAYSIZE(s_pDifficultyNames) - 1);

	m_pRequestCoachCheckbox = new CheckButton(this, "RequestCoachCheckbox", "#TF_RequestCoach");
	m_pRequestCoachCheckbox->SetVisible(false);

	m_pMapList->AddActionSignalTarget(this);

	LoadControlSettings("resource/CreateOfflinePracticePage.res");

	if (m_pMapImageContainer)
	{
		m_pMapImage = new ImagePanel(m_pMapImageContainer, "MapImage");
		m_pMapImage->SetBounds(0, 0, m_pMapImageContainer->GetWide(), m_pMapImageContainer->GetTall());
	}

	LoadMapList();
}


COfflinePracticeServerPanel::~COfflinePracticeServerPanel()
{
	if (m_pOfflinePracticeConfig)
	{
		m_pOfflinePracticeConfig->deleteThis();
	}

	if (m_pOfflinePracticeRes)
	{
		m_pOfflinePracticeRes->deleteThis();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::OnPageShow(void)
{
	SetVisible(true);

	BaseClass::OnPageShow();

	LoadMapList();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

int COfflinePracticeServerPanel::GetDifficultyIndexForName(const char* pDifficultyName)
{
	for (int i = 0; i < ARRAYSIZE(s_pDifficultyNames); i++)
	{
		if (!V_stricmp(s_pDifficultyNames[i], pDifficultyName))
			return i;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::LoadMapList()
{
	if (!m_pMapList)
		return;

	char szLastMap[MAX_PATH] = { 0 };
	int nLastQuota = -1;
	int nLastDifficulty = -1;
	bool bLastRequestCoach = false;
	bool bHaveSavedConfig = false;

	if (!m_pOfflinePracticeConfig)
	{
		m_pOfflinePracticeConfig = new KeyValues("OfflinePracticeConfig");
	}

	if (m_pOfflinePracticeConfig->LoadFromFile(filesystem, "OfflinePracticeConfig.vdf", "MOD"))
	{
		V_strncpy(szLastMap, m_pOfflinePracticeConfig->GetString("map", ""), sizeof(szLastMap));
		nLastQuota = m_pOfflinePracticeConfig->GetInt("bot_quota", -1);
		nLastDifficulty = m_pOfflinePracticeConfig->GetInt("bot_difficulty", -1);
		bLastRequestCoach = m_pOfflinePracticeConfig->GetInt("request_coach", 0) != 0;
		bHaveSavedConfig = true;

			char szCfgPath[MAX_PATH];
	filesystem->RelativePathToFullPath("OfflinePracticeConfig.vdf", "MOD", szCfgPath, sizeof(szCfgPath));
	Msg("OfflinePracticeConfig.vdf FOUND at: %s (map='%s')\n", szCfgPath, szLastMap);
	}


	m_pMapList->DeleteAllItems();

	if (m_pOfflinePracticeRes)
	{
		m_pOfflinePracticeRes->deleteThis();
		m_pOfflinePracticeRes = NULL;
	}

	m_pOfflinePracticeRes = new KeyValues("offline_practice.res");
	if (!m_pOfflinePracticeRes->LoadFromFile(filesystem, "resource/offline_practice.res", "MOD"))
	{
		return;
	}


	KeyValues* pDefaults = m_pOfflinePracticeRes->FindKey("defaults");
	int nDefaultQuota = 16;
	int nDefaultDifficulty = 0;
	const char* pDefaultMap = "";
	if (pDefaults)
	{
		m_nMaxPlayers = pDefaults->GetInt("max_players", 32);
		m_nMinPlayers = MAX(0, pDefaults->GetInt("min_players", 0));
		nDefaultQuota = pDefaults->GetInt("suggested_players", 16);
		nDefaultDifficulty = GetDifficultyIndexForName(pDefaults->GetString("difficulty", "easy"));
		if (nDefaultDifficulty == -1)
			nDefaultDifficulty = 0;
		pDefaultMap = pDefaults->GetString("map", "");
	}

	if (!bHaveSavedConfig)
	{
		V_strncpy(szLastMap, pDefaultMap, sizeof(szLastMap));
		nLastQuota = nDefaultQuota;
		nLastDifficulty = nDefaultDifficulty;
		bLastRequestCoach = false;
	}

	int nSelectedItemID = -1;

	KeyValues* pMaps = m_pOfflinePracticeRes->FindKey("maps");
	if (pMaps)
	{
		for (KeyValues* pMap = pMaps->GetFirstSubKey(); pMap != NULL; pMap = pMap->GetNextKey())
		{
			const char* pMapName = pMap->GetName();

			int nItemID = m_pMapList->AddItem(pMapName, pMap);

			if (szLastMap[0] && !V_stricmp(pMapName, szLastMap))
			{
				nSelectedItemID = nItemID;
			}
		}
	}

	if (nSelectedItemID == -1 && m_pMapList->GetItemCount() > 0)
	{
		nSelectedItemID = 0;
	}

	if (nSelectedItemID != -1)
	{
		m_pMapList->ActivateItem(nSelectedItemID);
	}
	Msg("Final szLastMap='%s' nSelectedItemID=%d\n", szLastMap, nSelectedItemID);
	UpdateSelectedMapDisplay();

	if (nLastQuota > 0 && m_pPlayerCountEntry)
	{
		char szQuota[16];
		V_snprintf(szQuota, sizeof(szQuota), "%d", clamp(nLastQuota, m_nMinPlayers, m_nMaxPlayers));
		m_pPlayerCountEntry->SetText(szQuota);
	}

	if (nLastDifficulty >= 0 && m_pDifficultySlider)
	{
		m_pDifficultySlider->SetValue(nLastDifficulty);
	}

	if (m_pRequestCoachCheckbox)
	{
		m_pRequestCoachCheckbox->SetSelected(bLastRequestCoach);
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::LoadMapDescriptionFromFile(const char* pMapName, const char* pGametypeToken)
{
	if (!m_pMapDescription || !pMapName || !pMapName[0])
		return;

	char szUILanguage[64];
	engine->GetUILanguage(szUILanguage, sizeof(szUILanguage));

	char szPath[MAX_PATH];
	V_snprintf(szPath, sizeof(szPath), "maps/%s_%s.txt", pMapName, szUILanguage);

	if (!filesystem->FileExists(szPath, "GAME"))
	{
		V_snprintf(szPath, sizeof(szPath), "maps/%s_english.txt", pMapName);

		if (!filesystem->FileExists(szPath, "GAME"))
		{
			V_snprintf(szPath, sizeof(szPath), "maps/%s.txt", pMapName);
		}
	}

	if (!filesystem->FileExists(szPath, "GAME"))
	{
		if (filesystem->FileExists("maps/default.txt", "GAME"))
		{
			V_strncpy(szPath, "maps/default.txt", sizeof(szPath));
		}
		else
		{
			const char* pDefaultFile = NULL;

			if (pGametypeToken)
			{
				if (!V_stricmp(pGametypeToken, "#Gametype_CP"))
					pDefaultFile = "maps/default_cp.txt";
				else if (!V_stricmp(pGametypeToken, "#Gametype_Escort"))
					pDefaultFile = "maps/default_payload.txt";
				else if (!V_stricmp(pGametypeToken, "#Gametype_EscortRace"))
					pDefaultFile = "maps/default_payload_race.txt";
				else if (!V_stricmp(pGametypeToken, "#Gametype_CTF"))
					pDefaultFile = "maps/default_ctf.txt";
				else if (!V_stricmp(pGametypeToken, "#Gametype_Koth"))
					pDefaultFile = "maps/default_koth.txt";
				else if (!V_stricmp(pGametypeToken, "#Gametype_Arena"))
					pDefaultFile = "maps/default_arena.txt";
			}

			if (!pDefaultFile || !filesystem->FileExists(pDefaultFile, "GAME"))
			{
				return;
			}

			V_strncpy(szPath, pDefaultFile, sizeof(szPath));
		}
	}

	FileHandle_t hFile = filesystem->Open(szPath, "rb", "GAME");
	if (!hFile)
		return;

	int nSize = filesystem->Size(hFile);
	int nBufSize = nSize + 2;
	if ((nBufSize & 1) != 0)
	{
		nBufSize++;
	}

	byte* pBuffer = (byte*)malloc(nBufSize);
	memset(pBuffer, 0, nBufSize);

	int nRead = filesystem->Read(pBuffer, nSize, hFile);
	filesystem->Close(hFile);

	if (nRead < nSize)
	{
		pBuffer[nRead] = 0;
		if (nRead + 1 < nBufSize)
			pBuffer[nRead + 1] = 0;
	}
	pBuffer[nBufSize - 2] = 0;
	pBuffer[nBufSize - 1] = 0;

	unsigned short* pWide = (unsigned short*)pBuffer;
	if (pWide[0] == 0xFEFF)
	{
		wchar_t* pText = (wchar_t*)(pWide + 1);
		for (wchar_t* p = pText; *p != 0; p++)
		{
			if (*p == L'\r')
				*p = L' ';
		}
		m_pMapDescription->SetText(pText);
	}
	else
	{
		char* pText = (char*)pBuffer;
		for (char* p = pText; *p != '\0'; p++)
		{
			if (*p == '\r')
				*p = ' ';
		}
		m_pMapDescription->SetText(pText);
	}

	free(pBuffer);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char* COfflinePracticeServerPanel::GetGameTypeForMap(const char* pMapName, KeyValues* pMapData)
{
	if (!pMapName || !pMapName[0])
		return NULL;

	struct MapNameOverride_t
	{
		const char* pMapName;
		const char* pToken;
	};

	// handle players adding in custom offline practice
	static const MapNameOverride_t s_MapOverrides[] =
	{
		{ "ctf_2fort",			"#Gametype_CTF" },
		{ "cp_dustbowl",		"#TF_AttackDefend" },
		{ "cp_granary",			"#Gametype_CP" },
		{ "cp_well",			"#Gametype_CP" },
		{ "cp_gravelpit",		"#TF_AttackDefend" },
		{ "tc_hydro",			"#TF_TerritoryControl" },
		{ "ctf_well",			"#Gametype_CTF" },
		{ "cp_badlands",		"#Gametype_CP" },
		{ "pl_goldrush",		"#Gametype_Escort" },
		{ "pl_badwater",		"#Gametype_Escort" },
		{ "plr_pipeline",		"#Gametype_EscortRace" },
		{ "cp_gorge",			"#TF_AttackDefend" },
		{ "ctf_doublecross",	"#Gametype_CTF" },
		{ "pl_thundermountain", "#Gametype_Escort" },
		{ "tr_target",			"#GameType_Training" },
		{ "tr_dustbowl",		"#GameType_Training" },
	};

	for (int i = 0; i < ARRAYSIZE(s_MapOverrides); i++)
	{
		if (!V_stricmp(pMapName, s_MapOverrides[i].pMapName))
			return s_MapOverrides[i].pToken;
	}

	// map prefix check
	if (!V_strnicmp(pMapName, "cp_", 3))
		return "#Gametype_CP";

	if (!V_strnicmp(pMapName, "tc_", 3))
		return "#TF_TerritoryControl";

	if (!V_strnicmp(pMapName, "pl_", 3))
		return "#Gametype_Escort";

	if (!V_strnicmp(pMapName, "plr_", 4))
		return "#Gametype_EscortRace";

	if (!V_strnicmp(pMapName, "ctf_", 4))
		return "#Gametype_CTF";

	if (!V_strnicmp(pMapName, "koth_", 5))
		return "#Gametype_Koth";

	if (!V_strnicmp(pMapName, "arena_", 6))
		return "#Gametype_Arena";

	if (pMapData)
	{
		const char* pOverride = pMapData->GetString("gametype", NULL);
		if (pOverride && pOverride[0])
			return pOverride;
	}

	return "";
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::UpdateSelectedMapDisplay()
{
	KeyValues* pMapData = GetSelectedMapData();
	if (!pMapData)
		return;

	if (m_pMapImage)
	{
		const char* pImagePath = pMapData->GetString("image", NULL);
		char szThumbVmt[MAX_PATH];
		V_snprintf(szThumbVmt, sizeof(szThumbVmt), "materials/vgui/maps/menu_thumb_%s.vmt", pMapData->GetName());

		const char* pThumbMapName = pMapData->GetName();
		if (!pImagePath && !filesystem->FileExists(szThumbVmt, "MOD"))
		{
			pThumbMapName = "default";
		}

		char szThumbImage[MAX_PATH];
		if (pImagePath)
		{
			V_strncpy(szThumbImage, pImagePath, sizeof(szThumbImage));
		}
		else
		{
			V_snprintf(szThumbImage, sizeof(szThumbImage), "../vgui/maps/menu_thumb_%s", pThumbMapName);
		}

		m_pMapImage->SetImage(szThumbImage);
	}

	const char* pGametypeToken = GetGameTypeForMap(pMapData->GetName(), pMapData);
	const wchar_t* pLocalizedGametype = L"";
	if (pGametypeToken && pGametypeToken[0] && g_pVGuiLocalize)
	{
		const wchar_t* pFound = g_pVGuiLocalize->Find(pGametypeToken);
		if (pFound)
		{
			pLocalizedGametype = pFound;
		}
	}
	SetDialogVariable("gametype", pLocalizedGametype);

	LoadMapDescriptionFromFile(pMapData->GetName(), pGametypeToken);

	int nMapMin = pMapData->GetInt("min_players", m_nMinPlayers);
	int nMapMax = pMapData->GetInt("max_players", m_nMaxPlayers);
	
	int nGlobalSuggested = nMapMax;
	if (m_pOfflinePracticeRes)
	{
		KeyValues* pDefaults = m_pOfflinePracticeRes->FindKey("defaults");
		if (pDefaults)
		{
			nGlobalSuggested = pDefaults->GetInt("suggested_players", nMapMax);
		}
	}

	int nSuggested = pMapData->GetInt("suggested_players", nGlobalSuggested);

	if (m_pSuggestedPlayersLabel)
	{
		char szSuggested[64];
		V_snprintf(szSuggested, sizeof(szSuggested), "%d-%d Suggested", nMapMin, nMapMax);
		m_pSuggestedPlayersLabel->SetText(szSuggested);
	}

	if (m_pPlayerCountEntry)
	{
		char szCount[16];
		V_snprintf(szCount, sizeof(szCount), "%d", clamp(nSuggested, nMapMin, nMapMax));
		m_pPlayerCountEntry->SetText(szCount);
	}

	if (m_pDifficultySlider)
	{
		const char* pDefaultDifficulty = pMapData->GetString("difficulty", NULL);
		if (pDefaultDifficulty && pDefaultDifficulty[0])
		{
			int nIndex = GetDifficultyIndexForName(pDefaultDifficulty);
			if (nIndex != -1)
			{
				m_pDifficultySlider->SetValue(nIndex);
			}
		}
	}

	m_nMinPlayers = nMapMin;
	m_nMaxPlayers = nMapMax;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::OnMapSelectionChanged()
{
	UpdateSelectedMapDisplay();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
KeyValues* COfflinePracticeServerPanel::GetSelectedMapData()
{
	if (!m_pMapList)
		return NULL;

	int nActiveItem = m_pMapList->GetActiveItem();
	if (nActiveItem < 0)
		return NULL;

	return m_pMapList->GetItemUserData(nActiveItem);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COfflinePracticeServerPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool COfflinePracticeServerPanel::StartServer()
{
	KeyValues* pMapData = GetSelectedMapData();
	if (!pMapData)
		return false;

	const char* pMapName = pMapData->GetName();
	if (!pMapName || !pMapName[0])
		return false;

	int nBotQuota = m_nMaxPlayers;
	if (m_pPlayerCountEntry)
	{
		char szText[16];
		m_pPlayerCountEntry->GetText(szText, sizeof(szText));
		nBotQuota = atoi(szText);
	}
	nBotQuota = clamp(nBotQuota, m_nMinPlayers, m_nMaxPlayers);

	int nBotDifficulty = m_pDifficultySlider ? m_pDifficultySlider->GetValue() : 1;
	nBotDifficulty = clamp(nBotDifficulty, 0, ARRAYSIZE(s_pDifficultyNames) - 1);

	bool bRequestCoach = m_pRequestCoachCheckbox ? m_pRequestCoachCheckbox->IsSelected() : false;

	static ConVarRef tf_bot_quota("tf_bot_quota");
	static ConVarRef tf_bot_quota_mode("tf_bot_quota_mode");
	static ConVarRef tf_bot_auto_vacate("tf_bot_auto_vacate");
	static ConVarRef tf_bot_difficulty("tf_bot_difficulty");
	static ConVarRef tf_bot_offline_practice("tf_bot_offline_practice");

	if (!tf_bot_quota.IsValid() || !tf_bot_difficulty.IsValid() || !tf_bot_offline_practice.IsValid())
	{
		return false;
	}

	tf_bot_quota.SetValue(nBotQuota);
	tf_bot_quota_mode.SetValue("normal");
	tf_bot_auto_vacate.SetValue(0);
	tf_bot_difficulty.SetValue(nBotDifficulty);
	tf_bot_offline_practice.SetValue(1);

	char szRequestCoach[32];
	V_snprintf(szRequestCoach, sizeof(szRequestCoach), "request_coach %d", bRequestCoach ? 1 : 0);
	engine->ClientCmd_Unrestricted(szRequestCoach);

	SaveOfflinePracticeConfig(nBotQuota, nBotDifficulty, bRequestCoach);

	char szCommand[MAX_PATH];
	V_snprintf(szCommand, sizeof(szCommand), "map %s\n", pMapName);
	engine->ClientCmd_Unrestricted(szCommand);

	m_bWaitingForServer = true;
	return true;
}

void COfflinePracticeServerPanel::SaveOfflinePracticeConfig(int nBotQuota, int nBotDifficulty, bool bRequestCoach)
{
	KeyValues* pKV = new KeyValues("OfflinePracticeConfig");
	pKV->SetInt("bot_quota", nBotQuota);
	pKV->SetInt("bot_difficulty", nBotDifficulty);
	pKV->SetInt("request_coach", bRequestCoach ? 1 : 0);

	KeyValues* pMapData = GetSelectedMapData();
	if (pMapData)
	{
		pKV->SetString("map", pMapData->GetName());
	}

	pKV->SaveToFile(filesystem, "OfflinePracticeConfig.vdf", "MOD");
	pKV->deleteThis();
}
