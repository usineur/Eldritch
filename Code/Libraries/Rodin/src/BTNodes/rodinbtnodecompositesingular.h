#ifndef RODINBTNODECOMPOSITESINGULAR_H
#define RODINBTNODECOMPOSITESINGULAR_H

#include "rodinbtnodecomposite.h"

class RodinBTNodeCompositeSingular : public RodinBTNodeComposite
{
public:
	RodinBTNodeCompositeSingular();
	virtual ~RodinBTNodeCompositeSingular();

	virtual void	OnStart();
	virtual void	OnFinish();
	virtual void	OnChildCompleted( RodinBTNode* pChildNode, ETickStatus TickStatus );

#if BUILD_DEV
	virtual void	Report();
#endif

protected:
	uint		m_ChildIndex;
	ETickStatus m_ChildStatus;
};

#endif // RODINBTNODECOMPOSITESINGULAR_H