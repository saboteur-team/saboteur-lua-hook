#pragma once
namespace config
{
	int			console_rows = 8;
	std::string lua_file = "debug.lua";
	bool		external_console = false;
	bool		debug_strings = false;

	void init()
	{
		CIniFile ini;
		ini.Load("hookconfig.ini");
		/*if (!ini.GetSection("console"))
		{
			CIniSection* sec = ini.AddSection("console");
			sec->AddKey("rows");
			sec->SetKeyValue("rows", "8");
			ini.Save("hookconfig.ini");
		}

		if (!ini.GetSection("lua"))
		{
			CIniSection* sec = ini.AddSection("lua");
			sec->AddKey("file");
			sec->SetKeyValue("file", "debug.lua");
			ini.Save("hookconfig.ini");
		}*/

		CIniSection* console_section = ini.GetSection("console");

		if (console_section)
		{
			console_rows = std::stoi(console_section->GetKeyValue("rows"));
			external_console = (bool)std::stoi(console_section->GetKeyValue("external_console"));
			debug_strings = (bool)std::stoi(console_section->GetKeyValue("debug_strings"));
		}

		CIniSection* lua_section = ini.GetSection("lua");

		if (console_section)
		{
			lua_file = lua_section->GetKeyValue("file");
		}
	}
};