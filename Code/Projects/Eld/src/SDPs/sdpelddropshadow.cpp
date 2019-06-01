#include "core.h"
#include "sdpelddropshadow.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "eldframework.h"
#include "eldgame.h"
#include "vector4.h"

SDPEldDropShadow::SDPEldDropShadow()
{
}

SDPEldDropShadow::~SDPEldDropShadow()
{
}

/*virtual*/ void SDPEldDropShadow::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& View ) const
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

	STATIC_HASHED_STRING( ConstantColor );
	pRenderer->SetPixelShaderUniform( sConstantColor,	pMesh->m_ConstantColor );
}
