#include "utility.h"
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>
#include <string.h>

using namespace std;

std::string getTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d--%H-%M-%S", localtime(&timep));
	return tmp;
}

std::string deleteLine(std::string FileName)
{
	vector<string> vecContent;
	string strLine, str;
	ifstream inFile(FileName);

	getline(inFile, str);
	vecContent.push_back(str);
	while (inFile)
	{
		getline(inFile, strLine);
		vecContent.push_back(strLine);
	}
	inFile.close();
	
	vecContent.erase(vecContent.begin());
	ofstream outFile(FileName);
	vector<string>::const_iterator iter = vecContent.begin();
	for (; vecContent.end() != iter; ++iter)
	{
		outFile.write((*iter).c_str(), (*iter).size()) << endl;
	}

	outFile.close();

	return str;
}


std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen]();
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

std::string ws2s(const std::wstring& ws)
{
	return WChar2Ansi(ws.c_str());
}

std::string ws2sT(const WString& ws)
{
	return WChar2Ansi(ws.c_str());
}
std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;

	WCHAR *pwszDst = new WCHAR[nSize + 1]();
	if (NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF) // skip Oxfeff  
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];

	std::wstring wcharString(pwszDst);
	delete pwszDst;


	return wcharString;
}

std::wstring s2ws(const std::string& s)
{
	return Ansi2WChar(s.c_str(), (int)(s.size()));
}

WString s2wsT(const std::string & s)
{
	return WString(s.c_str());
}


WString getRootPath(ROOTFILE rf)
{
	WString wstr = L"";
	WString filePath = ISessionMgr::GetActiveDgnFile()->GetFileName();
	WString model = ISessionMgr::GetActiveDgnModelP()->GetDgnModelP()->GetModelName();
	std::string sFilePath = ws2sT(filePath);
	replace_all(sFilePath, "\\", "/");
	filePath = s2wsT(sFilePath);

	switch (rf)
	{
	case ROOTFILE::rootFilePath:
		wstr = filePath;
		break;
	case ROOTFILE::rootPath:
		wstr = filePath.substr(0, filePath.rfind(L"/"));
		break;
	case ROOTFILE::rootModel:
		wstr = model;
		break;
	case ROOTFILE::rootFile:
		wstr = filePath.substr(filePath.rfind(L"/")+1,filePath.rfind(L".")- filePath.rfind(L"/") -1);
		break;
	case ROOTFILE::rootFileAndModelPath:
		wstr = filePath + L"(" + model + L")";
		break;
	default:
		break;
	}
	return wstr;
}

void deleteAllMark(WString& wstr, const std::string & mark)
{
	std::string s = ws2s(wstr.GetWCharCP());
	int nSize = (int)(mark.size());
	while (1)
	{
		std::string::size_type pos = (int)(s.find(mark));
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
	wstr = s2wsT(s);
}

int checkFileDB(WString file, CHECKDB flag)
{	
	std::string fileSQL = ws2s(file.GetWCharCP());
	replace_all(fileSQL, "\\", "/");
	fileSQL = fileSQL.substr(0,fileSQL.rfind("/")) + "/file.db";
#ifdef MY_DEBUG
	pri(s2wsT(fileSQL).GetWCharCP());
#endif // MY_DEBUG
	if (flag == CHECKDB::del)
	{
		if ((_access(fileSQL.c_str(), 0)) != -1)
		{
			if (remove(fileSQL.c_str()) != 0)
				return 2;//删除失败
			else
				return 0;//删除成功
		}
	}
	else
	{
		if ((_access(fileSQL.c_str(), 0)) != -1)
			return 1;//文件存在
		else
			return 0;//文件不存在
	}
	return 0;
}

void replace_all(std::string & str, char * oldValue, char * newValue)
{
	std::string::size_type pos(0);

	while (true) 
	{
		pos = str.find(oldValue, pos);
		if (pos != (std::string::npos))
		{
			str.replace(pos, strlen(oldValue), newValue);
			pos += 2;//注意是加2，为了跳到下一个反斜杠
		}
		else
			break;
	}
}

void fileAndModel(const std::string & fileModel, std::string & file, std::string& model)
{
	if (fileModel.find("<->")!=std::string::npos)
	{
		int len = (int)(fileModel.find("<->"));
		file = fileModel.substr(0, len);
		model = fileModel.substr(len + 3, fileModel.length() - len - 3);
	}
}


//int compress(const char * str1)
//{
	//char* str = new char[strlen(str1)];
	//strcpy(str,str1);
	//
	//while (strlen(str) > 4)
	//{
	//	int len = strlen(str) % 2 > 0 ? strlen(str) / 2 : strlen(str) / 2 + 1;
	//	for (int i = 0; i < strlen(str) / 2; i++)
	//	{
	//		str[i] ^= str[i + len];
	//		str[i + len] = '\0';
	//	}
	//}

	//unsigned int a = 0;
	//for (int i = 0; i < strlen(str); i++)
	//{
	//	a += (unsigned int)str[i] << 4 * i;
	//}

	//return a;
//}

WCHAR* mbcsToUnicode(const char *zFilename)
{
	int nByte;
	WCHAR *zMbcsFilename;
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL, 0) * sizeof(WCHAR);
	zMbcsFilename = (WCHAR *)malloc(nByte * sizeof(zMbcsFilename[0]));
	if (zMbcsFilename == 0)
	{
		return 0;
	}
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
	if (nByte == 0)
	{
		free(zMbcsFilename);
		zMbcsFilename = 0;
	}
	return zMbcsFilename;
}

//有Unicode转为UTF-8

char* unicodeToUtf8(const WCHAR *zWideFilename)
{
	int nByte; char *zFilename;
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char *)malloc(nByte);
	if (zFilename == 0)
	{
		return 0;
	}
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte, 0, 0);
	if (nByte == 0)
	{
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

std::string toUtf8(const std::string& str)
{
	char * pPath;
	WCHAR *wcPath;
	wcPath = mbcsToUnicode(str.c_str());//strFulPath即为带有中文的全路径字符串
	pPath = unicodeToUtf8(wcPath);
	std::string szDBFile = pPath;

	return szDBFile;
}

