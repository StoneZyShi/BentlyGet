#pragma once
#include "polyfacetool.h"
#include "..\include\dgnplatform\MeshHeaderHandler.h"
#include "..\include\DgnPlatform\ElementGraphics.h"
#include "..\include\DgnView\LocateSubEntityTool.h"
#include <Mstn\ISessionMgr.h> 
#include <Mstn\MdlApi\MdlApi.h>
#include <DgnPlatform\DgnPlatformApi.h>
#include <DgnPlatform\Handler.h>
#include <PSolid\PSolidCoreAPI.h>
#include <DgnPlatform\SectionClip.h>

#include "utility.h"
#include <Windows.h>
#include <iostream>                                       
#include <fstream>
#include <string>
#include "sqlliteRW.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include  <direct.h>  

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;


#define pri(str) mdlDialog_dmsgsPrint(str);

//#define MY_DEBUG
#define MY_OBJ
//#define MY_MATRIX

std::string getTime();

std::string deleteLine(std::string FileName);

std::string WChar2Ansi(LPCWSTR pwszSrc);

std::string ws2s(const std::wstring& ws);
std::string ws2sT(const WString& ws);
std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen);

std::wstring s2ws(const std::string& s);

WString s2wsT(const std::string& s);

//int compress(const char* str1);
enum ROOTFILE {
	rootModel = 0,
	rootFile,
	rootPath,
	rootFilePath,
	rootFileAndModelPath,
};
WString getRootPath(ROOTFILE rf);

void deleteAllMark(WString& wstr, const std::string & mark);

enum CHECKDB
{
	del = 0,
	check,
};
int checkFileDB(WString file, CHECKDB flag);


void replace_all(std::string & str, char * oldValue, char * newValue);

void fileAndModel(const std::string& fileModel,std::string& file,std::string& model);



WCHAR* mbcsToUnicode(const char *zFilename);
char* unicodeToUtf8(const WCHAR *zWideFilename);


std::string toUtf8(const std::string& str);
