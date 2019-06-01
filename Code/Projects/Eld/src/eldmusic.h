#ifndef ELDMUSIC_H
#define ELDMUSIC_H

class ISoundInstance;

class EldMusic
{
public:
	EldMusic();
	~EldMusic();

	void	PlayMusic( const SimpleString& MusicSoundDef );
	void	StopMusic();

	static void		InstanceDeleteCallback( void* pVoid, ISoundInstance* pInstance );

private:
	void			OnInstanceDeleted( ISoundInstance* const pInstance );

	ISoundInstance*	m_MusicInstance;
};

#endif // ELDMUSIC_H
