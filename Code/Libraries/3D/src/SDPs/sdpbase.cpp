#include "core.h"
#include "sdpbase.h"
#include "irenderer.h"
#include "mesh.h"
#include "view.h"
#include "bonearray.h"
#include "reversehash.h"
#include "simplestring.h"

SDPBase::SDPBase()
{
}

SDPBase::~SDPBase()
{
}

/*virtual*/ void SDPBase::SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const
{
	const Matrix&	WorldMatrix		= pRenderer->GetWorldMatrix();
	const Matrix&	VPMatrix		= pRenderer->GetViewProjectionMatrix();
	const Matrix	WVP				= WorldMatrix * VPMatrix;
	const Vector4	ViewPosition	= CurrentView.m_Location;

	// Set RT dimensions for vertex shader VPos stuff
	const Vector2	ViewportDims		= pRenderer->GetRenderTargetOrViewportDimensions();
	const Vector4	RTDims				= Vector4( 0.5f * ViewportDims.x, 0.5f * ViewportDims.y, 0.5f / ViewportDims.x, 0.5f / ViewportDims.y );
	const Vector4	HalfPixelOffsetFix	= Vector4( -1.0f / ViewportDims.x, 1.0f / ViewportDims.y, 0.0f, 0.0f );	// Legacy D3D9 half-pixel offset fix

	// Clip planes for pixel shader world space depth normalization
	const Vector4	ClipPlanes			= Vector4( CurrentView.m_NearClip, CurrentView.m_FarClip, 1.0f / CurrentView.m_NearClip, 1.0f / CurrentView.m_FarClip );

	STATIC_HASHED_STRING( WorldMatrix );
	pRenderer->SetVertexShaderUniform( sWorldMatrix,		WorldMatrix );

	STATIC_HASHED_STRING( VP );
	pRenderer->SetVertexShaderUniform( sVP,					VPMatrix );

	// HACKHACK: Clear this first, because for some reason it fixes rendering glitch. Driver issue?
	STATIC_HASHED_STRING( WVP );
	pRenderer->SetVertexShaderUniform( sWVP,				Matrix() );
	pRenderer->SetVertexShaderUniform( sWVP,				WVP );

	STATIC_HASHED_STRING( ViewPosition );
	pRenderer->SetVertexShaderUniform( sViewPosition,		ViewPosition );
	pRenderer->SetPixelShaderUniform( sViewPosition,		ViewPosition );

	STATIC_HASHED_STRING( RTDims );
	pRenderer->SetVertexShaderUniform( sRTDims,				RTDims );

	STATIC_HASHED_STRING( HalfPixelOffsetFix );
	pRenderer->SetVertexShaderUniform( sHalfPixelOffsetFix,	HalfPixelOffsetFix );

	STATIC_HASHED_STRING( ClipPlanes );
	pRenderer->SetPixelShaderUniform( sClipPlanes,			ClipPlanes );

	if( pMesh->IsAnimated() )
	{
		const float* const	pBoneMatrixFloats	= pMesh->m_BoneMatrices[0].GetArray();
		const uint			NumBones			= pMesh->m_Bones->GetNumBones();

		STATIC_HASHED_STRING( BoneMatrices );
		pRenderer->SetVertexShaderMatrices( sBoneMatrices, pBoneMatrixFloats, NumBones );
	}
}
