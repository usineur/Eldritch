#include "core.h"
#include "eldsurfaces.h"
#include "map.h"
#include "hashedstring.h"
#include "configmanager.h"

typedef Map<HashedString, EldSurfaces::SSurface> TSurfaceMap;

static TSurfaceMap	sSurfaceMap;
static int			sRefCount = 0;

static void StaticInitialize();
static void StaticShutDown();

void EldSurfaces::AddRef()
{
	if( sRefCount++ == 0 )
	{
		StaticInitialize();
	}
}

void EldSurfaces::Release()
{
	if( --sRefCount == 0 )
	{
		StaticShutDown();
	}
}

const EldSurfaces::SSurface& EldSurfaces::GetSurface( const HashedString& Surface )
{
	DEVASSERT( sRefCount > 0 );

	const TSurfaceMap::Iterator SurfaceIter = sSurfaceMap.Search( Surface );
	if( SurfaceIter.IsValid() )
	{
		return SurfaceIter.GetValue();
	}
	else
	{
		WARNDESC( "Unknown surface looked up in EldSurfaces::GetSurface" );
		return sSurfaceMap[ HashedString::NullString ];	// Will create the default surface if it doesn't exist
	}
}

SimpleString EldSurfaces::GetSound( const HashedString& Surface )
{
	return GetSurface( Surface ).m_Sound;
}

float EldSurfaces::GetVolumeScalar( const HashedString& Surface )
{
	return GetSurface( Surface ).m_VolumeScalar;
}

float EldSurfaces::GetRadiusScalar( const HashedString& Surface )
{
	return GetSurface( Surface ).m_RadiusScalar;
}

void StaticInitialize()
{
	STATICHASH( EldSurfaces );

	STATICHASH( NumSurfaces );
	const uint NumSurfaces = ConfigManager::GetInt( sNumSurfaces, 0, sEldSurfaces );

	for( uint SurfaceIndex = 0; SurfaceIndex < NumSurfaces; ++SurfaceIndex )
	{
		const HashedString			SurfaceName	= ConfigManager::GetSequenceHash( "Surface%d", SurfaceIndex, HashedString::NullString, sEldSurfaces );
		EldSurfaces::SSurface&	Surface		= sSurfaceMap[ SurfaceName ];

		Surface.m_Sound							= ConfigManager::GetSequenceString( "Surface%dSound", SurfaceIndex, "", sEldSurfaces );
		Surface.m_VolumeScalar					= ConfigManager::GetSequenceFloat( "Surface%dVolumeScalar", SurfaceIndex, 1.0f, sEldSurfaces );
		Surface.m_RadiusScalar					= ConfigManager::GetSequenceFloat( "Surface%dRadiusScalar", SurfaceIndex, 1.0f, sEldSurfaces );
	}
}

void StaticShutDown()
{
	sSurfaceMap.Clear();
}