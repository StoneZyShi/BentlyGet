
#include "findAllReference.h"
#include "fileControl.h"
#include "utility.h"


#include "getData.h"
#include <string>

void materialInfo()
{
	WString myString;
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	DgnModel::ElementsCollection elemColl = pActiveModel->GetElementsCollection();

	MaterialTablePtr m_table = MaterialManager::GetManagerR().LoadActiveTable(*ISessionMgr::GetActiveDgnModelP());
	pri(m_table->GetName().GetWCharCP());
	MaterialPaletteList list = m_table->GetPaletteList();
	for (auto patette : list)
	{ 
		myString.Sprintf(L"%ls:%d", patette->GetName().GetWCharCP(), patette->GetRefCount());
		pri(myString.GetWCharCP());
	}
	
	for (PersistentElementRefP elemRef : elemColl)
	{
		EditElementHandle eeh(elemRef);
		ElementHandle eh(elemRef);
		
		if (elemRef->IsGraphics())
		{
			pri(L"\n step 1");
			IMaterialPropertiesExtension* mExtension = IMaterialPropertiesExtension::Cast(eh.GetHandler());
			DgnModelRefP modelRef = eh.GetModelRef();
			MaterialCP m = mExtension->FindMaterialAttachment(eh, *modelRef);
			if (m != NULL)
			{
				PaletteInfoCR p = m->GetPalette();
				myString.Sprintf(L"%ls:%d", p.GetName(), p.GetRefCount());
				pri(myString.GetWCharCP());
				
				MaterialId  tempId(*m);
				MaterialMapCollectionCR maps = m->GetSettings().GetMaps();
				MaterialMapCP pMap = maps.GetMapCP(MaterialMap::MAPTYPE_Pattern);
				if (pMap != NULL && pMap->GetType() == MaterialMap::MAPTYPE_Pattern)
				{
					MaterialMapLayerCR  layer = pMap->GetLayers().GetTopLayer();
					DPoint3d offset = layer.GetOffset();
					DPoint3d scale = layer.GetScale();

					myString.Sprintf(L"Face material=%ls, offset(%.4f,%.4f,%.4f) scale(%.4f,%.4f,%.4f)",
						tempId.GetName(), offset.x, offset.y, offset.z, scale.x, scale.y, scale.z);
					pri(myString.GetWCharCP());
				}
				else
				{
					pri(L"map 为空");
				}
			}
			else
			{
				pri(L"Material 为空");
			}

		}
	}
}

void getColor()
{
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	DgnModel::ElementsCollection elemColl = pActiveModel->GetElementsCollection();

	for (PersistentElementRefP elemRef : elemColl)
	{
		EditElementHandle eh(elemRef);
		if (elemRef->IsGraphics())
		{
		}
	}
}


Public void startPolyfaceConvertTool(WCharCP unparsed)
{
	fileControl file(ws2sT(getRootPath(ROOTFILE::rootFilePath)), ws2sT(getRootPath(ROOTFILE::rootModel)));
	file.start();

	//materialInfo();
	//getColor();
	

	pri(L"提取完成！！！");
}



