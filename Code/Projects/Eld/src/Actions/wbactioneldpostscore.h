#ifndef WBACTIONELDPOSTSCORE_H
#define WBACTIONELDPOSTSCORE_H

#include "wbaction.h"
#include "simplestring.h"
#include "wbparamevaluator.h"

class WBActionEldPostScore : public WBAction
{
public:
	WBActionEldPostScore();
	virtual ~WBActionEldPostScore();

	DEFINE_WBACTION_FACTORY( EldPostScore );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	Execute();

private:
	SimpleString		m_LeaderboardTag;
	WBParamEvaluator	m_ScorePE;
};

#endif // WBACTIONELDPOSTSCORE_H
