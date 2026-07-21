//========= Copyright Preconomy Team, 2026. All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//


#include "cbase.h"
#include "offline_practice_dialog.h"
#include "offline_practice_serverpanel.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include "vgui_controls/PropertyDialog.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

static COfflinePracticeDialog* g_pOfflinePracticeDialog = NULL;

COfflinePracticeDialog::COfflinePracticeDialog(vgui::Panel* parent)
	: BaseClass(parent, "OfflinePracticeDialog")
{
	SetDeleteSelfOnClose(true);
	SetSizeable(false);
	SetMoveable(true);

	SetSize(320, 480);

	SetTitle("#TF_OfflinePractice", true);

	SetOKButtonText("#GameUI_Start");

	m_pServerPage = new COfflinePracticeServerPanel(this, "ServerPage");
	//AddPage(m_pServerPage, "#TF_OfflinePractice_Settings");

	SetApplyButtonVisible(false);
	SetCancelButtonVisible(true);
}


COfflinePracticeDialog::~COfflinePracticeDialog()
{
	if (g_pOfflinePracticeDialog == this)
	{
		g_pOfflinePracticeDialog = NULL;
	}
}

void COfflinePracticeDialog::OnCommand(const char* command)
{
	if (!V_stricmp(command, "OK") || !V_stricmp(command, "Apply"))
	{
		if (m_pServerPage && m_pServerPage->StartServer())
		{
			Close();
		}
		return;
	}

	BaseClass::OnCommand(command);
}

void COfflinePracticeDialog::Open()
{
	InvalidateLayout(false, true);
	SetVisible(true);
	MoveToFront();
	RequestFocus();
	MoveToCenterOfScreen();
}

void ShowOfflinePracticeDialog()
{
	if (!g_pOfflinePracticeDialog)
	{
		g_pOfflinePracticeDialog = new COfflinePracticeDialog(NULL);
	}
	g_pOfflinePracticeDialog->Open();
}

CON_COMMAND_F(OpenOfflinePracticeDialog, "Displays the offline practice dialog.", FCVAR_DONTRECORD)
{
	ShowOfflinePracticeDialog();
}
