#include "core.h"
#include "sdpeldhudcalibration.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "eldframework.h"
#include "eldgame.h"
#include "vector4.h"

SDPEldHUDCalibration::SDPEldHUDCalibration()
{
}

SDPEldHUDCalibration::~SDPEldHUDCalibration()
{
}

/*virtual*/ void SDPEldHUDCalibration::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const
{
	SDPEldHUD::SetShaderParameters( pRenderer, pMesh, CurrentView );

	EldFramework* const	pFramework	= EldFramework::GetInstance();
	EldGame* const			pGame		= pFramework->GetGame();
	const Vector4			Gamma		= Vector4( pGame->GetGamma(), 0.0f, 0.0f, 0.0f );

	STATIC_HASHED_STRING( Gamma );
	pRenderer->SetPixelShaderUniform( sGamma, Gamma );

	STATIC_HASHED_STRING( ConstantColor );
	pRenderer->SetPixelShaderUniform( sConstantColor, pMesh->m_ConstantColor );
}
