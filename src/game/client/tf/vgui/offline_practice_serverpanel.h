//========= Copyright Preconomy Team, 2026. All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef OFFLINE_PRACTICE_SERVERPANEL_H
#define OFFLINE_PRACTICE_SERVERPANEL_H
#ifdef _WIN32
#pragma once
#endif
#include "vgui_controls/PropertyPage.h"

class KeyValues;

namespace vgui
{
	class ComboBox;
	class ImagePanel;
	class Label;
	class RichText;
	class TextEntry;
	class Slider;
	class CheckButton;
}

class COfflinePracticeServerPanel : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COfflinePracticeServerPanel, vgui::PropertyPage);
public:
	COfflinePracticeServerPanel(vgui::Panel* parent, const char* panelName);
	virtual ~COfflinePracticeServerPanel();

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnCommand(const char* command);
	virtual void OnPageShow(void);

	bool StartServer();
	
	MESSAGE_FUNC(OnMapSelectionChanged, "TextChanged");

private:
	void LoadMapList();
	void LoadMapDescriptionFromFile(const char* pMapName, const char* pGametypeToken);
	const char* GetGameTypeForMap(const char* pMapName, KeyValues* pMapData);
	void UpdateSelectedMapDisplay();
	void SaveOfflinePracticeConfig(int nBotQuota, int nBotDifficulty, bool bRequestCoach);

	static int GetDifficultyIndexForName(const char* pDifficultyName);

	KeyValues* GetSelectedMapData();

	vgui::ComboBox* m_pMapList;
	vgui::Panel* m_pMapImageContainer;
	vgui::ImagePanel* m_pMapImage;
	vgui::Label* m_pGametypeLabel;
	vgui::RichText* m_pMapDescription;
	vgui::Label* m_pSuggestedPlayersLabel;
	vgui::TextEntry* m_pPlayerCountEntry;
	vgui::Slider* m_pDifficultySlider;
	vgui::CheckButton* m_pRequestCoachCheckbox;


	int		m_nMinPlayers;
	int		m_nMaxPlayers;
	bool	m_bWaitingForServer;

	KeyValues* m_pOfflinePracticeConfig;
	KeyValues* m_pOfflinePracticeRes;
};

#endif // OFFLINE_PRACTICE_SERVERPANEL_H