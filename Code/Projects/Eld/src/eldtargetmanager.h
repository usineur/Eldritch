#ifndef ELDTARGETMANAGER_H
#define ELDTARGETMANAGER_H

#include "map.h"
#include "hashedstring.h"

class IRenderer;
class IRenderTarget;

class EldTargetManager
{
public:
	EldTargetManager( IRenderer* const pRenderer );
	~EldTargetManager();

	void	CreateTargets( const uint DisplayWidth, const uint DisplayHeight );
	void	ReleaseTargets();

	IRenderTarget*	GetRenderTarget( const HashedString& Tag ) const;

private:
	IRenderer*							m_Renderer;
	Map<HashedString, IRenderTarget*>	m_RenderTargets;
};

#endif // ELDTARGETMANAGER_H
