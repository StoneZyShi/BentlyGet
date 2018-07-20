#pragma once
#include "utility.h"
#include "sqlliteRW.h"

#define ¦Ð 3.1415926535897932384626433832795
#define my_pi 57.295779513082323

class getData
{
public:
	getData(const std::string path, const std::string& fileName, const std::string& modelName, const std::string& referFileName, const std::string& referModelName, const std::string& transform,const std::string& logicalName);
	~getData();

private:
	std::string getFileNameWithoutPath();
	std::string getModelNameWithoutPath();
	std::string getReferFileNameWithoutPath();
	std::string getReferModelNameWithoutPath();
	std::string changeToDbFileName();	
public:
	void gotoNewModel();

private:
	std::string path;
	std::string fileName;
	std::string modelName;
	std::string referFileName;
	std::string referModelName;
	std::string transform;
	std::string logicalName;

public:
	void findAllActive();
private:
	bool creatReference(EditElementHandleR sourceEh, WString modelNamel, sqlliteRW& sqlW, const WString dbName, double myTransform[4][4]);
	WString get(PolyfaceHeaderPtr meshData,double myTransform[4][4]);
	bool ElementToApproximateFacets(ElementHandleCR source, bvector<PolyfaceHeaderPtr> &output, IFacetOptionsP options);
};


struct MeshProcessor : public IElementGraphicsProcessor
{
	bvector <PolyfaceHeaderPtr> &m_output;
	IFacetOptionsP               m_options;
	Transform                    m_currentTransform;

	MeshProcessor(bvector<PolyfaceHeaderPtr> &output, IFacetOptionsP options)
		: m_output(output), m_options(options)
	{
	}

	virtual IFacetOptionsP _GetFacetOptionsP()
	{
		return m_options;
	}

	virtual bool _ProcessAsFacets(bool isPolyface) const override
	{
		return true;
	}

	virtual bool _ProcessAsBody(bool isCurved) const
	{
		return false;
	}

	virtual void _AnnounceTransform(TransformCP trans) override
	{
		if (trans)
			m_currentTransform = *trans;
		else
			m_currentTransform.InitIdentity();
	}


	virtual BentleyStatus _ProcessFacets(PolyfaceQueryCR facets, bool isFilled = false) override
	{
		PolyfaceHeaderPtr header = PolyfaceHeader::CreateTriangleGrid(3);
		header->CopyFrom(facets);
		header->Transform(m_currentTransform);
		m_output.push_back(header);
		return SUCCESS;
	}
};

