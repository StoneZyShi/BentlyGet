#include "fileControl.h"
#include <io.h>
#include <vector>
#include "utility.h"


fileControl::fileControl(std::string rootFile, std::string rootModel)
	:rootFile(rootFile), rootModel(rootModel)
{
	this->referencesTxtFile = ws2sT(getRootPath(ROOTFILE::rootPath));
	this->referencesNum = 0;
}


fileControl::~fileControl()
{
}

WString fileControl::start()
{
#ifdef MY_DEBUG
	pri(L"进入start");
#endif // MY_DEBUG

	//获取根目录所有element
	std::string path = this->rootFile.substr(0, this->rootFile.rfind("/") + 1);
	getData data(path, "./", "./", this->rootFile, this->rootModel, "", "");
	data.findAllActive();
	
	//扫描所有参考文件
	findAllReference references(ws2sT(getRootPath(ROOTFILE::rootFilePath)), ws2sT(getRootPath(ROOTFILE::rootModel)));
	std::string txt = ws2sT(getRootPath(ROOTFILE::rootPath)) +"/"+getTime()+".txt";
	std::string txtTem = ws2sT(getRootPath(ROOTFILE::rootPath)) + "/" + getTime() + "Tem.txt";
	int line = 0, temLine = 0;

	double myTransform[4][4] = {1,0,0,0,
							    0,1,0,0,
							    0,0,1,0, 
							    0,0,0,1};
	references.ergodicReference(txt, txtTem,line,temLine, myTransform);

	//跳转模型
	WString wfile(this->rootFile.c_str());
	WString wmode(this->rootModel.c_str());
	mdlSystem_setBatchProcessingState(PROCESSSTATE_Processing, NULL);
	if (SUCCESS == mdlSystem_newDesignFileAndModel(wfile.GetWCharCP(), wmode.GetWCharCP()))
	{
		if ((_access(txtTem.c_str(), 0)) != -1)
		{
			if (remove(txtTem.c_str()) != 0)
			{
				pri(L"删除文件失败");
			}
		}
	}
 	
	//获取所有参考element
	dbData(txt,line,this->rootFile,this->rootModel);

	if (SUCCESS == mdlSystem_newDesignFileAndModel(wfile.GetWCharCP(), wmode.GetWCharCP()))
	{
		if ((_access(txt.c_str(), 0)) != -1)
		{
			if (remove(txt.c_str()) != 0)
			{
				pri(L"删除文件失败");
			}
		}
	}


	return s2wsT(txt);
}

WString fileControl::dbData(const std::string referTxtFile,int& line, std::string& oldFile, std::string& oldModel)
{
	//pri(L"获取全部");
	while (line > 0)
	{
		std::string str = deleteLine(referTxtFile);
		while (str == "" || str == "\n")
			str = deleteLine(referTxtFile);
		line--;

		std::string fileName = str.substr(0, str.find("<->")).c_str();
		std::string modelName = str.substr(str.find("<->") + 3, str.find(">>>")- str.find("<->")-3);
		std::string transform = str.substr(str.find(">>>") + 3, str.rfind("<<<") - str.find(">>>") - 3);
		std::string logicalName = str.substr(str.find("<<<") + 3, str.length() - str.find("<<<") - 3);

		getData data(this->rootFile.substr(0, this->rootFile.rfind("/") + 1),oldFile, oldModel, fileName, modelName, transform, logicalName);

#ifdef MY_DEBUG
		pri(L"跳转:");
		pri(s2wsT(fileName + "    " + modelName).GetWCharCP());
#endif // MY_DEBUG
		if (SUCCESS == mdlSystem_newDesignFileAndModel(s2wsT(fileName).GetWCharCP(), s2wsT(modelName).GetWCharCP()))
		{
			Sleep(500);
		}
		
		data.findAllActive();
		dbData(referTxtFile, line, fileName, modelName);
	}

	return WString();
}

