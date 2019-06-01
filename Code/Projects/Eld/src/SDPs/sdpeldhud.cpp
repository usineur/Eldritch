#include "core.h"
#include "sdpeldhud.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"

SDPEldHUD::SDPEldHUD()
{
}

SDPEldHUD::~SDPEldHUD()
{
}

/*virtual*/ void SDPEldHUD::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const
{
	SDPBase::SetShaderParameters( pRenderer, pMesh, CurrentView );

	STATIC_HASHED_STRING( ConstantColor );
	pRenderer->SetPixelShaderUniform( sConstantColor,	pMesh->m_ConstantColor );
}
