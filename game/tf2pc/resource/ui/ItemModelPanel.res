"Resource/UI/ItemModelPanel.res"
{
	"itemmodelpanel"
	{
		"ControlName"		"CEmbeddedItemModelPanel"
		"fieldName"		"itemmodelpanel"
	
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"1"		
		"wide"			"140"
		"tall"			"100"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"useparentbg"		"1"
		
		"inset_eq_x"	"2"
		"inset_eq_y"	"2"

		"fov"			"54"
		"start_framed"		"1"

		"disable_manipulation"	"1"

		"model"
		{
			"angles_x"		"10"
			"angles_y"		"130"
			"angles_z"		"0"
		}
	}
	
	"namelabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"namelabel"
		"font"			"ItemFontNameLarge"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"2"
		"wide"			"140"
		"tall"			"30"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"%itemname%"
		"textAlignment"	"south"
		"fgcolor"		"235 226 202 255"
		"centerwrap"	"1"
	}
	"attriblabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"attriblabel"
		"font"			"ItemFontAttribLarge"
		"xpos"			"0"
		"ypos"			"30"
		"zpos"			"2"
		"wide"			"140"
		"tall"			"60"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"%attriblist%"
		"textAlignment"	"south"
		"fgcolor"		"117 107 94 255"
		"centerwrap"	"1"
	}
	
	"equippedlabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"equippedlabel"
		"font"			"ItemFontAttribSmall"
		"xpos"			"37"
		"ypos"			"28"
		"zpos"			"2"
		"wide"			"35"
		"tall"			"10"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"#ItemPanelEquipped"
		"textAlignment"	"center"
		"fgcolor"		"200 80 60 255"
		"bgcolor_override"		"0 0 0 255"
		"PaintBackgroundType"	"2"
	}
}
