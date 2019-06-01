#include "core.h"
#include "wbactioneldlaunchwebsite.h"
#include "eldframework.h"
#include "eldgame.h"
#include "wbeventmanager.h"

WBActionEldLaunchWebSite::WBActionEldLaunchWebSite()
{
}

WBActionEldLaunchWebSite::~WBActionEldLaunchWebSite()
{
}

/*virtual*/ void WBActionEldLaunchWebSite::Execute()
{
	WBAction::Execute();

	EldGame* const		pGame			= EldFramework::GetInstance()->GetGame();
	ASSERT( pGame );

	WBEventManager* const	pEventManager	= WBWorld::GetInstance()->GetEventManager();
	ASSERT( pEventManager );

	WB_MAKE_EVENT( LaunchWebSite, NULL );
	WB_LOG_EVENT( LaunchWebSite );
	WB_DISPATCH_EVENT( pEventManager, LaunchWebSite, pGame );
}
