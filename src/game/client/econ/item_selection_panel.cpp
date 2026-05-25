//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Loads the old item selection panel (thanks Gold Rush for source code)
//
//=============================================================================//

#include "cbase.h"
#include "item_selection_panel.h"
#include "vgui/ISurface.h"
#include "c_tf_player.h"
#include "gamestringpool.h"
#include "iclientmode.h"
#include "tf_item_inventory.h"
#include "ienginevgui.h"
#include <vgui/ILocalize.h>
#include "vgui_controls/TextImage.h"
#include "vgui_controls/CheckButton.h"
#include "item_model_panel.h"
#include "econ_item_constants.h"
#include "econ_item_system.h"
#include "class_loadout_panel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CItemSelectionPanel::CItemSelectionPanel( vgui::Panel* parent ) : BaseClass( parent, "ItemSelectionPanel" )
{
	m_iCurrentClassIndex = TF_CLASS_UNDEFINED;
	m_iCurrentSlotIndex = LOADOUT_POSITION_PRIMARY;
	m_iSelectedPreset = 0;

	m_pItemContainer = new vgui::EditablePanel( this, "itemcontainer" );
	m_pSlotLabel = new CExLabel( this, "ItemSlotLabel", "" );
	InvalidateLayout( true, true );
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CItemSelectionPanel::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetProportional( true );
	LoadControlSettings( "Resource/UI/ItemSelectionPanel.res" );
}

//-----------------------------------------------------------------------------
// Purpose: Apply resource settings
//-----------------------------------------------------------------------------
void CItemSelectionPanel::ApplySettings( KeyValues* inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	wchar_t* wzLocalizedClassName = g_pVGuiLocalize->Find( g_aPlayerClassNames[m_iCurrentClassIndex] );
	SetDialogVariable( "loadoutclass", wzLocalizedClassName );

	KeyValues* pItemsKV = inResourceData->FindKey( "itemskv" );
	KeyValues* pButtonsKV = inResourceData->FindKey( "buttonskv" );
	if ( pItemsKV )
	{
		// Apply the keyvalues specified in the res to our panels and buttons
		FOR_EACH_VEC( m_vecItemPanels, i )
		{
			m_vecItemPanels[i]->ApplySettings( pItemsKV );

			if ( pButtonsKV && i < m_vecChangeButtons.Count() )
			{
				m_vecChangeButtons[i]->ApplySettings( pButtonsKV );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle commands
//-----------------------------------------------------------------------------
void CItemSelectionPanel::OnCommand( const char* command )
{
	if ( !Q_stricmp( command, "vguicancel" ) )
	{
		SetVisible( false );
		return;
	}

	if ( !Q_strnicmp( command, "equip ", 6 ) )
	{
		int iItemNum = atoi( command + 6 );
#ifdef DEBUG
		Msg( "Got told to change slot %i to weapon ID %i!\n", m_iCurrentSlotIndex, iItemNum );
#endif

		CTFInventory *pInventory = TFInventoryManager()->GetInventory();
		if ( pInventory )
		{
			pInventory->SetWeaponPreset( m_iCurrentClassIndex, m_iCurrentSlotIndex, iItemNum );

			if ( engine->IsInGame() )
			{
				char szCmd[64];
				Q_snprintf( szCmd, sizeof( szCmd ), "weaponpresetclass %d %d %d;", m_iCurrentClassIndex, m_iCurrentSlotIndex, iItemNum );
				engine->ClientCmd( szCmd );
			}

			// Notify parent panel to update (should be CClassLoadoutPanel)
			CClassLoadoutPanel* pParent = dynamic_cast<CClassLoadoutPanel*>( GetParent() );
			if ( pParent )
			{
				pParent->UpdateModelPanels();
			}
		}
		return;
	}

	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: Perform layout
//-----------------------------------------------------------------------------
void CItemSelectionPanel::PerformLayout( void )
{
	BaseClass::PerformLayout();

	int y = m_nItemYDelta;
	
	// Set the position of each loadout item in the scrollable panel
	FOR_EACH_VEC( m_vecItemPanels, i )
	{
		m_vecItemPanels[i]->SetPos( m_nItemX, y );

		// If this is the currently equipped weapon, show the currently equipped label and background
		if ( m_iSelectedPreset == i )
		{
			vgui::EditablePanel* pBG = dynamic_cast<vgui::EditablePanel*>( FindChildByName( "CurrentlyEquippedBackground" ) );
			vgui::Label* pLabel = dynamic_cast<vgui::Label*>( FindChildByName( "CurrentlyEquippedLabel" ) );
			
			if ( pBG && pLabel )
			{
				pBG->SetVisible( true );
				pBG->SetPos( pBG->GetXPos(), y );
				int iLabelY = ( pBG->GetTall() - pLabel->GetTall() ) / 2;
				pLabel->SetVisible( true );
				pLabel->SetPos( pLabel->GetXPos(), y + iLabelY );
			}
			
			if ( i < m_vecChangeButtons.Count() )
			{
				m_vecChangeButtons[i]->SetText( "#Keep" );
			}
		}

		if ( i < m_vecChangeButtons.Count() )
		{
			int iButtonY = ( m_vecItemPanels[i]->GetTall() - m_vecChangeButtons[i]->GetTall() ) / 2;
			m_vecChangeButtons[i]->SetPos( m_nButtonXPos, y + iButtonY );
		}

		y += m_vecItemPanels[i]->GetTall() + m_nItemYDelta;
	}

	// Update slot label
	switch ( m_iCurrentSlotIndex )
	{
	case LOADOUT_POSITION_PRIMARY:
		m_pSlotLabel->SetText( "#ItemSel_PRIMARY" );
		break;
	case LOADOUT_POSITION_SECONDARY:
		m_pSlotLabel->SetText( "#ItemSel_SECONDARY" );
		break;
	case LOADOUT_POSITION_MELEE:
		m_pSlotLabel->SetText( "#ItemSel_MELEE" );
		break;
	default:
		m_pSlotLabel->SetText( "" );
	}

	SetVisible( true );
}

//-----------------------------------------------------------------------------
// Purpose: Set class and slot, and set everything up accordingly
//-----------------------------------------------------------------------------
void CItemSelectionPanel::SetClassAndSlot( int iClass, int iSlot )
{
	m_iCurrentClassIndex = iClass;
	m_iCurrentSlotIndex = iSlot;

	m_vecItemPanels.PurgeAndDeleteElements();
	m_vecChangeButtons.PurgeAndDeleteElements();
	InvalidateLayout( true, true );

	CTFInventory *pInventory = TFInventoryManager()->GetInventory();
	if ( !pInventory )
		return;

	int iNumWeaponsForSlot = pInventory->GetItemCount( m_iCurrentClassIndex, m_iCurrentSlotIndex );
	
	char itempanelnamebuffer[30];
	char buttonnamebuffer[30];
	char buttoncommand[10];

	for ( int i = 0; i < iNumWeaponsForSlot; i++ )
	{
		// Create the vgui name of this panel
		Q_snprintf( itempanelnamebuffer, sizeof( itempanelnamebuffer ), "weapon%i", i );
		Q_snprintf( buttonnamebuffer, sizeof( buttonnamebuffer ), "change%i", i );
		Q_snprintf( buttoncommand, sizeof( buttoncommand ), "equip %i", i );

		CItemModelPanel* itemPanel = new CItemModelPanel( this, itempanelnamebuffer );
		m_vecItemPanels.AddToTail( itemPanel );
		m_vecItemPanels[i]->SetParent( m_pItemContainer );

		// Set the econ item data
		CEconItemView* pItem = pInventory->GetItem( m_iCurrentClassIndex, m_iCurrentSlotIndex, i );
		if ( pItem )
		{
			m_vecItemPanels[i]->SetItem( pItem );
		}

		vgui::Button* changeButton = new vgui::Button( this, buttonnamebuffer, "Equip" );
		changeButton->SetVisible( true );
		changeButton->AddActionSignalTarget( this );
		changeButton->SetParent( m_pItemContainer );
		changeButton->SetCommand( buttoncommand );

		m_vecChangeButtons.AddToTail( changeButton );
	}

	// Cache the currently selected slot preset
	m_iSelectedPreset = pInventory->GetWeaponPreset( m_iCurrentClassIndex, m_iCurrentSlotIndex );

	InvalidateLayout( true, true );
	SetVisible( true );
}