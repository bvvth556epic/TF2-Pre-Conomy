"GameMenu" [$WIN32]
{
	"StartPlayingButton"
	{
		"label" "#MMenu_StartPlaying" 
		"command" "OpenServerBrowser"
		"subimage" "glyph_server"
		"OnlyAtMenu" "1"
	} 
	"ChangeServerButton"
	{
		"label" "#MMenu_ChangeServer" 
		"command" "OpenServerBrowser"
		"subimage" "glyph_server"
		"OnlyInGame" "1"
	} 
	"CharacterSetupButton"
	{
		"label" "#MMenu_CharacterSetup"
		"command" "engine open_charinfo"
		"subimage" "glyph_achievements"
	}

	// These buttons get positioned by the MainMenuOverride.res	
	"CreateServerButton"
	{
		"label" "#GameUI_GameMenu_CreateServer"
		"command" "OpenCreateMultiplayerGameDialog"
		"OnlyAtMenu" "1"
		"tooltip" "#GameUI_GameMenu_CreateServer"
	}
	"OfflinePracticeButton"
	{
		"label" "#GameUI_GameMenu_OfflinePractice"
		"command" "offlinepractice"
		"OnlyAtMenu" "1"
		"tooltip" "#GameUI_GameMenu_OfflinePractice"
	}
}
