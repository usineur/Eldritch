#include "core.h"
#include "wbpereactionsself.h"
#include "wbevent.h"
#include "wbactionstack.h"

WBPEReactionsSelf::WBPEReactionsSelf()
{
}

WBPEReactionsSelf::~WBPEReactionsSelf()
{
}

/*virtual*/ void WBPEReactionsSelf::Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const
{
	Unused( Context );

	if( Context.m_Entity )
	{
		EvaluatedParam.m_Type	= WBParamEvaluator::EPT_Entity;
		EvaluatedParam.m_Entity	= WBActionStack::TopReactionsSelf();
	}
}