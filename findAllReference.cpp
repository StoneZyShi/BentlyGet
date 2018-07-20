#include "findAllReference.h"
#include "getData.h"
#include "utility.h"
#include "fileControl.h"

using namespace std;
 
findAllReference::findAllReference(std::string rootFile, std::string rootModel)
	:rootFile(rootFile),rootModel(rootModel)
{
}

findAllReference::~findAllReference()
{
}

void findAllReference::ergodicReference(const std::string& txt, const std::string& txtTem, int& line,int& temLine, double myTransform[4][4])
{
#ifdef MY_MATRIX
	WString ws1;
	ws1.Sprintf(L"输入矩阵参数 file:%ls  model:%ls   \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f\n",
		s2wsT(this->rootFile).GetWCharCP(), s2wsT(this->rootModel).GetWCharCP(),
		myTransform[0][0], myTransform[0][1], myTransform[0][2], myTransform[0][3],
		myTransform[1][0], myTransform[1][1], myTransform[1][2], myTransform[1][3],
		myTransform[2][0], myTransform[2][1], myTransform[2][2], myTransform[2][3],
		myTransform[3][0], myTransform[3][1], myTransform[3][2], myTransform[3][3]);
	pri(ws1.GetWCharCP());
#endif // MY_MATRIX
	
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	DgnModel::ElementsCollection elemColl = pActiveModel->GetElementsCollection();

	for (PersistentElementRefP const& elemRef : elemColl)//循环一次即可，每次循环里面得到内容相同
	{
		ModelRefIteratorP   iterator;
		DgnModelRefP       modelRef;
		mdlModelRefIterator_create(&iterator, elemRef->GetDgnModelP(), MRITERATE_PrimaryChildRefs, 0);

		ofstream out(txt, ios::app);
		ofstream outTem(txtTem, ios::app);
		if (out.is_open() && outTem.is_open())
		{
			while (nullptr != (modelRef = mdlModelRefIterator_getNext(iterator)))
			{
				if (nullptr != modelRef->GetDgnFileP())
				{
					WString fileName = modelRef->GetDgnFileP()->GetFileName();
					WString dgn = L"dgn";
					if (fileName.substr(fileName.size() - 3, 3) == dgn)
					{
						WString modelName = modelRef->GetDgnModelP()->GetModelName();
						char str[256] = "\0";
						sprintf(str, "%ls<->%ls", fileName.GetWCharCP(), modelName.GetWCharCP());
						
						if (modelRef->AsDgnAttachmentP()->IsDisplayed())
						{
							//层测试程序
							//for (LevelId i = 0; i < modelRef->GetLevelCache().GetLevelCount(); i++)
							//{
							//	LevelHandle handle = modelRef->GetLevelCache().GetLevel(i, false);
							//	WString wstr;
							//	wstr.Sprintf(L"levelCount: %d,handleName:%ls", i, handle->GetName());
							//	pri(wstr.GetWCharCP());
							//	if (handle->GetDisplay())
							//	{
							//		wstr.Sprintf(L"%ls", handle->GetName());
							//		pri(wstr.GetWCharCP());
							//	}
							//}
							Transform form;
							bool scaleZfor2dRef = true;
							modelRef->AsDgnAttachmentP()->GetTransformToParent(form, scaleZfor2dRef);
							DPoint3d translation;
							form.GetTranslation(translation);
							
							DPoint3d       fixedPoint;
							DVec3d       directionVector;
							double          radians;//旋转
							double          scale;//缩放
							form.IsUniformScaleAndRotateAroundLine(fixedPoint, directionVector, radians, scale);
							
							double newTransform[4][4] = { 1,0,0,0,
															0,1,0,0,
															0,0,1,0,
															0,0,0,1 };
							double matrix_result[4][4] = {0};
							//缩放
							for (int i = 0; i < 3; i++)
								newTransform[i][i] = scale*newTransform[i][i];
							//旋转
							double rotateTransform[4][4] = { cos(radians),-sin(radians),0,0,
														sin(radians),cos(radians),0,0,
														0,0,1,0,
														0,0,0,1 };

							double rotateNewTransform[4][4] = {0};
							for (int m = 0; m < 4; m++) {
								for (int s = 0; s < 4; s++) {
									rotateNewTransform[m][s] = 0;//变量使用前记得初始化,否则结果具有不确定性
									for (int n = 0; n < 4; n++) {
										rotateNewTransform[m][s] += newTransform[m][n] * rotateTransform[n][s];
									}
								}
							}
							for (int m = 0; m < 4; m++) {
								for (int s = 0; s < 4; s++) {
									newTransform[m][s] = rotateNewTransform[m][s];
								}
							}
							//平移
							newTransform[0][3] += translation.x / 10000;
							newTransform[1][3] += translation.y / 10000;
							newTransform[2][3] += translation.z / 10000;

#ifdef MY_MATRIX
							ws1.Sprintf(L"变换矩阵参数   \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f\n",
								newTransform[0][0], newTransform[0][1], newTransform[0][2], newTransform[0][3],
								newTransform[1][0], newTransform[1][1], newTransform[1][2], newTransform[1][3],
								newTransform[2][0], newTransform[2][1], newTransform[2][2], newTransform[2][3],
								newTransform[3][0], newTransform[3][1], newTransform[3][2], newTransform[3][3]);
							pri(ws1.GetWCharCP());
#endif // MY_MATRIX

							for (int m = 0; m < 4; m++) {
								for (int s = 0; s < 4; s++) {
									matrix_result[m][s] = 0;//变量使用前记得初始化,否则结果具有不确定性
									for (int n = 0; n < 4; n++) {
										matrix_result[m][s] += myTransform[m][n] * newTransform[n][s];
									}
								}
							}


#ifdef MY_MATRIX
							ws1.Sprintf(L"生成矩阵参数   \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f\n",
								matrix_result[0][0], matrix_result[0][1], matrix_result[0][2], matrix_result[0][3],
								matrix_result[1][0], matrix_result[1][1], matrix_result[1][2], matrix_result[1][3],
								matrix_result[2][0], matrix_result[2][1], matrix_result[2][2], matrix_result[2][3],
								matrix_result[3][0], matrix_result[3][1], matrix_result[3][2], matrix_result[3][3]);
							pri(ws1.GetWCharCP());
#endif // MY_MATRIX

							//WString ws;
							//ws.Sprintf(L">>>scale(%.7f)scale radians(%.7f)radians translation(%.7f,%.7f,%.7f)translation",
							//	scale, radians, translation.x / 10000, translation.y / 10000, translation.z / 10000);

							WString ws;
							ws.Sprintf(L">>>matrix_result(,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,%.7f,)matrix_result",
								matrix_result[0][0], matrix_result[0][1], matrix_result[0][2], matrix_result[0][3],
								matrix_result[1][0], matrix_result[1][1], matrix_result[1][2], matrix_result[1][3],
								matrix_result[2][0], matrix_result[2][1], matrix_result[2][2], matrix_result[2][3],
								matrix_result[3][0], matrix_result[3][1], matrix_result[3][2], matrix_result[3][3]);

							std::string logicalName = "<<<" + ws2sT(modelRef->AsDgnAttachmentP()->GetLogicalName());

							std::string my_str = str + ws2sT(ws) + logicalName;
							out << my_str.c_str() << endl;
							outTem << my_str.c_str() << endl;
							pri(s2wsT(my_str).GetWCharCP());
#ifdef MY_DEBUG
							pri(s2wsT(my_str).GetWCharCP());
							
							ws.Sprintf(L"fixedPoint(%.7f,%.7f,%.7f)directionVector(%.7f,%.7f,%.7f)",
								fixedPoint.x / 10000, fixedPoint.y / 10000, fixedPoint.z / 10000,
								directionVector.x, directionVector.y, directionVector.z);
							mdlDialog_dmsgsPrint(ws.GetWCharCP());
#endif // MY_DEBUG

							//out << str << endl;
							//outTem << str << endl;
							line++;
							temLine++;

						}

					}
				}
			}
			out.close();
			outTem.close();
		}

		mdlModelRefIterator_free(&iterator);

		while (temLine > 0)
		{
			string str = deleteLine(txtTem);
			while (str == "" || str == "\n")
			{
				str = deleteLine(txtTem);
			}
			temLine--;

//#ifdef MY_DEBUG
//			pri(s2wsT(str).GetWCharCP());
//#endif // MY_DEBUG

			string fileName = str.substr(0, str.find("<->")).c_str();
			string modelName = str.substr(str.find("<->") + 3, str.find(">>>")- str.find("<->")-3);
			string openTransform = str.substr(str.find(">>>matrix_result(,")+18, str.find(")matrix_result")- str.find(">>>matrix_result(,") - 18);

			double newTransform[4][4] = { 1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1 };
			if (openTransform != "")
			{
				std::string transformValue;
				openTransform = openTransform.substr(0, openTransform.length() - openTransform.find(",") - 1);
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						transformValue = openTransform.substr(0, openTransform.find(","));
						newTransform[i][j] = atof(transformValue.c_str());
						openTransform = openTransform.substr(openTransform.find(",") + 1, openTransform.length() - openTransform.find(",") - 1);
					}
				}
			}
			openModel(txt, txtTem, line,temLine, fileName, modelName, newTransform);

			//ws1.Sprintf(L"读出矩阵   \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f \n%.7f,%.7f,%.7f,%.7f\n",
			//	newTransform[0][0], newTransform[0][1], newTransform[0][2], newTransform[0][3],
			//	newTransform[1][0], newTransform[1][1], newTransform[1][2], newTransform[1][3],
			//	newTransform[2][0], newTransform[2][1], newTransform[2][2], newTransform[2][3],
			//	newTransform[3][0], newTransform[3][1], newTransform[3][2], newTransform[3][3]);
			//pri(ws1.GetWCharCP());
		}
		
		break;
	}
}

void findAllReference::openModel(const std::string& txt, const std::string& txtTem, int& line, int& temLine,const std::string & file, const std::string & model, double myTransform[4][4])
{
	WString wfile(file.c_str());
	WString wmode(model.c_str());

	mdlSystem_setBatchProcessingState(PROCESSSTATE_Processing, NULL);
	if (SUCCESS == mdlSystem_newDesignFileAndModel(wfile.GetWCharCP(), wmode.GetWCharCP()))
	{
		Sleep(500);

		findAllReference references(this->rootFile, this->rootModel);
		references.ergodicReference(txt, txtTem, line,temLine, myTransform);
	}
}


