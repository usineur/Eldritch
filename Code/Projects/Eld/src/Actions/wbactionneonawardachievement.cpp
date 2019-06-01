#include "core.h"
#include "wbactionneonawardachievement.h"
#include "configmanager.h"
#include "eldframework.h"
#include "Achievements/iachievementmanager.h"

WBActionNeonAwardAchievement::WBActionNeonAwardAchievement()
:	m_AchievementTag()
{
}

WBActionNeonAwardAchievement::~WBActionNeonAwardAchievement()
{
}

/*virtual*/ void WBActionNeonAwardAchievement::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( AchievementTag );
	m_AchievementTag = ConfigManager::GetString( sAchievementTag, "", sDefinitionName );
}

/*virtual*/ void WBActionNeonAwardAchievement::Execute()
{
	WBAction::Execute();

	// NEONTODO: Do some validation so users can't easily make mods to award achievements.
	AWARD_ACHIEVEMENT( m_AchievementTag );
}
