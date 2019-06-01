#include "core.h"
#include "eldtargetmanager.h"
#include "3d.h"
#include "irenderer.h"
#include "irendertarget.h"
#include "eldgame.h"
#include "Screens/uiscreen-eldmirror.h"
#include "configmanager.h"

EldTargetManager::EldTargetManager( IRenderer* const pRenderer )
:	m_Renderer( pRenderer )
,	m_RenderTargets()
{
}

EldTargetManager::~EldTargetManager()
{
	ReleaseTargets();
}

void EldTargetManager::CreateTargets( const uint DisplayWidth, const uint DisplayHeight )
{
	ReleaseTargets();

	// Original RT, i.e. the backbuffer (possibly sized different from DisplayWidth, if upscaling to fullscreen)
	m_RenderTargets.Insert( "Original", m_Renderer->GetDefaultRenderTarget() );

	m_RenderTargets.Insert( "Primary", m_Renderer->CreateRenderTarget(
		SRenderTargetParams( DisplayWidth, DisplayHeight, ERTF_A16B16G16R16F, ERTF_D24S8 ) ) );		// NOTE: This used to be ERTF_UseDefault, but I think some platform didn't support that?

	// We need another buffer for bouncing FXAA
	m_RenderTargets.Insert( "Post", m_Renderer->CreateRenderTarget(
		SRenderTargetParams( DisplayWidth, DisplayHeight, ERTF_A16B16G16R16F, ERTF_None ) ) );

	// HACK: Grabbing mirror UI screen to get desired RT dimensions.
	UIScreenEldMirror* const	pMirrorScreen	= EldGame::GetMirrorScreen();
	const uint					MirrorRTWidth	= pMirrorScreen->GetMirrorRTWidth();
	const uint					MirrorRTHeight	= pMirrorScreen->GetMirrorRTHeight();

	m_RenderTargets.Insert( "Mirror", m_Renderer->CreateRenderTarget(
		SRenderTargetParams( MirrorRTWidth, MirrorRTHeight, ERTF_A16B16G16R16F, ERTF_D24S8 ) ) );	// NOTE: This used to be ERTF_UseDefault; I could attach the depth from Primary, but this should be fine

	m_RenderTargets.Insert( "MirrorFXAA", m_Renderer->CreateRenderTarget(
		SRenderTargetParams( MirrorRTWidth, MirrorRTHeight, ERTF_A16B16G16R16F, ERTF_None ) ) );

	STATICHASH( EldMinimap );
	STATICHASH( MinimapRTWidth );
	STATICHASH( MinimapRTHeight );
	const uint	MinimapRTWidth	= ConfigManager::GetInt( sMinimapRTWidth, 0, sEldMinimap );
	const uint	MinimapRTHeight	= ConfigManager::GetInt( sMinimapRTHeight, 0, sEldMinimap );
	m_RenderTargets.Insert( "Minimap", m_Renderer->CreateRenderTarget(
		SRenderTargetParams( MinimapRTWidth, MinimapRTHeight, ERTF_A16B16G16R16F, ERTF_D24S8 ) ) );	// NOTE: This used to be ERTF_UseDefault; I could attach the depth from Primary, but this should be fine
}

void EldTargetManager::ReleaseTargets()
{
	m_Renderer->FreeRenderTargets();

	FOR_EACH_MAP( RenderTargetIter, m_RenderTargets, HashedString, IRenderTarget* )
	{
		const HashedString& Tag			= RenderTargetIter.GetKey();
		IRenderTarget* pRenderTarget	= RenderTargetIter.GetValue();

		// Don't free the original render target! The renderer owns that.
		if( Tag == "Original" )
		{
			continue;
		}

		SafeDelete( pRenderTarget );
	}
}

IRenderTarget* EldTargetManager::GetRenderTarget( const HashedString& Tag ) const
{
	const Map<HashedString, IRenderTarget*>::Iterator RenderTargetIter = m_RenderTargets.Search( Tag );
	ASSERT( RenderTargetIter.IsValid() );
	return ( RenderTargetIter.IsValid() ) ? RenderTargetIter.GetValue() : NULL;
}