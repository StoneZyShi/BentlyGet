#pragma once
#include <string>
#include "sqlliteRW.h"

extern "C" void mdlSystem_setBatchProcessingState(BatchProcessing_States, const WChar*);

class findAllReference
{
public:
	findAllReference(std::string rootFile,std::string rootModel);
	~findAllReference();

	void ergodicReference(const std::string& txt,const std::string& txtTem,int& line, int& temLine,double myTransform[4][4]);
	void openModel(const std::string& txt, const std::string& txtTem, int& line, int& temLine,const std::string& file,const std::string& model, double myTransform[4][4]);

private:
	std::string rootFile;
	std::string rootModel;
};
