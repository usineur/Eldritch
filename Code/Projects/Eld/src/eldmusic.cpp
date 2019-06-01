#include "core.h"
#include "eldmusic.h"
#include "isoundinstance.h"
#include "eldframework.h"

EldMusic::EldMusic()
:	m_MusicInstance( NULL )
{
	IAudioSystem* const pAudioSystem = EldFramework::GetInstance()->GetAudioSystem();
	ASSERT( pAudioSystem );

	SInstanceDeleteCallback Callback;
	Callback.m_Callback	= InstanceDeleteCallback;
	Callback.m_Void		= this;
	pAudioSystem->RegisterInstanceDeleteCallback( Callback );
}

EldMusic::~EldMusic()
{
	IAudioSystem* const pAudioSystem = EldFramework::GetInstance()->GetAudioSystem();
	ASSERT( pAudioSystem );

	if( m_MusicInstance )
	{
		pAudioSystem->RemoveSoundInstance( m_MusicInstance );
	}

	SInstanceDeleteCallback Callback;
	Callback.m_Callback	= InstanceDeleteCallback;
	Callback.m_Void		= this;
	pAudioSystem->UnregisterInstanceDeleteCallback( Callback );
}

void EldMusic::PlayMusic( const SimpleString& MusicSoundDef )
{
	StopMusic();

	if( MusicSoundDef == "" )
	{
		return;
	}

	IAudioSystem* const pAudioSystem = EldFramework::GetInstance()->GetAudioSystem();
	ASSERT( pAudioSystem );

	m_MusicInstance = pAudioSystem->CreateSoundInstance( MusicSoundDef );
	ASSERT( m_MusicInstance );

	m_MusicInstance->Tick();
	m_MusicInstance->Play();
}

void EldMusic::StopMusic()
{
	if( m_MusicInstance )
	{
		m_MusicInstance->Stop();
	}
}

/*static*/ void EldMusic::InstanceDeleteCallback( void* pVoid, ISoundInstance* pInstance )
{
	EldMusic* pMusic = static_cast<EldMusic*>( pVoid );
	ASSERT( pMusic );

	pMusic->OnInstanceDeleted( pInstance );
}

void EldMusic::OnInstanceDeleted( ISoundInstance* const pInstance )
{
	if( pInstance == m_MusicInstance )
	{
		m_MusicInstance = NULL;
	}
}