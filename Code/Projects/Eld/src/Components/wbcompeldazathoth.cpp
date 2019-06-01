#include "core.h"
#include "wbcompeldazathoth.h"
#include "configmanager.h"
#include "wbentity.h"
#include "idatastream.h"

WBCompEldAzathoth::WBCompEldAzathoth()
:	m_AzathothSpawnTime( 0.0f )
,	m_AzathothSpawnEventUID( 0 )
{
}

WBCompEldAzathoth::~WBCompEldAzathoth()
{
}

/*virtual*/ void WBCompEldAzathoth::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	Super::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( AzathothSpawnTime );
	m_AzathothSpawnTime = ConfigManager::GetInheritedFloat( sAzathothSpawnTime, 0.0f, sDefinitionName );
}

/*virtual*/ void WBCompEldAzathoth::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	Super::HandleEvent( Event );

	STATIC_HASHED_STRING( OnSpawned );

	const HashedString EventName = Event.GetEventName();
	if( EventName == sOnSpawned )
	{
		WB_MAKE_EVENT( SpawnAzathoth, GetEntity() );
		m_AzathothSpawnEventUID = WB_QUEUE_EVENT_DELAY( GetEventManager(), SpawnAzathoth, GetEntity(), m_AzathothSpawnTime );
	}
}

/*virtual*/ void WBCompEldAzathoth::Tick( const float DeltaTime )
{
	XTRACE_FUNCTION;

	Unused( DeltaTime );

	PublishToHUD();
}

void WBCompEldAzathoth::PublishToHUD() const
{
	const float fRemainingTime		= GetEventManager()->GetRemainingTime( m_AzathothSpawnEventUID );
	const int	RemainingTime		= static_cast<uint>( fRemainingTime );
	const int	RemainingMinutes	= RemainingTime / 60;
	const int	RemainingSeconds	= RemainingTime % 60;

	STATICHASH( HUD );
	STATICHASH( AzathothMinutes );
	ConfigManager::SetInt( sAzathothMinutes, RemainingMinutes, sHUD );

	STATICHASH( AzathothSeconds );
	ConfigManager::SetInt( sAzathothSeconds, RemainingSeconds, sHUD );

	STATICHASH( AzathothTimer );

	{
		WB_MAKE_EVENT( SetWidgetHidden, NULL );
		WB_SET_AUTO( SetWidgetHidden, Hash, Screen, sHUD );
		WB_SET_AUTO( SetWidgetHidden, Hash, Widget, sAzathothTimer );
		WB_SET_AUTO( SetWidgetHidden, Bool, Hidden, false );
		WB_DISPATCH_EVENT( GetEventManager(), SetWidgetHidden, NULL );
	}

	{
		const bool		IsTimeRemaining	= fRemainingTime > 0.0f;
		const Vector	TimerColor		= IsTimeRemaining ? Vector( 1.0f, 1.0f, 1.0f ) : Vector( 1.0f, 0.0f, 0.0f );

		WB_MAKE_EVENT( SetWidgetColor, NULL );
		WB_SET_AUTO( SetWidgetColor, Hash, Screen, sHUD );
		WB_SET_AUTO( SetWidgetColor, Hash, Widget, sAzathothTimer );
		WB_SET_AUTO( SetWidgetColor, Vector, Color, TimerColor );
		WB_DISPATCH_EVENT( GetEventManager(), SetWidgetColor, NULL );
	}
}

#define VERSION_EMPTY			0
#define VERSION_SPAWNEVENTUID	1
#define VERSION_CURRENT			1

uint WBCompEldAzathoth::GetSerializationSize()
{
	uint Size = 0;

	Size += 4;	// Version

	Size += sizeof( TEventUID );	// m_AzathothSpawnEventUID

	return Size;
}

void WBCompEldAzathoth::Save( const IDataStream& Stream )
{
	Stream.WriteUInt32( VERSION_CURRENT );

	Stream.Write( sizeof( TEventUID ), &m_AzathothSpawnEventUID );
}

void WBCompEldAzathoth::Load( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	const uint Version = Stream.ReadUInt32();

	if( Version >= VERSION_SPAWNEVENTUID )
	{
		Stream.Read( sizeof( TEventUID ), &m_AzathothSpawnEventUID );
	}
}
