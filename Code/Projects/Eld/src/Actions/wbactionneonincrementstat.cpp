#include "core.h"
#include "wbactionneonincrementstat.h"
#include "configmanager.h"
#include "eldframework.h"
#include "Achievements/iachievementmanager.h"

WBActionNeonIncrementStat::WBActionNeonIncrementStat()
:	m_StatTag()
,	m_Amount( 0 )
,	m_AmountPE()
{
}

WBActionNeonIncrementStat::~WBActionNeonIncrementStat()
{
}

/*virtual*/ void WBActionNeonIncrementStat::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( StatTag );
	m_StatTag = ConfigManager::GetString( sStatTag, "", sDefinitionName );

	STATICHASH( Amount );
	m_Amount = ConfigManager::GetInt( sAmount, 1, sDefinitionName );

	STATICHASH( AmountPE );
	const SimpleString AmountPEDef = ConfigManager::GetString( sAmountPE, "", sDefinitionName );
	m_AmountPE.InitializeFromDefinition( AmountPEDef );
}

// NEONTODO: Do some validation so users can't easily make mods to increment stats.
/*virtual*/ void WBActionNeonIncrementStat::Execute()
{
	WBAction::Execute();

	WBParamEvaluator::SPEContext PEContext;
	PEContext.m_Entity = GetEntity();

	m_AmountPE.Evaluate( PEContext );
	const uint Amount = m_AmountPE.HasRoot() ? m_AmountPE.GetInt() : m_Amount;

	Unused( Amount );
	INCREMENT_STAT( m_StatTag, Amount );
}
