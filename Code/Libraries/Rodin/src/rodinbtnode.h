#ifndef RODINBTNODE_H
#define RODINBTNODE_H

#include "simplestring.h"

class WBCompRodinBehaviorTree;
class WBEntity;
class WBEventManager;

#define DEFINE_RODINBTNODE_FACTORY( type ) static class RodinBTNode* Factory() { return new RodinBTNode##type; }
typedef class RodinBTNode* ( *RodinBTNodeFactoryFunc )( void );

class RodinBTNode
{
public:
	enum ETickStatus
	{
		ETS_None,
		ETS_Fail,
		ETS_Running,
		ETS_Success
	};

	RodinBTNode();
	virtual ~RodinBTNode();

	virtual void		InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual ETickStatus	Tick( const float DeltaTime );
	virtual void		OnStart();
	virtual void		OnFinish();
	virtual void		OnChildCompleted( RodinBTNode* pChildNode, ETickStatus TickStatus );

#if BUILD_DEV
	virtual void		Report();
	SimpleString		GetName() const { return m_Name; }
#endif

	WBEntity*			GetEntity() const;
	float				GetTime() const;
	WBEventManager*		GetEventManager() const;

	WBCompRodinBehaviorTree*	m_BehaviorTree;	// Provided for behavior context--this is our scheduler, and we can get back to the entity through it
	bool						m_IsSleeping;

#if BUILD_DEV
	SimpleString				m_Name;
	uint						m_Depth;
#endif
};

#endif // RODINBTNODE_H