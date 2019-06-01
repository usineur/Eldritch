#include "core.h"
#include "wbactioneldpostscore.h"
#include "configmanager.h"
#include "eldframework.h"
#include "Leaderboards/ileaderboards.h"

WBActionEldPostScore::WBActionEldPostScore()
:	m_LeaderboardTag()
,	m_ScorePE()
{
}

WBActionEldPostScore::~WBActionEldPostScore()
{
}

/*virtual*/ void WBActionEldPostScore::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( LeaderboardTag );
	m_LeaderboardTag = ConfigManager::GetString( sLeaderboardTag, "", sDefinitionName );

	STATICHASH( ScorePE );
	const SimpleString ScorePEDef = ConfigManager::GetString( sScorePE, "", sDefinitionName );
	m_ScorePE.InitializeFromDefinition( ScorePEDef );
}

/*virtual*/ void WBActionEldPostScore::Execute()
{
	WBAction::Execute();

	WBParamEvaluator::SPEContext PEContext;
	PEContext.m_Entity = GetEntity();

	m_ScorePE.Evaluate( PEContext );
	DEVASSERT( m_ScorePE.HasRoot() );
	const c_int32 Score = m_ScorePE.GetInt();

	Unused( Score );
	POST_SCORE( m_LeaderboardTag, Score );
}
