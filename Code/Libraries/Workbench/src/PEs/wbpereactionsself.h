#ifndef WBPEREACTIONSSELF_H
#define WBPEREACTIONSSELF_H

#include "wbpe.h"

class WBPEReactionsSelf : public WBPE
{
public:
	WBPEReactionsSelf();
	virtual ~WBPEReactionsSelf();

	DEFINE_WBPE_FACTORY( ReactionsSelf );

	virtual void	Evaluate( const WBParamEvaluator::SPEContext& Context, WBParamEvaluator::SEvaluatedParam& EvaluatedParam ) const;
};

#endif // WBPEREACTIONSSELF_H
