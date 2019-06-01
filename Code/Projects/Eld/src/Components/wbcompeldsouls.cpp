#include "core.h"
#include "wbcompeldsouls.h"
#include "configmanager.h"
#include "idatastream.h"
#include "mathcore.h"
#include "eldgame.h"
#include "Achievements/iachievementmanager.h"
#include "Components/wbcompstatmod.h"

WBCompEldSouls::WBCompEldSouls()
:	m_Souls( 0 )
,	m_HidePickupScreenDelay( 0.0f )
,	m_HidePickupScreenUID( 0 )
{
}

WBCompEldSouls::~WBCompEldSouls()
{
}

/*virtual*/ void WBCompEldSouls::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	Super::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( HidePickupScreenDelay );
	m_HidePickupScreenDelay = ConfigManager::GetInheritedFloat( sHidePickupScreenDelay, 0.0f, sDefinitionName );
}

/*virtual*/ void WBCompEldSouls::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	Super::HandleEvent( Event );

	STATIC_HASHED_STRING( AddSouls );
	STATIC_HASHED_STRING( OnInitialized );
	STATIC_HASHED_STRING( PushPersistence );
	STATIC_HASHED_STRING( PullPersistence );

	const HashedString EventName = Event.GetEventName();
	if( EventName == sOnInitialized )
	{
		PublishToHUD();
	}
	else if( EventName == sAddSouls )
	{
		STATIC_HASHED_STRING( Souls );
		const uint Souls = Event.GetInt( sSouls );

		STATIC_HASHED_STRING( ShowPickupScreen );
		const bool ShowPickupScreen = Event.GetBool( sShowPickupScreen );

		AddSouls( Souls, ShowPickupScreen );
	}
	else if( EventName == sPushPersistence )
	{
		PushPersistence();
	}
	else if( EventName == sPullPersistence )
	{
		PullPersistence();
	}
}

/*virtual*/ void WBCompEldSouls::AddContextToEvent( WBEvent& Event ) const
{
	Super::AddContextToEvent( Event );

	WB_SET_CONTEXT( Event, Int, Souls, m_Souls );
}

void WBCompEldSouls::AddSouls( const uint UnmoddedSouls, const bool ShowPickupScreen )
{
	// Apply any stat mods at the last minute. This isn't how money works,
	// since money can come from many sources. But it's easier here.
	WBCompStatMod* const	pStatMod		= GET_WBCOMP( GetEntity(), StatMod );
	float					fModdedSouls	= static_cast<float>( UnmoddedSouls );
	WB_MODIFY_FLOAT_SAFE( SoulsAmount, fModdedSouls, pStatMod );
	fModdedSouls							= WB_MODDED( SoulsAmount );
	const uint				ModdedSouls		= static_cast<uint>( fModdedSouls );

	m_Souls += ModdedSouls;

	PublishToHUD();

	if( ShowPickupScreen )
	{
		// Show the souls pickup screen and hide it after some time

		STATICHASH( SoulsPickup );
		STATICHASH( Souls );
		ConfigManager::SetInt( sSouls, ModdedSouls, sSoulsPickup );

		STATIC_HASHED_STRING( SoulsPickupScreen );

		{
			WB_MAKE_EVENT( PushUIScreen, NULL );
			WB_SET_AUTO( PushUIScreen, Hash, Screen, sSoulsPickupScreen );
			WB_DISPATCH_EVENT( GetEventManager(), PushUIScreen, NULL );
		}

		{
			// Remove previously queued hide event if any
			GetEventManager()->UnqueueEvent( m_HidePickupScreenUID );

			WB_MAKE_EVENT( RemoveUIScreen, NULL );
			WB_SET_AUTO( RemoveUIScreen, Hash, Screen, sSoulsPickupScreen );
			m_HidePickupScreenUID = WB_QUEUE_EVENT_DELAY( GetEventManager(), RemoveUIScreen, NULL, m_HidePickupScreenDelay );
		}
	}
}

void WBCompEldSouls::PublishToHUD() const
{
	STATICHASH( HUD );
	STATICHASH( Souls );
	ConfigManager::SetInt( sSouls, m_Souls, sHUD );
}

void WBCompEldSouls::PushPersistence() const
{
	TPersistence& Persistence = EldGame::StaticGetTravelPersistence();

	STATIC_HASHED_STRING( Souls );
	Persistence.SetInt( sSouls, m_Souls );
}

void WBCompEldSouls::PullPersistence()
{
	TPersistence& Persistence = EldGame::StaticGetTravelPersistence();

	STATIC_HASHED_STRING( Souls );
	m_Souls = Persistence.GetInt( sSouls );

	PublishToHUD();
}

#define VERSION_EMPTY	0
#define VERSION_SOULS	1
#define VERSION_CURRENT	1

uint WBCompEldSouls::GetSerializationSize()
{
	uint Size = 0;

	Size += 4;	// Version

	Size += 4;	// m_Souls

	return Size;
}

void WBCompEldSouls::Save( const IDataStream& Stream )
{
	Stream.WriteUInt32( VERSION_CURRENT );
	Stream.WriteUInt32( m_Souls );
}

void WBCompEldSouls::Load( const IDataStream& Stream )
{
	XTRACE_FUNCTION;

	const uint Version = Stream.ReadUInt32();

	if( Version >= VERSION_SOULS )
	{
		m_Souls = Stream.ReadUInt32();
	}
}
