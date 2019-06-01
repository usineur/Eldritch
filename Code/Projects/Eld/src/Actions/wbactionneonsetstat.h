#ifndef WBACTIONNEONSETSTAT_H
#define WBACTIONNEONSETSTAT_H

#include "wbaction.h"
#include "simplestring.h"
#include "wbparamevaluator.h"

class WBActionNeonSetStat : public WBAction
{
public:
	WBActionNeonSetStat();
	virtual ~WBActionNeonSetStat();

	DEFINE_WBACTION_FACTORY( NeonSetStat );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	Execute();

private:
	SimpleString		m_StatTag;
	uint				m_Value;
	WBParamEvaluator	m_ValuePE;
};

#endif // WBACTIONNEONSETSTAT_H
