// installer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "installer.h"

 void debug_main();

extern "C" {
	// module exports file: $(ProjectDir)\exports.def
	void __stdcall TestFunc(char* p)
	{
		strcpy_s(p, 16, "HI");
		// debug_main();
	}
}