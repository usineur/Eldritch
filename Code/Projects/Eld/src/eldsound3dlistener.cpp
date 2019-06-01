#include "core.h"
#include "eldsound3dlistener.h"
#include "eldworld.h"
#include "collisioninfo.h"
#include "mathcore.h"
#include "isoundinstance.h"
#include "configmanager.h"

EldSound3DListener::EldSound3DListener()
:	m_World( NULL )
,	m_VerticalScalar( 0.0f )
{
}

EldSound3DListener::~EldSound3DListener()
{
}

void EldSound3DListener::Initialize()
{
	STATICHASH( AudioSystem );

	STATICHASH( VerticalScalar );
	m_VerticalScalar = ConfigManager::GetFloat( sVerticalScalar, 1.0f, sAudioSystem );
}

/*virtual*/ void EldSound3DListener::ModifyAttenuation( ISoundInstance* const pSoundInstance, float& Attenuation ) const
{
	PROFILE_FUNCTION;

	if( !m_World )
	{
		return;
	}

	if( !pSoundInstance->ShouldCalcOcclusion() )
	{
		return;
	}

	CollisionInfo FromListenerInfo;
	FromListenerInfo.m_In_CollideWorld		= true;
	FromListenerInfo.m_In_CollideEntities	= true;
	FromListenerInfo.m_In_UserFlags			= EECF_Occlusion;

	CollisionInfo FromSoundInfo;
	FromSoundInfo.m_In_CollideWorld			= true;
	FromSoundInfo.m_In_CollideEntities		= true;
	FromSoundInfo.m_In_UserFlags			= EECF_Occlusion;

	const Vector	SoundLocation	= pSoundInstance->GetLocation();
	const bool		Occluded		= m_World->LineCheck( m_Location, SoundLocation, FromListenerInfo ) &&
									  m_World->LineCheck( SoundLocation, m_Location, FromSoundInfo );

	if( Occluded )
	{
		// Use the ratio between the distances to sound source and to occlusion as a factor in attenuation.
		Vector				ToOcclusionNear				= FromListenerInfo.m_Out_Intersection - m_Location;
		ToOcclusionNear.z								*= m_VerticalScalar;

		Vector				ToOcclusionFar				= FromSoundInfo.m_Out_Intersection - SoundLocation;
		ToOcclusionFar.z								*= m_VerticalScalar;

		Vector				ToSound						= SoundLocation - m_Location;
		ToSound.z										*= m_VerticalScalar;

		const float			OccludedFalloffRadius		= pSoundInstance->GetOccludedFalloffRadius();

		const float			DistanceToOcclusionNear		= ToOcclusionNear.Length();
		const float			DistanceToOcclusionFar		= ToOcclusionFar.Length();
		const float			DistanceToSound				= ToSound.Length();
		const float			DistanceRatio				= ( DistanceToOcclusionNear + DistanceToOcclusionFar ) / DistanceToSound;

		// And attenuate occluded sounds more if they're more distant.
		const float			OcclusionAttenuation		= Attenuate( DistanceToSound, OccludedFalloffRadius );

		Attenuation *= DistanceRatio * OcclusionAttenuation;
	}
}