#ifndef VIEW_RECIPES_PANEL_H
#define VIEW_RECIPES_PANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/ScrollableEditablePanel.h"
#include "crafting_panel.h"

class CViewRecipePanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CViewRecipePanel, vgui::EditablePanel);
public:
	CViewRecipePanel(vgui::Panel* parent, const char* panelName);
	~CViewRecipePanel(void);

	virtual const char* GetResFile(void) { return "resource/UI/ViewRecipesPanel.res"; }
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void ApplySettings(KeyValues* inResourceData);
	virtual void OnCommand(const char* command);

	void Populate(void);

	MESSAGE_FUNC_PTR(OnRecipePanelEntered, "RecipePanelEntered", panel);
	MESSAGE_FUNC(OnRecipePanelExited, "RecipePanelExited");

private:
	vgui::EditablePanel					*m_pRecipeContainer;
	vgui::ScrollableEditablePanel		*m_pRecipeContainerScroller;
	CItemModelPanel						*m_pMouseOverItemPanel;
	CUtlVector<CRecipeButton*>			m_pRecipeButtons;
	CUtlVector<vgui::ImagePanel*>		m_pCheckmarks;

	KeyValues							*m_pRecipeKV;
	KeyValues							*m_pCheckmarkKV;

	CPanelAnimationVarAliasType( int, m_iRecipePanelXPos, "recipepanel_xpos", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iRecipePanelYDelta, "recipepanel_ydelta", "4", "proportional_int" );
};

CViewRecipePanel* OpenViewRecipesPanel(vgui::Panel* pParent);

#endif // VIEW_RECIPES_PANEL_H