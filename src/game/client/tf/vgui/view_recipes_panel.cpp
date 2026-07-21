//========= Copyright Pre-conomy Team, 2026. All rights reserved. ============//
//
// Purpose: Restores authentic 2010 menu functionality for "View all Known Blueprints" 
//			in the crafting panel
//
//=============================================================================//

#include "cbase.h"
#include "view_recipes_panel.h"
#include "tf_item_inventory.h"
#include "econ_item_system.h"
#include "econ_ui.h"
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

extern void SetItemPanelToRecipe(CItemModelPanel* pPanel, const CEconCraftingRecipeDefinition* pRecipeDef, bool bShowName);
extern void PositionMouseOverPanelForRecipe(vgui::Panel* pScissorPanel, vgui::Panel* pRecipePanel, vgui::ScrollableEditablePanel* pRecipeScroller, CItemModelPanel* pMouseOverItemPanel);

CViewRecipePanel::CViewRecipePanel(vgui::Panel* parent, const char* panelName)
	: BaseClass(parent, "ViewRecipesPanel")
{
	m_pRecipeContainer = new vgui::EditablePanel(this, "recipecontainer");
	m_pRecipeContainerScroller = new vgui::ScrollableEditablePanel(this, m_pRecipeContainer, "recipecontainerscroller");
	m_pMouseOverItemPanel = new CItemModelPanel(this, "mouseoveritempanel");
	m_pRecipeKV = NULL;
	m_pCheckmarkKV = NULL;
}

CViewRecipePanel::~CViewRecipePanel(void)
{
	if (m_pRecipeKV) { 
		m_pRecipeKV->deleteThis();   
		m_pRecipeKV = NULL; 
	}
	if (m_pCheckmarkKV) { 
		m_pCheckmarkKV->deleteThis(); 
		m_pCheckmarkKV = NULL; 
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewRecipePanel::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings( inResourceData );

	KeyValues* pRecipeKV = inResourceData->FindKey("recipeskv");
	if (pRecipeKV)
	{
		if (m_pRecipeKV)
		{ 
			m_pRecipeKV->deleteThis();
		}
		m_pRecipeKV = new KeyValues("recipeskv");
		pRecipeKV->CopySubkeys( m_pRecipeKV );
	}

	KeyValues* pCheckKV = inResourceData->FindKey("checkmarkskv");
	if (pCheckKV)
	{
		if (m_pCheckmarkKV) 
		{
			m_pCheckmarkKV->deleteThis();
		}
		m_pCheckmarkKV = new KeyValues("checkmarkskv");
		pCheckKV->CopySubkeys( m_pCheckmarkKV );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewRecipePanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("Resource/UI/ViewRecipesPanel.res");

	m_pMouseOverItemPanel->SetVisible(false);
	Populate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewRecipePanel::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "ok"))
	{
		TFModalStack()->PopModal(this);
		SetVisible(false);
		MarkForDeletion();
		EconUI()->SetPreventClosure(false);
		return;
	}
	BaseClass::OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewRecipePanel::Populate(void)
{
	FOR_EACH_VEC(m_pRecipeButtons, i)
		m_pRecipeButtons[i]->MarkForDeletion();
	m_pRecipeButtons.RemoveAll();

	FOR_EACH_VEC(m_pCheckmarks, i)
		m_pCheckmarks[i]->MarkForDeletion();
	m_pCheckmarks.RemoveAll();

	int iRowHeight = m_pRecipeKV ? m_pRecipeKV->GetInt("tall", 20) : 20;
	int iCount = 0;

	for (int i = 0; i < TFInventoryManager()->GetLocalTFInventory()->GetRecipeCount(); i++)
	{
		const CEconCraftingRecipeDefinition* pRecipeDef = TFInventoryManager()->GetLocalTFInventory()->GetRecipeDef(i);
		if (!pRecipeDef || pRecipeDef->IsDisabled())
			continue;

		wchar_t wTemp[256];
		wchar_t* pName_A = g_pVGuiLocalize->Find(pRecipeDef->GetName_A());
		g_pVGuiLocalize->ConstructString_safe(wTemp, g_pVGuiLocalize->Find(pRecipeDef->GetName()), 1, pName_A);

		int iYPos = m_iRecipePanelXPos == 0 ? 0 : 0; 
		iYPos = iCount * (iRowHeight + m_iRecipePanelYDelta);

		CRecipeButton* pButton = new CRecipeButton(m_pRecipeContainer, "reciperow", "", this, "");
		if (m_pRecipeKV)
			pButton->ApplySettings(m_pRecipeKV);
		pButton->SetText(wTemp);
		pButton->SetDefIndex(pRecipeDef->GetDefinitionIndex());
		pButton->SetPos(m_iRecipePanelXPos, iYPos);
		pButton->AddActionSignalTarget(this);
		pButton->MakeReadyForUse();
		pButton->SetVisible(true);
		m_pRecipeButtons.AddToTail(pButton);

		vgui::ImagePanel* pCheck = new vgui::ImagePanel(m_pRecipeContainer, "checkmark");
		if (m_pCheckmarkKV)
			pCheck->ApplySettings(m_pCheckmarkKV);
		pCheck->SetImage(m_pCheckmarkKV ? m_pCheckmarkKV->GetString("activeimage", "checkmark") : "checkmark");
		pCheck->SetPos(0, iYPos + (iRowHeight - pCheck->GetTall()) / 2);
		pCheck->SetVisible(true);
		m_pCheckmarks.AddToTail(pCheck);

		iCount++;
	}

	m_pRecipeContainer->SetSize(m_pRecipeContainer->GetWide(), MAX(1, iCount * (iRowHeight + m_iRecipePanelYDelta)));
	m_pRecipeContainer->InvalidateLayout(true);
	m_pRecipeContainerScroller->InvalidateLayout(true);

	vgui::Label* pNoRecipes = dynamic_cast<vgui::Label*>(FindChildByName("NoRecipesLabel"));
	if (pNoRecipes)
	{
		pNoRecipes->SetVisible(iCount == 0);
	}
	m_pRecipeContainerScroller->SetVisible(iCount > 0);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewRecipePanel::OnRecipePanelEntered(vgui::Panel* panel)
{
	CRecipeButton* pRecipePanel = dynamic_cast<CRecipeButton*>(panel);
	if (!pRecipePanel || !IsVisible())
		return;

	const CEconCraftingRecipeDefinition* pRecipeDef =
		TFInventoryManager()->GetLocalTFInventory()->GetRecipeDefByDefIndex(pRecipePanel->m_iRecipeDefIndex);

	SetItemPanelToRecipe(m_pMouseOverItemPanel, pRecipeDef, false);
	PositionMouseOverPanelForRecipe(this, pRecipePanel, m_pRecipeContainerScroller, m_pMouseOverItemPanel);
}

void CViewRecipePanel::OnRecipePanelExited(void)
{
	m_pMouseOverItemPanel->SetVisible(false);
}

static vgui::DHANDLE<CViewRecipePanel> g_ViewRecipesPanel;

CViewRecipePanel* OpenViewRecipesPanel(vgui::Panel* pParent)
{
	if (!g_ViewRecipesPanel.Get())
		g_ViewRecipesPanel = vgui::SETUP_PANEL(new CViewRecipePanel(pParent, NULL));

	g_ViewRecipesPanel->InvalidateLayout(true, true);
	g_ViewRecipesPanel->SetVisible(true);
	g_ViewRecipesPanel->MakePopup();
	g_ViewRecipesPanel->MoveToFront();
	g_ViewRecipesPanel->SetKeyBoardInputEnabled(true);
	g_ViewRecipesPanel->SetMouseInputEnabled(true);
	TFModalStack()->PushModal(g_ViewRecipesPanel);
	EconUI()->SetPreventClosure(true);

	return g_ViewRecipesPanel;
}