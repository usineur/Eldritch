#include "core.h"
#include "sdpeldpost.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "eldframework.h"
#include "eldgame.h"
#include "vector4.h"
#include "vector2.h"
#include "mathcore.h"
#include "mathfunc.h"
#include "display.h"
#include "matrix.h"

SDPEldPost::SDPEldPost()
{
}

SDPEldPost::~SDPEldPost()
{
}

/*virtual*/ void SDPEldPost::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const
{
	SDPBase::SetShaderParameters( pRenderer, pMesh, CurrentView );

	EldFramework* const		pFramework			= EldFramework::GetInstance();
	EldGame* const				pGame				= pFramework->GetGame();
	const Vector4				Gamma				= Vector4( pGame->GetGamma(), 0.0f, 0.0f, 0.0f );

	STATIC_HASHED_STRING( Gamma );
	pRenderer->SetPixelShaderUniform( sGamma, Gamma );
}
