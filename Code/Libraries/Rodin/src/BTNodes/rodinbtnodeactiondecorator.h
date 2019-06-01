#ifndef RODINBTNODEACTIONDECORATOR_H
#define RODINBTNODEACTIONDECORATOR_H

#include "rodinbtnodedecorator.h"
#include "array.h"

class WBAction;

class RodinBTNodeActionDecorator : public RodinBTNodeDecorator
{
public:
	RodinBTNodeActionDecorator();
	virtual ~RodinBTNodeActionDecorator();

	DEFINE_RODINBTNODE_FACTORY( ActionDecorator );

	virtual void		InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void		OnStart();
	virtual void		OnFinish();

private:
	Array<WBAction*>	m_StartActions;
	Array<WBAction*>	m_FinishActions;
};

#endif // RODINBTNODEACTIONDECORATOR_H