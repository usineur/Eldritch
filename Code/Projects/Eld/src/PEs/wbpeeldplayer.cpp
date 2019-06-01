#include "core.h"
#include "wbpeeldplayer.h"
#include "eldgame.h"

WBPEEldPlayer::WBPEEldPlayer()
{
}

WBPEEldPlayer::~WBPEEldPlayer()
{
}

/*virtual*/ void WBPEEldPlayer::Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const
{
	Unused( Context );

	// Warn if we're referencing the player entity before it has been spawned.
	DEVASSERT( EldGame::GetPlayer() );

	EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Entity;
	EvaluatedParam.m_Entity	= EldGame::GetPlayer();
}
