/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/Polyfacetool/transkit/polyfacetoolmsg.r $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include "..\polyfacetoolids.h"

/*----------------------------------------------------------------------+
|    Command Names                                                      |
+----------------------------------------------------------------------*/
MessageList STRINGLISTID_Commands =
{
    {
    { CMDNAME_PolyfaceCreateTool,       "Create a polyface by 3 points" },
    { CMDNAME_ConvertToPolyfaceTool,    "Convert an element to a polyface" },
    }
};

MessageList STRINGLISTID_Prompts =
{
    {
    { PROMPT_SelectElement,         "Select an element" },
    { PROMPT_FirstPoint,            "Enter first point" },
    { PROMPT_NextPoint,             "Enter next point" },
    { PROMPT_NextPointOrReset,      "Enter next point or reset to complete" },
	{ PROMPT_AcceptOrReset,         "Accept" },
    }
};



