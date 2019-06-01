#include "core.h"
#include "wbactionstack.h"
#include "array.h"
#include "wbevent.h"

static Array<const WBEvent*>*	sStack;
static Array<WBEntity*>*		sActingEntityStack;
static Array<WBEntity*>*		sReactionsSelfStack;

void WBActionStack::Initialize()
{
	DEVASSERT( !sStack );
	sStack = new Array<const WBEvent*>;
	sStack->SetDeflate( false );

	DEVASSERT( !sActingEntityStack );
	sActingEntityStack = new Array<WBEntity*>;
	sActingEntityStack->SetDeflate( false );

	DEVASSERT( !sReactionsSelfStack );
	sReactionsSelfStack = new Array<WBEntity*>;
	sReactionsSelfStack->SetDeflate( false );
}

void WBActionStack::ShutDown()
{
	SafeDelete( sStack );

	SafeDelete( sActingEntityStack );
	SafeDelete( sReactionsSelfStack );
}

void WBActionStack::Push( const WBEvent& Event, WBEntity* const pActingEntity )
{
	DEVASSERT( sStack );
	sStack->PushBack( &Event );

	DEVASSERT( sActingEntityStack );
	sActingEntityStack->PushBack( pActingEntity );
}

void WBActionStack::Pop()
{
	DEVASSERT( sStack );
	sStack->PopBack();

	DEVASSERT( sActingEntityStack );
	sActingEntityStack->PopBack();
}

const WBEvent& WBActionStack::TopEvent()
{
	DEVASSERT( sStack );
	DEVASSERT( sStack->Size() );
	return *(sStack->Last());
}

WBEntity* WBActionStack::TopActingEntity()
{
	DEVASSERT( sActingEntityStack );
	DEVASSERT( sActingEntityStack->Size() );
	return sActingEntityStack->Last();
}

void WBActionStack::PushReactionsSelf( WBEntity* const pReactionsSelf )
{
	DEVASSERT( sReactionsSelfStack );
	sReactionsSelfStack->PushBack( pReactionsSelf );
}

void WBActionStack::PopReactionsSelf()
{
	DEVASSERT( sReactionsSelfStack );
	sReactionsSelfStack->PopBack();
}

const WBEntity* WBActionStack::TopReactionsSelf()
{
	DEVASSERT( sReactionsSelfStack );
	DEVASSERT( sReactionsSelfStack->Size() );
	return sReactionsSelfStack->Last();
}
