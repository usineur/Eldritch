#ifndef RODINBTNODECONDITIONTIMEOUT_H
#define RODINBTNODECONDITIONTIMEOUT_H

#include "rodinbtnode.h"
#include "wbparamevaluator.h"

class RodinBTNodeConditionTimeout : public RodinBTNode
{
public:
	RodinBTNodeConditionTimeout();
	virtual ~RodinBTNodeConditionTimeout();

	DEFINE_RODINBTNODE_FACTORY( ConditionTimeout );

	virtual void		InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual ETickStatus	Tick( const float DeltaTime );

protected:
	WBParamEvaluator	m_TimeoutPE;
	float				m_NextCanExecuteTime;
};

#endif // RODINBTNODETIMEOUT_H