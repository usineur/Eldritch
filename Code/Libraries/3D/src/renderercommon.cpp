#include "core.h"
#include "renderercommon.h"
#include "bucket.h"
#include "mesh.h"
#include "meshfactory.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "font.h"
#include "fontmanager.h"
#include "vertexdeclarationmanager.h"
#include "ivertexbuffer.h"
#include "ivertexdeclaration.h"
#include "configmanager.h"
#include "shaderdataprovider.h"
#include "mathcore.h"
#include "itexture.h"
#include "frustum.h"

RendererCommon::RendererCommon()
:	m_OrderedBuckets()
,	m_NamedBuckets()
#if BUILD_DEV
,	m_DeferredDeleteDebugMeshes()
#endif
,	m_WorldMatrix()
,	m_ViewMatrix()
,	m_ProjectionMatrix()
,	m_ViewProjectionMatrix()
,	m_View()
,	m_CurrentRenderTarget( NULL )
,	m_DefaultRenderTarget( NULL )
,	m_RenderTargets()
,	m_DynamicVertexBuffers()
,	m_ShaderManager( NULL )
,	m_TextureManager( NULL )
,	m_FontManager( NULL )
,	m_VertexDeclarationManager( NULL )
,	m_MeshFactory( NULL )
,	m_DoFrustumCulling( false )
,	m_DoMaterialSort( false )
,	m_ShaderProgram( NULL )
,	m_VertexShader( NULL )
,	m_PixelShader( NULL )
,	m_VertexDeclaration( NULL )
,	m_RenderState()
,	m_SamplerStates()
,	m_MaxVertexAttribs( 0 )
,	m_Display( NULL )
#if BUILD_DEBUG
,	m_DEBUGRenderStats()
#endif
{
#if BUILD_DEV
	m_DeferredDeleteDebugMeshes.SetDeflate( false );
#endif
}

RendererCommon::~RendererCommon()
{
	SafeDelete( m_DefaultRenderTarget );

	SafeDelete( m_ShaderManager );
	SafeDelete( m_TextureManager );
	SafeDelete( m_FontManager );
	SafeDelete( m_VertexDeclarationManager );
	SafeDelete( m_MeshFactory );

	FreeBuckets();
}

void RendererCommon::Initialize()
{
	XTRACE_FUNCTION;

	STATICHASH( Renderer );

	STATICHASH( FrustumCulling );
	m_DoFrustumCulling =
		ConfigManager::GetBool( sFrustumCulling, false, sRenderer ) ||
		ConfigManager::GetBool( sFrustumCulling );	// Support for old projects

	STATICHASH( DoMaterialSort );
	m_DoMaterialSort = ConfigManager::GetBool( sDoMaterialSort, false, sRenderer );
}

void RendererCommon::SetWorldMatrix( const Matrix& WorldMatrix )
{
	m_WorldMatrix = WorldMatrix;
}

void RendererCommon::SetViewMatrix( const Matrix& ViewMatrix )
{
	m_ViewMatrix			= ViewMatrix;
	m_ViewProjectionMatrix	= ViewMatrix * m_ProjectionMatrix;
}

void RendererCommon::SetProjectionMatrix( const Matrix& ProjectionMatrix )
{
	m_ProjectionMatrix		= ProjectionMatrix;
	m_ViewProjectionMatrix	= m_ViewMatrix * ProjectionMatrix;
}

void RendererCommon::AddMesh( Mesh* pMesh )
{
	XTRACE_FUNCTION;

	DEVASSERT( pMesh );

	if( pMesh->m_VertexBuffer->GetNumVertices() == 0 )
	{
		return;
	}

	const HashedString& PrescribedBucket = pMesh->GetBucket();
	if( PrescribedBucket == HashedString::NullString )
	{
		// Pick one or more buckets by flags
#if BUILD_DEV
		uint NumBucketsAdded = 0;
#endif

		const uint MaterialFlags = pMesh->GetMaterialFlags();

		Bucket* pBucket;
		uint NumBuckets = m_OrderedBuckets.Size();
		for( uint i = 0; i < NumBuckets; ++i )
		{
			pBucket = m_OrderedBuckets[i];
			// This conditional means that the mesh must have all the
			// flags of the bucket and none of the filtered flags
			if( ( MaterialFlags & pBucket->m_Flags ) == pBucket->m_Flags &&
				( MaterialFlags & pBucket->m_FilterFlags ) == 0 )
			{
#if BUILD_DEV
				++NumBucketsAdded;
#endif
				pBucket->m_Meshes.PushBack( pMesh );
				if( pBucket->m_ExclusiveMeshes )
				{
					return;
				}
			}
		}

		DEVASSERTDESC( NumBucketsAdded > 0, "Mesh was added but fell into no buckets." );
	}
	else
	{
		Bucket* const pBucket = GetBucket( PrescribedBucket );
		if( pBucket )
		{
			pBucket->m_Meshes.PushBack( pMesh );
		}
		else
		{
			DEVWARNDESC( "Mesh was added but prescribed bucket did not exist." );
		}
	}
}

void RendererCommon::AddBucket( const HashedString& Name, Bucket* pBucket )
{
	m_OrderedBuckets.PushBack( pBucket );
	m_NamedBuckets[ Name ] = pBucket;
}

Bucket*	RendererCommon::GetBucket( const HashedString& Name ) const
{
	Map<HashedString, Bucket*>::Iterator BucketIter = m_NamedBuckets.Search( Name );
	return BucketIter.IsValid() ? BucketIter.GetValue() : NULL;
}

Bucket*	RendererCommon::GetBucket( uint Index ) const
{
	if( Index < m_OrderedBuckets.Size() )
	{
		return m_OrderedBuckets[ Index ];
	}
	return NULL;
}

void RendererCommon::FreeBuckets()
{
	uint NumBuckets = m_OrderedBuckets.Size();
	for( uint i = 0; i < NumBuckets; ++i )
	{
		SafeDelete( m_OrderedBuckets[i] );
	}
	m_OrderedBuckets.Clear();
	m_NamedBuckets.Clear();
}

/*virtual*/ void RendererCommon::FlushBuckets()
{
	for( uint BucketIndex = 0; BucketIndex < m_OrderedBuckets.Size(); ++BucketIndex )
	{
		Bucket* const pBucket = m_OrderedBuckets[ BucketIndex ];
		if( pBucket )
		{
			pBucket->m_Meshes.Clear();
		}
	}
}

/*virtual*/ void RendererCommon::SetBucketsEnabled( const HashedString& GroupTag, const bool Enabled )
{
	FOR_EACH_ARRAY( BucketIter, m_OrderedBuckets, Bucket* )
	{
		Bucket* const pBucket = BucketIter.GetValue();

		if( pBucket->m_Tag != GroupTag )
		{
			continue;
		}

		pBucket->m_Enabled = Enabled;
	}
}

IVertexDeclaration* RendererCommon::GetVertexDeclaration( const uint VertexSignature )
{
	return GetVertexDeclarationManager()->GetVertexDeclaration( VertexSignature );
}

void RendererCommon::FreeRenderTargets()
{
	m_RenderTargets.Clear();
}

IRenderTarget* RendererCommon::GetCurrentRenderTarget()
{
	return m_CurrentRenderTarget;
}

IRenderTarget* RendererCommon::GetDefaultRenderTarget()
{
	return m_DefaultRenderTarget;
}

Vector2 RendererCommon::GetRenderTargetOrViewportDimensions() const
{
	Vector2 RenderTargetDimensions;

	if( NULL != m_CurrentRenderTarget && m_DefaultRenderTarget != m_CurrentRenderTarget )
	{
		RenderTargetDimensions.x = static_cast<float>( m_CurrentRenderTarget->GetWidth() );
		RenderTargetDimensions.y = static_cast<float>( m_CurrentRenderTarget->GetHeight() );
	}
	else if( NULL != m_Display )
	{
		RenderTargetDimensions.x = static_cast<float>( m_Display->m_Width );
		RenderTargetDimensions.y = static_cast<float>( m_Display->m_Height );
	}
	else
	{
		DEVWARN;
	}

	return RenderTargetDimensions;
}

void RendererCommon::AddDynamicVertexBuffer( IVertexBuffer* pBuffer )
{
	m_DynamicVertexBuffers.Insert( pBuffer );
}

void RendererCommon::RemoveDynamicVertexBuffer( IVertexBuffer* pBuffer )
{
	m_DynamicVertexBuffers.Remove( pBuffer );
}

void RendererCommon::ClearDynamicVertexBuffers()
{
	m_DynamicVertexBuffers.Clear();
}

/*virtual*/ void RendererCommon::SetDisplay( Display* const pDisplay )
{
	m_Display = pDisplay;
}

ShaderManager* RendererCommon::GetShaderManager()
{
	if( !m_ShaderManager )
	{
		m_ShaderManager = new ShaderManager( this );
	}
	return m_ShaderManager;
}

TextureManager* RendererCommon::GetTextureManager()
{
	if( !m_TextureManager )
	{
		m_TextureManager = new TextureManager( this );
	}
	return m_TextureManager;
}

FontManager* RendererCommon::GetFontManager()
{
	if( !m_FontManager )
	{
		m_FontManager = new FontManager( this );
	}
	return m_FontManager;
}

VertexDeclarationManager* RendererCommon::GetVertexDeclarationManager()
{
	if( !m_VertexDeclarationManager )
	{
		m_VertexDeclarationManager = new VertexDeclarationManager( this );
	}
	return m_VertexDeclarationManager;
}

MeshFactory* RendererCommon::GetMeshFactory()
{
	if( !m_MeshFactory )
	{
		m_MeshFactory = new MeshFactory( this );
	}
	return m_MeshFactory;
}

void RendererCommon::RenderBucket( Bucket* pBucket )
{
	XTRACE_FUNCTION;

	PROFILE_FUNCTION;

	if( !pBucket->m_Enabled )
	{
		return;
	}

	if( pBucket->m_RenderTarget )
	{
		SetRenderTarget( pBucket->m_RenderTarget );
	}

	Clear( pBucket->m_ClearFlags, pBucket->m_ClearColor, pBucket->m_ClearDepth, pBucket->m_ClearStencil );

	// Depth range is set from bucket, not material
	SetZRange( pBucket->m_DepthMin, pBucket->m_DepthMax );

	if( pBucket->m_View )
	{
		SetView( *pBucket->m_View );
	}

	if( pBucket->m_Flags & MAT_ALPHA )
	{
		pBucket->Sort( m_View );
	}
	else if( m_DoMaterialSort && pBucket->m_SortByMaterial )
	{
		pBucket->SortByMaterials();
	}

	const bool		DoFrustumCulling	= m_DoFrustumCulling && pBucket->m_DoFrustumCulling;
	const Frustum	ViewFrustum			= Frustum( GetViewProjectionMatrix() );
	const uint		NumMeshes			= pBucket->m_Meshes.Size();
	for( uint MeshIndex = 0; MeshIndex < NumMeshes; ++MeshIndex )
	{
		Mesh* const pMesh = pBucket->m_Meshes[ MeshIndex ];

		// Frustum culling--I can't do this earlier, when a mesh is added, because
		// it might be visible in one view (e.g., shadow map depth) and not to player.
		if( !DoFrustumCulling || PassesFrustum( pMesh, ViewFrustum ) )
		{
			RenderBucketMesh( pMesh, NULL );
		}

#if BUILD_DEV
		if( pMesh->m_IsDebugMesh )
		{
			m_DeferredDeleteDebugMeshes.PushBackUnique( pMesh );
		}
#endif
	}
}

bool RendererCommon::PassesFrustum( Mesh* const pMesh, const Frustum& ViewFrustum )
{
	DEVASSERT( pMesh );

	const uint MaterialFlags = pMesh->GetMaterialFlags();

	return
#if BUILD_DEV
		MaterialFlags & MAT_DEBUG_ALWAYS	||
#endif
		MaterialFlags & MAT_ALWAYS			||
		ViewFrustum.Intersects( pMesh->m_AABB );
}

void RendererCommon::RenderBucketMesh( Mesh* const pMesh, Material* const pMaterialOverride )
{
	XTRACE_FUNCTION;

	DEVASSERT( pMesh );
	DEVASSERT( pMesh->m_VertexBuffer );
	DEVASSERT( pMesh->m_IndexBuffer );
	DEVASSERT( pMesh->m_VertexBuffer->GetNumVertices() > 0 );

	SetWorldMatrix( pMesh->GetConcatenatedTransforms() );
	SetVertexDeclaration( pMesh->m_VertexDeclaration );
	SetVertexArrays( pMesh );

	if( pMesh->HasMultiPassMaterials() )
	{
		FOR_EACH_ARRAY( MultiPassMaterialIter, pMesh->m_MultiPassMaterials, Material )
		{
			const Material& MultiPassMaterial = MultiPassMaterialIter.GetValue();
			ApplyMaterial( MultiPassMaterial, pMesh, m_View );
			DrawElements( pMesh->m_VertexBuffer, pMesh->m_IndexBuffer );
#if BUILD_DEBUG
			++m_DEBUGRenderStats.m_NumDrawCalls;
#endif
		}
	}
	else
	{
		Material& UsingMaterial = pMaterialOverride ? *pMaterialOverride : pMesh->m_Material;
		ApplyMaterial( UsingMaterial, pMesh, m_View );
		DrawElements( pMesh->m_VertexBuffer, pMesh->m_IndexBuffer );
#if BUILD_DEBUG
		++m_DEBUGRenderStats.m_NumDrawCalls;
#endif
	}

#if BUILD_DEBUG
	++m_DEBUGRenderStats.m_NumMeshes;
	m_DEBUGRenderStats.m_NumPrimitives += pMesh->m_IndexBuffer->GetNumPrimitives();
#endif
}

void RendererCommon::SetView( const View& rView )
{
	m_View = rView;
	SetViewMatrix( rView.GetViewMatrix() );
	SetProjectionMatrix( rView.GetProjectionMatrix() );
}

// NOTE: This is a stripped-down version of RenderBucket with special rules
// because I can make assumptions about the content.
void RendererCommon::RenderShadowBucket( Bucket* const pShadowLightsBucket, Bucket* const pShadowCastersBucket )
{
	XTRACE_FUNCTION;

	PROFILE_FUNCTION;

	if( !pShadowLightsBucket->m_Enabled ||
		!pShadowCastersBucket->m_Enabled )
	{
		return;
	}

	IRenderTarget* const pLightRenderTarget = pShadowLightsBucket->m_RenderTarget ? pShadowLightsBucket->m_RenderTarget : m_CurrentRenderTarget;
	IRenderTarget* const pCastersRenderTarget = pShadowCastersBucket->m_RenderTarget;
	DEVASSERT( pCastersRenderTarget );
	DEVASSERT( pShadowCastersBucket->m_View );

	Clear( pShadowLightsBucket->m_ClearFlags, pShadowLightsBucket->m_ClearColor, pShadowLightsBucket->m_ClearDepth, pShadowLightsBucket->m_ClearStencil );

	if( pShadowLightsBucket->m_View )
	{
		SetView( *pShadowLightsBucket->m_View );
	}

	const Angles* const pCubemapRenderOrientations	= GetCubemapRenderOrientations();
	const bool			DoLightFrustumCulling		= m_DoFrustumCulling && pShadowLightsBucket->m_DoFrustumCulling;
	const Frustum		LightViewFrustum			= Frustum( GetViewProjectionMatrix() );
	const uint			NumLightMeshes				= pShadowLightsBucket->m_Meshes.Size();
	const View			MainView					= m_View;
	View				LightView;
	Material			ShadowMaterial;
	for( uint LightMeshIndex = 0; LightMeshIndex < NumLightMeshes; ++LightMeshIndex )
	{
		Mesh* const pLightMesh = pShadowLightsBucket->m_Meshes[ LightMeshIndex ];
		if( !DoLightFrustumCulling || PassesFrustum( pLightMesh, LightViewFrustum ) )
		{
			// Bit of a hack, but we can't get light radius from game code here
			const Vector	LightExtents	= pLightMesh->m_AABB.GetExtents();
			const float		LightFar		= Max( LightExtents.x, Max( LightExtents.y, LightExtents.z ) );
			const uint		LightCubeMask	= pLightMesh->m_Material.GetLightCubeMask();

			LightView				= *pShadowCastersBucket->m_View;	// This should already be set up with everything except transform
			LightView.m_Location	= pLightMesh->m_Location;
			LightView.SetClipPlanes( LightView.m_NearClip, LightFar );

			// Making assumption that these are cube RTs for point lights!
			for( uint FaceIndex = 0; FaceIndex < 6; ++FaceIndex )
			{
				if( 0 == ( LightCubeMask & ( 1 << FaceIndex ) ) )
				{
					// Optimization to selectively skip rendering certain side of the cubemap if we're
					// certain they won't be used (e.g. a spotlight pointed straight down). This also
					// skips clearing the target, so be certain it will not be used or we'll sample garbage.
					continue;
				}

				SetCubeRenderTarget( pCastersRenderTarget, FaceIndex );

				Clear( pShadowCastersBucket->m_ClearFlags, pShadowCastersBucket->m_ClearColor, pShadowCastersBucket->m_ClearDepth, pShadowCastersBucket->m_ClearStencil );

				LightView.m_Rotation	= pCubemapRenderOrientations[ FaceIndex ];
				SetView( LightView );

				const bool		DoShadowFrustumCulling	= m_DoFrustumCulling && pShadowCastersBucket->m_DoFrustumCulling;
				const Frustum	ShadowViewFrustum		= Frustum( GetViewProjectionMatrix() );
				const uint		NumShadowMeshes			= pShadowCastersBucket->m_Meshes.Size();
				for( uint ShadowMeshIndex = 0; ShadowMeshIndex < NumShadowMeshes; ++ShadowMeshIndex )
				{
					Mesh* const pShadowMesh = pShadowCastersBucket->m_Meshes[ ShadowMeshIndex ];

					if( !DoShadowFrustumCulling || PassesFrustum( pShadowMesh, ShadowViewFrustum ) )
					{
						// Copy material's sampler states into the override material since we may need its textures
						ShadowMaterial = pShadowCastersBucket->GetShadowMaterialOverride( pShadowMesh->m_Material, pShadowMesh->m_VertexDeclaration->GetSignature(), this );
						ShadowMaterial.CopySamplerStatesFrom( pShadowMesh->m_Material );
						RenderBucketMesh( pShadowMesh, &ShadowMaterial );
					}
				}
			}

			SetRenderTarget( pLightRenderTarget );
			SetView( MainView );
			RenderBucketMesh( pLightMesh, NULL );
		}
	}
}

void RendererCommon::RenderBuckets()
{
	XTRACE_FUNCTION;

	for( uint BucketIndex = 0; BucketIndex < m_OrderedBuckets.Size(); ++BucketIndex )
	{
		Bucket* const pBucket = m_OrderedBuckets[ BucketIndex ];
		if( pBucket->m_IsShadowMaster )
		{
			// For shadow-casting lights, render the following bucket once for each mesh in this bucket
			Bucket* const pBucketShadowCasters = m_OrderedBuckets[ ++BucketIndex ];
			RenderShadowBucket( pBucket, pBucketShadowCasters );
		}
		else
		{
			RenderBucket( pBucket );
		}
	}
}

void RendererCommon::PostRenderBuckets()
{
	XTRACE_FUNCTION;

	for( uint i = 0; i < m_OrderedBuckets.Size(); ++i )
	{
		Bucket* pBucket = m_OrderedBuckets[i];
		pBucket->m_Meshes.Clear();
	}

#if BUILD_DEV
	PROFILE_BEGIN( RendererCommon_DeferredDeleteDebugMeshes );
	const uint NumDeferredDeleteDebugMeshes = m_DeferredDeleteDebugMeshes.Size();
	for( uint DebugMeshIndex = 0; DebugMeshIndex < NumDeferredDeleteDebugMeshes; ++DebugMeshIndex )
	{
		Mesh* pDebugMesh = m_DeferredDeleteDebugMeshes[ DebugMeshIndex ];
		ASSERT( pDebugMesh );
		SafeDelete( pDebugMesh );
	}
	m_DeferredDeleteDebugMeshes.Clear();
	PROFILE_END;
#endif
}

#if BUILD_DEV
void RendererCommon::DEBUGDrawLine( const Vector& Start, const Vector& End, unsigned int Color )
{
	Mesh* LineMesh = GetMeshFactory()->CreateDebugLine( Start, End, Color );
	LineMesh->m_IsDebugMesh = true;
	LineMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	LineMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	LineMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( LineMesh );
}

void RendererCommon::DEBUGDrawTriangle( const Vector& V1, const Vector& V2, const Vector& V3, unsigned int Color )
{
	Mesh* TriMesh = GetMeshFactory()->CreateDebugTriangle( V1, V2, V3, Color );
	TriMesh->m_IsDebugMesh = true;
	TriMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	TriMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	TriMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( TriMesh );
}

void RendererCommon::DEBUGDrawBox( const Vector& Min, const Vector& Max, unsigned int Color )
{
	Mesh* BoxMesh = GetMeshFactory()->CreateDebugBox( Min, Max, Color );
	BoxMesh->m_IsDebugMesh = true;
	BoxMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	BoxMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	BoxMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( BoxMesh );
}

void RendererCommon::DEBUGDrawFrustum( const View& rView, unsigned int Color )
{
	Mesh* FrustumMesh = GetMeshFactory()->CreateDebugFrustum( rView, Color );
	FrustumMesh->m_IsDebugMesh = true;
	FrustumMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	FrustumMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	FrustumMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( FrustumMesh );
}

/*virtual*/ void RendererCommon::DEBUGDrawCircleXY( const Vector& Center, float Radius, unsigned int Color )
{
	Mesh* CircleXYMesh = GetMeshFactory()->CreateDebugCircleXY( Center, Radius, Color );
	CircleXYMesh->m_IsDebugMesh = true;
	CircleXYMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	CircleXYMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	CircleXYMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( CircleXYMesh );
}

void RendererCommon::DEBUGDrawSphere( const Vector& Center, float Radius, unsigned int Color )
{
	Mesh* SphereMesh = GetMeshFactory()->CreateDebugSphere( Center, Radius, Color );
	SphereMesh->m_IsDebugMesh = true;
	SphereMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	SphereMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	SphereMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( SphereMesh );
}

void RendererCommon::DEBUGDrawEllipsoid( const Vector& Center, const Vector& Extents, unsigned int Color )
{
	Mesh* EllipsoidMesh = GetMeshFactory()->CreateDebugEllipsoid( Center, Extents, Color );
	EllipsoidMesh->m_IsDebugMesh = true;
	EllipsoidMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	EllipsoidMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	EllipsoidMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( EllipsoidMesh );
}

void RendererCommon::DEBUGDrawCross( const Vector& Center, const float Length, unsigned int Color )
{
	Mesh* CrossMesh = GetMeshFactory()->CreateDebugCross( Center, Length, Color );
	CrossMesh->m_IsDebugMesh = true;
	CrossMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	CrossMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	CrossMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( CrossMesh );
}

void RendererCommon::DEBUGDrawArrow( const Vector& Root, const Angles& Direction, const float Length, unsigned int Color )
{
	Mesh* ArrowMesh = GetMeshFactory()->CreateDebugArrow( Root, Direction, Length, Color );
	ArrowMesh->m_IsDebugMesh = true;
	ArrowMesh->SetMaterialDefinition( DEFAULT_NEWMATERIAL, this );
	ArrowMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	ArrowMesh->SetMaterialFlags( MAT_DEBUG_WORLD );
	AddMesh( ArrowMesh );
}

void RendererCommon::DEBUGDrawLine2D( const Vector& Start, const Vector& End, unsigned int Color )
{
	Mesh* LineMesh = GetMeshFactory()->CreateDebugLine( Start, End, Color );
	LineMesh->m_IsDebugMesh = true;
	LineMesh->SetMaterialDefinition( DEFAULT_NEWMATERIALHUD, this );
	LineMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	LineMesh->SetMaterialFlags( MAT_DEBUG_HUD );
	AddMesh( LineMesh );
}

void RendererCommon::DEBUGDrawBox2D( const Vector& Min, const Vector& Max, unsigned int Color )
{
	Mesh* BoxMesh = GetMeshFactory()->CreateDebugBox( Min, Max, Color );
	BoxMesh->m_IsDebugMesh = true;
	BoxMesh->SetMaterialDefinition( DEFAULT_NEWMATERIALHUD, this );
	BoxMesh->SetTexture( 0, GetTextureManager()->GetTexture( DEFAULT_TEXTURE, TextureManager::ETL_Permanent ) );
	BoxMesh->SetMaterialFlags( MAT_DEBUG_HUD );
	AddMesh( BoxMesh );
}

void RendererCommon::DEBUGPrint( const SimpleString& UTF8String, const Font* const pFont, const SRect& Bounds, const Vector4& Color )
{
	Mesh* const	pPrintMesh		= Print( UTF8String, pFont, Bounds, 0 );
	pPrintMesh->m_IsDebugMesh	= true;
	pPrintMesh->m_ConstantColor	= Color;
	pPrintMesh->SetMaterialFlags( MAT_DEBUG_HUD );
	AddMesh( pPrintMesh );
}

/*virtual*/ void RendererCommon::DEBUGPrint( const SimpleString& UTF8String, const Vector& Location, const View* const pView, const Display* const pDisplay, const SimpleString& FontName, const Vector4& Color, const Vector4& ShadowColor )
{
	DEVASSERT( pView );
	DEVASSERT( pDisplay );

	const Matrix	ViewMatrix			= pView->GetViewMatrix();
	const Matrix	ProjectionMatrix	= pView->GetProjectionMatrix();
	const Matrix	VPMatrix			= ViewMatrix * ProjectionMatrix;
	const Vector4	ProjectedLocation	= Vector4( Location ) * VPMatrix;

	if( ProjectedLocation.z < 0.0f )
	{
		return;
	}

	const float		DisplayWidth		= static_cast<float>( pDisplay->m_Width );
	const float		DisplayHeight		= static_cast<float>( pDisplay->m_Height );
	const Vector4	NormalizedLocation	= ProjectedLocation / ProjectedLocation.w;
	const Vector2	ScaledLocation		= Vector2( DisplayWidth * ( NormalizedLocation.x * 0.5f + 0.5f ), DisplayHeight * ( -NormalizedLocation.y * 0.5f + 0.5f ) );
	const SRect		Rect				= SRect( Floor( ScaledLocation.x ), Floor( ScaledLocation.y ), 0.0f, 0.0f );
	const SRect		ShadowRect			= SRect( Rect.m_Left + 1.0f, Rect.m_Top + 1.0f, 0.0f, 0.0f );
	Font* const		pFont				= GetFontManager()->GetFont( FontName.CStr() );

	DEBUGPrint( UTF8String, pFont, ShadowRect, ShadowColor );
	DEBUGPrint( UTF8String, pFont, Rect, Color );
}
#endif // BUILD_DEV

Mesh* RendererCommon::Print( const SimpleString& UTF8String, const Font* const pFont, const SRect& Bounds, unsigned int Flags )
{
	DEVASSERT( pFont );
	Mesh* StringMesh = pFont->Print( UTF8String, Bounds, Flags );
	StringMesh->SetMaterialFlags( MAT_HUD );
	StringMesh->SetMaterialDefinition( "Material_HUD", this );
	return StringMesh;
}

/*virtual*/ void RendererCommon::Arrange( const SimpleString& UTF8String, const Font* const pFont, const SRect& Bounds, unsigned int Flags, Vector2& OutExtents )
{
	DEVASSERT( pFont );
	Array<STypesetGlyph> UnusedTypesetting;
	pFont->Arrange( UTF8String, Bounds, Flags, UnusedTypesetting, OutExtents );
}

#if BUILD_DEBUG
IRenderer::SDEBUGRenderStats& RendererCommon::DEBUGGetStats()
{
	return m_DEBUGRenderStats;
}
#endif

void RendererCommon::ApplyMaterial( const Material& Material, Mesh* const pMesh, const View& CurrentView )
{
	ExecuteRenderOps( Material.GetRenderOps() );
	SetShaderProgram( Material.GetShaderProgram() );
	ApplyRenderState( Material.GetRenderState() );

	const uint	NumSamplers		= Material.GetNumSamplers();
	uint		SamplerIndex	= 0;
	for( ; SamplerIndex < NumSamplers; ++SamplerIndex )
	{
		ApplySamplerState( SamplerIndex, Material.GetSamplerState( SamplerIndex ) );
	}
	for( ; SamplerIndex < MAX_TEXTURE_STAGES; ++SamplerIndex )
	{
		ResetSamplerState( SamplerIndex );
	}

	DEBUGASSERT( Material.GetSDP() );
	Material.GetSDP()->SetShaderParameters( this, pMesh, CurrentView );

	// Make sure we've got the right vertex streams bound; seems like there should be a better way to automate this but shrug
	DEBUGASSERT( Material.GetExpectedVD() == ( pMesh->m_VertexDeclaration->GetSignature() & Material.GetExpectedVD() ) );
}

// NOTE: This is pretty stubby, since clearing the stencil buffer is the only
// per-material thing I support right now. As I add more, break it out into
// functions like ApplyRenderState.
void RendererCommon::ExecuteRenderOps( const SRenderOps& RenderOps )
{
	uint ClearFlags = CLEAR_NONE;

	if( RenderOps.m_ClearStencil == EE_True )
	{
		ClearFlags |= CLEAR_STENCIL;
	}

	if( ClearFlags != CLEAR_NONE )
	{
		Clear( ClearFlags, 0xff000000, 1.0f, RenderOps.m_ClearStencilValue );
	}
}

void RendererCommon::ApplyRenderState( const SRenderState& RenderState )
{
	SetCullMode(			RenderState.m_CullMode );
	SetColorWriteEnable(	RenderState.m_ColorWriteEnable );
	SetAlphaBlendEnable(	RenderState.m_AlphaBlendEnable );
	SetZEnable(				RenderState.m_ZEnable );
	SetZWriteEnable(		RenderState.m_ZWriteEnable );
	SetStencilEnable(		RenderState.m_StencilEnable );

	if( RenderState.m_AlphaBlendEnable == EE_True )
	{
		SetBlend( RenderState.m_SrcBlend, RenderState.m_DestBlend );
	}

	if( RenderState.m_ZEnable == EE_True )
	{
		SetZFunc( RenderState.m_ZFunc );
	}

	if( RenderState.m_StencilEnable == EE_True )
	{
		SetStencilFunc(			RenderState.m_StencilFunc,		RenderState.m_StencilRef,		RenderState.m_StencilMask );
		SetStencilOp(			RenderState.m_StencilFailOp,	RenderState.m_StencilZFailOp,	RenderState.m_StencilPassOp );
		SetStencilWriteMask(	RenderState.m_StencilWriteMask );
	}
}

void RendererCommon::ApplySamplerState( const uint SamplerStage, const SSamplerState& SamplerState )
{
	DEBUGASSERT( SamplerState.m_Texture );

	if( SamplerState.m_Texture->IsCubemap() )
	{
		SetCubemap(					SamplerStage, SamplerState.m_Texture );
		SetCubemapAddressing(		SamplerStage, SamplerState.m_AddressU, SamplerState.m_AddressV, SamplerState.m_AddressW );
		SetCubemapMinMipFilters(	SamplerStage, SamplerState.m_MinFilter, SamplerState.m_MipFilter );
		SetCubemapMagFilter(		SamplerStage, SamplerState.m_MagFilter );
	}
	else
	{
		SetTexture(			SamplerStage, static_cast<ITexture*>( SamplerState.m_Texture ) );
		SetAddressing(		SamplerStage, SamplerState.m_AddressU, SamplerState.m_AddressV );
		SetMinMipFilters(	SamplerStage, SamplerState.m_MinFilter, SamplerState.m_MipFilter );
		SetMagFilter(		SamplerStage, SamplerState.m_MagFilter );
		SetMaxAnisotropy(	SamplerStage, SamplerState.m_MaxAnisotropy );
	}
}

void RendererCommon::ResetSamplerState( const uint SamplerStage )
{
	ResetTexture( SamplerStage );
}

void RendererCommon::ResetRenderState()
{
	// Reset our shadowed state so we'll update the D3D state properly after reset.
	m_RenderState = SRenderState();
	for( uint SamplerStage = 0; SamplerStage < MAX_TEXTURE_STAGES; ++SamplerStage )
	{
		m_SamplerStates[ SamplerStage ] = SSamplerState();
	}
}