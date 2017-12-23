#pragma once

namespace lua 
{
	
	//---------------------------------------
	//			 GLOBAL VARIABLES
	//---------------------------------------
	typedef int(__cdecl* luaL_do_string)(DWORD lua_State, const char *buff);
	unsigned long				internal_lua_state = NULL;
	luaL_do_string				internal_fdostring = nullptr;
	std::function<void(const char*)>	print_callback = NULL;
	

	//---------------------------------------
	//			 HOOKS FUNCITIONS
	//---------------------------------------
	int __cdecl hook_luaL_dostring(DWORD L, const char *buff)
	{
		if (config::debug_strings)
		{
			printf("Internal Lua doString: %s\n", buff);
		}
		if (internal_lua_state == NULL)
		{
			internal_lua_state = L;
		}
		
		return internal_fdostring(L, buff);
	}

	/* 
	* Procedure wich loads and execute lua code  
	*/
	void execute_buffer(const char* buffer)
	{
		unsigned long function_address			= 0x4041B0;
		unsigned long function_address_execute	= 0x401F90;

		if (internal_lua_state == NULL) return;

		__asm
		{
			push buffer
			push internal_lua_state
			call function_address
			add esp, 0x8

			push    0
			push    0
			push    0
			push    internal_lua_state
			call    function_address_execute
			add     esp, 0x10
		}
	}

	int __cdecl hook_fputs(const char *Str, FILE *File)
	{
		if (config::debug_strings == true)
		{
			printf(Str);
		}
		if (print_callback)
			print_callback(Str);
		return 1;
	}


	/*
	* Procedure wich hooks lua_do_string for getting lua state 
	*/
	void hook()
	{
		internal_fdostring = (luaL_do_string)DetourFunction((PBYTE)0x004041B0, (PBYTE)&hook_luaL_dostring);
		MemoryPatcher::InstallNopPatch(0x006FAB6B, 5);
		DetourFunction((PBYTE)0x00E4DFAC, (PBYTE)&hook_fputs);

	}
};