//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "item_selection_panel.h"
#include "item_model_panel.h"
#include "tf_item_inventory.h"
#include "econ_item_view.h"
#include "econ_item_system.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CItemSelectionPanel::CItemSelectionPanel(vgui::Panel* parent) : BaseClass(parent, "ItemSelectionPanel")
{
	m_iCurrentClassIndex = TF_CLASS_UNDEFINED;
	m_iCurrentSlotIndex = LOADOUT_POSITION_PRIMARY;
	m_iSelectedPreset = 0;

	m_pItemContainer = new vgui::EditablePanel(this, "itemcontainer");
	m_pSlotLabel = new CExLabel(this, "ItemSlotLabel", "");
	InvalidateLayout(true, true);
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CItemSelectionPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetProportional(true);
	LoadControlSettings("Resource/UI/ItemSelectionPanel.res");
}

//-----------------------------------------------------------------------------
// Purpose: Apply resource settings
//-----------------------------------------------------------------------------
void CItemSelectionPanel::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	wchar_t* wzLocalizedClassName = g_pVGuiLocalize->Find(g_aPlayerClassNames[m_iCurrentClassIndex]);
	SetDialogVariable("loadoutclass", wzLocalizedClassName);

	KeyValues* pItemsKV = inResourceData->FindKey("itemskv");
	KeyValues* pButtonsKV = inResourceData->FindKey("buttonskv");
	if (pItemsKV)
	{
		// Apply the keyvalues specified in the res to our panels and buttons
		FOR_EACH_VEC(m_vecItemPanels, i)
		{
			m_vecItemPanels[i]->ApplySettings(pItemsKV);

			if (pButtonsKV && i < m_vecChangeButtons.Count())
			{
				m_vecChangeButtons[i]->ApplySettings(pButtonsKV);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle commands
//-----------------------------------------------------------------------------
void CItemSelectionPanel::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		SetVisible(false);
		return;
	}

	if (!Q_strnicmp(command, "equip ", 6))
	{
		int iItemNum = atoi(command + 6);
#ifdef DEBUG
		Msg("Got told to change slot %i to item ID %i!\n", m_iCurrentSlotIndex, iItemNum);
#endif

		auto pInventory = InventoryManager()->GetLocalInventory();
		if (pInventory)
		{
			// Find the item by ID in the inventory
			for (int i = 0; i < pInventory->GetItemCount(); i++)
			{
				CEconItemView* pItem = pInventory->GetItem(i);
				if (pItem && pItem->GetItemID() == iItemNum)
				{
					if (engine->IsInGame())
					{
						char szCmd[64];
						Q_snprintf(szCmd, sizeof(szCmd), "equip_item %lld;", pItem->GetItemID());
						engine->ClientCmd(szCmd);
					}
					break;
				}
			}
		}
		return;
	}

	BaseClass::OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: Perform layout
//-----------------------------------------------------------------------------
void CItemSelectionPanel::PerformLayout(void)
{
	BaseClass::PerformLayout();

	int y = m_nItemYDelta;

	// Set the position of each loadout item in the scrollable panel
	FOR_EACH_VEC(m_vecItemPanels, i)
	{
		m_vecItemPanels[i]->SetPos(m_nItemX, y);

		// If this is the currently equipped weapon, show the currently equipped label and background
		if (m_iSelectedPreset == i)
		{
			vgui::EditablePanel* pBG = dynamic_cast<vgui::EditablePanel*>(FindChildByName("CurrentlyEquippedBackground"));
			vgui::Label* pLabel = dynamic_cast<vgui::Label*>(FindChildByName("CurrentlyEquippedLabel"));

			if (pBG && pLabel)
			{
				pBG->SetVisible(true);
				pBG->SetPos(pBG->GetXPos(), y);
				int iLabelY = (pBG->GetTall() - pLabel->GetTall()) / 2;
				pLabel->SetVisible(true);
				pLabel->SetPos(pLabel->GetXPos(), y + iLabelY);
			}

			if (i < m_vecChangeButtons.Count())
			{
				m_vecChangeButtons[i]->SetText("#Keep");
			}
		}

		if (i < m_vecChangeButtons.Count())
		{
			int iButtonY = (m_vecItemPanels[i]->GetTall() - m_vecChangeButtons[i]->GetTall()) / 2;
			m_vecChangeButtons[i]->SetPos(m_nButtonXPos, y + iButtonY);
		}

		y += m_vecItemPanels[i]->GetTall() + m_nItemYDelta;
	}

	// Update slot label
	if (m_pSlotLabel)
	{
		switch (m_iCurrentSlotIndex)
		{
		case LOADOUT_POSITION_PRIMARY:
			m_pSlotLabel->SetText("#ItemSel_PRIMARY");
			break;
		case LOADOUT_POSITION_SECONDARY:
			m_pSlotLabel->SetText("#ItemSel_SECONDARY");
			break;
		case LOADOUT_POSITION_MELEE:
			m_pSlotLabel->SetText("#ItemSel_MELEE");
			break;
		default:
			m_pSlotLabel->SetText("");
		}
	}

	SetVisible(true);
}

//-----------------------------------------------------------------------------
// Purpose: Set class and slot, and set everything up accordingly
//-----------------------------------------------------------------------------
void CItemSelectionPanel::SetClassAndSlot(int iClass, int iSlot)
{
	m_iCurrentClassIndex = iClass;
	m_iCurrentSlotIndex = iSlot;

	m_vecItemPanels.PurgeAndDeleteElements();
	m_vecChangeButtons.PurgeAndDeleteElements();
	InvalidateLayout(true, true);

	auto pInventory = InventoryManager()->GetLocalInventory();
	if (!pInventory)
		return;

	// Build item panels from inventory
	int iNumWeaponsForSlot = 0;

	char itempanelnamebuffer[30];
	char buttonnamebuffer[30];
	char buttoncommand[64];

	// Iterate through inventory items and find those for this slot
	for (int i = 0; i < pInventory->GetItemCount(); i++)
	{
		CEconItemView* pItem = pInventory->GetItem(i);
		if (!pItem || !pItem->IsValid())
			continue;

		// Check if item belongs to this class and slot
		CTFItemDefinition* pItemData = pItem->GetStaticData();
		if (!pItemData || !pItemData->CanBeUsedByClass(m_iCurrentClassIndex))
			continue;

		int iItemSlot = pItemData->GetLoadoutSlot(m_iCurrentClassIndex);
		if (iItemSlot != m_iCurrentSlotIndex)
			continue;

		// Create panel for this item
		Q_snprintf(itempanelnamebuffer, sizeof(itempanelnamebuffer), "weapon%i", iNumWeaponsForSlot);
		Q_snprintf(buttonnamebuffer, sizeof(buttonnamebuffer), "change%i", iNumWeaponsForSlot);
		Q_snprintf(buttoncommand, sizeof(buttoncommand), "equip %lld", pItem->GetItemID());

		CItemModelPanel* itemPanel = new CItemModelPanel(this, itempanelnamebuffer);
		m_vecItemPanels.AddToTail(itemPanel);
		m_vecItemPanels[iNumWeaponsForSlot]->SetParent(m_pItemContainer);
		m_vecItemPanels[iNumWeaponsForSlot]->SetItem(pItem);

		vgui::Button* changeButton = new vgui::Button(this, buttonnamebuffer, "Equip");
		changeButton->SetVisible(true);
		changeButton->AddActionSignalTarget(this);
		changeButton->SetParent(m_pItemContainer);
		changeButton->SetCommand(buttoncommand);

		m_vecChangeButtons.AddToTail(changeButton);

		iNumWeaponsForSlot++;
	}

	// Set the currently selected preset index (default to first item)
	m_iSelectedPreset = 0;

	InvalidateLayout(true, true);
	SetVisible(true);
}

void CItemSelectionPanel::BuildItemPanels()
{
	SetClassAndSlot(m_iCurrentClassIndex, m_iCurrentSlotIndex);
}