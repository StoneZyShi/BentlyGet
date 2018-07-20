#include "getData.h"
#include "findAllReference.h"
#include <math.h>
#include "utility.h"

using namespace std;
getData::getData(const std::string path, const std::string & fileName, const std::string & modelName, const std::string & referFileName, const std::string & referModelName, const std::string& transform, const std::string& logicalName)
	:path(path), fileName(fileName), modelName(modelName), referFileName(referFileName), referModelName(referModelName), transform(transform), logicalName(logicalName)
{
}

getData::~getData()
{
}

std::string getData::getFileNameWithoutPath()
{
	return this->fileName.substr(this->fileName.rfind("/") + 1, this->fileName.rfind(".") - this->fileName.rfind("/") - 2);
}
std::string getData::getModelNameWithoutPath()
{
	return this->modelName.substr(this->modelName.rfind("/") + 1, this->modelName.rfind(".") - this->modelName.rfind("/") - 2);
}
std::string getData::getReferFileNameWithoutPath()
{
	return this->referFileName.substr(this->referFileName.rfind("/") + 1, this->referFileName.rfind(".") - this->referFileName.rfind("/") - 2);
}
std::string getData::getReferModelNameWithoutPath()
{
	return this->referModelName.substr(this->referModelName.rfind("/") + 1, this->referModelName.rfind(".") - this->referModelName.rfind("/") - 2);
}

std::string getData::changeToDbFileName()
{
	return getFileNameWithoutPath() + "-" + this->modelName + "_"
		+ getReferFileNameWithoutPath() + "-" + this->referModelName;
}

void getData::gotoNewModel()
{
	mdlSystem_setBatchProcessingState(PROCESSSTATE_Processing, NULL);
	if (SUCCESS == mdlSystem_newDesignFileAndModel(s2wsT(this->referFileName).GetWCharCP(), s2wsT(this->referModelName).GetWCharCP()))
	{
		Sleep(500);
#ifdef MY_DEBUG
		pri(L"进入新模型成功");
#endif // MY_DEBUG
	}
	else
	{
#ifdef MY_DEBUG
		pri(L"进入新模型失败");
#endif // MY_DEBUG
	}
}

void getData::findAllActive()
{
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	DgnModel::ElementsCollection elemColl = pActiveModel->GetElementsCollection();

	replace_all(this->fileName, "\\", "/");
	replace_all(this->referFileName, "\\", "/");
	std::string oldFile;
	if (this->fileName == "./")
		oldFile = this->fileName;
	else
		oldFile = this->fileName.substr(this->fileName.rfind("/") + 1, this->fileName.rfind(".") - this->fileName.rfind("/") - 1);
	std::string referFile = this->referFileName.substr(this->referFileName.rfind("/") + 1, this->referFileName.rfind(".") - this->referFileName.rfind("/") - 1);

	std::string db = this->path + oldFile + "_" + this->modelName+"("+ this->logicalName + ")--" + referFile + "_" + this->referModelName + ".db";
	//std::string db = "D:/MDLsource/" + oldFile + "_" + this->modelName + "--" + referFile + "_" + this->referModelName + ".db";
	WString dbName = s2wsT(db);



	double myTransform[4][4] = { 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };

	pri(s2wsT(this->transform).GetWCharCP());
	if (this->transform != "")
	{
		std::string transformValue;
		this->transform = this->transform.substr(this->transform.find(",") + 1, this->transform.length() - this->transform.find(",") - 1);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				transformValue = this->transform.substr(0, this->transform.find(","));
				myTransform[i][j] = atof(transformValue.c_str());
				this->transform = this->transform.substr(this->transform.find(",") + 1, this->transform.length() - this->transform.find(",") - 1);
			}
		}

#ifdef MY_MATRIX
		WString ws;
		ws.Sprintf(L"生成obj变换矩阵 modelName:%s referModelName:%s   \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f\n",
			this->modelName.c_str(), this->referModelName.c_str(),
			myTransform[0][0], myTransform[0][1], myTransform[0][2], myTransform[0][3],
			myTransform[1][0], myTransform[1][1], myTransform[1][2], myTransform[1][3],
			myTransform[2][0], myTransform[2][1], myTransform[2][2], myTransform[2][3],
			myTransform[3][0], myTransform[3][1], myTransform[3][2], myTransform[3][3]);
		pri(ws.GetWCharCP());
#endif // MY_MATRIX
		
#ifdef MY_DEBUG
		WString ws;
		ws.Sprintf(L"scale(%.7f)scale radians(%.7f)radians translation(%.7f,%.7f,%.7f)translation",
			scale, radians, translation.x, translation.y, translation.z);
		pri(ws.GetWCharCP());
#endif // MY_DEBUG
	}



	sqlliteRW sqlW(dbName);
	sqlW.creatObjTable();
	for (PersistentElementRefP elemRef : elemColl)
	{		
		if (elemRef->IsGraphics())// &&
			//elemRef->GetDgnModelP()->AsDgnAttachmentP()->GetLevelCache().GetLevel(elemRef->GetLevel(), false)->GetDisplay())
		{
			EditElementHandle sourceEh(elemRef);
			WString name = sourceEh.GetDgnModelP()->GetModelName();

			creatReference(sourceEh,name,sqlW,dbName, myTransform);
		}
		else
		{
			//WString ws;
			//ws.Sprintf(L"",elemRef->GetElementId());
			//std::string s = "模型不可提取:ID = " + ws2sT(ws);
			//pri(s2wsT(s).GetWCharCP());
		}
	}
}

bool getData::creatReference(EditElementHandleR sourceEh, WString modelNamel, sqlliteRW& sqlW,const WString dbName, double myTransform[4][4])
{
	bool rtn = true;

	//pri(L"CreateElement");
	IFacetOptionsPtr facetOptions = IFacetOptions::New();
	//Set different parameters for facet.
	facetOptions->SetIgnoreFaceMaterialAttachments(true); // Don't separate multi-symbology BReps by face symbology...
	facetOptions->SetChordTolerance(10);                 //many different parameters to control the final result mesh
	facetOptions->SetAngleTolerance(0.0);
	facetOptions->SetMaxEdgeLength(0.0);
	facetOptions->SetMaxFacetWidth(0.0);
	facetOptions->SetNormalsRequired(true);
	facetOptions->SetParamsRequired(true);
	facetOptions->SetVertexColorsRequired(true);
	facetOptions->SetMaxPerFace(3);
	facetOptions->SetCurvedSurfaceMaxPerFace(3);
	facetOptions->SetEdgeHiding(true);
	facetOptions->SetSmoothTriangleFlowRequired(true);

	unsigned int id = (unsigned int)(sourceEh.GetElementId());
	bvector<PolyfaceHeaderPtr> meshes;

	//sourceEh.GetDgnModelP()->AsDgnAttachmentP()->get
	if (true == ElementToApproximateFacets(sourceEh, meshes, facetOptions.get()))
	{
		EditElementHandle tmpEeh;
		size_t n = meshes.size();

		for (size_t i = 0; i < n; i++)
		{
			sqlW.set_id();
			sqlW.set_hostfile_name(dbName);
			sqlW.set_id_infile((unsigned int)id);
			WString str = get(meshes[i], myTransform);
			sqlW.addData(str);
		}
	}
	else
		rtn = false;

	return rtn;
}

WString getData::get(PolyfaceHeaderPtr meshData, double myTransform[4][4])
{
	std::string str = "o\n";
	char buf[256] = "\0";
	WString msg;

	size_t pointCount = meshData->GetPointCount();
	size_t PointIndexCount = meshData->GetPointIndexCount();
	size_t normalCount = meshData->GetNormalCount();


	//double scale = 1;//缩放
	//double radians = 0;//旋转
	//DPoint3d translation;
	//translation.Zero();


	//ofstream out(this->modelName + "-" + this->referModelName + ".txt", ios::app);
	//点
	DPoint3dCP point = meshData->GetPointCP();
	for (size_t i = 0; i < pointCount; i++)
	{
		//double x = scale * point->x / 10000;
		//double y = scale * point->y / 10000;
		//double z = scale * point->z / 10000;
		//
		//double x1 = cos(radians)*x - sin(radians)*y;
		//double y1 = sin(radians)*x + cos(radians)*y;
		//double z1 = z;

		//x1 += translation.x;
		//y1 += translation.y;
		//z1 += translation.z;

		//sprintf(buf, "v %.7f %.7f %.7f\n", x1,y1,z1);
		//str += buf;
		//memset(buf, '\0', 256);

		//WString ws;
		//ws.Sprintf(L"scale:%.7f radians:%.7f   translation(%.7f,%.7f,%.7f) \n原模型中点(%.7f,%.7f,%.7f)  变换后点(%.7f,%.7f,%.7f)",
		//	scale, radians, translation.x, translation.y, translation.x,
		//	point->x / 10000, point->y / 10000, point->z / 10000, x1, y1, z1);
		//pri(ws.GetWCharCP());

		double myPoint[4] = { point->x / 10000,point->y / 10000,point->z / 10000,1 };

		double myOut[3] = { 0 };


		for (int n = 0; n < 4; n++)
		{
			myOut[n] = myTransform[n][0] * myPoint[0] + myTransform[n][1] * myPoint[1] + myTransform[n][2] * myPoint[2] + myTransform[n][3] * myPoint[3];

			//if (out.is_open())
			//{
			//	out << myTransform[n][0] << "," << myTransform[n][1] << "," << myTransform[n][2] << "," << myTransform[n][3] << endl;
			//}
		}
		//if (out.is_open())
		//{
		//	out << endl;
		//}
		sprintf(buf, "v %.7f %.7f %.7f\n", myOut[0], myOut[1], myOut[2]);
		str += buf;
		memset(buf, '\0', 256);
		
		point++;
	}

	//法线
	DVec3dCP normal = meshData->GetNormalCP();
	for (size_t i = 0; i < normalCount; i++)
	{
		//double x = normal->x;
		//double y = normal->y;
		//double z = normal->z;

		//double x1 = cos(radians)*x - sin(radians)*y;
		//double y1 = sin(radians)*x + cos(radians)*y;
		//
		//sprintf(buf, "vn %.7f %.7f %.7f\n", x1,y1,z);
		//str += buf;
		//memset(buf, '\0', 256);

		double myPoint[4] = { 0 };
		myPoint[0] = normal->x;
		myPoint[1] = normal->y;
		myPoint[2] = normal->z;
		myPoint[3] = 1;
		double myOut[3] = { 0 };

		for (int n = 0; n < 4; n++)
		{
			myOut[n] = myTransform[n][0] * myPoint[0] + myTransform[n][1] * myPoint[1] + myTransform[n][2] * myPoint[2] + myTransform[n][3] * myPoint[3];
		}

		double divide = sqrt(myOut[0]* myOut[0]+ myOut[1]* myOut[1]+ myOut[2]* myOut[2]);
		//if (out.is_open())
		//{
		//	out << "divide: " << divide << endl;
		//	out.close();
		//}

		sprintf(buf, "vn %.7f %.7f %.7f\n", myOut[0]/ divide, myOut[1]/ divide, myOut[2]/ divide);
		str += buf;
		memset(buf, '\0', 256);
		
		normal++;
	}

	int32_t const* normalIndex = meshData->GetNormalIndexCP(false);

	//面		
	const int32_t* pi = meshData->GetPointIndexCP();
	str += "f ";
	for (size_t i = 0; i < PointIndexCount; i++)
	{
		if (normalIndex != nullptr)
			sprintf(buf, "%d//%d ", (int)(abs(*pi)), *normalIndex);
		else
			sprintf(buf, "%d ", (int)(abs(*pi)));
		if (*pi != 0)
		{
			str += buf;
			memset(buf, '\0', 256);
		}
		else
		{
			if (i < PointIndexCount - 1)
				str += "\nf ";
		}
		pi++;
		if (normalIndex != nullptr)
			normalIndex++;
	}
	
	return WString(str.c_str());
}

bool getData::ElementToApproximateFacets(ElementHandleCR source, bvector<PolyfaceHeaderPtr>& output, IFacetOptionsP options)
{
	output.clear();
	MeshProcessor dest(output, options);

	ElementGraphicsOutput::Process(source, dest);
	return output.size() > 0 ? true : false;
}



