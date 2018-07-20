#pragma once
#include "getData.h"
#include "sqlliteRW.h"
#include "findAllReference.h"


class fileControl
{
public:
	fileControl(std::string rootFile, std::string rootModel);
	~fileControl();

	WString start();
	WString dbData(const std::string referTxtFile, int& line, std::string& oldFile, std::string& oldModel);
private:
	std::string referencesTxtFile;
	int referencesNum;
	std::vector<std::string> allReferences;

	std::string rootFile;
	std::string rootModel;
};

