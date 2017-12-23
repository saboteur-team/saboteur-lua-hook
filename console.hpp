#pragma once

namespace console
{
	std::vector<std::wstring>	console_messages;
	std::vector<std::wstring>	console_inputs;
	constexpr int				console_start_offset = 3;
	constexpr int				console_gap_offset	 = 3;
	std::wstring				console_input;
	int							select_offset = 0;
	using						convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	bool						console_visible = false;

	auto console_font = new gui::Font;

	inline void gui_on_draw()
	{	
		if (!console_visible)
			return;
		int	current_y = 0;

		int rows = config::console_rows + 1;

		//draw background rectangle
		gui::draw_rect(0, 0, gui::internal_resolution.x, current_y + (rows * (console_font->get_height() + console_gap_offset * 2)), D3DCOLOR_ARGB(77, 0, 0, 0));

		int startPos = console_messages.size() > rows ? console_messages.size() - rows : 0;

		//draw messages
		for (int i = startPos; i < console_messages.size(); i++) {

			console_font->set_pos(console_start_offset, current_y);
			console_font->draw(console_messages.at(i).c_str());
			current_y += (console_font->get_height() + console_gap_offset);

		}

		console_font->set_pos(console_start_offset, rows * (console_font->get_height() + console_gap_offset));
		console_font->draw((L"Lua> " + console_input).c_str());
	}


	inline bool on_keyboard(RAWKEYBOARD keyboard)
	{
		ULONG message = keyboard.Message;
		USHORT VKey = keyboard.VKey;


		if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
		{

			if (VKey == VK_F8)
			{
				console_visible = !console_visible;
			}

			if (VKey == VK_F9)
			{
				std::FILE *fp = std::fopen(config::lua_file.c_str(), "rb");
				if (fp)
				{
					std::string contents;
					std::fseek(fp, 0, SEEK_END);
					contents.resize(std::ftell(fp));
					std::rewind(fp);
					std::fread(&contents[0], 1, contents.size(), fp);
					std::fclose(fp);
					lua::execute_buffer(contents.c_str());
					std::stringstream ss;
					ss << "LUA File \"" << config::lua_file << "\" executed!";
					console_messages.push_back(converter.from_bytes(ss.str()));
				}
				else
				{
					std::stringstream ss;
					ss << "Failed to load LUA File \"" << config::lua_file << "\"!";
					console_messages.push_back(converter.from_bytes(ss.str()));
				}
			}

			if (!console_visible)
				return false;

			if (GetAsyncKeyState(VK_CONTROL) && VKey == 0x56)
			{
				OpenClipboard(NULL);
				HANDLE text = GetClipboardData(CF_TEXT);
				if (text != NULL)
				{
					console_input += converter.from_bytes((char*)text);
					CloseClipboard();
				}
				return true;
			}

			if (VKey == VK_BACK)
			{
				if (console_input.size() > 0)
					console_input.pop_back();

				return true;
			}

			if (VKey == VK_RETURN)
			{
				console_messages.push_back(L"> " + console_input);
				console_inputs.push_back(console_input);
				lua::execute_buffer(converter.to_bytes(console_input).c_str());
				console_input.clear();
				select_offset = 0;
				return true;
			}
			
			if (VKey == VK_DOWN)
			{
				if ((select_offset - 1) > 0)
				{
					select_offset--;
					console_input = console_inputs.at(console_inputs.size() - select_offset);
				}
				else
				{
					console_input = L"";
					select_offset = 0;
				}
				return true;
			}


			if (VKey == VK_UP)
			{
				if (console_inputs.size() - (select_offset) > 0)
				{
					select_offset++;
					console_input = console_inputs.at(console_inputs.size() - select_offset);
				}
				return true;
			}

			const int BUFFER_LENGTH = 10;
			WCHAR wChars[BUFFER_LENGTH];
			BYTE  bKeyboardState[256];

			GetKeyboardState(bKeyboardState);
			UINT uScanCode = MapVirtualKey(VKey, MAPVK_VK_TO_VSC);
			int iResult = ToUnicode(VKey, uScanCode, bKeyboardState, wChars, 2, 0);

			if (iResult > 0)
			{
				if (iResult > 2) iResult = 2;

				wChars[iResult] = 0;

				std::wstring toAddToLine;
				for (int i = 0; i < iResult; i++)
				{
					toAddToLine += wChars[i];
				}

				console_input += toAddToLine;
				return true;
			}

		}
		return false;
	}

	inline void on_game_print(LPCSTR message)
	{
		if (strlen(message) == 1 && message[0] == '\n' || message[0] == '\t')
			return;
		console_messages.push_back(converter.from_bytes(message));
	}

	inline void init()
	{
		console_font->set_pos(10, 10);
		console_font->set_height(18);
		console_font->set_name("Consolas");
		console_font->set_color(D3DCOLOR_ARGB(255, 255, 255, 255));
		console_font->create();
		
		gui::on_draw(gui_on_draw);
		rawinput::on_keyboard = on_keyboard;

#ifndef _DEBUG
		console_messages.push_back(L"The Saboteur Lua hook 1.1 by GAMELASTER and DavoSK");
#endif
		lua::print_callback = on_game_print;
	}
};