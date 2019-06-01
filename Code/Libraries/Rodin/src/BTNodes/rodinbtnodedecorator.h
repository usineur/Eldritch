#ifndef RODINBTNODEDECORATOR_H
#define RODINBTNODEDECORATOR_H

#include "rodinbtnode.h"

class RodinBTNodeDecorator : public RodinBTNode
{
public:
	RodinBTNodeDecorator();
	virtual ~RodinBTNodeDecorator();

	virtual void		InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual ETickStatus Tick( const float DeltaTime );
	virtual void		OnStart();
	virtual void		OnFinish();
	virtual void		OnChildCompleted( RodinBTNode* pChildNode, ETickStatus TickStatus );

#if BUILD_DEV
	virtual void		Report();
#endif

protected:
	RodinBTNode*	m_Child;
	ETickStatus		m_ChildStatus;
};

#endif // RODINBTNODEDECORATOR_H