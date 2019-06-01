#include "core.h"
#include "eldfactions.h"
#include "map.h"
#include "hashedstring.h"
#include "configmanager.h"

typedef Map<HashedString, EldFactions::EFactionCon> TFactionConMap;
typedef Map<HashedString, TFactionConMap> TFactionMap;

static TFactionMap	sFactionMap;
static int			sRefCount = 0;

static void StaticInitialize();
static void StaticShutDown();
EldFactions::EFactionCon StaticGetCon( const HashedString& Con );

void EldFactions::AddRef()
{
	if( sRefCount++ == 0 )
	{
		StaticInitialize();
	}
}

void EldFactions::Release()
{
	if( --sRefCount == 0 )
	{
		StaticShutDown();
	}
}

EldFactions::EFactionCon EldFactions::GetCon( const HashedString& FactionA, const HashedString& FactionB )
{
	// If faction relationship isn't described, default to being friendly to same faction and neutral to others
	const EFactionCon DefaultCon = ( FactionA == FactionB ) ? EFR_Friendly : EFR_Neutral;

	const TFactionMap::Iterator FactionAIter = sFactionMap.Search( FactionA );
	if( FactionAIter.IsNull() )
	{
		return DefaultCon;
	}

	const TFactionConMap& FactionConMap = FactionAIter.GetValue();
	const TFactionConMap::Iterator FactionBIter = FactionConMap.Search( FactionB );
	if( FactionBIter.IsNull() )
	{
		return DefaultCon;
	}

	return FactionBIter.GetValue();
}

void StaticInitialize()
{
	STATICHASH( EldritchFactions );

	STATICHASH( NumFactionCons );
	const uint NumFactionCons = ConfigManager::GetInt( sNumFactionCons, 0, sEldritchFactions );

	for( uint FactionConIndex = 0; FactionConIndex < NumFactionCons; ++FactionConIndex )
	{
		const HashedString FactionA = ConfigManager::GetSequenceHash( "FactionCon%dA", FactionConIndex, HashedString::NullString, sEldritchFactions );
		const HashedString FactionB = ConfigManager::GetSequenceHash( "FactionCon%dB", FactionConIndex, HashedString::NullString, sEldritchFactions );
		const HashedString FactionC = ConfigManager::GetSequenceHash( "FactionCon%dC", FactionConIndex, HashedString::NullString, sEldritchFactions );
		EldFactions::EFactionCon Con = StaticGetCon( FactionC );

		TFactionConMap& FactionConMap = sFactionMap[ FactionA ];
		FactionConMap[ FactionB ] = Con;
	}
}

void StaticShutDown()
{
	FOR_EACH_MAP( FactionAIter, sFactionMap, HashedString, TFactionConMap )
	{
		TFactionConMap& FactionConMap = FactionAIter.GetValue();
		FactionConMap.Clear();
	}
	sFactionMap.Clear();
}

EldFactions::EFactionCon StaticGetCon( const HashedString& Con )
{
	STATIC_HASHED_STRING( Hostile );
	STATIC_HASHED_STRING( Neutral );
	STATIC_HASHED_STRING( Friendly );

	if( Con == sHostile )
	{
		return EldFactions::EFR_Hostile;
	}
	else if( Con == sNeutral )
	{
		return EldFactions::EFR_Neutral;
	}
	else if( Con == sFriendly )
	{
		return EldFactions::EFR_Friendly;
	}
	else
	{
		WARN;
		return EldFactions::EFR_Neutral;
	}
}
