#include "core.h"
#include "wbactioneldopenuserdatapath.h"
#include "eldframework.h"
#include "eldgame.h"
#include "wbeventmanager.h"

WBActionEldOpenUserDataPath::WBActionEldOpenUserDataPath()
{
}

WBActionEldOpenUserDataPath::~WBActionEldOpenUserDataPath()
{
}

/*virtual*/ void WBActionEldOpenUserDataPath::Execute()
{
	WBAction::Execute();

	EldGame* const		pGame				= EldFramework::GetInstance()->GetGame();
	ASSERT( pGame );

	WBEventManager* const	pEventManager	= GetEventManager();
	ASSERT( pEventManager );

	WB_MAKE_EVENT( OpenUserDataPath, NULL );
	WB_LOG_EVENT( OpenUserDataPath );
	WB_DISPATCH_EVENT( pEventManager, OpenUserDataPath, pGame );
}
