#include "core.h"
#include "wbactionelddamageworld.h"
#include "eldworld.h"
#include "Components/wbcompeldtransform.h"
#include "configmanager.h"
#include "wbactionstack.h"
#include "eldframework.h"

WBActionEldDamageWorld::WBActionEldDamageWorld()
:	m_Radius( 0.0f )
{
}

WBActionEldDamageWorld::~WBActionEldDamageWorld()
{
}

/*virtual*/ void WBActionEldDamageWorld::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	WBAction::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( Radius );
	m_Radius = ConfigManager::GetFloat( sRadius, 0.0f, sDefinitionName );
}

/*virtual*/ void WBActionEldDamageWorld::Execute()
{
	WBAction::Execute();

	STATIC_HASHED_STRING( EventOwner );
	WBEntity* const				pEntity		= WBActionStack::TopEvent().GetEntity( sEventOwner );
	DEVASSERT( pEntity );

	WBCompEldTransform* const	pTransform	= pEntity->GetTransformComponent<WBCompEldTransform>();
	DEVASSERT( pTransform );

	EldWorld* const		pWorld		= EldFramework::GetInstance()->GetWorld();
	ASSERT( pWorld );

	pWorld->RemoveVoxelsAt( pTransform->GetLocation(), m_Radius );
}
