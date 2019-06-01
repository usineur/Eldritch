#include "core.h"
#include "sdpeldworld.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "eldframework.h"
#include "eldgame.h"
#include "vector4.h"

SDPEldWorld::SDPEldWorld()
{
}

SDPEldWorld::~SDPEldWorld()
{
}

/*virtual*/ void SDPEldWorld::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& View ) const
{
	SDPBase::SetShaderParameters( pRenderer, pMesh, View );

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	EldGame* const			pGame			= pFramework->GetGame();
	const Vector4				FogParams		= pGame->GetFogParams();
	const Vector4				FogPosition		= View.m_Location;

	STATIC_HASHED_STRING( FogParams );
	pRenderer->SetPixelShaderUniform( sFogParams,		FogParams );

	STATIC_HASHED_STRING( FogViewPosition );
	pRenderer->SetPixelShaderUniform( sFogViewPosition,	FogPosition );
}
