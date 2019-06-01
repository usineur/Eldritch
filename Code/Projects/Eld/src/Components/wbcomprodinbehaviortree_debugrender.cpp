#include "core.h"
#include "Components/wbcomprodinbehaviortree.h"
#include "eldframework.h"
#include "irenderer.h"
#include "view.h"
#include "display.h"
#include "wbcompeldtransform.h"
#include "wbentity.h"
#include "fontmanager.h"
#include "vector4.h"

#if BUILD_DEV
/*virtual*/ void WBCompRodinBehaviorTree::DebugRender() const
{
	EldFramework*				pFramework	= EldFramework::GetInstance();
	IRenderer* const			pRenderer	= pFramework->GetRenderer();
	View* const					pView		= pFramework->GetMainView();
	Display* const				pDisplay	= pFramework->GetDisplay();
	WBCompEldTransform* const	pTransform	= GetEntity()->GetTransformComponent<WBCompEldTransform>();
	const Vector				Location	= pTransform->GetLocation();

	if( m_ScheduledNodes.Size() > 0 )
	{
		const SimpleString		TreeString	= SimpleString::PrintF( "%sWBCompRodinBehaviorTree : %s", DebugRenderLineFeed().CStr(), GetEntity()->GetUniqueName().CStr() );
		pRenderer->DEBUGPrint( TreeString, Location, pView, pDisplay, DEFAULT_FONT_TAG, ARGB_TO_COLOR( 255, 192, 255, 128 ), ARGB_TO_COLOR( 255, 0, 0, 0 ) );
		FOR_EACH_ARRAY( NodeIter, m_ScheduledNodes, SScheduledNode )
		{
			const SScheduledNode&	Node		= NodeIter.GetValue();

			SimpleString NodeString = DebugRenderLineFeed();
			NodeString += "  -";
			FOR_EACH_INDEX( NodeDepth, Node.m_Node->m_Depth )
			{
				NodeString += "-";
			}
			NodeString += " ";
			NodeString += Node.m_Node->GetName();

			pRenderer->DEBUGPrint( NodeString, Location, pView, pDisplay, DEFAULT_FONT_TAG, ARGB_TO_COLOR( 255, 192, 255, 128 ), ARGB_TO_COLOR( 255, 0, 0, 0 ) );
		}
	}
}
#endif
