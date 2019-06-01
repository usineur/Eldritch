#include "core.h"
#include "sdpeldlit.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "eldframework.h"
#include "eldgame.h"
#include "eldmesh.h"
#include "vector4.h"

SDPEldLit::SDPEldLit()
{
}

SDPEldLit::~SDPEldLit()
{
}

/*virtual*/ void SDPEldLit::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& View ) const
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

	EldMesh* const pEldMesh = static_cast<EldMesh*>( pMesh );
	STATIC_HASHED_STRING( LightCube );
	const SVoxelIrradiance& Irradiance = pEldMesh->GetIrradianceCube();
	pRenderer->SetPixelShaderFloat4s( sLightCube, Irradiance.m_Light[0].GetArray(), 6 );
}
