#include "stdafx.h"

char* dwInitAddress = "cod7-steam-auth.live.demonware.net";

typedef int(__cdecl * DB_GetXAssetSizeHandler_t)();
DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x00E064A0;

void** DB_XAssetPool = (void**)0x00E06888;
unsigned int* g_poolSize = (unsigned int*)0x00E065C8;

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")

bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader)
{
	HRESULT hr = D3DXCreateBuffer(shaderLen, shader);

	if (!SUCCEEDED(hr))
	{
		// Com_PrintError(8, "ERROR: Material_CopyTextToDXBuffer: D3DXCreateBuffer(%d) failed: %s (0x%08x)\n", shaderLen, R_ErrorDescription(hr), hr);

		free(cachedShader);
		return false;
	}

	memcpy((*shader)->GetBufferPointer(), cachedShader, shaderLen);
	return true;
}

FILE *Material_OpenShader_BlackOps(const char *shaderTarget, const char *shaderName)
{
	//
	// Determine if this was a vertex shader or pixel shader
	//
	const char *shaderMain;

	if (shaderTarget[0] == 'v' && shaderTarget[1] == 's')
		shaderMain = "vs_main";
	else
		shaderMain = "ps_main";

	//
	// Load the shader directly from the name
	//
	char shaderPath[MAX_PATH];
	sprintf_s(shaderPath, "%s\\raw\\shadercache_mods\\%s_%s_%s",
		*(const char **)(*(DWORD *)0x099D3C64 + 0x18),
		shaderMain,
		shaderTarget,
		shaderName);

	return fopen(shaderPath, "rb");
}

bool Material_FindCachedShaderDX(const char *shaderText, const unsigned int shaderTextLen, const char *entryPoint, const char *target, ID3DXBuffer **shader, const char *shaderName, int useUPDB)
{
	int shaderDataSize = 0;
	FILE *shaderFile = Material_OpenShader_BlackOps(target, shaderName);

	if (shaderFile)
	{
		// Skip the first 4 bytes (zeros)
		fpos_t pos = 4;
		fsetpos(shaderFile, &pos);

		// Read the real data size
		if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
		{
			fclose(shaderFile);
			return false;
		}
	}

	void *shaderMemory = malloc(shaderDataSize);
	fread(shaderMemory, 1, shaderDataSize, shaderFile);

	if (!Material_CopyTextToDXBuffer(shaderMemory, shaderDataSize, shader))
		ASSERT_MSG(false, "SHADER UPLOAD FAILED\n");

	fclose(shaderFile);
	free(shaderMemory);
	return true;
}

void* ReallocateAssetPool(int type, unsigned int newSize)
{
	int elSize = DB_GetXAssetSizeHandlers[type]();
	void* poolEntry = malloc(newSize * elSize);

	if (!poolEntry)
		__debugbreak();

	DB_XAssetPool[type] = poolEntry;
	g_poolSize[type] = newSize;
	return poolEntry;
}

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES = 0x0,
	ASSET_TYPE_PHYSPRESET = 0x1,
	ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
	ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
	ASSET_TYPE_XANIMPARTS = 0x4,
	ASSET_TYPE_XMODEL = 0x5,
	ASSET_TYPE_MATERIAL = 0x6,
	ASSET_TYPE_TECHNIQUE_SET = 0x7,
	ASSET_TYPE_IMAGE = 0x8,
	ASSET_TYPE_SOUND = 0x9,
	ASSET_TYPE_SOUND_PATCH = 0xA,
	ASSET_TYPE_CLIPMAP = 0xB,
	ASSET_TYPE_CLIPMAP_PVS = 0xC,
	ASSET_TYPE_COMWORLD = 0xD,
	ASSET_TYPE_GAMEWORLD_SP = 0xE,
	ASSET_TYPE_GAMEWORLD_MP = 0xF,
	ASSET_TYPE_MAP_ENTS = 0x10,
	ASSET_TYPE_GFXWORLD = 0x11,
	ASSET_TYPE_LIGHT_DEF = 0x12,
	ASSET_TYPE_UI_MAP = 0x13,
	ASSET_TYPE_FONT = 0x14,
	ASSET_TYPE_MENULIST = 0x15,
	ASSET_TYPE_MENU = 0x16,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
	ASSET_TYPE_WEAPON = 0x18,
	ASSET_TYPE_WEAPONDEF = 0x19,
	ASSET_TYPE_WEAPON_VARIANT = 0x1A,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
	ASSET_TYPE_FX = 0x1C,
	ASSET_TYPE_IMPACT_FX = 0x1D,
	ASSET_TYPE_AITYPE = 0x1E,
	ASSET_TYPE_MPTYPE = 0x1F,
	ASSET_TYPE_MPBODY = 0x20,
	ASSET_TYPE_MPHEAD = 0x21,
	ASSET_TYPE_CHARACTER = 0x22,
	ASSET_TYPE_XMODELALIAS = 0x23,
	ASSET_TYPE_RAWFILE = 0x24,
	ASSET_TYPE_STRINGTABLE = 0x25,
	ASSET_TYPE_PACK_INDEX = 0x26,
	ASSET_TYPE_XGLOBALS = 0x27,
	ASSET_TYPE_DDL = 0x28,
	ASSET_TYPE_GLASSES = 0x29,
	ASSET_TYPE_EMBLEMSET = 0x2A,
	ASSET_TYPE_COUNT = 0x2B,
	ASSET_TYPE_STRING = 0x2B,
	ASSET_TYPE_ASSETLIST = 0x2C,
};

unsigned int __cdecl DB_GetImageIndex(char *image)
{
	return ((char *)image - (char *)DB_XAssetPool[ASSET_TYPE_IMAGE]) / 52;
}

void *__cdecl DB_GetImageAtIndex(unsigned int index)
{
	/*if (index >= 0x1080
		&& !Assert_MyHandler(
		"C:\\projects_pc\\cod\\codsrc\\src\\database\\db_registry.cpp",
		5951,
		0,
		"index doesn't index ARRAY_COUNT( g_GfxImagePool.entries )\n\t%i not in [0, %i)",
		index,
		4224))
		__debugbreak();*/
	char *ptr = (char *)DB_XAssetPool[ASSET_TYPE_IMAGE];

	return ptr + (index * 52);
	//return &g_GfxImagePool.entries[index].entry;

}

bool Live_IsSignedIn(int controllerIndex)
{
	return true;
}

void(*SteamAPI_Init)();

void(*LiveSteam_Init)();
void hk_LiveSteam_Init()
{
	HMODULE hSteam = LoadLibraryA("steam_api.dll");

	*(FARPROC *)&SteamAPI_Init = GetProcAddress(hSteam, "SteamAPI_Init");

	SteamAPI_Init();
	LiveSteam_Init();
}

typedef short scr_entref_t;

struct BuiltinMethodDef
{
	const char *actionString;
	void(__cdecl *actionFunc)(scr_entref_t);
	int type;
};

struct BuiltinFunctionDef
{
	const char *actionString;
	void(__cdecl *actionFunc)();
	int type;
};

BuiltinMethodDef *methods_3 = (BuiltinMethodDef *)0x00E09708;
BuiltinFunctionDef *functions = (BuiltinFunctionDef *)0x00E08510;

void RandomFunc(scr_entref_t entref)
{
}

void RandomFunc2()
{
}

void(__cdecl *__cdecl BuiltIn_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	for (int i = 0; i < 234; ++i)
	{
		if (!strcmp(*pName, methods_3[i].actionString))
		{
			*pName = methods_3[i].actionString;
			*type = methods_3[i].type;
			return methods_3[i].actionFunc;
		}
	}

	if (!_stricmp(*pName, "GetClientFlag"))
	{
		*pName = "getclientflag";
		*type = 0;
		return (void(__cdecl *)(scr_entref_t))0x00660BD0;
	}

	return RandomFunc;
}

void(__cdecl *__cdecl Scr_GetFunction(const char **pName, int *type))()
{
	for (int i = 0; i < 383; ++i)
	{
		if (!strcmp(*pName, functions[i].actionString))
		{
			*pName = functions[i].actionString;
			*type = functions[i].type;
			return functions[i].actionFunc;
		}
	}

	*type = 0;
	return RandomFunc2;
}

//char *test = "maps/%s.d3dbsp";
char *test = "maps/asset_viewer.d3dbsp";

char *defModel = "viewmodel_default";

void Patch_DisabledFastFiles()
{
	// Patches to get the game running when 'useFastFile' is 0

	// Live_Init hacks
	PatchMemory_WithNOP(0x0098F9C3, 2);// Live_InitPlatform => 'live_service' set to 0
	PatchMemory_WithNOP(0x0098F9D6, 5);

	PatchMemory_WithNOP(0x0098FAE6, 5);
	PatchMemory_WithNOP(0x0098FB07, 5);
	PatchMemory_WithNOP(0x0098FB11, 5);
	PatchMemory_WithNOP(0x0098FB16, 5);
	PatchMemory_WithNOP(0x0098FB2A, 5);
	PatchMemory_WithNOP(0x0098FB41, 5);
	PatchMemory_WithNOP(0x0098FB6F, 5);
	PatchMemory_WithNOP(0x0098FB86, 5);
	PatchMemory_WithNOP(0x0098FB9D, 5);
	PatchMemory_WithNOP(0x0098FBB4, 5);
	PatchMemory_WithNOP(0x0098FBCB, 5);
	PatchMemory_WithNOP(0x0098FBE2, 5);
	PatchMemory_WithNOP(0x0098FBF9, 5);
	PatchMemory_WithNOP(0x0098FC01, 5);
	PatchMemory_WithNOP(0x0098FC0D, 5);
	PatchMemory_WithNOP(0x0098FC12, 5);
	PatchMemory_WithNOP(0x0098FC17, 5);
	PatchMemory_WithNOP(0x0098FC1C, 5);
	PatchMemory_WithNOP(0x0098FC21, 5);
	// end

	PatchMemory(0x00626C88, (PBYTE)"\xEB", 1);// G_InitGame hang on DB_SyncXAssets
	PatchMemory(0x009910D0, (PBYTE)"\xC3", 1);// PC_InitSigninState hang from steam not loaded
	PatchMemory_WithNOP(0x0041A9B0, 2);// BG_EmblemsInit hang on DB_FindXAssetHeader
	PatchMemory_WithNOP(0x0098807D, 2);// LiveStorage_ReadStatsIfDirChanged
	PatchMemory_WithNOP(0x009880BD, 2);// LiveStorage_UploadStats
	PatchMemory_WithNOP(0x007DC64A, 2);// Sys_GetPacket
	PatchMemory_WithNOP(0x006D6C03, 5);// LiveSteam_Frame
	PatchMemory(0x0097D570, (PBYTE)"\xC3", 1);// LiveSteam_CheckAccess
	PatchMemory(0x00ABA1E0, (PBYTE)"\xC3", 1);// R_StreamAlloc_InitTempImages crash/horribly undefined behavior
	//PatchMemory_WithNOP(0x006CDC4E, 2);// ValidateGameModes

	//PatchMemory_WithNOP(0x006D66AB, 5);// Com_LoadFrontEnd => Dvar_SetBool(xblive_matchEndingSoon = null)

	//PatchMemory_WithNOP(0x006D4867, 2);// Forcefully run LiveSteam_Init
	//PatchMemory_WithNOP(0x006D4875, 5);// Forcefully run LiveSteam_Init

	PatchMemory(0x00570EBE, (PBYTE)"\xE9\x93\x00\x00\x00", 5);// CL_ParseGamestate LiveStat dvars


	PatchMemory_WithNOP(0x006D4114, 5);// Com_InitUIAndCommonXAssets hang on Com_UnloadLevelFastFiles
	PatchMemory_WithNOP(0x006D4177, 5);// Com_InitUIAndCommonXAssets hang on DB_LoadFastFilesForPC

	PatchMemory(0x00613310, (PBYTE)"\xC3", 1);// G_ParseHitLocDmgTable
	PatchMemory(0x0044F0A0, (PBYTE)"\xC3", 1);// BG_LoadPenetrationDepthTable


	//PatchMemory_WithNOP(0x00800D39, 5);// XmodelLoad hack
	//PatchMemory(0x00800D4D, (PBYTE)"\xEB", 1);// XmodelLoad hack
	//PatchMemory_WithNOP(0x0080336F, 5);// XModelGenerateHighMipVolume

	// XModelAllowLoadMesh
	PatchMemory_WithNOP(0x00805A98, 7);

	// GC_InitWeaponOptions
	PatchMemory_WithNOP(0x004C61BC, 5);

	// cg_drawBudgets/DB_GetAssetTypeUsageInfo
	PatchMemory(0x00497481, (PBYTE)"\xE9\xB7\x01\x00\x00", 5);

	// R_StreamUpdateDynamicModels (db code)
	PatchMemory(0x00AA4700, (PBYTE)"\xC3", 1);

	// R_StreamUpdateStatic (db code)
	PatchMemory(0x00AA63E0, (PBYTE)"\xC3", 1);

	// TODO Expression_Free crash/memory leak
	PatchMemory(0x00789E50, (PBYTE)"\xC3", 1);
}

BOOL ServerMod_Init()
{
	// Always disable system keyboard hooking (Sys_SetBlockSystemHotkeys)
	PatchMemory(0x007E08E0, (PBYTE)"\xC3", 1);

	//Patch_DisabledFastFiles();

	PatchMemory(0x0059E83E, (PBYTE)&dwInitAddress, 4);
	PatchMemory(0x0059EE78, (PBYTE)&dwInitAddress, 4);

	PatchMemory(0x007A7DC8, (PBYTE)&test, 4);

	// hack for stats bypass
	PatchMemory(0x00708A3C, (PBYTE)"\xEB", 1);
	// hack to disable asserts
	//PatchMemory(0x00788720, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

#if RAW_MAP_LOADING_HACK

	// map bsp loading hacks
	PatchMemory_WithNOP(0x00541E99, 2);			// CL_CM_LoadMap	useFF Com_LoadBsp
	PatchMemory_WithNOP(0x00710B7A, 2);			// SV_SpawnServer	useFF Com_UnloadBsp
	PatchMemory_WithNOP(0x00542A9F, 2);			// LoadWorld		useFF Com_UnloadBsp
	PatchMemory_WithNOP(0x00710AF3, 2);			// SV_SpawnServer	useFF Com_LoadBsp
	PatchMemory(0x006A93A7, (PBYTE)"\xEB", 1);	// CM_LoadMapData	loadObj
	PatchMemory(0x00AB98C4, (PBYTE)"\xEB", 1);	// R_LoadWorld		loadObj
	PatchMemory(0x006CC0B7, (PBYTE)"\xEB", 1);	// Com_LoadWorld	loadObj
	PatchMemory(0x00800CD7, (PBYTE)"\xEB", 1);	// XModelPrecache	loadObj
	//PatchMemory(0x00A4C947, (PBYTE)"\xEB", 1);	// Material_Register loadObj
	//PatchMemory(0x00A4BC07, (PBYTE)"\xEB", 1);	// Material_RegisterTechniqueSet loadObj

	// XModelAllowLoadMesh
	PatchMemory_WithNOP(0x00805A98, 7);

	PatchMemory(0x00998264, (PBYTE)"\xEB", 1);	// GetGlasses		loadObj

	// hunk allocation hack for raw d3dbsp maps
	PatchMemory_WithNOP(0x007B2993, 2);
	PatchMemory(0x007B29B1, (PBYTE)"\xEB", 1);

	// disable session creation crash (offline)
	PatchMemory(0x00972E10, (PBYTE)"\xC3", 1);

	// R_CheckValidStaticModel bypass
	PatchMemory(0x00A9AFA0, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);
	PatchMemory(0x00A9B0E3, (PBYTE)&defModel, 4);
	PatchMemory_WithNOP(0x00A9B046, 5);

#endif


	int proto = 2117;
	PatchMemory(0x00973095, (PBYTE)&proto, 4);

	PatchMemory(0x0071163B, (PBYTE)&proto, 4);
	PatchMemory(0x00711640, (PBYTE)&proto, 4);
	PatchMemory(0x00711645, (PBYTE)&proto, 4);
	PatchMemory(0x00706EAF, (PBYTE)&proto, 4);
	PatchMemory(0x00713A26, (PBYTE)&proto, 4);
	PatchMemory(0x005612DB, (PBYTE)&proto, 4);

	ReallocateAssetPool(ASSET_TYPE_STRINGTABLE, 1024);
	ReallocateAssetPool(ASSET_TYPE_GAMEWORLD_SP, 1);
	ReallocateAssetPool(ASSET_TYPE_FX, 1024);
	ReallocateAssetPool(ASSET_TYPE_LOCALIZE_ENTRY, 20400);
	ReallocateAssetPool(ASSET_TYPE_SNDDRIVER_GLOBALS, 16);
	//ReallocateAssetPool(ASSET_TYPE_IMAGE, 8192);

	//Detours::X86::DetourFunction((PBYTE)0x005947E0, (PBYTE)&DB_GetImageAtIndex);
	//Detours::X86::DetourFunction((PBYTE)0x00594750, (PBYTE)&DB_GetImageIndex);

	Detours::X86::DetourFunction((PBYTE)0x00AD0660, (PBYTE)&Material_FindCachedShaderDX);
	PatchMemory(0x00ACF7BF, (PBYTE)"\xEB", 1);

	Detours::X86::DetourFunction((PBYTE)0x00661680, (PBYTE)&BuiltIn_GetMethod);
	Detours::X86::DetourFunction((PBYTE)0x00660A50, (PBYTE)&Scr_GetFunction);

	PatchMemory_WithNOP(0x008BDCF9, 2);
	PatchMemory_WithNOP(0x008BDCFF, 2);
	PatchMemory(0x008BDEB0, (PBYTE)"\xC3", 1);

	// PatchMemory_WithNOP(0x005644ED, 2); patch to force using default materials

	PatchMemory_WithNOP(0x008B8396, 2);

	//PatchMemory(0x005A0274, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	//PatchMemory(0x0071023F, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x0093CA00, (PBYTE)"\xC3", 1);

	//PatchMemory(0x00708A3C, (PBYTE)"\xEB", 1); stats bug

	PatchMemory(0x00564720, (PBYTE)"\xC3", 1);	// CL_DevGuiDvar_f

	PatchMemory_WithNOP(0x006D1C7B, 5);	// Com_DPrintf (logging)

	PatchMemory(0x00661FD0, (PBYTE)"\xC3", 1);

	//PatchMemory(0x008BD8EB, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	//PatchMemory(0x008BD7E5, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// BEGIN SERVER CHECK PATCHES
	//

	// Com_Init_Try_Block_Function
	PatchMemory_WithNOP(0x006D49BF, 7);// CL_InitDedicated disable
	PatchMemory_WithNOP(0x006D4D2A, 2);// Client initializations (CL_Init*)
	PatchMemory_WithNOP(0x006D4D96, 2);// Renderer and sound

	// Com_Frame_Try_Block_Function
	PatchMemory_WithNOP(0x006D6C6D, 6);// Renderer
	PatchMemory_WithNOP(0x006D6979, 2);// Max FPS
	PatchMemory_WithNOP(0x006D69F3, 6);// Event loop
	PatchMemory_WithNOP(0x006D6E27, 2);// Phys_RunToTime
	PatchMemory_WithNOP(0x006D6E38, 5);// DWDedicatedLobbyPump disable

	// Com_Init
	PatchMemory_WithNOP(0x006D424D, 2);// Renderer init
	PatchMemory_WithNOP(0x006D42F1, 2);// UI_LoadMap/Arena

	// Com_Frame
	PatchMemory_WithNOP(0x006D68CE, 2);// CL_InitRenderer/Com_StartHunkUsers

	// SV_SpawnServer
	PatchMemory_WithNOP(0x00710666, 2);// Client setup
	PatchMemory_WithNOP(0x00710C4A, 6);// Session update/exit after tool completion

	// Live* functions
	PatchMemory_WithNOP(0x0099148B, 7);// Live_Frame
	PatchMemory_WithNOP(0x0097DA14, 7);// LiveSteam_Init
	PatchMemory_WithNOP(0x0097DDC4, 2);// LiveSteam_Frame
	PatchMemory_WithNOP(0x0097D57D, 4);// LiveSteam_CheckAccess
	PatchMemory_WithNOP(0x0097836B, 2);// LiveStats_GetPlayerStat
	PatchMemory_WithNOP(0x0097773C, 2);// LiveStats_GetItemStat
	PatchMemory_WithNOP(0x00974D50, 2);// LiveStats_GetIntPlayerStatInternal
	PatchMemory_WithNOP(0x00975297, 2);// LiveStats_SetIntPlayerStatInternal
	PatchMemory_WithNOP(0x0096D68A, 2);// LiveNews_PopulateFriendNews
	PatchMemory_WithNOP(0x00986BD8, 2);// LiveStorage_Init ("listcustomgametypes" command)
	PatchMemory_WithNOP(0x00987570, 9);// LiveStorage_FetchOnlineWAD (SV_IsConnectedToDW bypass)
	PatchMemory_WithNOP(0x00981431, 2);// LiveStorage_ReadDWFileByUserID
	PatchMemory_WithNOP(0x009818DD, 2);// LiveStorage_GetServerTimeComplete
	PatchMemory_WithNOP(0x00985183, 2);// LiveStorage_FileShare_WriteSummarySuccess
	PatchMemory(0x00962FE4, (PBYTE)"\xEB", 1);// Live_FileShare_Read_f (Dedicated server custom gametype auth)

	// UI
	PatchMemory_WithNOP(0x00740C02, 2);// UI_Gametype_FileShareDownloadComplete
	PatchMemory_WithNOP(0x0050BE53, 2);// UI_SetLocalVarStringByName
	PatchMemory_WithNOP(0x0077697B, 7);// UI_SetLoadingScreenMaterial
	PatchMemory(0x0073EB5B, (PBYTE)"\xEB", 1);// UI_Gametype_IsUsingCustom (SV_GetLicenseType bypass)

	// DevGui
	PatchMemory_WithNOP(0x0059676F, 7);// DevGui_CreateMenu
	PatchMemory_WithNOP(0x005970E4, 5);// DevGui_AddGraph

	// FastFile loading
	PatchMemory_WithNOP(0x0058F101, 2);// DB_Sleep
	PatchMemory_WithNOP(0x00594227, 2);// DB_SyncExternalAssets
	PatchMemory_WithNOP(0x00594EF4, 2);// DB_LoadFastFilesForPC ("ui_mp")
	PatchMemory_WithNOP(0x00594F63, 2);// DB_LoadFastFilesForPC ("ui_viewer_mp")
	PatchMemory_WithNOP(0x006D5DCD, 7);// Com_LoadUiFastFile
	PatchMemory_WithNOP(0x006D410B, 9);// Com_InitUIAndCommonXAssets
	PatchMemory_WithNOP(0x006D5F0B, 4);// Com_LoadMapLoadingScreenFastFile



	// todo
	PatchMemory_WithNOP(0x006D4342, 2);	// Com_ErrorCleanup

	PatchMemory_WithNOP(0x007DB71E, 2);	// WinMain
	PatchMemory_WithNOP(0x007DB6CC, 2);	// WinMain

	PatchMemory(0x007101B9, (PBYTE)"\xE9\x98\x00\x00\x00", 5);	// SV_Startup (Dedicated DW startup) ????????????????

	PatchMemory_WithNOP(0x005A020A, 2);	// DWDedicatedLogon (Never called; skipped by PC_InitSigninState patch)
	PatchMemory_WithNOP(0x0059EE1B, 7);	// dwLogOnComplete
	PatchMemory_WithNOP(0x005A100C, 2);	// dwUpdateSessionComplete


	PatchMemory_WithNOP(0x00713FAD, 2);	// SVC_Info

	PatchMemory_WithNOP(0x00703CE3, 2);	// SV_Map_f
	PatchMemory_WithNOP(0x0071681C, 2);	// SV_MasterHeartbeat
	PatchMemory_WithNOP(0x0071682E, 2);	// SV_MasterHeartbeat
	PatchMemory(0x007179E0, (PBYTE)"\xC3", 1);	// SV_SysLog_LogMessage (DANGEROUS - DISABLED)

	PatchMemory_WithNOP(0x006F587A, 2);	// Sys_WaitRenderer

	PatchMemory_WithNOP(0x0041A9BE, 2);	// BG_EmblemsInit

	PatchMemory_WithNOP(0x007A79CD, 2);	// FS_Restart

	PatchMemory_WithNOP(0x0093110B, 6);	// SND_ShouldInit

	PatchMemory_WithNOP(0x006CD65B, 6);	// Com_ControllerIndexes_GetPrimary

	PatchMemory_WithNOP(0x006D1952, 2);	// Com_PrintMessage

	PatchMemory_WithNOP(0x00AB750F, 6);	// RB_RenderCommandFrame
	PatchMemory(0x00AB768E, (PBYTE)"\xEB", 1);	// RB_RenderCommandFrame

	PatchMemory_WithNOP(0x0097287A, 6);	// Session_IsHost

	PatchMemory(0x0060D582, (PBYTE)"\xEB", 1);	// PlayerCmd_IsLocalToHost

	//PatchMemory(0x00972EED, (PBYTE)"\xEB", 1);	// Session_StartHost XBOXLIVE_SIGNEDOUTOFLIVE
	//PatchMemory(0x00972F0D, (PBYTE)"\xEB", 1);	// Session_StartHost assert #1
	//PatchMemory(0x00972FCB, (PBYTE)"\xEB", 1);	// Session_StartHost assert #2

	// SocketRouter_EmergencyFrame
	PatchMemory_WithNOP(0x0099487D, 6);

	// !C .P !D PC_InitSigninState
	PatchMemory(0x009911A2, (PBYTE)"\xEB", 1);

	// Do not execute SV_DWWriteClientStats
	PatchMemory(0x0098B0BD, (PBYTE)"\xE9\x54\x02\x00\x00", 5);

	// Con_Restricted_InitLists (Execute on dedicated server for RCON only)
	PatchMemory(0x0054D870, (PBYTE)"\xC3", 1);

	// Com_IntroPlayed
	PatchMemory_WithNOP(0x006D500A, 4);

	// Com_WriteConfigToFile
	PatchMemory_WithNOP(0x006D5A8C, 2);

	// Com_WriteKeyConfigToFile
	PatchMemory_WithNOP(0x006D5BEF, 2);

	PatchMemory_WithNOP(0x007D3EEB, 2);// KeyValueToField

	// !C !P !D Com_LoadFrontEnd
	// PatchMemory_WithNOP(0x006D66BA, 9); ?????????

	PatchMemory_WithNOP(0x007077CB, 2);// SV_DirectConnect (Already enabled by default, does nothing)

	// script
	PatchMemory(0x008DC190, (PBYTE)"\xEB", 1);

	// xuid != PCACHE_INVALID_XUID assert
	PatchMemory(0x0096FD07, (PBYTE)"\xEB", 1);
	PatchMemory(0x0096F8AC, (PBYTE)"\xEB", 1);
	PatchMemory(0x0096FA6B, (PBYTE)"\xEB", 1);

	// Duplicate QPORT on server
	PatchMemory(0x007075E0, (PBYTE)"\xEB", 1);

	// Patch XBOXLIVE_SIGNEDOUTOFLIVE error when skipping the main menu
	PatchMemory(0x00972EED, (PBYTE)"\xEB", 1);

	// VAC: MOV EDX, 0
	PatchMemory(0x0097F72B, (PBYTE)"\xBA\x00\x00\x00\x00", 5);

	Patch_DDL();

	Detours::X86::DetourFunction((PBYTE)0x009908C0, (PBYTE)&Live_IsSignedIn);

	*(uint8_t **)&LiveSteam_Init = Detours::X86::DetourFunction((PBYTE)0x0097D9E0, (PBYTE)&hk_LiveSteam_Init);

	MessageBoxA(nullptr, "a", "a", 0);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return ServerMod_Init();
	}

	return TRUE;
}

/*
Address        Function                                                                                                                                                                                       Instruction
-------        --------                                                                                                                                                                                       -----------
.text:0042781F ?Phys_Vec3ToNitrousVec@@YAXQBMAAVphys_vec3@@@Z                                                                                                                                                 mov     eax, 1
.text:0043CC3F ?Phys_NitrousVecToVec3@@YAXABVphys_vec3@@QAM@Z                                                                                                                                                 mov     eax, 1
.text:0050BE4C ?UI_SetLocalVarStringByName@@YAXPBD0H@Z                                                                                                                                                        mov     eax, 1
.text:00562ADD ?CL_AllocatePerLocalClientMemory@@YAXPBDI@Z                                                                                                                                                    mov     eax, 1
.text:0058F0FA DB_Sleep                                                                                                                                                                                       mov     eax, 1
.text:00594EED ?DB_LoadFastFilesForPC@@YAXXZ                                                                                                                                                                  mov     eax, 1
.text:00594F5C ?DB_LoadFastFilesForPC@@YAXXZ                                                                                                                                                                  mov     eax, 1
.text:00596766 DevGui_CreateMenu                                                                                                                                                                              mov     eax, 1
.text:0059EE14 ?dwLogOnComplete@@YAXH@Z                                                                                                                                                                       mov     eax, 1
.text:005A0203 ?DWDedicatedLogon@@YAXXZ                                                                                                                                                                       mov     eax, 1
.text:005A1005 ?dwUpdateSessionComplete@@YA?AW4taskCompleteResults@@QAUoverlappedTask@@@Z                                                                                                                     mov     eax, 1
.text:0060D57B PlayerCmd_IsLocalToHost                                                                                                                                                                        mov     eax, 1
.text:0061155E ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                                                                                                               mov     eax, 1
.text:0066F4E2 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                                                                                        mov     eax, 1
.text:006CD654 ?Com_ControllerIndexes_GetPrimary@@YAHXZ                                                                                                                                                       mov     eax, 1
.text:006D4104 ?Com_InitUIAndCommonXAssets@@YAXXZ                                                                                                                                                             mov     eax, 1
.text:006D433B Com_ErrorCleanup                                                                                                                                                                               mov     eax, 1
.text:006D5003 COM_PlayIntroMovies                                                                                                                                                                            mov     eax, 1
.text:006D5240 Com_InitDvars                                                                                                                                                                                  mov     eax, 1
.text:006D5A85 Com_WriteConfigToFile                                                                                                                                                                          mov     eax, 1
.text:006D5BE8 Com_WriteKeyConfigToFile                                                                                                                                                                       mov     eax, 1
.text:006D5DC6 Com_LoadUiFastFile                                                                                                                                                                             mov     eax, 1
.text:006D5F04 ?Com_LoadMapLoadingScreenFastFile@@YAXPBD@Z                                                                                                                                                    mov     eax, 1
.text:006D6E20 Com_Frame_Try_Block_Function                                                                                                                                                                   mov     eax, 1
.text:006D6FCC Com_ModifyMsec                                                                                                                                                                                 mov     eax, 1
.text:006DD966 GDT_RemoteXModelUpdate                                                                                                                                                                         mov     eax, 1
.text:006DDDBC GDT_RemoteMaterialUpdate                                                                                                                                                                       mov     eax, 1
.text:006E2F63 ?MSG_GetMapCenter@@YAPAY02MXZ                                                                                                                                                                  mov     eax, 1
.text:006F5873 ?Sys_WaitRenderer@@YAHXZ                                                                                                                                                                       mov     eax, 1
.text:00703AF4 ?SV_AddOperatorCommands@@YAXXZ                                                                                                                                                                 mov     eax, 1
.text:00707816 ?SV_DirectConnect@@YAXUnetadr_t@@@Z                                                                                                                                                            mov     eax, 1
.text:0070F5F8 SV_InitGameVM                                                                                                                                                                                  mov     eax, 1
.text:007101D3 ?SV_Startup@@YAXH@Z                                                                                                                                                                            mov     eax, 1
.text:00711908 ?SV_Init@@YAXXZ                                                                                                                                                                                mov     eax, 1
.text:00713D2B ?SVC_Info@@YAXUnetadr_t@@PAVbdSecurityID@@_N@Z                                                                                                                                                 mov     eax, 1
.text:00716B81 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                                                                                                                 mov     eax, 1
.text:007179A3 ?SV_MatchEnd@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:0073EB54 ?UI_Gametype_IsUsingCustom@@YA_NXZ                                                                                                                                                             mov     eax, 1
.text:00752954 ?UI_DrawConnectScreen@@YAXH@Z                                                                                                                                                                  mov     eax, 1
.text:007631D3 Script_Play                                                                                                                                                                                    mov     eax, 1
.text:00776974 ?UI_SetLoadingScreenMaterial@@YAXPBD@Z                                                                                                                                                         mov     eax, 1
.text:007949C4 CanRenderClip                                                                                                                                                                                  mov     eax, 1
.text:007A79C6 ?FS_Restart@@YAXHH@Z                                                                                                                                                                           mov     eax, 1
.text:007D3F55 KeyValueToField                                                                                                                                                                                mov     eax, 1          ; jumptable 007D3DB3 cases 11,12
.text:007D99CE ?Sys_Error@@YAXPBDZZ                                                                                                                                                                           mov     eax, 1
.text:007DB6C5 _WinMain@16                                                                                                                                                                                    mov     eax, 1
.text:007E7F89 ?DObjCalcSkel@@YAXPBUDObj@@QAH@Z                                                                                                                                                               mov     eax, 1
.text:0081F122 ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F156 ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F1BB ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F827 ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:0081F85B ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:0081F8C0 ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:008202A5 ?unpause_physics@NitrousVehicle@@QAEXXZ                                                                                                                                                        mov     eax, 1
.text:0082040D ?pause_physics@NitrousVehicle@@QAEX_N@Z                                                                                                                                                        mov     eax, 1
.text:008211B5 ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                                                                                                    mov     eax, 1
.text:008213BE ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                                                                                                    mov     eax, 1
.text:008242D2 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                                                                                        mov     eax, 1
.text:00825171 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:008253CE ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00825402 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00825467 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:008255E2 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00828342 ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:00828387 ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:008283FB ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:00828AA1 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828B83 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828BB7 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828C15 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:008312D9 ?set_geom_id_new@gjk_base_t@@QAEXI@Z                                                                                                                                                           mov     eax, 1
.text:00832A09 ?set_contents@gjk_base_t@@QAEXH@Z                                                                                                                                                              mov     eax, 1
.text:00838209 ?set_rb2_entity@contact_point_info@@QAEXPBX@Z                                                                                                                                                  mov     eax, 1
.text:00839217 ?set@broad_phase_info@@QAEXPAVrigid_body@@PBVphys_mat44@@11PBVphys_gjk_geom@@I_NHPAXI@Z                                                                                                        mov     eax, 1
.text:008482E3 ?environment_collision_list_add@@YAXPAVbroad_phase_base@@@Z                                                                                                                                    mov     eax, 1
.text:0084ADED ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                                                                                mov     eax, 1
.text:0084B1FF ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                                                                                mov     eax, 1
.text:0084B76A ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                                                                            mov     eax, 1
.text:0084BA5C ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                                                                            mov     eax, 1
.text:0085488D ?remove@?$phys_link_list1@UPhysObjUserData@@@@QAEXPAUPhysObjUserData@@@Z                                                                                                                       mov     eax, 1
.text:00855E8B ?add_brush@debug_brush_info_t@@QAEXPBUcbrush_t@@PBVphys_mat44@@@Z                                                                                                                              mov     eax, 1
.text:00856173 ?add_brush@debug_brush_info_t@@QAEXPBUcbrush_t@@PBVphys_mat44@@@Z                                                                                                                              mov     eax, 1
.text:008692C9 Ragdoll_CreatePhysObj                                                                                                                                                                          mov     eax, 1
.text:008693AF Ragdoll_CreatePhysObj                                                                                                                                                                          mov     eax, 1
.text:0086A447 Ragdoll_TunnelTest                                                                                                                                                                             mov     eax, 1
.text:00936076 ?SNDL_Update@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:00936E66 ?SND_StopVoice@@YAXH@Z                                                                                                                                                                         mov     eax, 1
.text:00947233 ?SNDL_AliasName@@YAXPBDI@Z                                                                                                                                                                     mov     eax, 1
.text:00947286 ?SNDL_Play@@YAHIHMTSndEntHandle@@QBM1_NPAUsnd_playback@@@Z                                                                                                                                     mov     eax, 1
.text:009474B3 ?SNDL_StopSoundAliasOnEnt@@YAXTSndEntHandle@@I@Z                                                                                                                                               mov     eax, 1
.text:00947623 ?SNDL_StopSoundsOnEnt@@YAXTSndEntHandle@@@Z                                                                                                                                                    mov     eax, 1
.text:00947673 ?SNDL_NotifyCinematicStart@@YAXM@Z                                                                                                                                                             mov     eax, 1
.text:009476E3 ?SNDL_NotifyCinematicEnd@@YAXXZ                                                                                                                                                                mov     eax, 1
.text:00947746 ?SNDL_DisconnectListener@@YAXH@Z                                                                                                                                                               mov     eax, 1
.text:009478D9 ?SNDL_SetListener@@YAXHHW4team_t@@QBMQAY02$$CBM@Z                                                                                                                                              mov     eax, 1
.text:00948046 ?SNDL_FadeOut@@YAXXZ                                                                                                                                                                           mov     eax, 1
.text:009480B6 ?SNDL_FadeIn@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:00948126 ?SNDL_SetEnvironmentEffects@@YAXHIMMH@Z                                                                                                                                                        mov     eax, 1
.text:00948376 ?SNDL_DeactivateEnvironmentEffects@@YAXHH@Z                                                                                                                                                    mov     eax, 1
.text:00948536 ?SNDL_SetPlaybackAttenuation@@YAXHM@Z                                                                                                                                                          mov     eax, 1
.text:00948606 ?SNDL_SetPlaybackAttenuationRate@@YAXHM@Z                                                                                                                                                      mov     eax, 1
.text:00948676 ?SNDL_SetPlaybackPitch@@YAXHM@Z                                                                                                                                                                mov     eax, 1
.text:00948746 ?SNDL_SetPlaybackPitchRate@@YAXHM@Z                                                                                                                                                            mov     eax, 1
.text:009487B4 ?SNDL_StopPlayback@@YAXH@Z                                                                                                                                                                     mov     eax, 1
.text:00948824 ?SNDL_SetSnapshot@@YAXW4snd_snapshot_type@@IMM@Z                                                                                                                                               mov     eax, 1
.text:00948923 ?SNDL_SetGameState@@YAX_N0MII@Z                                                                                                                                                                mov     eax, 1
.text:00948A06 ?SNDL_PlayLoopAt@@YAXIQBM@Z                                                                                                                                                                    mov     eax, 1
.text:00948AE6 ?SNDL_StopLoopAt@@YAXIQBM@Z                                                                                                                                                                    mov     eax, 1
.text:00948C66 ?SNDL_PlayLineAt@@YAXIQBM0@Z                                                                                                                                                                   mov     eax, 1
.text:00948D86 ?SNDL_StopLineAt@@YAXIQBM0@Z                                                                                                                                                                   mov     eax, 1
.text:009495E6 ?SND_LogSkip@@YA_NIIPAI0@Z                                                                                                                                                                     mov     eax, 1
.text:0094B15E ?SND_Play@@YAXPBDHMTSndEntHandle@@QBM2_N@Z                                                                                                                                                     mov     eax, 1
.text:0094B24F ?SND_StopSoundAliasOnEnt@@YAXTSndEntHandle@@I@Z                                                                                                                                                mov     eax, 1
.text:0094B2CF ?SND_StopSoundsOnEnt@@YAXTSndEntHandle@@@Z                                                                                                                                                     mov     eax, 1
.text:0094B33F ?SND_NotifyCinematicStart@@YAXM@Z                                                                                                                                                              mov     eax, 1
.text:0094B3BF ?SND_NotifyCinematicEnd@@YAXXZ                                                                                                                                                                 mov     eax, 1
.text:0094B42F ?SND_DisconnectListener@@YAXH@Z                                                                                                                                                                mov     eax, 1
.text:0094B4A4 ?SND_SetListener@@YAXHHW4team_t@@QBMQAY02$$CBM@Z                                                                                                                                               mov     eax, 1
.text:0094B64F ?SND_StopSounds@@YAXW4snd_stop_sound_flags@@@Z                                                                                                                                                 mov     eax, 1
.text:0094B6CF ?SND_FadeIn@@YAXXZ                                                                                                                                                                             mov     eax, 1
.text:0094B73F ?SND_FadeOut@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:0094B7B4 ?SND_SetEnvironmentEffects@@YAXHPBDMMH@Z                                                                                                                                                       mov     eax, 1
.text:0094B89F ?SND_DeactivateEnvironmentEffects@@YAXHH@Z                                                                                                                                                     mov     eax, 1
.text:0094BC5F ?SND_StopPlayback@@YAXH@Z                                                                                                                                                                      mov     eax, 1
.text:0094C572 ?SND_SetContext@@YAXPBD0@Z                                                                                                                                                                     mov     eax, 1
.text:0094C67F ?SND_SetScriptTimescale@@YAXM@Z                                                                                                                                                                mov     eax, 1
.text:0094C6F6 ?SND_FindEntState@@YAPAUsnd_ent_state@@TSndEntHandle@@_N@Z                                                                                                                                     mov     eax, 1
.text:0094CB06 ?SND_UpdateEntState@@YAXTSndEntHandle@@QBM1QAY02$$CBM@Z                                                                                                                                        mov     eax, 1
.text:0094CF06 ?SND_GetEntState@@YA_NTSndEntHandle@@QAM1QAY02M@Z                                                                                                                                              mov     eax, 1
.text:0094CFD3 ?SNDL_SetEntState@@YAXTSndEntHandle@@QBM1QAY02$$CBM@Z                                                                                                                                          mov     eax, 1
.text:0094D026 ?SND_EntStateFrame@@YAXXZ                                                                                                                                                                      mov     eax, 1
.text:0094D474 ?SND_EntStateRequest@@YAXTSndEntHandle@@@Z                                                                                                                                                     mov     eax, 1
.text:0094D4E4 ?SND_SubtitleNotify@@YAXPBDI@Z                                                                                                                                                                 mov     eax, 1
.text:0094D554 ?SND_LengthNotify@@YAXII@Z                                                                                                                                                                     mov     eax, 1
.text:0094D5C4 ?SND_FreePlaybackNotify@@YAXPAUsnd_playback@@@Z                                                                                                                                                mov     eax, 1
.text:0094D696 ?SND_AllocatePlayback@@YAPAUsnd_playback@@XZ                                                                                                                                                   mov     eax, 1
.text:0094D7A3 ?SND_FreePlayback@@YAXPAUsnd_playback@@@Z                                                                                                                                                      mov     eax, 1
.text:0094D866 SND_FindPlayback                                                                                                                                                                               mov     eax, 1
.text:0094DBBF ?SND_GameReset@@YAXXZ                                                                                                                                                                          mov     eax, 1
.text:0094DC32 ?SND_BeginFrame@@YAX_N0MII@Z                                                                                                                                                                   mov     eax, 1
.text:0094DC61 ?SND_BeginFrame@@YAX_N0MII@Z                                                                                                                                                                   mov     eax, 1
.text:0094F096 ?SND_NotifyPush@@YAXPAUsnd_notify@@@Z                                                                                                                                                          mov     eax, 1
.text:0094F314 ?SND_NotifyPeek@@YAPBUsnd_notify@@XZ                                                                                                                                                           mov     eax, 1
.text:0094F413 ?SND_NotifyPop@@YAXXZ                                                                                                                                                                          mov     eax, 1
.text:00950ED6 ?Snd_StreamFini@@YAXXZ                                                                                                                                                                         mov     eax, 1
.text:00951815 ?Snd_StreamStatus@@YA?AW4snd_stream_status@@I@Z                                                                                                                                                mov     eax, 1
.text:00951A65 ?Snd_StreamGetFreeWindows@@YAII@Z                                                                                                                                                              mov     eax, 1
.text:00951C85 ?Snd_StreamAcquireWindow@@YA?AW4snd_stream_status@@IPAI0PAPAD@Z                                                                                                                                mov     eax, 1
.text:00962F8E Live_FileShare_Read_f                                                                                                                                                                          mov     eax, 1
.text:00969484 ?LB_SetGametypeDvar@@YA_NXZ                                                                                                                                                                    mov     eax, 1
.text:00972873 ?Session_IsHost@@YA_NPAUSessionData_s@@H@Z                                                                                                                                                     mov     eax, 1
.text:00974D49 ?LiveStats_GetIntPlayerStatInternal@@YA_NPAHHQAPBDPAD@Z                                                                                                                                        mov     eax, 1
.text:00975290 ?LiveStats_SetIntPlayerStatInternal@@YA_NHHQAPBDPADI@Z                                                                                                                                         mov     eax, 1
.text:00977735 LiveStats_GetItemStat                                                                                                                                                                          mov     eax, 1
.text:0097FA7E ?Register@?$CCallback@VLiveSteamServer@@USteamServersConnected_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUSteamServersConnected_t@@@Z@Z                                                         mov     eax, 1
.text:0097FAEE ?Register@?$CCallback@VLiveSteamServer@@USteamServersDisconnected_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUSteamServersDisconnected_t@@@Z@Z                                                   mov     eax, 1
.text:0097FB5E ?Register@?$CCallback@VLiveSteamServer@@UGSPolicyResponse_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSPolicyResponse_t@@@Z@Z                                                                   mov     eax, 1
.text:0097FBCE ?Register@?$CCallback@VLiveSteamServer@@UGSClientApprove_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientApprove_t@@@Z@Z                                                                     mov     eax, 1
.text:0097FC4E ?Register@?$CCallback@VLiveSteamServer@@UGSClientDeny_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientDeny_t@@@Z@Z                                                                           mov     eax, 1
.text:0097FCCE ?Register@?$CCallback@VLiveSteamServer@@UGSClientKick_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientKick_t@@@Z@Z                                                                           mov     eax, 1
.text:0098142A ?LiveStorage_ReadDWFileByUserID@@YAPAUTaskRecord@@HPAUdwFileOperationInfo@@_K@Z                                                                                                                mov     eax, 1
.text:009818D6 ?LiveStorage_GetServerTimeComplete@@YAXPAUTaskRecord@@@Z                                                                                                                                       mov     eax, 1
.text:00983858 ?LiveStorage_FileShare_ReadListingSuccess@@YAXPAUTaskRecord@@@Z                                                                                                                                mov     eax, 1
.text:0098517C ?LiveStorage_FileShare_WriteSummarySuccess@@YAXPAUTaskRecord@@@Z                                                                                                                               mov     eax, 1
.text:00986BD1 ?LiveStorage_Init@@YA_NXZ                                                                                                                                                                      mov     eax, 1
.text:00987B06 LiveStorage_ProcessOnlineWAD                                                                                                                                                                   mov     eax, 1
.text:0098971A ?SV_CommitClientLeaderboards@@YAXXZ                                                                                                                                                            mov     eax, 1
.text:0098B0B6 ?SV_DWWriteClientStats@@YAXPAUclient_t@@@Z                                                                                                                                                     mov     eax, 1
.text:0098C6B6 ?SV_CACValidateWriteCAC@@YAX_KPAEI@Z                                                                                                                                                           mov     eax, 1
.text:0098C896 ?SV_CACValidateWriteGlobal@@YAX_KPAEI@Z                                                                                                                                                        mov     eax, 1
.text:00999E96 ?GenerateVerts@GlassRenderer@@QAEXHII@Z                                                                                                                                                        mov     eax, 1
.text:0099B2B2 ?GetSmallestShards@GlassRenderer@@QAEXI_N0@Z                                                                                                                                                   mov     eax, 1
.text:0099B402 ?GetLargestShards@GlassRenderer@@QAEXI_N0@Z                                                                                                                                                    mov     eax, 1
.text:0099CF71 ?DrawDebug@GlassRenderer@@QAEXXZ                                                                                                                                                               mov     eax, 1
.text:0099E1AF ?back@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAPAUGlassShard@@XZ                                                                                            mov     eax, 1
.text:0099E20F ?pop_back@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEXXZ                                                                                                        mov     eax, 1
.text:0099E409 ??D?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBEABQAUGlassShard@@XZ                                                                         mov     eax, 1
.text:0099E497 ??9?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBE_NABV012@@Z                                                                                 mov     eax, 1
.text:0099E709 ??E?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                 mov     eax, 1
.text:0099E797 ??8?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBE_NABV012@@Z                                                                                 mov     eax, 1
.text:0099E809 ??0?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAE@PAU_Node@?$_List_nod@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@2@PBV12@@Z mov     eax, 1
.text:0099E849 ??F?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                   mov     eax, 1
.text:0099E87C ??F?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                   mov     eax, 1
.text:0099E9EA ?Allocate@?$FixedSizeAllocator@PAUShardGroup@@@@QAEPAUShardGroup@@XZ                                                                                                                           mov     eax, 1
.text:0099EC2B ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                                                                              mov     eax, 1
.text:0099ECB7 ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                                                                              mov     eax, 1
.text:0099EE23 ?FreeAll@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXXZ                                                                                                                                          mov     eax, 1
.text:0099F43D ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                                                                                                               mov     eax, 1
.text:009AB58C ?Split@GlassShard@@QAEHQAPAU1@MIM@Z                                                                                                                                                            mov     eax, 1
.text:009D3527 sub_9D3510                                                                                                                                                                                     mov     eax, 1
.text:00BB9DE6 sub_BB9DE0                                                                                                                                                                                     mov     eax, 1
.text:00BBAB96 sub_BBAB90                                                                                                                                                                                     mov     eax, 1
.text:00BCFCBD sub_BCFC90                                                                                                                                                                                     mov     eax, 1
.text:00BD3DE7 sub_BD3C40                                                                                                                                                                                     mov     eax, 1
.text:00BD9F36 sub_BD9F30                                                                                                                                                                                     mov     eax, 1
.text:00BDC8DA sub_BDC850                                                                                                                                                                                     mov     eax, 1
.text:00BE1C8A _HTNextField                                                                                                                                                                                   mov     eax, 1
.text:00BE1F8A _HTNextPair                                                                                                                                                                                    mov     eax, 1
.text:00BE21DC _HTNextElement                                                                                                                                                                                 mov     eax, 1

Address        Function                                                                                                                            Instruction
-------        --------                                                                                                                            -----------
.text:00439A23 ?Phys_Vec3ToNitrousVec@@YA?BVphys_vec3@@QBM@Z                                                                                       mov     edx, 1
.text:00440374 ?comp_aabb_loc@gjk_base_t@@UAEXXZ                                                                                                   mov     edx, 1
.text:004B3EC6 ?CG_EntityEvent@@YAXHPAUcentity_s@@H@Z                                                                                              mov     edx, 1
.text:004B7DC6 ?CG_PhysLaunch@@YAXHPAUcentity_s@@PBUentityState_s@@@Z                                                                              mov     edx, 1
.text:00594220 DB_SyncExternalAssets                                                                                                               mov     edx, 1
.text:005970DB ?DevGui_AddGraph@@YAXPBDPAUDevGraph@@@Z                                                                                             mov     edx, 1
.text:005F4FAB Bullet_ImpactEffect                                                                                                                 mov     edx, 1
.text:0061149D ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                                                    mov     edx, 1
.text:0066F06D ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F0A1 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F100 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F462 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:006D194B ?Com_PrintMessage@@YAXHPBDH@Z                                                                                                       mov     edx, 1
.text:006D24D7 ?Com_Error@@YAXW4errorParm_t@@PBDZZ                                                                                                 mov     edx, 1
.text:006D6972 Com_Frame_Try_Block_Function                                                                                                        mov     edx, 1
.text:006D6C66 Com_Frame_Try_Block_Function                                                                                                        mov     edx, 1
.text:0070F654 SV_InitGameVM                                                                                                                       mov     edx, 1
.text:007101B2 ?SV_Startup@@YAXH@Z                                                                                                                 mov     edx, 1
.text:0071020B ?SV_Startup@@YAXH@Z                                                                                                                 mov     edx, 1
.text:00712DED ?SV_SendServerCommand@@YAXPAUclient_t@@W4svscmd_type@@PBDZZ                                                                         mov     edx, 1
.text:00716827 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                                                      mov     edx, 1
.text:0071BDC0 ?SV_AddModifiedStats@@YAXH@Z                                                                                                        mov     edx, 1
.text:0073E836 ?UI_Gametype_ReadFromMemFile@@YA_NAAUMemoryFile@@@Z                                                                                 mov     edx, 1
.text:00740BFB ?UI_Gametype_FileShareDownloadComplete@@YAXPAUdwFileShareReadFileTask@@@Z                                                           mov     edx, 1
.text:007841D9 ?AddPlayerToScene@UIViewer@@QAEXABUPlayerParams@1@ABUWeaponParams@1@_N@Z                                                            mov     edx, 1
.text:007D3EE4 KeyValueToField                                                                                                                     mov     edx, 1          ; jumptable 007D3DB3 case 9
.text:007D9EE1 Win_GetEvent                                                                                                                        mov     edx, 1
.text:007DB72D _WinMain@16                                                                                                                         mov     edx, 1
.text:00823F7E ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:0082432B ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:00824467 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:00828D7C ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                  mov     edx, 1
.text:0082A93F ?UpdateScriptVehicleControl@NitrousVehicleController@@AAEXAAVNitrousVehicle@@M@Z                                                    mov     edx, 1
.text:00831145 ?create@gjk_aabb_t@@SAPAU1@ABVphys_vec3@@0HPAVgjk_collision_visitor@@@Z                                                             mov     edx, 1
.text:008313B5 ?support@gjk_aabb_t@@UBEXABVphys_vec3@@PAV2@1@Z                                                                                     mov     edx, 1
.text:008328B8 ?create@gjk_brush_t@@SAPAUgjk_base_t@@PBUcbrush_t@@HPAVgjk_collision_visitor@@@Z                                                    mov     edx, 1
.text:00833FF6 ?create@gjk_double_sphere_t@@SAPAU1@ABVphys_vec3@@0MHPAVgjk_collision_visitor@@@Z                                                   mov     edx, 1
.text:00839378 ?set_bpi_env@broad_phase_info@@QAEXPAVphys_auto_activate_callback@@@Z                                                               mov     edx, 1
.text:0083BE60 ?debug_callback@@YAXPAX@Z                                                                                                           mov     edx, 1
.text:0083C2A2 ?collide_vehicle_wheels@@YAXAAUPhysObjUserData@@@Z                                                                                  mov     edx, 1
.text:0083F03E ?Phys_EffectsProcess@@YAXXZ                                                                                                         mov     edx, 1
.text:008480C9 ?set@broad_phase_group@@QAEXXZ                                                                                                      mov     edx, 1
.text:0084AE2C ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                     mov     edx, 1
.text:0084B14A ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                     mov     edx, 1
.text:0084B6BD ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                 mov     edx, 1
.text:0084BACF ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                 mov     edx, 1
.text:0085934B ?init_winding@@YAXABUplane_lt@@AAV?$phys_static_array@Vphys_vec3@@$0CAA@@@@Z                                                        mov     edx, 1
.text:0086A489 Ragdoll_TunnelTest                                                                                                                  mov     edx, 1
.text:0086A6C0 Ragdoll_TunnelTest                                                                                                                  mov     edx, 1
.text:009372DE ?SND_Init@@YAXXZ                                                                                                                    mov     edx, 1
.text:0094971C ?SND_LogMissingAliasId@@YAXI@Z                                                                                                      mov     edx, 1
.text:0094ABC8 ?SND_PlayInternal@@YAXIHMTSndEntHandle@@QBM1_NPAUsnd_playback@@@Z                                                                   mov     edx, 1
.text:0094BFA8 ?SND_PlayLoopAt@@YAXIQBM@Z                                                                                                          mov     edx, 1
.text:0094C0E8 ?SND_StopLoopAt@@YAXIQBM@Z                                                                                                          mov     edx, 1
.text:0094E327 ?SND_CommandPump@@YAIXZ                                                                                                             mov     edx, 1
.text:0094F594 ?SND_NotifyPump@@YAXXZ                                                                                                              mov     edx, 1
.text:0095119C ?Snd_StreamOpen@@YAXIPBD_NIPAD@Z                                                                                                    mov     edx, 1
.text:009515AC ?Snd_StreamClose@@YAXI@Z                                                                                                            mov     edx, 1
.text:00962FDD Live_FileShare_Read_f                                                                                                               mov     edx, 1
.text:0096F93E ?PCache_GetEntry@@YAPAUPCacheEntry@@H_K@Z                                                                                           mov     edx, 1
.text:0098CB1B ?SV_CACValidateReadCAC@@YAPAUTaskRecord@@_KPAEI@Z                                                                                   mov     edx, 1
.text:0098CEFC ?SV_CACValidateReadGlobal@@YAPAUTaskRecord@@_KPAEI@Z                                                                                mov     edx, 1
.text:00999FB4 ?ExplosionEvent@GlassRenderer@@AAEXQBMMMMH@Z                                                                                        mov     edx, 1
.text:0099A0CB ?TracePoint@GlassRenderer@@AAEHQBM0@Z                                                                                               mov     edx, 1
.text:0099A585 ?GetShardGroup@GlassRenderer@@QAEPAUShardGroup@@IPBUGlassDef@@@Z                                                                    mov     edx, 1
.text:0099E157 ?front@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAPAUGlassShard@@XZ                                mov     edx, 1
.text:0099E374 ?end@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@XZ                           mov     edx, 1
.text:0099E42D ??D?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBEABQAUGlassShard@@XZ              mov     edx, 1
.text:0099E72D ??E?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAEAAV012@XZ                      mov     edx, 1
.text:0099EC6D ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                   mov     edx, 1
.text:0099EFC7 ?push_front@?$list@PAUGlassPhysics@@V?$SmallAllocatorTemplate@PAPAUGlassPhysics@@@@@std@@QAEXABQAUGlassPhysics@@@Z                  mov     edx, 1
.text:0099F229 ?erase@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@V?$_Const_iterator@$00@12@@Z mov     edx, 1
.text:0099F4CD ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                                                    mov     edx, 1
.text:00A56C3D R_MarkModelCoreCallback_1_                                                                                                          mov     edx, 1
.text:00A747B4 RB_StandardDrawCommands                                                                                                             mov     edx, 1
.text:00B61BB1 _jinit_color_converter                                                                                                              mov     edx, 1
.text:00BBA5FF sub_BBA5F0                                                                                                                          mov     edx, 1
.text:00BBB06D sub_BBB040                                                                                                                          mov     edx, 1
.text:00BC8CE3 _HTDoConnect                                                                                                                        mov     edx, 1
.text:00BC94B3 _HTDoListen                                                                                                                         mov     edx, 1
.text:00BD04D9 sub_BD0400                                                                                                                          mov     edx, 1

Address        Function                                                                                                    Instruction
-------        --------                                                                                                    -----------
.text:0041A9B7 ?BG_EmblemsInit@@YAXXZ                                                                                      mov     ecx, 1
.text:00539E4E ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539F3A ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539FC6 ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539FFA ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:0053A060 ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:0053A1BA ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:005D2549 FX_DrawElement_Setup_1_                                                                                     mov     ecx, 1
.text:006114CA ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                            mov     ecx, 1
.text:0066F411 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                     mov     ecx, 1
.text:0068D437 ?ConsoleCommand@@YAHXZ                                                                                      mov     ecx, 1
.text:006D1F4F ?Com_Shutdown@@YAXPBD@Z                                                                                     mov     ecx, 1
.text:006D66B3 ?Com_LoadFrontEnd@@YAXXZ                                                                                    mov     ecx, 1
.text:006D69EC Com_Frame_Try_Block_Function                                                                                mov     ecx, 1
.text:00703CDC SV_Map_f                                                                                                    mov     ecx, 1
.text:00703F82 ShowLoadErrorsSummary                                                                                       mov     ecx, 1
.text:007077C4 ?SV_DirectConnect@@YAXUnetadr_t@@@Z                                                                         mov     ecx, 1
.text:007083D2 ?SV_DropClient@@YAXPAUclient_t@@PBD_N2@Z                                                                    mov     ecx, 1
.text:00711693 ?SV_Init@@YAXXZ                                                                                             mov     ecx, 1
.text:00712186 ?SV_Shutdown@@YAXPBD@Z                                                                                      mov     ecx, 1
.text:007121F2 ?SV_Shutdown@@YAXPBD@Z                                                                                      mov     ecx, 1
.text:00713FA6 ?SVC_Info@@YAXUnetadr_t@@PAVbdSecurityID@@_N@Z                                                              mov     ecx, 1
.text:00714410 ?SV_ConnectionlessPacket@@YAXUnetadr_t@@PAUmsg_t@@@Z                                                        mov     ecx, 1
.text:00716815 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                              mov     ecx, 1
.text:00716BBC ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                              mov     ecx, 1
.text:0076567F ?Item_ListBox_Viewmax@@YAHHHPAUitemDef_s@@@Z                                                                mov     ecx, 1
.text:0078A546 ?Expression_Parse@@YA_NPAPBDPAUExpressionStatement@@PAXH@Z                                                  mov     ecx, 1
.text:0078AAC7 Expression_ParseOperatorToken                                                                               mov     ecx, 1
.text:0078AF0F ?Expression_Init@@YAXXZ                                                                                     mov     ecx, 1
.text:007DB717 _WinMain@16                                                                                                 mov     ecx, 1
.text:007DCA8A ?dwPlatformInit@@YAHAAVbdNetStartParams@@@Z                                                                 mov     ecx, 1
.text:007DE9F1 ConWndProc                                                                                                  mov     ecx, 1          ; jumptable 007DE8F9 case 11
.text:0081EB9C ??0NitrousVehicle@@QAE@XZ                                                                                   mov     ecx, 1
.text:0082158A ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                 mov     ecx, 1
.text:008243E7 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                     mov     ecx, 1
.text:0082D29C ?SetScriptTarget@NitrousVehicleController@@QAEXAAVNitrousVehicle@@AAVphys_vec3@@MM_N@Z                      mov     ecx, 1
.text:0082D5A3 ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:0082D5D7 ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:0082D63C ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:00831CFB ?create@gjk_obb_t@@SAPAU1@ABVphys_mat44@@ABVphys_vec3@@HPAVgjk_collision_visitor@@@Z                        mov     ecx, 1
.text:00831E9D ?support@gjk_obb_t@@UBEXABVphys_vec3@@PAV2@1@Z                                                              mov     ecx, 1
.text:00832EBB ?create@gjk_partition_t@@SAPAU1@PBUCollisionAabbTree@@PAVgjk_collision_visitor@@@Z                          mov     ecx, 1
.text:0083435E ?create@gjk_cylinder_t@@SAPAU1@HMMABVphys_mat44@@HPAVgjk_collision_visitor@@@Z                              mov     ecx, 1
.text:00835C3E ?create@gjk_polygon_cylinder_t@@SAPAU1@AAY02$$CBM0MHPAVgjk_collision_visitor@@@Z                            mov     ecx, 1
.text:00835DE2 ?destroy_gjk_geom@@YAXPAUgjk_base_t@@@Z                                                                     mov     ecx, 1          ; jumptable 00835D76 default case
.text:00836F30 ?set_xform@gjk_base_t@@QAEXPBVphys_mat44@@@Z                                                                mov     ecx, 1
.text:0083D1B2 ?collide_vehicle_wheels@@YAXAAUPhysObjUserData@@@Z                                                          mov     ecx, 1
.text:00848161 ?add_bpi@broad_phase_group@@QAEXPAVbroad_phase_info@@@Z                                                     mov     ecx, 1
.text:0084ABCC ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084AE9A ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084B18C ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084B4D6 ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084B6FC ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084BA1A ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084BE20 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084BE54 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084BEB9 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C270 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C2A4 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C309 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084D493 ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0084D4C7 ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0084D52C ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0085940E ?init_winding@@YAXABUplane_lt@@AAV?$phys_static_array@Vphys_vec3@@$0CAA@@@@Z                                mov     ecx, 1
.text:00869346 Ragdoll_CreatePhysObj                                                                                       mov     ecx, 1
.text:0086A4FA Ragdoll_TunnelTest                                                                                          mov     ecx, 1
.text:008E3F5B yylex                                                                                                       mov     ecx, 1
.text:00931104 ?SND_ShouldInit@@YA_NXZ                                                                                     mov     ecx, 1
.text:0094B96A ?SND_SetPlaybackAttenuation@@YAXHM@Z                                                                        mov     ecx, 1
.text:0094BA3A ?SND_SetPlaybackAttenuationRate@@YAXHM@Z                                                                    mov     ecx, 1
.text:0094BB0A ?SND_SetPlaybackPitch@@YAXHM@Z                                                                              mov     ecx, 1
.text:0094BBDA ?SND_SetPlaybackPitchRate@@YAXHM@Z                                                                          mov     ecx, 1
.text:0094BD6D ?SND_SetSnapshot@@YAXW4snd_snapshot_type@@PBDMM@Z                                                           mov     ecx, 1
.text:0094BE7F ?SND_SetEntState@@YAXTSndEntHandle@@@Z                                                                      mov     ecx, 1
.text:0094C2B6 ?SND_PlayLineAt@@YAXIQBM0@Z                                                                                 mov     ecx, 1
.text:0094C4A6 ?SND_StopLineAt@@YAXIQBM0@Z                                                                                 mov     ecx, 1
.text:0096D683 ?LiveNews_PopulateFriendNews@@YAXH_KPAD@Z                                                                   mov     ecx, 1
.text:0096E04F ?LiveNews_GetOwnNews@@YAXH@Z                                                                                mov     ecx, 1
.text:00978364 LiveStats_GetPlayerStat                                                                                     mov     ecx, 1
.text:0098D9DB ?SV_FetchWADDeferred@@YAXXZ                                                                                 mov     ecx, 1
.text:009992AC ?Reset@GlassRenderer@@QAEXXZ                                                                                mov     ecx, 1
.text:0099955C ?RemoveGlassShards@GlassRenderer@@QAEXI@Z                                                                   mov     ecx, 1
.text:00999803 ?RemovePhysicsShards@GlassRenderer@@QAEXXZ                                                                  mov     ecx, 1
.text:00999C90 ?Update@GlassRenderer@@QAEXH@Z                                                                              mov     ecx, 1
.text:0099A8CF ?Insert@SortedShardsList@GlassRenderer@@QAEXPAUGlassShard@@@Z                                               mov     ecx, 1
.text:0099ABAF ?InsertReverse@SortedShardsList@GlassRenderer@@QAEXPAUGlassShard@@@Z                                        mov     ecx, 1
.text:0099E3CC ?begin@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@XZ mov     ecx, 1
.text:0099ECFA ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                           mov     ecx, 1
.text:0099F3FB ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                            mov     ecx, 1
.text:0099F487 ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                            mov     ecx, 1
.text:009CA813 ?Demo_WriteToStream@@YAIPAXII@Z                                                                             mov     ecx, 1
.text:00A57217 R_AddMarkFragment_1_                                                                                        mov     ecx, 1
.text:00A74410 RB_StandardDrawCommands                                                                                     mov     ecx, 1
.text:00A7A2D7 ?R_IssueRenderCommands@@YAXI@Z                                                                              mov     ecx, 1
.text:00AA64F4 R_StreamUpdateAabbNode_r_0_                                                                                 mov     ecx, 1
.text:00AB7687 RB_RenderCommandFrame                                                                                       mov     ecx, 1
.text:00B6910C _jinit_color_deconverter                                                                                    mov     ecx, 1
.text:00BB921C sub_BB90C0                                                                                                  mov     ecx, 1
.text:00BDB226 sub_BDB1B0                                                                                                  mov     ecx, 1
.text:00BDDA52 sub_BDD9A0                                                                                                  mov     ecx, 1
*/