//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CRAFTING_PANEL_H
#define CRAFTING_PANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "backpack_panel.h"
#include "vgui_controls/ScrollableEditablePanel.h"
#include "tf_gcmessages.h"
#include "econ_gcmessages.h"
#include "tf_imagepanel.h"
#include "tf_controls.h"
#include "item_selection_panel.h"

class CImageButton;

// Crafting slots on crafting page
#define CRAFTING_SLOTS_INPUT_ROWS					3
#define CRAFTING_SLOTS_INPUT_COLUMNS				4
#define CRAFTING_SLOTS_INPUTPANELS					(CRAFTING_SLOTS_INPUT_ROWS * CRAFTING_SLOTS_INPUT_COLUMNS)
#define CRAFTING_SLOTS_OUTPUT_ROWS					1
#define CRAFTING_SLOTS_OUTPUT_COLUMNS				4
#define CRAFTING_SLOTS_COUNT						(CRAFTING_SLOTS_INPUTPANELS + (CRAFTING_SLOTS_OUTPUT_ROWS * CRAFTING_SLOTS_OUTPUT_COLUMNS))

// Backpack slots
#define BACKPACK_COLUMNS							5
#define BACKPACK_ROWS								5
#define BACKPACK_SLOTS								(BACKPACK_COLUMNS * BACKPACK_ROWS)

#define RECIPE_CUSTOM								-2

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CRecipeButton : public CExButton
{
private:
	DECLARE_CLASS_SIMPLE( CRecipeButton, CExButton );

public:
	CRecipeButton( vgui::Panel *parent, const char *name, const char *text, vgui::Panel *pActionSignalTarget = NULL, const char *cmd = NULL )
		: CExButton( parent, name, text, pActionSignalTarget, cmd )
	{
	}

	virtual void ApplySettings( KeyValues *inResourceData )
	{
		BaseClass::ApplySettings( inResourceData );
		SetEnabled( m_iRecipeDefIndex != -1 );
	}

	void SetDefIndex( int iIndex )
	{
		m_iRecipeDefIndex = iIndex;
		SetEnabled( m_iRecipeDefIndex != -1 );
	}

	void OnCursorEntered( void )
	{
		PostActionSignal( new KeyValues("RecipePanelEntered") );
		BaseClass::OnCursorEntered();
	}

	void OnCursorExited( void )
	{
		PostActionSignal( new KeyValues("RecipePanelExited") );
		BaseClass::OnCursorExited();
	}


public:
	int		m_iRecipeDefIndex;
};

//-----------------------------------------------------------------------------
// An inventory screen that handles displaying the crafting screen
//-----------------------------------------------------------------------------
class CCraftingPanel : public CBaseLoadoutPanel
{
	DECLARE_CLASS_SIMPLE( CCraftingPanel, CBaseLoadoutPanel );
public:
	CCraftingPanel( vgui::Panel *parent, const char *panelName );
	~CCraftingPanel( void );

	virtual const char *GetResFile( void ) { return "Resource/UI/CraftingPanel.res"; }
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void PerformLayout( void );
	virtual void OnShowPanel( bool bVisible, bool bReturningFromArmory );
	virtual void OnCommand( const char *command );

	virtual void OnCursorMoved( int x, int y );
	virtual void OnMouseReleased( vgui::MouseCode code );

	void		 CreateRecipeFilterButtons( void );
	void		 UpdateRecipeFilter( void );

	//old crafting panel
	void UpdateBackpackPage();
	void LayoutBackpackPanels();


	virtual int	 GetNumItemPanels( void ) { return CRAFTING_SLOTS_COUNT; };

	float		 m_flStartExplanationsAt;
	bool		 IsInputItemPanel( int iSlot ) { return (iSlot < CRAFTING_SLOTS_INPUTPANELS); }
	virtual void PositionItemPanel( CItemModelPanel *pPanel, int iIndex );
	int			 GetItemPanelIndex( CItemModelPanel *pItemPanel );

	void		 UpdateSelectedRecipe( bool bClearInputItems );
	void		 UpdateRecipeItems( bool bClearInputItems );
	void		 UpdateCraftButton( void );

	const char	 *GetItemTextForCriteria( const CItemSelectionCriteria *pCriteria );
	CEconItemDefinition *GetItemDefFromCriteria( const CItemSelectionCriteria *pCriteria );
	virtual void AddNewItemPanel( int iPanelIndex );
	virtual void UpdateModelPanels( void );
	void		 SetButtonToRecipe( int iButton, int iDefIndex, wchar_t *pszText );

	bool		 CheckForUntradableItems( void );
	void		 Craft( void );
	void		 OnCraftResponse( EGCMsgResponse eResponse, CUtlVector<uint64> *vecCraftedIndices, int iRecipeUsed );
	bool		 ShouldShowExplanations(void) { return true; }
	void		 ShowCraftFinish( void );
	virtual void OnTick( void );
	void		 CleanupPostCraft( bool bClearInputItems );

	MESSAGE_FUNC_PTR( OnItemPanelMousePressed, "ItemPanelMousePressed", panel );
	MESSAGE_FUNC_PTR( OnRecipePanelEntered, "RecipePanelEntered", panel );
	MESSAGE_FUNC_PTR( OnRecipePanelExited, "RecipePanelExited", panel );
	MESSAGE_FUNC_PTR( OnItemPanelMouseReleased, "ItemPanelMouseReleased", panel );
	MESSAGE_FUNC_PTR( OnItemPanelEntered, "ItemPanelEntered", panel );
	//client.dll, never doubt yourself
	MESSAGE_FUNC_PTR( OnItemPanelMouseDoublePressed, "ItemPanelMouseDoublePressed", panel );
	MESSAGE_FUNC( OnCancelSelection, "CancelSelection" );
	MESSAGE_FUNC_PARAMS( OnSelectionReturned, "SelectionReturned", data );

	MESSAGE_FUNC( OnClosing, "Closing" );

	virtual ConVar	*GetExplanationConVar( void );

private:
	// Items in the input model panels
	itemid_t						m_InputItems[CRAFTING_SLOTS_INPUTPANELS];
	const CItemSelectionCriteria	*m_ItemPanelCriteria[CRAFTING_SLOTS_INPUTPANELS];

	CExButton						*m_pCraftButton;
	CExButton						*m_pUpgradeButton;
	CExLabel						*m_pFreeAccountLabel;
	vgui::EditablePanel				*m_pRecipeListContainer;
	vgui::ScrollableEditablePanel	*m_pRecipeListContainerScroller;
	vgui::EditablePanel				*m_pSelectedRecipeContainer;

	KeyValues						*m_pRecipeButtonsKV;
	CUtlVector<CRecipeButton*>		m_pRecipeButtons;
	KeyValues						*m_pRecipeFilterButtonsKV;
	CUtlVector<CImageButton*>		m_pRecipeFilterButtons;

	//old crafting panel
	KeyValues						*m_pItemModelPanelKVs;
	CUtlVector<CItemModelPanel*>	m_pBackpackPanels;
	CItemModelPanel					*m_pDragPanel;
	//dragging functionality
	bool						    m_bDragging;
	itemid_t						m_iDragItemID;
	bool							m_bDragFromBackpack;
	int								m_iDragFromCraftSlot;

	int								m_iBackpackPage;
	int 							m_iBackpackPageCount;
	int								m_iBackpackOriginX;
	int								m_iBackpackOriginY;
	int								m_iCraftAreaOriginX;
	int								m_iCraftAreaOriginY;
	//
	int								m_iCurrentlySelectedRecipe;
	int								m_iCurrentRecipeTotalInputs;
	int								m_iCurrentRecipeTotalOutputs;
	recipecategories_t				m_iRecipeCategoryFilter;

	CUtlVector<itemid_t>			m_vecNewlyCraftedItems;
	double							m_flAbortCraftingAt;
	bool							m_bWaitingForCraftItems;
	int								m_iRecipeIndexTried;
	int								m_iNewRecipeIndex;
	bool							m_bEventLogging;
	int								m_iCraftingAttempts;

	CTFTextToolTip					*m_pToolTip;
	vgui::EditablePanel				*m_pToolTipEmbeddedPanel;

	CCraftingItemSelectionPanel		*m_pSelectionPanel;
	int								m_iSelectingForSlot;

	CPanelAnimationVarAliasType( int, m_iItemCraftingOffcenterX, "item_crafting_offcenter_x", "50", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iFilterOffcenterX, "filter_xoffset", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iFilterYPos, "filter_ypos", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iFilterDeltaX, "filter_xdelta", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iFilterDeltaY, "filter_ydelta", "0", "proportional_int" );

	CPanelAnimationVarAliasType( int, m_iOutputItemYPos, "output_item_ypos", "0", "proportional_int" );

	CPanelAnimationVarAliasType(int, m_iXPosOffcenterA, "item_xpos_offcenter_a", "-310", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iXPosOffcenterB, "item_xpos_offcenter_b", "165", "proportional_int");
	CPanelAnimationVarAliasType( int, m_iItemBackpackXDelta, "item_backpack_xdelta", "4", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iItemBackpackYDelta, "item_backpack_ydelta", "3", "proportional_int" );
};

//-----------------------------------------------------------------------------
// Purpose: A dialog used to show the current state of a crafting request.
//-----------------------------------------------------------------------------
class CCraftingStatusDialog : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CCraftingStatusDialog, vgui::EditablePanel );

public:
	CCraftingStatusDialog( vgui::Panel *pParent, const char *pElementName );

	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	OnCommand( const char *command );
	virtual void	OnTick( void );
	void			UpdateSchemeForVersion( bool bRecipe );
	void			ShowStatusUpdate( bool bAnimateEllipses, bool bAllowed, bool bShowOnExit );

private:
	bool			m_bShowOnExit;
	bool			m_bAnimateEllipses;
	int				m_iNumEllipses;
	bool			m_bShowNewRecipe;
	CItemModelPanel	*m_pRecipePanel;
};
CCraftingStatusDialog *OpenCraftingStatusDialog( vgui::Panel *pParent, const char *pszText, bool bAnimateEllipses, bool bAllowClose, bool bShowOnExit );
CCraftingStatusDialog *OpenNewRecipeFoundDialog( vgui::Panel *pParent, const CEconCraftingRecipeDefinition *pRecipeDef );
void CloseCraftingStatusDialog( void );

#endif // CRAFTING_PANEL_H
