#include "core.h"
#include "sdpeldfxaa.h"
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

SDPEldFXAA::SDPEldFXAA()
{
}

SDPEldFXAA::~SDPEldFXAA()
{
}

/*virtual*/ void SDPEldFXAA::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const
{
	SDPBase::SetShaderParameters( pRenderer, pMesh, CurrentView );

	const Vector2	ViewportDims	= pRenderer->GetRenderTargetOrViewportDimensions();

	// ELDNOTE: These are different than the usual vertex shader RTDims!
	const Vector4	RTDims			= Vector4( 1.0f / ViewportDims.x, 1.0f / ViewportDims.y, 0.5f / ViewportDims.x, 0.5f / ViewportDims.y );
	const Vector4	RTDims2			= Vector4( -1.0f / ViewportDims.x, -1.0f / ViewportDims.y, 1.0f / ViewportDims.x, 1.0f / ViewportDims.y );

	STATIC_HASHED_STRING( RTDims );
	pRenderer->SetPixelShaderUniform( sRTDims, RTDims );

	STATIC_HASHED_STRING( RTDims2 );
	pRenderer->SetPixelShaderUniform( sRTDims2, RTDims2 );
}
