//========= Copyright Preconomy Team, 2026. All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef OFFLINE_PRACTICE_DIALOG_H
#define OFFLINE_PRACTICE_DIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyDialog.h>

class COfflinePracticeServerPanel;

class COfflinePracticeDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(COfflinePracticeDialog, vgui::PropertyDialog);

public:
	COfflinePracticeDialog(vgui::Panel* pParent);
	virtual ~COfflinePracticeDialog();
	virtual void OnCommand(const char* command);
	void Open();

private:
	COfflinePracticeServerPanel *m_pServerPage;
};

#endif // OFFLINE_PRACTICE_DIALOG_H