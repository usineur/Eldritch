#include "core.h"
#include "rodinbtnodeconditiontimeout.h"
#include "configmanager.h"

RodinBTNodeConditionTimeout::RodinBTNodeConditionTimeout()
:	m_TimeoutPE()
,	m_NextCanExecuteTime( 0.0f )
{
}

RodinBTNodeConditionTimeout::~RodinBTNodeConditionTimeout()
{
}

void RodinBTNodeConditionTimeout::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	MAKEHASH( DefinitionName );

	STATICHASH( TimeoutPE );
	m_TimeoutPE.InitializeFromDefinition( ConfigManager::GetString( sTimeoutPE, "", sDefinitionName ) );
}

/*virtual*/ RodinBTNode::ETickStatus RodinBTNodeConditionTimeout::Tick( const float DeltaTime )
{
	Unused( DeltaTime );

	const float Time = GetTime();

	if( Time < m_NextCanExecuteTime )
	{
		return ETS_Fail;
	}

	WBParamEvaluator::SPEContext Context;
	Context.m_Entity = GetEntity();
	m_TimeoutPE.Evaluate( Context );
	m_NextCanExecuteTime = Time + m_TimeoutPE.GetFloat();

	return ETS_Success;
}