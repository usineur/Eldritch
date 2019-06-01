#include "core.h"
#include "eldgame.h"
#include "wbeventmanager.h"
#include "eldframework.h"
#include "eldworld.h"
#include "wbworld.h"
#include "eldsaveload.h"
#include "eldpersistence.h"
#include "eldbank.h"
#include "eldmusic.h"
#include "configmanager.h"
#include "texturemanager.h"
#include "irenderer.h"
#include "mesh.h"
#include "eldtargetmanager.h"
#include "meshfactory.h"
#include "irendertarget.h"
#include "shadermanager.h"
#include "Common/uimanagercommon.h"
#include "uiscreen.h"
#include "uiwidget.h"
#include "wbscene.h"
#include "Components/wbcompeldhealth.h"
#include "Components/wbcompeldtransform.h"
#include "Components/wbcompeldcamera.h"
#include "Components/wbcompeldplayer.h"
#include "Components/wbcompeldvisible.h"
#include "Screens/uiscreen-eldmirror.h"
#include "Widgets/uiwidget-image.h"
#include "allocator.h"
#include "wbcomponentarrays.h"
#include "ivertexdeclaration.h"

#if BUILD_WINDOWS
#include <Windows.h>	// For ShellExecute
#endif

EldGame::EldGame()
:	m_GoToLevelOnNextTick( false )
,	m_IsRestarting( false )
,	m_FlushHubOnRestart( false )
,	m_RestoreSpawnPoint( false )
,	m_NextLevelName()
,	m_NextWorldDef()
,	m_CurrentLevelName()
,	m_TravelPersistence()
,	m_SaveLoad( NULL )
,	m_GenerationPersistence( NULL )
,	m_Bank( NULL )
,	m_Music( NULL )
,	m_Gamma( 0.0f )
,	m_PostQuad( NULL )
,	m_FXAAQuad( NULL )
,	m_MirrorPostQuad( NULL )
,	m_MirrorFXAAQuad( NULL )
,	m_UpscaleQuad( NULL )
,	m_ColorGradingTexture()
,	m_FogTexture( NULL )
,	m_FogParams()
{
	m_ColorGradingTexture	= DEFAULT_TEXTURE;
	m_SaveLoad				= new EldSaveLoad;
	m_GenerationPersistence	= new EldPersistence;
	m_Bank					= new EldBank;
	m_Music					= new EldMusic;
}

EldGame::~EldGame()
{
	SafeDelete( m_SaveLoad );
	SafeDelete( m_GenerationPersistence );
	SafeDelete( m_Bank );
	SafeDelete( m_Music );
	SafeDelete( m_PostQuad );
	SafeDelete( m_FXAAQuad );
	SafeDelete( m_MirrorPostQuad );
	SafeDelete( m_MirrorFXAAQuad );
	SafeDelete( m_UpscaleQuad );
}

void EldGame::SetCurrentLevelName( const SimpleString& LevelName )
{
	XTRACE_FUNCTION;

	m_CurrentLevelName = LevelName;

	MAKEHASH( m_CurrentLevelName );
	STATICHASH( Name );
	const SimpleString LevelFriendlyName = ConfigManager::GetString( sName, "", sm_CurrentLevelName );

	STATICHASH( HUD );
	STATICHASH( WorldName );
	ConfigManager::SetString( sWorldName, LevelFriendlyName.CStr(), sHUD );
}

/*static*/ TPersistence& EldGame::StaticGetTravelPersistence()
{
	return EldFramework::GetInstance()->GetGame()->GetTravelPersistence();
}

/*virtual*/ void EldGame::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	STATIC_HASHED_STRING( ReturnToHub );
	STATIC_HASHED_STRING( ReturnToAsylumHub );
	STATIC_HASHED_STRING( GoToNextLevel );
	STATIC_HASHED_STRING( GoToPrevLevel );
	STATIC_HASHED_STRING( GoToLevel );
	STATIC_HASHED_STRING( Checkpoint );
	STATIC_HASHED_STRING( TweetRIP );
	STATIC_HASHED_STRING( PlayMusic );
	STATIC_HASHED_STRING( StopMusic );
	STATIC_HASHED_STRING( LaunchWebSite );
	STATIC_HASHED_STRING( OpenUserDataPath );
	STATIC_HASHED_STRING( GoToLevelImmediate );

	const HashedString EventName = Event.GetEventName();
	if( EventName == sReturnToHub )
	{
		STATIC_HASHED_STRING( Restart );
		const bool Restart = Event.GetBool( sRestart );

		STATIC_HASHED_STRING( FlushHub );
		const bool FlushHub = Event.GetBool( sFlushHub );

		RequestReturnToHub( Restart, FlushHub );
	}
	else if( EventName == sReturnToAsylumHub )
	{
		RequestReturnToAsylumHub();
	}
	else if( EventName == sGoToNextLevel )
	{
		RequestGoToNextLevel();
	}
	else if( EventName == sGoToPrevLevel )
	{
		RequestGoToPrevLevel();
	}
	else if( EventName == sGoToLevel )
	{
		STATIC_HASHED_STRING( Level );
		const SimpleString Level = Event.GetString( sLevel );

		STATIC_HASHED_STRING( WorldDef );
		const HashedString WorldDef = Event.GetHash( sWorldDef );

		RequestGoToLevel( Level, WorldDef, true );
	}
	else if( EventName == sCheckpoint )
	{
		Checkpoint();
	}
	else if( EventName == sTweetRIP )
	{
		LaunchRIPTweet();
	}
	else if( EventName == sLaunchWebSite )
	{
		LaunchWebSite();
	}
	else if( EventName == sOpenUserDataPath )
	{
		OpenUserDataPath();
	}
	else if( EventName == sPlayMusic )
	{
		STATIC_HASHED_STRING( Music );
		const SimpleString Music = Event.GetString( sMusic );

		m_Music->PlayMusic( ( Music == "" ) ? m_CurrentMusic : Music );
	}
	else if( EventName == sStopMusic )
	{
		m_Music->StopMusic();
	}
	else if( EventName == sGoToLevelImmediate )
	{
		// This should only be called after we've queued a go-to.
		ASSERT( m_GoToLevelOnNextTick );
		if( m_GoToLevelOnNextTick )
		{
			GoToLevel();
		}

		// HACK: Tick world once to pump the event queue. Fixes title screen bugs. (Hack because this assumes we only ever use this when returning to title screen.)
		EldFramework* const pFramework = EldFramework::GetInstance();
		EldWorld* const pWorld = pFramework->GetWorld();
		pWorld->Tick( 0.0f );
	}
}

void EldGame::Initialize()
{
	if( GetSaveLoad()->TryLoadMaster() )
	{
		// We're good! Don't flush the master file; if the game crashes, we still want it around!
	}
	else
	{
		// There was no master file, or the master file didn't have a world state (was saved
		// when the player was dead, etc.). Create a new world, but don't mess with persistence.

		STATICHASH( EldritchWorld );
		STATICHASH( InitialLevel );
		SetCurrentLevelName( ConfigManager::GetString( sInitialLevel, "", sEldritchWorld ) );

		MAKEHASH( m_CurrentLevelName );

		STATICHASH( WorldDef );
		const HashedString InitialWorldDef = ConfigManager::GetHash( sWorldDef, HashedString::NullString, sm_CurrentLevelName );

		EldFramework* const pFramework = EldFramework::GetInstance();
		EldWorld* const pWorld = pFramework->GetWorld();
		pWorld->SetCurrentWorld( InitialWorldDef );
		pWorld->Create();
		pFramework->InitializeTools();

		RefreshUIReturnToHubEnabled();
	}
}

void EldGame::ShutDown()
{
	PRINTF( "Shutting down game\n" );

	GetSaveLoad()->SaveMaster();
	GetSaveLoad()->FlushWorldFiles();
}

void EldGame::Tick()
{
	XTRACE_FUNCTION;

	// This stuff is done in a tick instead of being event-driven,
	// because it needs to happen before the world tick.

	if( m_GoToLevelOnNextTick )
	{
		GoToLevel();
	}
}

void EldGame::Render() const
{
	XTRACE_FUNCTION;

	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	IRenderer* const		pRenderer		= pFramework->GetRenderer();
	ASSERT( pRenderer );

	pRenderer->AddMesh( GetPostQuad() );
	pRenderer->AddMesh( GetFXAAQuad() );
	pRenderer->AddMesh( GetMirrorPostQuad() );
	pRenderer->AddMesh( GetMirrorFXAAQuad() );
	pRenderer->AddMesh( GetUpscaleQuad() );
}

void EldGame::ClearTravelPersistence()
{
	m_TravelPersistence.Clear();
}

// Restart means we're going to flush all the dungeon world files and travel back to the old hub (e.g., "Return to Hub" from menu).
// FlushHub means we're also flushing the hub, and not traveling (e.g., restarting after death).
// If neither of these is set, then we're traveling back and keeping dungeon world files (e.g., returning after finishing a world).
void EldGame::RequestReturnToHub( const bool Restart, const bool FlushHub )
{
	XTRACE_FUNCTION;

	const SimpleString InitialLevelName = GetHubLevelName();

	MAKEHASH( InitialLevelName );

	STATICHASH( WorldDef );
	const HashedString InitialWorldDef = ConfigManager::GetHash( sWorldDef, HashedString::NullString, sInitialLevelName );

	RequestGoToLevel( InitialLevelName, InitialWorldDef, false );

	if( Restart )
	{
		m_IsRestarting = true;

		if( FlushHub )
		{
			m_FlushHubOnRestart = true;
			ClearTravelPersistence();
		}
	}
}

void EldGame::RequestReturnToAsylumHub()
{
	XTRACE_FUNCTION;

	const SimpleString AsylumLevelName = GetAsylumHubLevelName();

	MAKEHASH( AsylumLevelName );

	STATICHASH( WorldDef );
	const HashedString InitialWorldDef = ConfigManager::GetHash( sWorldDef, HashedString::NullString, sAsylumLevelName );

	RequestGoToLevel( AsylumLevelName, InitialWorldDef, false );

	m_IsRestarting = true;
	m_FlushHubOnRestart = true;
	ClearTravelPersistence();
}

void EldGame::RequestGoToNextLevel()
{
	XTRACE_FUNCTION;

	MAKEHASH( m_CurrentLevelName );

	STATICHASH( NextLevel );
	const SimpleString NextLevelName = ConfigManager::GetString( sNextLevel, "", sm_CurrentLevelName );

	MAKEHASH( NextLevelName );

	STATICHASH( WorldDef );
	const HashedString NextWorldDef = ConfigManager::GetHash( sWorldDef, HashedString::NullString, sNextLevelName );

	RequestGoToLevel( NextLevelName, NextWorldDef, true );
}

void EldGame::RequestGoToPrevLevel()
{
	XTRACE_FUNCTION;

	MAKEHASH( m_CurrentLevelName );

	STATICHASH( PrevLevel );
	const SimpleString PrevLevelName = ConfigManager::GetString( sPrevLevel, "", sm_CurrentLevelName );

	MAKEHASH( PrevLevelName );

	STATICHASH( WorldDef );
	const HashedString PrevWorldDef = ConfigManager::GetHash( sWorldDef, HashedString::NullString, sPrevLevelName );

	RequestGoToLevel( PrevLevelName, PrevWorldDef, false );
}

void EldGame::RequestGoToLevel( const SimpleString& NextLevel, const HashedString& NextWorldDef, const bool RestoreSpawnPoint )
{
	XTRACE_FUNCTION;

	if( NextLevel == "" || NextWorldDef == HashedString::NullString )
	{
		WARN;
		return;
	}

	// Make a checkpoint save, for crash protection
	Checkpoint();

	ASSERT( !m_GoToLevelOnNextTick );
	m_GoToLevelOnNextTick	= true;
	m_RestoreSpawnPoint		= RestoreSpawnPoint;
	m_NextLevelName			= NextLevel;
	m_NextWorldDef			= NextWorldDef;

	WBEventManager* const pEventManager = WBWorld::GetInstance()->GetEventManager();

	WB_MAKE_EVENT( PreLevelTransition, NULL );
	WB_DISPATCH_EVENT( pEventManager, PreLevelTransition, NULL );
}

// NEONNOTE: No longer decorating the world filename with the save/load path,
// because that prevents migration of the saved game to other accounts/systems.
SimpleString EldGame::DecorateWorldFileName( const SimpleString& LevelName ) const
{
	return SimpleString::PrintF( "%s.eldritchworldsave", LevelName.CStr() );
}

void EldGame::Checkpoint() const
{
	XTRACE_FUNCTION;

	if( GetSaveLoad()->ShouldSaveCurrentWorld() )
	{
		GetSaveLoad()->SaveMaster();
	}
}

void EldGame::GoToLevel()
{
	XTRACE_FUNCTION;

	PRINTF( "Traveling to level %s\n", m_NextLevelName.CStr() );

	const bool IsRestarting			= m_IsRestarting;
	const bool FlushHubOnRestart	= m_FlushHubOnRestart;

	if( IsRestarting )
	{
		if( FlushHubOnRestart )
		{
			// Don't save the current world, and flush everything.
			GetSaveLoad()->FlushWorldFiles();
		}
		else
		{
			// Don't save the current world, but wait to flush until we're done traveling.
		}
	}
	else
	{
		// Store a record of the world we're leaving so we can come back to it.
		GetSaveLoad()->SaveWorld( DecorateWorldFileName( m_CurrentLevelName ) );
	}

	m_GoToLevelOnNextTick	= false;
	m_IsRestarting			= false;
	m_FlushHubOnRestart		= false;
	SetCurrentLevelName( m_NextLevelName );

	if( GetSaveLoad()->TryLoadWorld( DecorateWorldFileName( m_NextLevelName ) ) )
	{
		// We're good!
	}
	else
	{
		EldFramework::GetInstance()->GoToLevel( m_NextWorldDef );
	}

	RefreshUIReturnToHubEnabled();

	if( IsRestarting && !FlushHubOnRestart )
	{
		// Flush worlds now that we're back in the library.
		GetSaveLoad()->FlushWorldFiles();
	}

	WBEventManager* const pEventManager = WBWorld::GetInstance()->GetEventManager();

	WB_MAKE_EVENT( PostLevelTransition, NULL );
	WB_SET_AUTO( PostLevelTransition, Bool, RestoreSpawnPoint, m_RestoreSpawnPoint );
	WB_DISPATCH_EVENT( pEventManager, PostLevelTransition, NULL );

	// Top off player's health if we're returning to the hub.
	if( IsInHub() )
	{
		WB_MAKE_EVENT( RestoreHealth, NULL );
		WB_LOG_EVENT( RestoreHealth );
		WB_DISPATCH_EVENT( pEventManager, RestoreHealth, GetPlayer() );
	}

	// Make a checkpoint save, for crash protection
	// (Queued because we want to pump the event queue once to initialize the world.)
	WB_MAKE_EVENT( Checkpoint, NULL );
	WB_LOG_EVENT( Checkpoint );
	WB_QUEUE_EVENT( pEventManager, Checkpoint, this );

	// Clear travel persistence now that we've successfully traveled and don't need it.
	ClearTravelPersistence();

	// And fade up.
	static const float kFadeInDuration = 0.5f;
	WB_MAKE_EVENT( FadeIn, NULL );
	WB_SET_AUTO( FadeIn, Float, Duration, kFadeInDuration );
	WB_DISPATCH_EVENT( pEventManager, FadeIn, NULL );

	PRINTF( "Travel finished\n" );
}

void EldGame::RefreshRTDependentSystems()
{
	XTRACE_FUNCTION;

	CreatePostQuad();
	CreateFXAAQuad();
	CreateMirrorPostQuad();
	CreateMirrorFXAAQuad();
	CreateUpscaleQuad();

	UpdateMirror();
	UpdateMinimap();
}

void EldGame::UpdateMirror()
{
	XTRACE_FUNCTION;

	UIScreenEldMirror* const		pMirrorScreen	= GetMirrorScreen();
	ASSERT( pMirrorScreen );

	pMirrorScreen->OnMirrorRTUpdated();
}

void EldGame::UpdateMinimap()
{
	XTRACE_FUNCTION;

	UIWidgetImage* const pMinimapImage = GetMinimapImage();
	if( !pMinimapImage )
	{
		return;
	}

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();
	ASSERT( pTargetManager );

	STATIC_HASHED_STRING( Minimap );
	IRenderTarget* const			pMinimapRT		= pTargetManager->GetRenderTarget( sMinimap );
	ASSERT( pMinimapRT );

	ITexture* const					pTexture		= pMinimapRT->GetColorTextureHandle( 0 );
	ASSERT( pTexture );

	pMinimapImage->SetTexture( pTexture, 0 );
}

Mesh* EldGame::CreateFullscreenQuad( const uint Width, const uint Height, const HashedString& PrescribedBucket, const SimpleString& MaterialDef )
{
	EldFramework* const	pFramework	= EldFramework::GetInstance();
	IRenderer* const		pRenderer	= pFramework->GetRenderer();

	const float QuadWidth		= static_cast<float>( Width );
	const float QuadHeight		= static_cast<float>( Height );
	const float OffsetWidth		= 0.5f * QuadWidth;
	const float OffsetHeight	= 0.5f * QuadHeight;

	Mesh* const pFSQuadMesh = pRenderer->GetMeshFactory()->CreatePlane( QuadWidth, QuadHeight, 1, 1, XZ_PLANE, false );
	pFSQuadMesh->m_Location = Vector( OffsetWidth, 0.0f, OffsetHeight );
	pFSQuadMesh->SetVertexDeclaration( pRenderer->GetVertexDeclaration( VD_POSITIONS | VD_UVS ) );	// Override what CreatePlane gives us
	pFSQuadMesh->SetBucket( PrescribedBucket );
	pFSQuadMesh->SetMaterialDefinition( MaterialDef, pRenderer );

	return pFSQuadMesh;
}

void EldGame::CreatePostQuad()
{
	XTRACE_FUNCTION;

	SafeDelete( m_PostQuad );

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	Display* const					pDisplay		= pFramework->GetDisplay();
	ASSERT( pDisplay );

	IRenderer* const				pRenderer		= pFramework->GetRenderer();
	ASSERT( pRenderer );

	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();
	ASSERT( pTargetManager );

	m_PostQuad = CreateFullscreenQuad( pDisplay->m_Width, pDisplay->m_Height, "Post", "Material_Post" );
	m_PostQuad->SetTexture( 0, pTargetManager->GetRenderTarget( "Primary" )->GetColorTextureHandle( 0 ) );
	m_PostQuad->SetTexture( 1, pRenderer->GetTextureManager()->GetTextureNoMips( m_ColorGradingTexture.CStr() ) );
}

void EldGame::CreateFXAAQuad()
{
	XTRACE_FUNCTION;

	SafeDelete( m_FXAAQuad );

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	Display* const				pDisplay		= pFramework->GetDisplay();
	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();

	m_FXAAQuad = CreateFullscreenQuad( pDisplay->m_Width, pDisplay->m_Height, "FXAA", "Material_FXAA" );
	m_FXAAQuad->SetTexture( 0, pTargetManager->GetRenderTarget( "Post" )->GetColorTextureHandle( 0 ) );
}

void EldGame::CreateMirrorPostQuad()
{
	XTRACE_FUNCTION;

	SafeDelete( m_MirrorPostQuad );

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	IRenderer* const				pRenderer		= pFramework->GetRenderer();
	ASSERT( pRenderer );

	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();
	ASSERT( pTargetManager );

	UIScreenEldMirror* const	pMirrorScreen	= EldGame::GetMirrorScreen();
	const uint					MirrorRTWidth = pMirrorScreen->GetMirrorRTWidth();
	const uint					MirrorRTHeight = pMirrorScreen->GetMirrorRTHeight();

	m_MirrorPostQuad = CreateFullscreenQuad( MirrorRTWidth, MirrorRTHeight, "MirrorPost", "Material_MirrorPost" );
	m_MirrorPostQuad->SetTexture( 0, pTargetManager->GetRenderTarget( "Mirror" )->GetColorTextureHandle( 0 ) );
}

void EldGame::CreateMirrorFXAAQuad()
{
	XTRACE_FUNCTION;

	SafeDelete( m_MirrorFXAAQuad );

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();

	UIScreenEldMirror* const	pMirrorScreen	= EldGame::GetMirrorScreen();
	const uint					MirrorRTWidth = pMirrorScreen->GetMirrorRTWidth();
	const uint					MirrorRTHeight = pMirrorScreen->GetMirrorRTHeight();

	m_MirrorFXAAQuad = CreateFullscreenQuad( MirrorRTWidth, MirrorRTHeight, "MirrorFXAA", "Material_FXAA" );
	m_MirrorFXAAQuad->SetTexture( 0, pTargetManager->GetRenderTarget( "MirrorFXAA" )->GetColorTextureHandle( 0 ) );
}

void EldGame::CreateUpscaleQuad()
{
	XTRACE_FUNCTION;

	SafeDelete( m_UpscaleQuad );

	EldFramework* const		pFramework		= EldFramework::GetInstance();
	Display* const				pDisplay		= pFramework->GetDisplay();
	EldTargetManager* const	pTargetManager	= pFramework->GetTargetManager();

	// Accomodate different aspect ratios by using the proportional dimensions
	const float					LocationX		= static_cast<float>( ( pDisplay->m_FrameWidth - pDisplay->m_PropWidth ) / 2 );
	const float					LocationY		= static_cast<float>( ( pDisplay->m_FrameHeight - pDisplay->m_PropHeight ) / 2 );

	m_UpscaleQuad = CreateFullscreenQuad( pDisplay->m_PropWidth, pDisplay->m_PropHeight, "Upscale", "Material_Upscale" );
	m_UpscaleQuad->SetTexture( 0, pTargetManager->GetRenderTarget( "Primary" )->GetColorTextureHandle( 0 ) );
	m_UpscaleQuad->m_Location.x += LocationX;
	m_UpscaleQuad->m_Location.z += LocationY;
}

void EldGame::SetColorGradingTexture( const SimpleString& TextureFilename )
{
	XTRACE_FUNCTION;

	m_ColorGradingTexture = TextureFilename;

	if( m_PostQuad )
	{
		EldFramework* const pFramework = EldFramework::GetInstance();
		ASSERT( pFramework );

		TextureManager* const pTextureManager = pFramework->GetRenderer()->GetTextureManager();
		ASSERT( pTextureManager );

		m_PostQuad->SetTexture( 1, pTextureManager->GetTextureNoMips( TextureFilename.CStr() ) );
	}
}

void EldGame::SetFogParams( const float FogNear, const float FogFar, const SimpleString& FogTextureFilename )
{
	XTRACE_FUNCTION;

	EldFramework* const pFramework = EldFramework::GetInstance();
	ASSERT( pFramework );

	TextureManager* const pTextureManager = pFramework->GetRenderer()->GetTextureManager();
	ASSERT( pTextureManager );

	m_FogParams.x	= FogNear;
	m_FogParams.y	= 1.0f / ( FogFar - FogNear );
	m_FogTexture	= pTextureManager->GetTextureNoMips( FogTextureFilename.CStr() );
}

SimpleString EldGame::GetHubLevelName() const
{
	STATICHASH( EldritchWorld );
	STATICHASH( InitialLevel );
	return ConfigManager::GetString( sInitialLevel, "", sEldritchWorld );
}

SimpleString EldGame::GetAsylumHubLevelName() const
{
	STATICHASH( EldritchWorld );
	STATICHASH( AsylumLevel );
	return ConfigManager::GetString( sAsylumLevel, "", sEldritchWorld );
}

bool EldGame::IsInHub() const
{
	return m_CurrentLevelName == GetHubLevelName();
}

void EldGame::SetUIReturnToHubDisabled( const bool Disabled )
{
	STATIC_HASHED_STRING( PauseScreen );
	STATIC_HASHED_STRING( PausedReturnButton );

	WB_MAKE_EVENT( SetWidgetDisabled, NULL );
	WB_SET_AUTO( SetWidgetDisabled, Hash, Screen, sPauseScreen );
	WB_SET_AUTO( SetWidgetDisabled, Hash, Widget, sPausedReturnButton );
	WB_SET_AUTO( SetWidgetDisabled, Bool, Disabled, Disabled );
	WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), SetWidgetDisabled, NULL );
}

void EldGame::RefreshUIReturnToHubEnabled()
{
	SetUIReturnToHubDisabled( IsInHub() );
}

const SimpleString EldGame::GetRIPDamage()
{
	XTRACE_FUNCTION;

	WBEntity* const			pPlayer		= GetPlayer();
	WBCompEldPlayer* const	pPlayerComp	= GET_WBCOMP( pPlayer, EldPlayer );

	const SimpleString DamageDesc = pPlayerComp->GetLastDamageDesc();

	MAKEHASH( DamageDesc );
	const SimpleString RIPDamage = ConfigManager::GetLocalizedString( sDamageDesc, "" );

	return RIPDamage;
}

const SimpleString EldGame::GetRIPLevel()
{
	STATICHASH( EldritchWorld );
	STATICHASH( DefaultRIPName );
	const SimpleString DefaultRIPName = ConfigManager::GetString( sDefaultRIPName, "", sEldritchWorld );

	MAKEHASH( m_CurrentLevelName );
	STATICHASH( RIPName );
	const SimpleString RIPLevelName = ConfigManager::GetString( sRIPName, DefaultRIPName.CStr(), sm_CurrentLevelName );

	MAKEHASH( RIPLevelName );
	const SimpleString RIPLevel = ConfigManager::GetLocalizedString( sRIPLevelName, "" );

	return RIPLevel;
}

// For a purchase link in the demo, if I do that.
void EldGame::LaunchWebSite()
{
	STATICHASH( WebSiteURL );
	const SimpleString WebSiteURL = ConfigManager::GetString( sWebSiteURL, "" );

#if BUILD_WINDOWS
	ShellExecute( NULL, "open", WebSiteURL.CStr(), NULL, NULL, SW_SHOWNORMAL );
#elif BUILD_MAC
	const SimpleString Command = SimpleString::PrintF( "open %s", WebSiteURL.CStr() );
	system( Command.CStr() );
#elif BUILD_LINUX
	const SimpleString Command = SimpleString::PrintF( "xdg-open %s", WebSiteURL.CStr() );
	system( Command.CStr() );
#endif
}

// TODO: Make sure this works in Mac and Linux!
void EldGame::OpenUserDataPath()
{
	EldFramework* const	pFramework		= EldFramework::GetInstance();
	DEVASSERT( pFramework );

	const SimpleString		UserDataPath	= pFramework->GetUserDataPath();

#if BUILD_WINDOWS
	ShellExecute( NULL, "open", UserDataPath.CStr(), NULL, NULL, SW_SHOWNORMAL );
#elif BUILD_MAC
	const SimpleString Command = SimpleString::PrintF( "open %s", UserDataPath.CStr() );
	system( Command.CStr() );
#elif BUILD_LINUX
	const SimpleString Command = SimpleString::PrintF( "xdg-open %s", UserDataPath.CStr() );
	system( Command.CStr() );
#endif
}

void EldGame::LaunchRIPTweet()
{
	STATICHASH( RIPFormat );
	const SimpleString RIPFormat = ConfigManager::GetLocalizedString( sRIPFormat, "" );

	STATICHASH( RIPURL );
	const SimpleString RIPURL = ConfigManager::GetLocalizedString( sRIPURL, "" );

	STATICHASH( RIPHashtags );
	const SimpleString RIPHashtags = ConfigManager::GetLocalizedString( sRIPHashtags, "" );

	STATICHASH( RIPTwitter );
	const SimpleString RIPTwitter = ConfigManager::GetLocalizedString( sRIPTwitter, "" );

	const SimpleString RIPDamage	= GetRIPDamage();
	const SimpleString RIPLevel		= GetRIPLevel();

	const SimpleString RIPFormatted	= SimpleString::PrintF( RIPFormat.CStr(), RIPDamage.CStr(), RIPLevel.CStr() );

	const SimpleString RIPEncoded	= RIPFormatted.URLEncodeUTF8();
	const SimpleString RIPTweet		= SimpleString::PrintF( RIPTwitter.CStr(), RIPEncoded.CStr(), RIPHashtags.CStr(), RIPURL.CStr() );

#if BUILD_WINDOWS
	ShellExecute( NULL, "open", RIPTweet.CStr(), NULL, NULL, SW_SHOWNORMAL );
#elif BUILD_MAC
	const SimpleString Command = SimpleString::PrintF( "open %s", RIPTweet.CStr() );
	system( Command.CStr() );
#elif BUILD_LINUX
	const SimpleString Command = SimpleString::PrintF( "xdg-open %s", RIPTweet.CStr() );
	system( Command.CStr() );
#endif
}

/*static*/ float EldGame::GetPlayTime()
{
	WBEntity* const			pPlayerEntity	= GetPlayer();
	DEVASSERT( pPlayerEntity );

	WBCompEldPlayer* const	pPlayer			= GET_WBCOMP( pPlayerEntity, EldPlayer );
	DEVASSERT( pPlayer );

	return pPlayer->GetPlayTime();
}

/*static*/ WBEntity* EldGame::GetPlayer()
{
	const Array<WBCompEldPlayer*>* const pPlayers = WBComponentArrays::GetComponents<WBCompEldPlayer>();
	if( !pPlayers )
	{
		return NULL;
	}

	const Array<WBCompEldPlayer*>& Players = *pPlayers;
	if( Players.Empty() )
	{
		return NULL;
	}

	WBCompEldPlayer* const pPlayer = Players[0];
	ASSERT( pPlayer );

	return pPlayer->GetEntity();
}

/*static*/ Vector EldGame::GetPlayerLocation()
{
	WBEntity* const pPlayer = GetPlayer();
	ASSERT( pPlayer );

	WBCompEldTransform* const pTransform = pPlayer->GetTransformComponent<WBCompEldTransform>();
	DEVASSERT( pTransform );

	return pTransform->GetLocation();
}

/*static*/ Vector EldGame::GetPlayerViewLocation()
{
	WBEntity* const pPlayer = GetPlayer();
	ASSERT( pPlayer );

	WBCompEldTransform* const pTransform = pPlayer->GetTransformComponent<WBCompEldTransform>();
	DEVASSERT( pTransform );

	WBCompEldCamera* const pCamera = GET_WBCOMP( pPlayer, EldCamera );
	ASSERT( pCamera );

	return pTransform->GetLocation() + pCamera->GetViewTranslationOffset( WBCompEldCamera::EVM_All );
}

/*static*/ Angles EldGame::GetPlayerViewOrientation()
{
	WBEntity* const pPlayer = GetPlayer();
	ASSERT( pPlayer );

	WBCompEldTransform* const pTransform = pPlayer->GetTransformComponent<WBCompEldTransform>();
	DEVASSERT( pTransform );

	WBCompEldCamera* const pCamera = GET_WBCOMP( pPlayer, EldCamera );
	ASSERT( pCamera );

	return pTransform->GetOrientation() + pCamera->GetViewOrientationOffset( WBCompEldCamera::EVM_All );
}

/*static*/ bool EldGame::IsPlayerAlive()
{
	WBEntity* const pPlayer = GetPlayer();
	if( !pPlayer )
	{
		return false;
	}

	WBCompEldHealth* const pHealth = GET_WBCOMP( pPlayer, EldHealth );
	ASSERT( pHealth );
	if( pHealth->IsDead() )
	{
		return false;
	}

	return true;
}

/*static*/ bool EldGame::IsPlayerDisablingPause()
{
	WBEntity* const pPlayer = GetPlayer();
	if( !pPlayer )
	{
		return false;
	}

	WBCompEldPlayer* const pPlayerComponent = GET_WBCOMP( pPlayer, EldPlayer );
	ASSERT( pPlayerComponent );

	return pPlayerComponent->IsDisablingPause();
}

/*static*/ bool EldGame::IsPlayerVisible()
{
	WBEntity* const pPlayer = GetPlayer();
	if( !pPlayer )
	{
		return false;
	}

	WBCompEldVisible* const pVisible = GET_WBCOMP( pPlayer, EldVisible );
	ASSERT( pVisible );

	return pVisible->IsVisible();
}

/*static*/ UIScreenEldMirror* EldGame::GetMirrorScreen()
{
	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	UIManager* const			pUIManager		= pFramework->GetUIManager();
	ASSERT( pUIManager );

	STATIC_HASHED_STRING( MirrorScreen );
	UIScreenEldMirror* const	pMirrorScreen	= pUIManager->GetScreen<UIScreenEldMirror>( sMirrorScreen );
	ASSERT( pMirrorScreen );

	return pMirrorScreen;
}

/*static*/ UIWidgetImage* EldGame::GetMinimapImage()
{
	EldFramework* const	pFramework		= EldFramework::GetInstance();
	ASSERT( pFramework );

	UIManager* const			pUIManager		= pFramework->GetUIManager();
	ASSERT( pUIManager );

	STATIC_HASHED_STRING( HUD );
	UIScreen* const				pHUDScreen		= pUIManager->GetScreen( sHUD );
	if( !pHUDScreen )
	{
		return NULL;
	}

	STATIC_HASHED_STRING( Minimap );
	UIWidgetImage* const		pMinimap		= pHUDScreen->GetWidget<UIWidgetImage>( sMinimap );
	return pMinimap;
}