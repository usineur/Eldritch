#ifndef ELDSOUND3DLISTENER_H
#define ELDSOUND3DLISTENER_H

#include "sound3dlistener.h"

class EldWorld;

class EldSound3DListener : public Sound3DListener
{
public:
	EldSound3DListener();
	virtual ~EldSound3DListener();

	virtual void	ModifyAttenuation( ISoundInstance* const pSoundInstance, float& Attenuation ) const;

	void			Initialize();
	void			SetWorld( EldWorld* const pWorld ) { m_World = pWorld; }

private:
	EldWorld*	m_World;
	float			m_VerticalScalar;
};

#endif // ELDSOUND3DLISTENER_H
