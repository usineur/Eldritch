#include "core.h"
#include "animeventeldexecuteaction.h"
#include "wbactionfactory.h"
#include "configmanager.h"
#include "wbactionstack.h"
#include "wbevent.h"
#include "eldmesh.h"
#include "Components/wbcompowner.h"

AnimEventEldExecuteAction::AnimEventEldExecuteAction()
:	m_Actions()
{
}

AnimEventEldExecuteAction::~AnimEventEldExecuteAction()
{
	WBActionFactory::ClearActionArray( m_Actions );
}

void AnimEventEldExecuteAction::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBActionFactory::InitializeActionArray( DefinitionName, m_Actions );
}

void AnimEventEldExecuteAction::Call( Mesh* pMesh, Animation* pAnimation )
{
	Unused( pMesh );
	Unused( pAnimation );

	EldMesh* const	pEldMesh	= static_cast<EldMesh*>( pMesh );
	WBEntity* const		pEntity			= WBCompOwner::GetTopmostOwner( pEldMesh->GetEntity() );
	DEVASSERT( pEntity );

	WBEvent OnAnimEventEvent;
	STATIC_HASHED_STRING( OnAnimEvent );
	OnAnimEventEvent.SetEventName( sOnAnimEvent );
	pEntity->AddContextToEvent( OnAnimEventEvent );

	WBActionFactory::ExecuteActionArray( m_Actions, OnAnimEventEvent, pEntity );
}