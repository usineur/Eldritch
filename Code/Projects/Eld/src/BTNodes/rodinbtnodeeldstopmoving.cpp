#include "core.h"
#include "rodinbtnodeeldstopmoving.h"
#include "Components/wbcompeldaimotion.h"

RodinBTNodeEldStopMoving::RodinBTNodeEldStopMoving()
{
}

RodinBTNodeEldStopMoving::~RodinBTNodeEldStopMoving()
{
}

RodinBTNode::ETickStatus RodinBTNodeEldStopMoving::Tick( const float DeltaTime )
{
	Unused( DeltaTime );

	WBEntity* const		pEntity		= GetEntity();
	WBCompEldAIMotion*	pAIMotion	= GET_WBCOMP( pEntity, EldAIMotion );
	ASSERT( pAIMotion );

	pAIMotion->StopMove();

	return ETS_Success;
}
