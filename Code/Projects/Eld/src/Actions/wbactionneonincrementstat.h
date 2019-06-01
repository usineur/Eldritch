#ifndef WBACTIONNEONINCREMENTSTAT_H
#define WBACTIONNEONINCREMENTSTAT_H

#include "wbaction.h"
#include "simplestring.h"
#include "wbparamevaluator.h"

class WBActionNeonIncrementStat : public WBAction
{
public:
	WBActionNeonIncrementStat();
	virtual ~WBActionNeonIncrementStat();

	DEFINE_WBACTION_FACTORY( NeonIncrementStat );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	Execute();

private:
	SimpleString		m_StatTag;
	uint				m_Amount;
	WBParamEvaluator	m_AmountPE;
};

#endif // WBACTIONNEONINCREMENTSTAT_H
