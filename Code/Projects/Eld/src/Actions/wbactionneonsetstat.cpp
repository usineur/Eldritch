#include "core.h"
#include "wbactionneonsetstat.h"
#include "configmanager.h"
#include "eldframework.h"
#include "Achievements/iachievementmanager.h"

WBActionNeonSetStat::WBActionNeonSetStat()
:	m_StatTag()
,	m_Value( 0 )
,	m_ValuePE()
{
}

WBActionNeonSetStat::~WBActionNeonSetStat()
{
}

/*virtual*/ void WBActionNeonSetStat::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( StatTag );
	m_StatTag = ConfigManager::GetString( sStatTag, "", sDefinitionName );

	STATICHASH( Value );
	m_Value = ConfigManager::GetInt( sValue, 0, sDefinitionName );

	STATICHASH( ValuePE );
	const SimpleString ValuePEDef = ConfigManager::GetString( sValuePE, "", sDefinitionName );
	m_ValuePE.InitializeFromDefinition( ValuePEDef );
}

// NEONTODO: Do some validation so users can't easily make mods to set stats.
/*virtual*/ void WBActionNeonSetStat::Execute()
{
	WBAction::Execute();

	WBParamEvaluator::SPEContext PEContext;
	PEContext.m_Entity = GetEntity();

	m_ValuePE.Evaluate( PEContext );
	const uint Value = m_ValuePE.HasRoot() ? m_ValuePE.GetInt() : m_Value;

	Unused( Value );
	SET_STAT( m_StatTag, Value );
}
