#pragma once
#include <vector>

//#define BO1_BUILD

#ifdef BO1_BUILD
#define R_MAX_PIXEL_SHADER_CONSTS	256
#define R_MAX_CODE_INDEX			197
#else
#define R_MAX_PIXEL_SHADER_CONSTS	256
#define R_MAX_CODE_INDEX			105
#endif // BO1_BUILD

#define SAMPLER_INDEX_INVALID		(-1)

/*
enum MaterialTextureSource
{
	TEXTURE_SRC_CODE_BLACK = 0x0,
	TEXTURE_SRC_CODE_WHITE = 0x1,
	TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
	TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
	TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
	TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
	TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x6,
	TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x7,
	TEXTURE_SRC_CODE_FEEDBACK = 0x8,
	TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0x9,
	TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xA,
	TEXTURE_SRC_CODE_POST_EFFECT_SRC = 0xB,
	TEXTURE_SRC_CODE_POST_EFFECT_GODRAYS = 0xC,
	TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xD,
	TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xE,
	TEXTURE_SRC_CODE_SKY = 0xF,
	TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0x10,
	TEXTURE_SRC_CODE_DLIGHT_ATTENUATION = 0x11,
	TEXTURE_SRC_CODE_OUTDOOR = 0x12,
	TEXTURE_SRC_CODE_FLOATZ = 0x13,
	TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x14,
	TEXTURE_SRC_CODE_RAW_FLOATZ = 0x15,
	TEXTURE_SRC_CODE_CASE_TEXTURE = 0x16,
	TEXTURE_SRC_CODE_CINEMATIC_Y = 0x17,
	TEXTURE_SRC_CODE_CINEMATIC_CR = 0x18,
	TEXTURE_SRC_CODE_CINEMATIC_CB = 0x19,
	TEXTURE_SRC_CODE_CINEMATIC_A = 0x1A,
	TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1B,
	TEXTURE_SRC_CODE_FEATHER_FLOAT_Z = 0x1C,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_0 = 0x1D,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_1 = 0x1E,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_2 = 0x1F,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_3 = 0x20,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_LAST = 0x20,
	TEXTURE_SRC_CODE_LIGHTMAP_SECONDARYB = 0x21,
	TEXTURE_SRC_CODE_TEXTURE_0 = 0x22,
	TEXTURE_SRC_CODE_TEXTURE_1 = 0x23,
	TEXTURE_SRC_CODE_TEXTURE_2 = 0x24,
	TEXTURE_SRC_CODE_TEXTURE_3 = 0x25,
	TEXTURE_SRC_CODE_IMPACT_MASK = 0x26,
	TEXTURE_SRC_CODE_UI3D = 0x27,
	TEXTURE_SRC_CODE_MISSILE_CAM = 0x28,
	TEXTURE_SRC_CODE_COMPOSITE_RESULT = 0x29,
	TEXTURE_SRC_CODE_HEATMAP = 0x2A,
	TEXTURE_SRC_CODE_COUNT = 0x2B,
};
*/

enum MaterialShaderType
{
	MTL_VERTEX_SHADER = 0x0,
	MTL_PIXEL_SHADER = 0x1,
};

enum MaterialUpdateFrequency_FIX
{
	MTL_UPDATE_PER_PRIM = 0x0,
	MTL_UPDATE_PER_OBJECT = 0x1,
	MTL_UPDATE_RARELY = 0x2,
	MTL_UPDATE_CUSTOM = 0x3,
};

enum MaterialArgumentSourceType
{
	MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
	MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
	MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x2,
	MTL_ARG_CODE_PRIM_BEGIN = 0x3,
	MTL_ARG_CODE_VERTEX_CONST = 0x3,
	MTL_ARG_CODE_PIXEL_SAMPLER = 0x4,
	MTL_ARG_CODE_PIXEL_CONST = 0x5,
	MTL_ARG_CODE_PRIM_END = 0x6,
	MTL_ARG_MATERIAL_PIXEL_CONST = 0x6,
	MTL_ARG_LITERAL_PIXEL_CONST = 0x7,
	MLT_ARG_COUNT = 0x8,
};

enum ShaderParamType
{
	SHADER_PARAM_FLOAT4 = 0x0,
	SHADER_PARAM_SAMPLER_2D = 0x1,
	SHADER_PARAM_SAMPLER_3D = 0x2,
	SHADER_PARAM_SAMPLER_CUBE = 0x3,
	SHADER_PARAM_SAMPLER_1D = 0x4,
};

enum
{
	CUSTOM_SAMPLER_REFLECTION_PROBE = 0x0,
	CUSTOM_SAMPLER_LIGHTMAP_PRIMARY = 0x1,
	CUSTOM_SAMPLER_LIGHTMAP_SECONDARY = 0x2,
	CUSTOM_SAMPLER_LIGHTMAP_SECONDARYB = 0x3,
	CUSTOM_SAMPLER_COUNT = 0x4,
};

typedef int MaterialUpdateFrequency;
typedef int MaterialTextureSource;

typedef void MaterialStateMap;

struct ShaderIndexRange
{
	unsigned int first;
	unsigned int count;
	bool isImplicit;
};
CHECK_SIZE(ShaderIndexRange, 12);

struct CodeSamplerSource
{
	const char *name;
	MaterialTextureSource source;
	CodeSamplerSource *subtable;
	int arrayCount;
	int arrayStride;
};
CHECK_SIZE(CodeSamplerSource, 20);

struct CodeConstantSource
{
	const char *name;
	unsigned char source;
	CodeConstantSource *subtable;
	int arrayCount;
	int arrayStride;
};
CHECK_SIZE(CodeConstantSource, 20);

struct ShaderArgumentSource
{
	ShaderIndexRange indexRange;
	unsigned __int16 type;

	union
	{
		const float *literalConst;
		unsigned __int16 codeIndex;
		const char *name;
	} u;
};

struct ShaderUniformDef
{
  ShaderParamType type;
  const char *name;
  unsigned __int16 index;
  unsigned __int16 resourceDest;
  bool isTransposed;
  bool isAssigned;
};
CHECK_SIZE(ShaderUniformDef, 16);

struct ShaderVaryingDef
{
  const char *name;
  char streamDest;
  char resourceDest;
  bool isAssigned;
  bool isSpecialFragmentRegister;
};
CHECK_SIZE(ShaderVaryingDef, 8);

struct ShaderParameterSet
{
  ShaderUniformDef uniformInputs[512];
  ShaderVaryingDef varyingInputs[512];
  ShaderVaryingDef outputs[16];
  unsigned int uniformInputCount;
  unsigned int varyingInputCount;
  unsigned int outputCount;
};
CHECK_SIZE(ShaderParameterSet, 12428);

struct MaterialArgumentCodeConst
{
	unsigned __int16 index;
	char firstRow;
	char rowCount;
};
CHECK_SIZE(MaterialArgumentCodeConst, 4);

union MaterialArgumentDef
{
	const float *literalConst;
	MaterialArgumentCodeConst codeConst;
	unsigned int codeSampler;
	unsigned int nameHash;
};
CHECK_SIZE(MaterialArgumentDef, 4);

struct MaterialShaderArgument
{
	unsigned __int16 type;
	unsigned __int16 dest;
	MaterialArgumentDef u;
};
CHECK_SIZE(MaterialShaderArgument, 8);

struct ShaderArgumentDest
{
	ShaderIndexRange indexRange;
	const char *paramName;
};
CHECK_SIZE(ShaderArgumentDest, 16);

struct MaterialStreamRouting
{
	union
	{
		struct
		{
			char source;
			char dest;
		};

		char data[2];
	};
};
CHECK_SIZE(MaterialStreamRouting, 2);

struct GfxVertexShaderLoadDef
{
	DWORD *program;
	unsigned __int16 programSize;
};

struct MaterialVertexShaderProgram
{
	IDirect3DVertexShader9 *vs;
	GfxVertexShaderLoadDef loadDef;
};

struct MaterialVertexShader
{
	const char *name;
	MaterialVertexShaderProgram prog;
};

struct GfxPixelShaderLoadDef
{
	DWORD *program;
	unsigned __int16 programSize;
};

struct MaterialPixelShaderProgram
{
	IDirect3DPixelShader9 *ps;
	GfxPixelShaderLoadDef loadDef;
};

struct MaterialPixelShader
{
	const char *name;
	MaterialPixelShaderProgram prog;
};

struct MaterialVertexStreamRouting
{
	MaterialStreamRouting data[16];
	LPDIRECT3DVERTEXDECLARATION9 decl[18];
};

struct MaterialVertexDeclaration
{
	char streamCount;
	bool hasOptionalSource;
	bool isLoaded;
	MaterialVertexStreamRouting routing;
};
CHECK_SIZE(MaterialVertexDeclaration, 108);

struct MaterialPass
{
	MaterialVertexDeclaration *vertexDecl;
	MaterialVertexShader *vertexShader;

	union
	{
		MaterialPixelShader *pixelShader;
		MaterialPixelShader *localPixelShader;
	};
	
	char perPrimArgCount;
	char perObjArgCount;
	char stableArgCount;
	char customSamplerFlags;

	union
	{
		MaterialShaderArgument *localArgs;
		MaterialShaderArgument *args;
	};
};
CHECK_SIZE(MaterialPass, 20);

struct stream_source_info_t
{
	char Stream;
	char Offset;
	char Type;
};

struct stream_dest_info_t
{
	char Usage;
	char UsageIndex;
};

struct MaterialConstantDefRaw
{
	unsigned int nameOffset;
	float literal[4];
};

struct MaterialTextureDefRaw
{
	unsigned int nameOffset;
	char samplerState;
	char semantic;
	union
	{
		unsigned int imageNameOffset;
		unsigned int waterDefOffset;
	};
};

struct MaterialInfoRaw
{
	unsigned int nameOffset;
	unsigned int refImageNameOffset;
	char gameFlags;
	char sortKey;
	char textureAtlasRowCount;
	char textureAtlasColumnCount;
	float maxDeformMove;
	char deformFlags;
	char usage;
	unsigned __int16 toolFlags;
	unsigned int locale;
	unsigned __int16 autoTexScaleWidth;
	unsigned __int16 autoTexScaleHeight;
	float tessSize;
	int surfaceFlags;
	int contents;
};

struct MaterialRaw
{
	MaterialInfoRaw info;
	unsigned int refStateBits[2];
	unsigned __int16 textureCount;
	unsigned __int16 constantCount;
	unsigned int techSetNameOffset;
	unsigned int textureTableOffset;
	unsigned int constantTableOffset;
};

struct techsetOverride
{
	std::string key;
	std::string replacement;
};

extern std::vector<techsetOverride> techsetOverrideList;

LPDIRECT3DVERTEXDECLARATION9 Material_BuildVertexDecl(MaterialStreamRouting *routingData, int streamCount, stream_source_info_t *sourceTable);
void Load_BuildVertexDecl(MaterialVertexDeclaration **mtlVertDecl);
unsigned int Material_HashVertexDecl(MaterialStreamRouting *routingData, int streamCount);
MaterialVertexDeclaration *Material_AllocVertexDecl(MaterialStreamRouting *routingData, int streamCount, bool *existing);
const float *Material_RegisterLiteral(const float *literal);
const char *Material_RegisterString(const char *string);
bool Material_UsingTechnique(int techType);
bool Material_MatchToken(const char **text, const char *match);
MaterialStateMap *Material_RegisterStateMap(const char *name);
bool Material_LoadPassStateMap(const char **text, MaterialStateMap **stateMap);
bool Material_ParseIndex(const char **text, int indexCount, int *index);
const char *Material_NameForStreamDest(char dest);
bool Material_StreamDestForName(const char **text, const char *destName, char *dest);
bool Material_StreamSourceForName(const char **text, const char *sourceName, char *source);
bool Material_ResourceDestForStreamDest(char streamDest, ShaderVaryingDef *inputTable, unsigned int inputCount, char *resourceDest);
bool Material_CheckUnspecifiedVertexInputs(ShaderVaryingDef *inputTable, unsigned int inputCount);
bool Material_LoadPassVertexDecl(const char **text, ShaderVaryingDef *inputTable, unsigned int inputCount, MaterialPass *pass);
bool Material_DefaultIndexRange(ShaderIndexRange *indexRangeRef, unsigned int arrayCount, ShaderIndexRange *indexRangeSet);
bool Material_ParseIndexRange(const char **text, unsigned int arrayCount, ShaderIndexRange *indexRange);
void *BufferOffset(void *buffer, int offset);
bool Material_ParseArrayOffset(const char **text, int arrayCount, int arrayStride, int *offset);
bool Material_CodeSamplerSource_r(const char **text, int offset, CodeSamplerSource *sourceTable, ShaderArgumentSource *argSource);
bool Material_ParseSamplerSource(const char **text, ShaderArgumentSource *argSource);
bool Material_DefaultSamplerSourceFromTable(const char *constantName, ShaderIndexRange *indexRange, CodeSamplerSource *sourceTable, ShaderArgumentSource *argSource);
bool Material_DefaultSamplerSource(const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource);
bool Material_ParseVector(const char **text, int elemCount, float *vector);
bool Material_ParseLiteral(const char **text, const char *token, float *literal);
bool Material_ParseCodeConstantSource_r(MaterialShaderType shaderType, const char **text, int offset, CodeConstantSource *sourceTable, ShaderArgumentSource *argSource);
bool Material_ParseConstantSource(MaterialShaderType shaderType, const char **text, ShaderArgumentSource *argSource);
bool Material_DefaultConstantSourceFromTable(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, CodeConstantSource *sourceTable, ShaderArgumentSource *argSource);
bool Material_DefaultConstantSource(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource);
bool Material_UnknownShaderworksConstantSource(MaterialShaderType shaderType, const char *constantName, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource);
unsigned int Material_ElemCountForParamName(const char *shaderName, ShaderUniformDef *paramTable, unsigned int paramCount, const char *name, ShaderParamType *paramType);
bool Material_ParseArgumentSource(MaterialShaderType shaderType, const char **text, const char *shaderName, ShaderParamType paramType, ShaderArgumentSource *argSource);
bool Material_DefaultArgumentSource(MaterialShaderType shaderType, const char *constantName, ShaderParamType paramType, ShaderIndexRange *indexRange, ShaderArgumentSource *argSource);
unsigned int R_SetParameterDefArray(LPD3DXSHADER_CONSTANTTABLE constantTable, unsigned int constantIndex, ShaderUniformDef *paramDef);
int Material_PrepareToParseShaderArguments(D3DXSHADER_CONSTANTTABLE *constantTable, ShaderUniformDef *paramTable);
int Material_CompareShaderArgumentsForCombining(const void *e0, const void *e1);
bool Material_AttemptCombineShaderArguments(MaterialShaderArgument *arg0, MaterialShaderArgument *arg1);
unsigned int Material_CombineShaderArguments(unsigned int usedCount, MaterialShaderArgument *localArgs);
bool Material_SetShaderArguments(unsigned int usedCount, MaterialShaderArgument *localArgs, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args);
ShaderUniformDef *Material_GetShaderArgumentDest(const char *paramName, unsigned int paramIndex, ShaderUniformDef *paramTable, unsigned int paramCount);
bool MaterialAddShaderArgument(const char *shaderName, const char *paramName, MaterialShaderArgument *arg, char(*registerUsage)[64]);
bool Material_AddShaderArgumentFromLiteral(const char *shaderName, const char *paramName, unsigned __int16 type, const float *literal, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64]);
bool Material_AddShaderArgumentFromCodeConst(const char *shaderName, const char *paramName, unsigned __int16 type, unsigned int codeIndex, unsigned int offset, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64]);
void Material_AddShaderArgumentFromCodeSampler(unsigned __int16 type, unsigned int codeSampler, ShaderUniformDef *dest, MaterialShaderArgument *arg);
bool Material_AddShaderArgumentFromMaterial(const char *shaderName, const char *paramName, unsigned __int16 type, const char *name, ShaderUniformDef *dest, MaterialShaderArgument *arg, char(*registerUsage)[64]);
bool Material_AddShaderArgument(const char *shaderName, ShaderArgumentSource *argSource, ShaderArgumentDest *argDest, ShaderUniformDef *paramTable, unsigned int paramCount, unsigned int *usedCount, MaterialShaderArgument *argTable, char(*registerUsage)[64]);
bool CodeConstIsOneOf(unsigned __int16 constCodeIndex, const unsigned __int16 *consts, int numConsts);
bool Material_ParseShaderArguments(const char **text, const char *shaderName, MaterialShaderType shaderType, ShaderUniformDef *paramTable, unsigned int paramCount, unsigned __int16 *techFlags, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args);
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader);
void *Material_LoadShader(const char *shaderName, const char *shaderVersion);
MaterialVertexShader *Material_RegisterVertexShader(const char *shaderName, char shaderVersion);
MaterialPixelShader *Material_RegisterPixelShader(const char *shaderName, char shaderVersion);
char Material_ParseShaderVersion(const char **text);
char Material_GetStreamDestForSemantic(D3DXSEMANTIC *semantic);
void Material_SetVaryingParameterDef(D3DXSEMANTIC *semantic, ShaderVaryingDef *paramDef);
bool Material_SetPassShaderArguments_DX(const char **text, const char *shaderName, MaterialShaderType shaderType, const DWORD *program, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args);
bool Material_LoadPassVertexShader(const char **text, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, MaterialPass *pass, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args);
bool Material_LoadDeclTypes(const char **text, MaterialPass *pass);
bool Material_LoadPassPixelShader(const char **text, unsigned __int16 *techFlags, ShaderParameterSet *paramSet, MaterialPass *pass, unsigned int argLimit, unsigned int *argCount, MaterialShaderArgument *args);
bool Material_HasMatchingParameter(char find, ShaderVaryingDef *paramTable, unsigned int paramCount);
bool Material_HasMatchingParameter_BuggySdkWorkaround(char find, ShaderVaryingDef *paramTable, unsigned int paramCount);
bool Material_ValidateShaderLinkage(ShaderVaryingDef *vertexOutputs, unsigned int vertexOutputCount, ShaderVaryingDef *pixelInputs, unsigned int pixelInputCount);
MaterialUpdateFrequency Material_GetArgUpdateFrequency(MaterialShaderArgument *arg);
int Material_CompareShaderArgumentsForRuntime(const void *e0, const void *e1);
char Material_CountArgsWithUpdateFrequency(MaterialUpdateFrequency updateFreq, MaterialShaderArgument *args, unsigned int argCount, unsigned int *firstArg);
bool Material_LoadPass(const char **text, unsigned __int16 *techFlags, MaterialPass *pass, MaterialStateMap **stateMap, int rendererInUse);
void *Material_RegisterTechnique(const char *name, int renderer);
bool Material_IgnoreTechnique(const char *name);
int Material_TechniqueTypeForName(const char *name);
void *__cdecl Material_LoadTechniqueSet(const char *name, int renderer);

void hk_Material_LoadShader();
void hk_Material_LoadPass();

extern stream_source_info_t s_streamSourceInfo[18][10];
extern stream_dest_info_t s_streamDestInfo[20];
extern unsigned int g_customSamplerSrc[4];
extern unsigned int g_customSamplerDest[4];
extern const MaterialUpdateFrequency s_codeConstUpdateFreq[197];
extern const MaterialUpdateFrequency s_codeSamplerUpdateFreq[43];
extern const bool g_useTechnique[130];
extern CodeSamplerSource s_codeSamplers[];
extern CodeSamplerSource s_defaultCodeSamplers[];
extern CodeConstantSource s_codeConsts[];
extern CodeConstantSource s_defaultCodeConsts[];

//
// Used to store the current materials rawfile size globally for use by Material_LoadRaw
//
void mfh_MaterialLoad();

typedef int __cdecl Material_LoadRaw_t(MaterialRaw *mtlRaw, unsigned int materialType, int imageTrack);
extern Material_LoadRaw_t* o_Material_LoadRaw;

int Material_LoadRaw(MaterialRaw *mtlRaw, unsigned int materialType, int imageTrack);

void mfh_Material_ParseValueForState();