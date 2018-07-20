/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/Polyfacetool/PolyfaceCreateTool.cpp $
|
|  $Copyright: (c) 2017 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "polyfacetool.h"
#include "..\include\dgnplatform\MeshHeaderHandler.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to create a mesh with only one triangle facet.
*
* Demonstrates using the RedrawElems class for element dynamics.
* 
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          PolyfaceCreateTool : DgnPrimitiveTool
{
protected:

bvector<DPoint3d>   m_points;

PolyfaceCreateTool(int toolName, int toolPrompt) : DgnPrimitiveTool(toolName, toolPrompt) {}

virtual void _OnPostInstall () override;
virtual void _OnRestartTool () override {InstallNewInstance (GetToolId (), GetToolPrompt ());}
virtual bool _OnDataButton (DgnButtonEventCR ev) override;
virtual bool _OnResetButton (DgnButtonEventCR ev) override {_OnRestartTool (); return true;}
virtual void _OnDynamicFrame (DgnButtonEventCR ev) override;

bool CreateElement (EditElementHandleR eeh, bvector<DPoint3d> const& points);
bool CreateDynamicElements(ElementAgendaR agenda, bvector<DPoint3d> const& points);
void SetupAndPromptForNextAction ();

public:

static void InstallNewInstance (int toolId, int toolPrompt);

}; // PolyfaceCreateTool

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool PolyfaceCreateTool::CreateDynamicElements(ElementAgendaR agenda, bvector<DPoint3d> const& points)
{
    if (2 > points.size())
        return false;

    // NOTE: Easier to always work with CurveVector/CurvePrimitive and not worry about element specific create methods, ex. LineHandler::CreateLineElement.
    EditElementHandle eeh;
    DPoint3d pt0, pt1;
    pt0 = points[0];
    for (size_t i = 1; i < points.size(); i++)
        {
        pt1 = points[i];
        if (SUCCESS == DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLine(DSegment3d::From(pt0, pt1)), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
            {
            ElementPropertyUtils::ApplyActiveSettings(eeh);
            agenda.Insert(eeh);
            }
        pt0 = pt1;
        }
    pt0 = points[0];
    pt1 = points[points.size()-1];
    if (SUCCESS == DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLine(DSegment3d::From(pt0, pt1)), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
        {
        ElementPropertyUtils::ApplyActiveSettings(eeh);
        agenda.Insert(eeh);
        }
    return true;
}
/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool PolyfaceCreateTool::CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points)
    {
    if (3 != points.size ())
        return false;

    PolyfaceHeaderPtr polyface = PolyfaceHeader::CreateVariableSizeIndexed();

    //Add a polygon directly to the arrays
    polyface->AddPolygon(points);
    
    if (SUCCESS != MeshHeaderHandler::CreateMeshElement(eeh, NULL, *polyface, true, *ACTIVEMODEL))
        return false;

    ElementPropertyUtils::ApplyActiveSettings (eeh);

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void PolyfaceCreateTool::SetupAndPromptForNextAction()
    {
    WString     msgStr;
    UInt32      msgId;
    size_t size = m_points.size();
    if (size == 0)
        msgId = PROMPT_FirstPoint;
    else if (size == 1)
        msgId = PROMPT_NextPoint;
    else
        msgId = PROMPT_NextPointOrReset;

    RmgrResource::LoadWString(msgStr, (RscFileHandle)0, STRINGLISTID_Prompts, msgId, GetMdlDesc());
    NotificationManager::OutputPrompt (msgStr.c_str ());

    if (3 != m_points.size ())
        return;

    DVec3d      xVec = DVec3d::FromStartEndNormalize (m_points.front (), m_points.back ());

    AccuDraw::GetInstance ().SetContext (ACCUDRAW_SetXAxis, NULL, &xVec); // Orient AccuDraw to last segment.
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void PolyfaceCreateTool::_OnDynamicFrame(DgnButtonEventCR ev)
    {
    bvector<DPoint3d>   tmpPts = m_points;
    ElementAgenda   agenda;

    tmpPts.push_back (*ev.GetPoint ()); // Use current button location as end point.

    if (!CreateDynamicElements(agenda, tmpPts))
        return;

    RedrawElems redrawElems;

    redrawElems.SetDynamicsViews (IViewManager::GetActiveViewSet (), ev.GetViewport ()); // Display in all views, draws to cursor view first...
    redrawElems.SetDrawMode (DRAW_MODE_TempDraw);
    redrawElems.SetDrawPurpose (DrawPurpose::Dynamics);

    for (size_t i = 0; i < agenda.GetCount(); i++)
        {
        redrawElems.DoRedraw(agenda[i]);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool PolyfaceCreateTool::_OnDataButton(DgnButtonEventCR ev)
    {
    if (0 == m_points.size ())
        _BeginDynamics (); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.

    m_points.push_back (*ev.GetPoint ()); // Save current data point location.
    SetupAndPromptForNextAction ();

    if (m_points.size () < 3)
        return false;

    EditElementHandle   eeh;

    if (CreateElement (eeh, m_points))
        eeh.AddToModel (); // Add a new polyface (mesh).

    m_points.clear (); m_points.push_back (*ev.GetPoint ()); // Store the points

    return false;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void PolyfaceCreateTool::_OnPostInstall()
    {
    AccuSnap::GetInstance ().EnableSnap (true); // Enable snapping for create tools.

    __super::_OnPostInstall ();
    }

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void PolyfaceCreateTool::InstallNewInstance(int toolId, int toolPrompt)
    {
    PolyfaceCreateTool* tool = new PolyfaceCreateTool(toolId, toolPrompt);

    tool->InstallTool ();
    }

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startPolyfaceCreateTool(WCharCP unparsed)
    {
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    //PolyfaceCreateTool::InstallNewInstance(CMDNAME_PolyfaceCreateTool, PROMPT_FirstPoint);
    }



