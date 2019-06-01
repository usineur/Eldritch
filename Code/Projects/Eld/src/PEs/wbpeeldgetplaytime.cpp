#include "core.h"
#include "wbpeeldgetplaytime.h"
#include "eldgame.h"
#include "configmanager.h"

WBPEEldGetPlayTime::WBPEEldGetPlayTime()
:	m_Minutes( false )
,	m_Seconds( false )
{
}

WBPEEldGetPlayTime::~WBPEEldGetPlayTime()
{
}

/*virtual*/ void WBPEEldGetPlayTime::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	MAKEHASH( DefinitionName );

	STATICHASH( Minutes );
	m_Minutes = ConfigManager::GetBool( sMinutes, false, sDefinitionName );

	STATICHASH( Seconds );
	m_Seconds = ConfigManager::GetBool( sSeconds, false, sDefinitionName );
}

/*virtual*/ void WBPEEldGetPlayTime::Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const
{
	Unused( Context );

	const float	fPlayTime	= EldGame::GetPlayTime();
	const int	PlayTime	= static_cast<int>( fPlayTime );
	const int	PlayTimeM	= PlayTime / 60;
	const int	PlayTimeS	= PlayTime % 60;

	if( m_Minutes )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Int;
		EvaluatedParam.m_Int	= PlayTimeM;
	}
	else if( m_Seconds )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Int;
		EvaluatedParam.m_Int	= PlayTimeS;
	}
	else
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Float;
		EvaluatedParam.m_Float	= fPlayTime;
	}
}
