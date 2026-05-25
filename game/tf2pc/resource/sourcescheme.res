#base "SourceSchemeBase.res"

Scheme
{
	// color details
	// this is a list of all the colors used by the scheme
	// the commented out text next to the normal values are the live tf2 values incase debugging is needed
	// and the text not commented out was referenced from the july 2010 tf2 sourcescheme
	// the only reason this has to be manually done for now is due to us not having the 2010 main menu yet meaning all the main menu text will be invisible	
	// however once it is added the last comment will be no longer valid
	Colors
	{
	    "TFDarkBrown"               "60 56 53 255" // "66 62 59 255" // 2010 ---> "60 56 53 255"
	    "TFDarkBrownTransparent"    "60 56 53 190" // "66 62 59 190" // 2010 ---> "60 56 53 190"
	    "TFTanBright"               "236 227 203 150" // "251 236 203 150" // 2010 ---> "236 227 203 150"
	    "TFTanLight"                "201 188 162 150" // "163 153 133 255" // 2010 ---> "201 188 162 150"
	    "TFTanMedium"               "131 121 104 150" // "104 97 84 255" // 2010 ---> "131 121 104 150"
	    
	    "TFTanLightBright"          "229 223 211 90"
	    "TFTanLightDark"            "96 90 78 90"
	    
	    "TFOrangeBright"            "156 82 33 255"
	    
	    "TFTextBright"              "251 236 203 150"
	    "TFTextLight"               "201 188 162 255"
	    "TFTextMedium"              "131 121 104 255"
	    "TFTextMediumDark"          "104 96 83 255"
	    "TFTextDark"                "77 69 61 230" // technically in the 2010 build dark didn't exist however incase it is still needed I wont remove it
	    "TFTextBlack"               "42 39 37 255"
	    "TFTextDull"                "131 121 104 255"

	    "TFMediumBrown"		"69 64 58 255"

	    "QuickListBGDeselected"		"69 64 58 255"
	    "QuickListBGSelected"               "131 121 104 150"
	    
	    "Blank"				"0 0 0 0"
	    
	    // background colors
	    // for the next one the comments will be next to the one that it was taken from
	    // this is only because I don't want to overwrite the already existing comments incase they're still useful
		"ControlBG"			"76 88 68 255"		// background color of controls
		"ControlDarkBG"		"90 106 80 255"		// darker background color; used for background of scrollbars
		"WindowBG"			"62 70 55 255"		// background color of text edit panes (chat, text entries, etc.)
//		"SelectionBG"		"90 84 75 255"	// background color of any selected text or menu item
		"SelectionBG"		"255 157 0 255"
		"SelectionBG2"		"165 93 0 255"	// selection background in window w/o focus
//		"SelectionBG2"		"92 0 128"
		"ListBG"			"39 36 34 255"		// background of server browser, buddy list, etc.
//		"ListBG"			"43 40 38 255"
	}
	BaseSettings
	{
		// scheme-specific colors
		Border.Bright					"TFTanLightDark"	// the lit side of a control
		Border.Dark						"TFTanLightDark"		// the dark/unlit side of a control
		Border.Selection				"BorderSelection"			// the additional border color for displaying the default/selected button
	// all of the TFTextDark had actually originally been TFDarkBrown instead but became deprecated after they made an actual dark color
		Button.TextColor				"TFDarkBrown" // "TFTextDark"
		Button.BgColor					"TFTanLight"
		Button.ArmedTextColor			"TFDarkBrown" // "TFTextDark"
		Button.ArmedBgColor				"TFTanBright"
		Button.DepressedTextColor		"TFDarkBrown" // "TFTextDark"
		Button.DepressedBgColor			"TFTanLight"	
		Button.FocusBorderColor			"TransparentBlack"
		
		CheckButton.TextColor			"TFTextBright"
		CheckButton.SelectedTextColor		"TFTextBright"
		CheckButton.BgColor				"ListBG"
		CheckButton.HighlightFgColor		"TFTextMedium"
		CheckButton.ArmedBgColor		"Blank"
		CheckButton.DepressedBgColor		"Blank"
		CheckButton.Border1  			"Border.Dark" 		// the left checkbutton border
		CheckButton.Border2  			"Border.Bright"		// the right checkbutton border
		CheckButton.Check				"TFTanBright"	// color of the check itself
		CheckButton.DisabledBgColor	    "ListBG"

		ToggleButton.SelectedTextColor	"TFTextBright"
		
		ComboBoxButton.ArrowColor		"TFTanLight"
		ComboBoxButton.ArmedArrowColor	"TFTanBright"
		ComboBoxButton.BgColor			"Blank"
		ComboBoxButton.DisabledBgColor	"Blank"
		
		RadioButton.TextColor		"TFTextBright"
		RadioButton.SelectedTextColor	"TFTextBright"
		RadioButton.ArmedTextColor	"TFTextMedium"
		
		Frame.BgColor					"TFDarkBrown"
		Frame.OutOfFocusBgColor			"TFDarkBrownTransparent"
		FrameGrip.Color1				"TFTanMedium"
		FrameGrip.Color2				"TFDarkBrown"
		FrameTitleButton.FgColor		"TFTanBright"
		FrameTitleBar.Font			"DefaultLarge"		[$WIN32]
		FrameTitleBar.TextColor			"TFTanLight"
		FrameTitleBar.DisabledTextColor	"TFTanLight"
		
		Label.TextDullColor				"TFTextDull"
		Label.TextColor					"TFTextBright"
		Label.TextBrightColor			"TFTextBright"
		Label.SelectedTextColor			"TFTextBright"
		Label.BgColor					"Blank"
		Label.DisabledFgColor1			"TFTextDull"	
		Label.DisabledFgColor2			"Blank"	
		
		ListPanel.TextColor					"TFTextBright"
		ListPanel.BgColor					"ListBG"
		ListPanel.SelectedTextColor			"TFTextBlack" // missing in the 2010 sourcescheme
		ListPanel.SelectedBgColor			"SelectionBG"
		ListPanel.SelectedOutOfFocusBgColor	"SelectionBG2"
		
		// scheme-specific colors
		MainMenu.TextColor			"TanLight"			[$WIN32]
		MainMenu.MenuItemHeight		"14"	// missing in the 2010 sourcescheme
		MainMenu.ArmedTextColor		"117 107 94 255"	[$WIN32]
		MainMenu.Inset				"32"	
		
		Menu.TextInset					"6"
		Menu.FgColor			"TFTextLight"
		Menu.BgColor			"ListBG"
		Menu.ArmedFgColor		"TFTextBright"
		Menu.ArmedBgColor		"TFOrangeBright"
		Menu.DividerColor		"BorderDark"
		
		ScrollBarButton.FgColor				"TFDarkBrown"
		ScrollBarButton.BgColor				"TFTanLight"
		ScrollBarButton.ArmedFgColor		"TFDarkBrown"
		ScrollBarButton.ArmedBgColor		"TFTanBright"
		ScrollBarButton.DepressedFgColor	"TFDarkBrown"
		ScrollBarButton.DepressedBgColor	"TFTanLight"

		ScrollBarSlider.BgColor				"TFTanMedium"		// this isn't really used
		ScrollBarSlider.FgColor				"TFTanLight"		// handle with which the slider is grabbed
		
	// the entire sectionedlistpanel section is missing in the 2010 build, most likely due to them updating the main menu to be entirely vgui panels
		SectionedListPanel.HeaderTextColor	"White"
		SectionedListPanel.HeaderBgColor	"Blank"
		SectionedListPanel.DividerColor		"TFTextBlack"
		SectionedListPanel.TextColor		"DullWhite"
		SectionedListPanel.BrightTextColor	"White"
		SectionedListPanel.BgColor			"TransparentBlack"
		SectionedListPanel.SelectedTextColor			"TFTextBlack" // most likely was dark brown before they updated the main menu
		SectionedListPanel.SelectedBgColor				"SelectionBG"
		SectionedListPanel.OutOfFocusSelectedTextColor	"TFTextBlack" // same as last comment
		SectionedListPanel.OutOfFocusSelectedBgColor	"SelectionBG2"
		
		Slider.NobColor				"TFTanLight"		
		Slider.TextColor			"TFTextBright"
		Slider.TrackColor			"ListBG"
		Slider.DisabledTextColor1	"TFTextMediumDark"
        Slider.DisabledTextColor2	"Blank"
		
		TextEntry.TextColor			        "TFTextBright"
		TextEntry.SelectedTextColor	        "TFTextBlack" // missing in the 2010 sourcescheme
		TextEntry.DisabledTextColor	        "TFTextMedium"
		TextEntry.SelectedBgColor	        "TFOrangeBright"
	}
	
	Fonts
	{
		"MainMenuFont"
		{
			"1"	[$WIN32]
			{
				"name"		"TF2 Build"
				"tall"		"8"	// "18" this is the only case in which the 2010 value isn't the one currently in use due to it making the text really big which obviously isn't correct
				"weight"	"500"
				"additive"	"0"
				"antialias" "1"
			}
		}
		"MenuLarge"
		{
			"1"	[$X360]
			{
				"tall_hidef"		"24"
			}
		}
	}
	// below menularge is where the rest of the menu elements would be listed and given their params
	// we don't have the 2010 main menu yet so I hadn't added them
	// these three comments can be overwritten with the text from the 2010 sourcescheme once we begin adding it
	
	CustomFontFiles
	{
		"9"
		{		
			"font" "resource/TF2Build.ttf"
			"name" "TF2 Build"
			"russian"
			{
				"range" "0x0000 0xFFFF"
			}
			"polish"
			{
				"range" "0x0000 0xFFFF"
			}
		}
	}
}
