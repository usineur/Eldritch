#include "core.h"
#include "wbactioneldlaunchuserid.h"
#include "configmanager.h"
#include "eldframework.h"
#include "inputsystem.h"
#include "Common/uimanagercommon.h"
#include "uistack.h"
#include "Screens/uiscreen-eldleaderboards.h"

WBActionEldLaunchUserID::WBActionEldLaunchUserID()
:	m_Row( 0 )
{
}

WBActionEldLaunchUserID::~WBActionEldLaunchUserID()
{
}

/*virtual*/ void WBActionEldLaunchUserID::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( Row );
	m_Row = ConfigManager::GetInt( sRow, 0, sDefinitionName );
}

/*virtual*/ void WBActionEldLaunchUserID::Execute()
{
	WBAction::Execute();

	EldFramework* const		pFramework			= EldFramework::GetInstance();
	DEVASSERT( pFramework );

	UIManager* const				pUIManager			= pFramework->GetUIManager();
	DEVASSERT( pUIManager );

	STATIC_HASHED_STRING( LeaderboardsScreen );
	UIScreenEldLeaderboards* const	pLeaderboardsScreen	= pUIManager->GetScreen<UIScreenEldLeaderboards>( sLeaderboardsScreen );
	DEVASSERT( pLeaderboardsScreen );

	pLeaderboardsScreen->LaunchUserID( m_Row );
}
