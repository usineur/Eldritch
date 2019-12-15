#include "core.h"

#include "eldframework.h"

#if BUILD_WINDOWS
#include "../resource.h"
#endif

#if BUILD_MAC
#include <CoreFoundation/CoreFoundation.h>
#include "objcjunk.h"
#endif

#if BUILD_LINUX
#include <stdlib.h>
#endif

#include "array.h"
#include "vector.h"
#include "view.h"
#include "bucket.h"
#include "angles.h"
#include "keyboard.h"
#include "mouse.h"
#include "xinputcontroller.h"
#include "eldgame.h"
#include "eldworld.h"
#include "eldsaveload.h"
#include "eldbank.h"
#include "eldpersistence.h"
#include "material.h"
#include "wbevent.h"
#include "wbeventmanager.h"
#include "reversehash.h"
#include "clock.h"
#include "frameworkutil.h"
#include "packstream.h"
#include "configmanager.h"
#include "filestream.h"
#include "audio.h"
#include "iaudiosystem.h"
#include "eldmesh.h"
#include "dynamicmeshmanager.h"
#include "inputsystem.h"
#include "fileutil.h"
#include "windowwrapper.h"
#include "irenderer.h"
#include "irendertarget.h"
#include "eldsound3dlistener.h"
#include "eldparticles.h"
#include "uiinputmapeld.h"
#include "eldtargetmanager.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "wbcomponentarrays.h"
#include "Achievements/achievementmanager_steam.h"
#include "Leaderboards/leaderboards_steam.h"
#include "stringmanager.h"

#include "eldtools.h"

#include "sdpfactory.h"
#include "eldsdps.h"

#include "Common/uimanagercommon.h"
#include "uistack.h"

#include "uifactory.h"
#include "uiscreens.h"
#include "elduiscreens.h"

#include "wbentity.h"
#include "wbcomponent.h"
#include "rodinwbcomponents.h"
#include "eldwbcomponents.h"

#include "wbparamevaluatorfactory.h"
#include "rodinwbpes.h"
#include "eldwbpes.h"

#include "wbactionfactory.h"
#include "uiwbactions.h"
#include "rodinwbactions.h"
#include "eldwbactions.h"

#include "rodinbtnodefactory.h"
#include "eldrodinbtnodes.h"

#include "animeventfactory.h"
#include "eldanimevents.h"

#include "wbactionstack.h"

#if BUILD_WINDOWS_NO_SDL
#include "D3D9/d3d9renderer.h"
#endif

#if BUILD_STEAM
extern "C" void __cdecl SteamAPIDebugTextHook( int Severity, const char* DebugText )
{
	Unused( Severity );	// 0 = message, 1 = warning

	if( !DebugText )
	{
		return;
	}

	PRINTF( "[STEAM]: %s", DebugText );
}
#endif // BUILD_STEAM

// Singleton accessor
static EldFramework*	gSingletonFramework = NULL;

/*static*/ EldFramework* EldFramework::GetInstance()
{
	return gSingletonFramework;
}

/*static*/ void EldFramework::SetInstance( EldFramework* const pFramework )
{
	ASSERT( gSingletonFramework == NULL );
	gSingletonFramework = pFramework;
}

EldFramework::EldFramework()
:	m_Game( NULL )
,	m_World( NULL )
#if BUILD_DEV
,	m_Tools( NULL )
#endif
,	m_Controller( NULL )
,	m_InputSystem( NULL )
,	m_DisplayWidth( 0 )
,	m_DisplayHeight( 0 )
,	m_TargetManager( NULL )
,	m_MainView( NULL )
,	m_FGView( NULL )
,	m_HUDView( NULL )
,	m_MirrorView( NULL )
,	m_MirrorBView( NULL )
,	m_MinimapView( NULL )
,	m_UpscaleView( NULL )
,	m_Audio3DListener( NULL )
,	m_AchievementManager( NULL )
,	m_Leaderboards( NULL )
,	m_SimTickHasRequestedRenderTick( false )
,	m_PauseOnLostFocus( false )
,	m_MuteWhenUnfocused( false )
{
	EldFramework::SetInstance( this );
	Framework3D::SetInstance( this );
}

EldFramework::~EldFramework()
{
	ASSERT( gSingletonFramework == this );
	gSingletonFramework = NULL;
}

/*virtual*/ void EldFramework::GetInitialWindowTitle( SimpleString& WindowTitle )
{
	STATICHASH( WindowTitle );
	WindowTitle = ConfigManager::GetLocalizedString( sWindowTitle, "Eld" );

	STATICHASH( Version );
	WindowTitle += SimpleString::PrintF( " (%s)", StringManager::ParseConfigString( ConfigManager::GetLocalizedString( sVersion, "" ) ).CStr() );

#if BUILD_WINDOWS_NO_SDL
	// HACKHACK because renderer doesn't actually exist yet
	STATICHASH( OpenGL );
	const bool OpenGL = ConfigManager::GetBool( sOpenGL );
	WindowTitle += OpenGL ? " [OpenGL 2.1]" : " [Direct3D 9]";
#endif

#if BUILD_WINDOWS
	const bool Is32Bit = sizeof( void* ) == sizeof( int );
	WindowTitle += Is32Bit ? " [32-bit]" : " [64-bit]";
#endif
}

/*virtual*/ void EldFramework::GetInitialWindowIcon( uint& WindowIcon )
{
#if BUILD_WINDOWS
	WindowIcon = IDI_ICON1;
#else
	Unused( WindowIcon );
#endif
}

/*virtual*/ void EldFramework::GetUIManagerDefinitionName( SimpleString& DefinitionName )
{
	DefinitionName = "EldritchUI";
}

/*virtual*/ void EldFramework::InitializeUIInputMap()
{
	m_UIInputMap = new UIInputMapEld( this );
}

void EldFramework::InitializePackages()
{
	STATICHASH( NumPackages );
	const uint NumPackages = ConfigManager::GetInt( sNumPackages );
	for( uint PackageIndex = 0; PackageIndex < NumPackages; ++PackageIndex )
	{
		const SimpleString PackageName = ConfigManager::GetSequenceString( "Package%d", PackageIndex, "" );

		// Don't preempt the base package, there should be no conflicts.
		const bool PreemptExistingPackages = false;
		PackStream::StaticAddPackageFile( PackageName.CStr(), PreemptExistingPackages );
	}
}

void EldFramework::InitializeDLC()
{
	STATICHASH( NumDLC );
	const uint NumDLC = ConfigManager::GetInt( sNumDLC );
	for( uint DLCIndex = 0; DLCIndex < NumDLC; ++DLCIndex )
	{
		const SimpleString DLCName = ConfigManager::GetSequenceString( "DLC%d", DLCIndex, "" );

		MAKEHASH( DLCName );

		STATICHASH( PackageFilename );
		const SimpleString PackageFilename = ConfigManager::GetString( sPackageFilename, "", sDLCName );

		// DLC will always preempt base content (so it can be used for patching as well as DLC per se).
		const bool PreemptExistingPackages = true;
		PackStream::StaticAddPackageFile( PackageFilename.CStr(), PreemptExistingPackages );

		// Load config files for DLC, if DLC was successfully loaded.
		// (We can't check a return value from StaticAddPackageFile, because I won't have package
		// files during development but it still needs to load loose DLC files.)
		STATICHASH( NumConfigFiles );
		const uint NumConfigFiles = ConfigManager::GetInt( sNumConfigFiles, 0, sDLCName );
		for( uint ConfigFileIndex = 0; ConfigFileIndex < NumConfigFiles; ++ConfigFileIndex )
		{
			const SimpleString ConfigFile = ConfigManager::GetSequenceString( "ConfigFile%d", ConfigFileIndex, "", sDLCName );
			if( PackStream::StaticFileExists( ConfigFile.CStr() ) )
			{
				ConfigManager::Load( PackStream( ConfigFile.CStr() ) );
			}
		}
	}
}

void EldFramework::InitializePackagesAndConfig()
{
	// HACKHACK: Hard-coded base package file!
	PackStream::StaticAddPackageFile( "eldritch-base.cpk" );
	FrameworkUtil::MinimalLoadConfigFiles( "Config/default.ccf" );

	// HACKHACK: Initialize Steam as soon as possible after loading default config.
	// This way, we can query SteamUGC while loading mods.
#if BUILD_STEAM
	STATICHASH( LoadSteam );
	const bool LoadSteam = ConfigManager::GetBool( sLoadSteam );
	if( LoadSteam )
	{
		PRINTF( "Initializing Steam API.\n" );
		const bool SteamInited = SteamAPI_Init();
		if( SteamInited )
		{
			PRINTF( "Steam API initialized successfully.\n" );
		}
		else
		{
			PRINTF( "Steam API failed to initialize. Steamworks functionality will be disabled.\n" );
		}

		if( SteamClient() )
		{
			SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );
		}

		if( SteamUtils() )
		{
			SteamUtils()->SetOverlayNotificationPosition( k_EPositionBottomRight );
		}
	}
	else
	{
		PRINTF( "Running Steam build without Steam API. Is this intended?\n" );
	}
#endif

	InitializePackages();
	InitializeDLC();

	// Load prefs over everything else.
	LoadPrefsConfig();

	// Push defaults to uninitialized options after loading prefs
	PushDefaultOptions();
}

void EldFramework::PushDefaultOptions()
{
	// Anything that is undefined due to a missing prefs.cfg will be initialized from the default contexts
	// Note that the input device sections are ignore if context exists; this is so that an unbound input
	// does not get stomped by the default.
	//																IgnoreIfContextExists	ResetTargetContext	Overwrite	Target
	STATICHASH( Defaults_Game );
	ConfigManager::PushDefaultsToContext( sDefaults_Game,			false,					false,				false );

	STATICHASH( Defaults_Framework );
	STATICHASH( Framework );
	ConfigManager::PushDefaultsToContext( sDefaults_Framework,		false,					false,				false,		sFramework );

	STATICHASH( Defaults_Controls );
	ConfigManager::PushDefaultsToContext( sDefaults_Controls,		false,					false,				false );

	STATICHASH( Defaults_EldInput );
	STATICHASH( EldInput );
	ConfigManager::PushDefaultsToContext( sDefaults_EldInput,		false,					false,				false,		sEldInput );

	STATICHASH( Defaults_EldKeyboard );
	STATICHASH( EldKeyboard );
	ConfigManager::PushDefaultsToContext( sDefaults_EldKeyboard,	true,					false,				false,		sEldKeyboard );

	STATICHASH( Defaults_EldMouse );
	STATICHASH( EldMouse );
	ConfigManager::PushDefaultsToContext( sDefaults_EldMouse,		true,					false,				false,		sEldMouse );

	STATICHASH( Defaults_EldController );
	STATICHASH( EldController );
	ConfigManager::PushDefaultsToContext( sDefaults_EldController,	true,					false,				false,		sEldController );

	STATICHASH( Defaults_Display );
	ConfigManager::PushDefaultsToContext( sDefaults_Display,		false,					false,				false );

	STATICHASH( Defaults_Graphics );
	ConfigManager::PushDefaultsToContext( sDefaults_Graphics,		false,					false,				false );

	STATICHASH( Defaults_Audio );
	ConfigManager::PushDefaultsToContext( sDefaults_Audio,			false,					false,				false );
}

#if BUILD_STEAM
ENotificationPosition GetSteamNoticePos( const HashedString& SteamNoticePosName )
{
#define TRYPOS( pos ) STATIC_HASHED_STRING( pos ); if( SteamNoticePosName == s##pos ) { return k_EPosition##pos; }
	TRYPOS( TopLeft );
	TRYPOS( TopRight );
	TRYPOS( BottomLeft );
	TRYPOS( BottomRight );
#undef TRYPOS

	return k_EPositionTopRight;
}
#endif

/*virtual*/ void EldFramework::Initialize()
{
	XTRACE_FUNCTION;

	XTRACE_BEGIN( PreFramework3D );
		ReverseHash::Initialize();

		PRINTF( "Checking user data path...\n" );
		const SimpleString UserDataPath = GetUserDataPath();
		PRINTF( "  %s\n", UserDataPath.CStr() );
		if( !FileUtil::PathExists( UserDataPath.CStr() ) )
		{
			PRINTF( "Creating user data game path...\n" );
			FileUtil::MakePath( UserDataPath.CStr() );

			if( !FileUtil::PathExists( UserDataPath.CStr() ) )
			{
				PRINTF( "Could not create user data game path!\n" );
			}
			else
			{
				PRINTF( "  ...done.\n" );
			}
		}

		InitializePackagesAndConfig();

		LOADPRINTLEVELS;

		// This should just be able to go away, it didn't exist in Eldritch
		//// Load save index file if it exists
		//const SimpleString SaveIndexFile = EldSaveLoad::GetSaveIndexFile();
		//if( FileUtil::Exists( SaveIndexFile.CStr() ) )
		//{
		//	ConfigManager::Load( FileStream( SaveIndexFile.CStr(), FileStream::EFM_Read ) );
		//}

		// LEGACY
		STATICHASH( ContentSyncer );
		STATICHASH( Version );
		SimpleString LocalVersion = ConfigManager::GetString( sVersion, "", sContentSyncer );
		PRINTF( "Version: %s\n", LocalVersion.CStr() );

		XTRACE_BEGIN( InitializeFactories );
			PRINTF( "Initializing factories...\n" );

			PRINTF( "Initializing SDP factories.\n" );
			SDPFactory::InitializeBaseFactories();
#define ADDSDPFACTORY( type ) SDPFactory::RegisterSDPFactory( #type, SDP##type::Factory )
#include "eldsdps.h"
#undef ADDSDPFACTORY

			PRINTF( "Initializing UI factories.\n" );
			UIFactory::InitializeBaseFactories();
#define ADDUISCREENFACTORY( type ) UIFactory::RegisterUIScreenFactory( #type, UIScreen##type::Factory )
#include "elduiscreens.h"
#undef ADDUISCREENFACTORY

			PRINTF( "Initializing anim event factories.\n" );
#define ADDANIMEVENTFACTORY( type ) AnimEventFactory::GetInstance()->Register( #type, AnimEvent##type::Factory )
#include "eldanimevents.h"
#undef ADDANIMEVENTFACTORY

			PRINTF( "Initializing PE factories.\n" );
			WBParamEvaluatorFactory::InitializeBaseFactories();
#define ADDWBPEFACTORY( type ) WBParamEvaluatorFactory::RegisterFactory( #type, WBPE##type::Factory )
#include "rodinwbpes.h"
#include "eldwbpes.h"
#undef ADDWBPEFACTORY

			PRINTF( "Initializing action factories.\n" );
			WBActionFactory::InitializeBaseFactories();
#define ADDWBACTIONFACTORY( type ) WBActionFactory::RegisterFactory( #type, WBAction##type::Factory )
#include "uiwbactions.h"
#include "rodinwbactions.h"
#include "eldwbactions.h"
#undef ADDWBPEFACTORY

			PRINTF( "Initializing BT factories.\n" );
			RodinBTNodeFactory::InitializeBaseFactories();
#define ADDRODINBTNODEFACTORY( type ) RodinBTNodeFactory::RegisterFactory( #type, RodinBTNode##type::Factory )
#include "eldrodinbtnodes.h"
#undef ADDRODINBTNODEFACTORY

			// Initialize core and Eld Workbench component factories.
			PRINTF( "Initializing component factories.\n" );
			WBComponent::InitializeBaseFactories();
#define ADDWBCOMPONENT( type ) WBComponent::RegisterWBCompFactory( #type, WBComp##type::Factory )
#include "rodinwbcomponents.h"
#include "eldwbcomponents.h"
#undef ADDWBCOMPONENT
		XTRACE_END;

		PRINTF( "Factories initialized.\n" );

		// Create input system before framework so it will exist for UI. But don't attach devices yet, as they don't exist.
		PRINTF( "Initializing input system.\n" );
		m_InputSystem = new InputSystem;
		m_InputSystem->Initialize( "EldritchInput" );
	XTRACE_END;

	Framework3D::Initialize();

	m_InputSystem->SetRelativeFrameTime( m_FixedFrameTime );

	STATICHASH( DisplayWidth );
	STATICHASH( DisplayHeight );
	m_DisplayWidth	= ConfigManager::GetInt( sDisplayWidth );
	m_DisplayHeight	= ConfigManager::GetInt( sDisplayHeight );

	STATICHASH( PauseOnLostFocus );
	m_PauseOnLostFocus = ConfigManager::GetBool( sPauseOnLostFocus );

	STATICHASH( MuteWhenUnfocused );
	m_MuteWhenUnfocused = ConfigManager::GetBool( sMuteWhenUnfocused );

#if USE_DIRECTINPUT
	m_Controller = new XInputController( m_Window->GetHWnd() );
#else
	m_Controller = new XInputController;
#endif

	m_TargetManager = new EldTargetManager( m_Renderer );
	m_TargetManager->CreateTargets( m_Display->m_Width, m_Display->m_Height );

	m_Audio3DListener = new EldSound3DListener;
	m_Audio3DListener->Initialize();

	ASSERT( m_AudioSystem );
	m_AudioSystem->Set3DListener( m_Audio3DListener );

	m_AchievementManager	= new AchievementManager_Steam;
	m_Leaderboards			= new Leaderboards_Steam;

	// HACKHACK: I only ever intend to have the two leaderboards in Eld, so just do this.
	// For a game with more, I'd want a list defined in data somewhere.
	m_Leaderboards->FindLeaderboard( "LB_PlayTime" );
	m_Leaderboards->FindLeaderboard( "LB_Souls" );

	PublishDisplayedBrightness();

	STATICHASH( FOV );
	const float FOV = ConfigManager::GetFloat( sFOV, 90.0f );
	PublishDisplayedFOV();

	STATICHASH( ForegroundFOV );
	const float FGFOV = ConfigManager::GetFloat( sForegroundFOV, 60.0f );

	STATICHASH( NearClip );
	const float NearClip = ConfigManager::GetFloat( sNearClip, 0.1f );

	STATICHASH( FarClip );
	const float FarClip = ConfigManager::GetFloat( sFarClip, 0.1f );

	const float fDisplayWidth = static_cast<float>( m_DisplayWidth );
	const float fDisplayHeight = static_cast<float>( m_DisplayHeight );
	const float AspectRatio = fDisplayWidth / fDisplayHeight;

	m_MainView		= new View( Vector(), Angles(), FOV,	AspectRatio,	NearClip, FarClip );
	m_FGView		= new View( Vector(), Angles(), FGFOV,	AspectRatio,	NearClip, FarClip );
	CreateHUDView();
	CreateMirrorView();
	CreateMinimapView();

	CreateBuckets();

	m_InputSystem->SetKeyboard( m_Keyboard );
	m_InputSystem->SetMouse( m_Mouse );
	m_InputSystem->SetController( m_Controller );
	m_InputSystem->SetClock( m_Clock );

	WBActionStack::Initialize();

	PRINTF( "Checking saved game path\n" );
	const SimpleString SaveLoadPath = GetSaveLoadPath();
	if( !FileUtil::PathExists( SaveLoadPath.CStr() ) )
	{
		PRINTF( "Creating saved game path\n" );
		FileUtil::MakePath( SaveLoadPath.CStr() );
	}

	PRINTF( "Initializing game.\n" );

	InitializeWorld( HashedString::NullString, false );

	m_Game = new EldGame;
	m_Game->RefreshRTDependentSystems();
	m_Game->Initialize();

	// Initialize config stuff
	OnHideHUDChanged();

	// ELDTODO: Move this into input system init?
	OnInvertYChanged();
	{
		STATICHASH( ControllerPower );
		const float ControllerPower = ConfigManager::GetFloat( sControllerPower );

		STATIC_HASHED_STRING( MoveX );
		m_InputSystem->SetControllerPower( sMoveX, ControllerPower );
		STATIC_HASHED_STRING( MoveY );
		m_InputSystem->SetControllerPower( sMoveY, ControllerPower );
		STATIC_HASHED_STRING( TurnX );
		m_InputSystem->SetControllerPower( sTurnX, ControllerPower );
		STATIC_HASHED_STRING( TurnY );
		m_InputSystem->SetControllerPower( sTurnY, ControllerPower );
	}

	// Initialize UI sliders. This could be neater.
	// This also pushes the initial values to their respective systems, which is pret-ty terrible design.
	m_UIManager->SetSliderValue( "ControlsOptionsScreen",	"MouseSpeedSlider",			GetSliderValueFromMouseSpeed(		ConfigManager::GetFloat( "MouseSpeed",		1.0f ) ) );
	m_UIManager->SetSliderValue( "ControlsOptionsScreen",	"ControllerSpeedSlider",	GetSliderValueFromControllerSpeed(	ConfigManager::GetFloat( "ControllerSpeed",	1.0f ) ) );
	m_UIManager->SetSliderValue( "DisplayOptionsScreen",	"BrightnessSlider",			GetSliderValueFromBrightness(		ConfigManager::GetFloat( "Brightness",		1.0f ) ) );
	m_UIManager->SetSliderValue( "DisplayOptionsScreen",	"FOVSlider",				GetSliderValueFromFOV( FOV ) );	// LEGACY
	m_UIManager->SetSliderValue( "GraphicsOptionsScreen",	"FOVSlider",				GetSliderValueFromFOV( FOV ) );
	m_UIManager->SetSliderValue( "AudioOptionsScreen",		"VolumeSlider",				ConfigManager::GetFloat( "MasterVolume" ) );
	m_UIManager->SetSliderValue( "AudioOptionsScreen",		"MusicVolumeSlider",		ConfigManager::GetFloat( "MusicVolume" ) );

	// For legacy reasons, this is still currently on its own screen
	m_UIManager->SetSliderValue( "BrightnessScreen",		"BrightnessSlider",			GetSliderValueFromBrightness(		ConfigManager::GetFloat( "Brightness",		1.0f ) ) );

	// Initialize UI callbacks
	{
		UIScreenEldSetRes* pSetRes = m_UIManager->GetScreen<UIScreenEldSetRes>( "SetResScreen" );
		pSetRes->SetUICallback( SUICallback( EldFramework::OnSetRes, NULL ) );

		UIScreenFade* pFade = m_UIManager->GetScreen<UIScreenFade>( "Fade" );
		pFade->SetFadeCallback( SUICallback( EldFramework::OnFadeFinished, NULL ) );
	}

	// From original Eldritch framework... none of this exists in current projects
	{
		WB_MAKE_EVENT(ResetToInitialScreens, NULL);
		WB_DISPATCH_EVENT(WBWorld::GetInstance()->GetEventManager(), ResetToInitialScreens, NULL);

		{
			// HACK: Too content aware
			STATIC_HASHED_STRING(MKGLogoScreen);
			WB_MAKE_EVENT(PushUIScreen, NULL);
			WB_SET_AUTO(PushUIScreen, Hash, Screen, sMKGLogoScreen);
			WB_DISPATCH_EVENT(WBWorld::GetInstance()->GetEventManager(), PushUIScreen, NULL);
		}

		// Tick world once to pump the event queue. Fixes title screen bugs.
		m_World->Tick( 0.0f );
	}

	// All done, show the window finally.
#if !BUILD_SWITCH
	SafeDelete( m_SplashWindow );
#endif
#if BUILD_WINDOWS_NO_SDL
	m_Window->Show( m_CmdShow );
#elif BUILD_SDL
	m_Window->Show();
#endif

	// Reattach GL context if needed.
	m_Renderer->Refresh();

	// Add a unique player stat
	//SET_STAT( "NumPlayers", 1 );

	PRINTF( "Eld initialization complete.\n" );
}

/*static*/ void EldFramework::OnSetRes( void* pUIElement, void* pVoid )
{
	Unused( pVoid );

	EldFramework* const		pFramework	= EldFramework::GetInstance();
	Display* const				pDisplay	= pFramework->GetDisplay();
	UIWidget* const				pWidget		= static_cast<UIWidget*>( pUIElement );
	UIScreenEldSetRes* const	pSetRes		= pFramework->GetUIManager()->GetScreen<UIScreenEldSetRes>( "SetResScreen" );
	const SDisplayMode			ChosenRes	= pSetRes->GetRes( pWidget->m_Name );

	if( !pDisplay->m_Fullscreen && ( ChosenRes.Width > pDisplay->m_ScreenWidth || ChosenRes.Height > pDisplay->m_ScreenHeight ) )
	{
		WARNDESC( "Mode too large for screen." );
	}
	else
	{
		pFramework->SetResolution( ChosenRes.Width, ChosenRes.Height );
	}
}

/*static*/ void EldFramework::OnFadeFinished( void* pUIElement, void* pVoid )
{
	Unused( pVoid );
	Unused( pUIElement );

	// HACKHACK: Handle logo fade sequence here because event manager isn't meant for timed UI stuff.
	enum EIntroFadePhases
	{
		ELFP_FadingInLogo,
		ELFP_WaitingLogo,
		ELFP_FadingOutLogo,
		ELFP_Waiting,
		ELFP_Finished,
	};

	WBEventManager* const pEventManager = WBWorld::GetInstance()->GetEventManager();
	ASSERT( pEventManager );

	static EIntroFadePhases sPhase = ELFP_FadingInLogo;
	if( sPhase == ELFP_FadingInLogo )
	{
		DEBUGPRINTF( "Faded in\n" );
		sPhase = ELFP_WaitingLogo;

		static const float kWaitDuration = 0.75f;
		WB_MAKE_EVENT( Fade, NULL );
		WB_SET_AUTO( Fade, Float, Duration, kWaitDuration );
		WB_SET_AUTO( Fade, Float, FadeColorAStart, 0.0f );
		WB_SET_AUTO( Fade, Float, FadeColorAEnd, 0.0f );
		WB_DISPATCH_EVENT( pEventManager, Fade, NULL );
	}
	else if( sPhase == ELFP_WaitingLogo )
	{
		DEBUGPRINTF( "Waited\n" );
		sPhase = ELFP_FadingOutLogo;

		static const float kFadeOutDuration = 0.5f;
		WB_MAKE_EVENT( FadeOut, NULL );
		WB_SET_AUTO( FadeOut, Float, Duration, kFadeOutDuration );
		WB_DISPATCH_EVENT( pEventManager, FadeOut, NULL );
	}
	else if( sPhase == ELFP_FadingOutLogo )
	{
		DEBUGPRINTF( "Faded out\n" );
		sPhase = ELFP_Waiting;

		static const float kWaitDuration = 0.25f;
		WB_MAKE_EVENT( Fade, NULL );
		WB_SET_AUTO( Fade, Float, Duration, kWaitDuration );
		WB_SET_AUTO( Fade, Float, FadeColorAStart, 1.0f );
		WB_SET_AUTO( Fade, Float, FadeColorAEnd, 1.0f );
		WB_DISPATCH_EVENT( pEventManager, Fade, NULL );
	}
	else if( sPhase == ELFP_Waiting )
	{
		DEBUGPRINTF( "Waited\n" );
		sPhase = ELFP_Finished;

		WB_MAKE_EVENT( PopUIScreen, NULL );
		WB_DISPATCH_EVENT( pEventManager, PopUIScreen, NULL );

		static const float kFadeInDuration = 0.5f;
		WB_MAKE_EVENT( FadeIn, NULL );
		WB_SET_AUTO( FadeIn, Float, Duration, kFadeInDuration );
		WB_DISPATCH_EVENT( pEventManager, FadeIn, NULL );
	}
	else
	{
		// Do nothing, intro has elapsed.
	}
}

/*virtual*/ void EldFramework::InitializeAudioSystem()
{
	m_AudioSystem = CreateSoLoudAudioSystem();
}

// Also used to reinitialize world.
void EldFramework::InitializeWorld( const HashedString& WorldDef, const bool CreateWorld )
{
	XTRACE_FUNCTION;

	PrepareForLoad();

	ShutDownWorld();

	WBWorld::CreateInstance();
	WBWorld::GetInstance()->SetClock( GetClock() );

	RegisterForEvents();

	m_World = new EldWorld;
	m_World->SetCurrentWorld( WorldDef );
	m_World->Initialize();

	m_Audio3DListener->SetWorld( m_World );

	m_UIManager->RegisterForEvents();

	if( m_Game )
	{
		m_Game->GetBank()->RegisterForEvents();
		m_Game->GetPersistence()->RegisterForEvents();
	}

	if( CreateWorld )
	{
		m_World->Create();
		InitializeTools();
	}
}

void EldFramework::RegisterForEvents()
{
	STATIC_HASHED_STRING( QuitGame );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sQuitGame, this, NULL );

	STATIC_HASHED_STRING( ToggleInvertY );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleInvertY, this, NULL );

	STATIC_HASHED_STRING( ToggleHideHUD );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleHideHUD, this, NULL );

	STATIC_HASHED_STRING( ToggleFullscreen );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleFullscreen, this, NULL );

	STATIC_HASHED_STRING( ToggleVSync );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleVSync, this, NULL );

	STATIC_HASHED_STRING( ToggleFixedDT );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleFixedDT, this, NULL );

	STATIC_HASHED_STRING( ToggleFXAA );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sToggleFXAA, this, NULL );

	STATIC_HASHED_STRING( ResetGameOptions );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetGameOptions, this, NULL );

	STATIC_HASHED_STRING( ResetControlsOptions );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetControlsOptions, this, NULL );

	STATIC_HASHED_STRING( ResetInputBindings );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetInputBindings, this, NULL );

	STATIC_HASHED_STRING( ResetDisplayOptions );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetDisplayOptions, this, NULL );

	STATIC_HASHED_STRING( ResetGraphicsOptions );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetGraphicsOptions, this, NULL );

	STATIC_HASHED_STRING( ResetAudioOptions );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sResetAudioOptions, this, NULL );

	STATIC_HASHED_STRING( OnSliderChanged );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sOnSliderChanged, this, NULL );

	STATIC_HASHED_STRING( WritePrefsConfig );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sWritePrefsConfig, this, NULL );
}

/*virtual*/ void EldFramework::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	Framework3D::HandleEvent( Event );

	STATIC_HASHED_STRING( ToggleInvertY );
	STATIC_HASHED_STRING( ToggleHideHUD );
	STATIC_HASHED_STRING( ToggleFullscreen );
	STATIC_HASHED_STRING( ToggleVSync );
	STATIC_HASHED_STRING( ToggleFixedDT );
	STATIC_HASHED_STRING( ToggleFXAA );
	STATIC_HASHED_STRING( ResetGameOptions );
	STATIC_HASHED_STRING( ResetControlsOptions );
	STATIC_HASHED_STRING( ResetInputBindings );
	STATIC_HASHED_STRING( ResetDisplayOptions );
	STATIC_HASHED_STRING( ResetGraphicsOptions );
	STATIC_HASHED_STRING( ResetAudioOptions );
	STATIC_HASHED_STRING( OnSliderChanged );
	STATIC_HASHED_STRING( WritePrefsConfig );

	const HashedString EventName = Event.GetEventName();
	if( EventName == sToggleInvertY )
	{
		XTRACE_NAMED( ToggleInvertY );

		STATIC_HASHED_STRING( TurnY );
		const bool InvertY = !m_InputSystem->GetMouseInvert( sTurnY );

		// Publish config var so UI can reflect the change.
		// I could make input system publish config vars for each adjustment, but
		// that seems wasteful since most inputs currently have no adjustment.
		STATICHASH( InvertY );
		ConfigManager::SetBool( sInvertY, InvertY );

		OnInvertYChanged();
	}
	else if( EventName == sToggleHideHUD )
	{
		STATICHASH( HideHUD );
		const bool HideHUD = !ConfigManager::GetBool( sHideHUD );

		ConfigManager::SetBool( sHideHUD, HideHUD );

		OnHideHUDChanged();
	}
	else if( EventName == sToggleFullscreen )
	{
		XTRACE_NAMED( ToggleFullscreen );
		ToggleFullscreen();
	}
	else if( EventName == sToggleVSync )
	{
		XTRACE_NAMED( ToggleVSync );
		ToggleVSync();
	}
	else if( EventName == sToggleFixedDT )
	{
		XTRACE_NAMED( ToggleFixedDT );
		ToggleFixedDT();
	}
	else if( EventName == sToggleFXAA )
	{
		XTRACE_NAMED( ToggleFXAA );
		ToggleFXAA();
	}
	else if( EventName == sResetGameOptions )
	{
		STATICHASH( Defaults_Game );
		ConfigManager::PushDefaultsToContext( sDefaults_Game, false, false, true );

		STATICHASH( Defaults_Framework );
		STATICHASH( Framework );
		ConfigManager::PushDefaultsToContext( sDefaults_Framework, false, false, true, sFramework );

		// Tell systems we've done this
		OnHideHUDChanged();
		OnFixedFrameTimeChanged();
	}
	else if( EventName == sResetControlsOptions )
	{
		STATICHASH( Defaults_Controls );
		ConfigManager::PushDefaultsToContext( sDefaults_Controls, false, false, true );

		STATICHASH( Defaults_EldInput );
		STATICHASH( EldInput );
		ConfigManager::PushDefaultsToContext( sDefaults_EldInput, false, false, true, sEldInput );

		// Tell systems we've done this
		m_UIManager->SetSliderValue( "ControlsOptionsScreen",	"MouseSpeedSlider",			GetSliderValueFromMouseSpeed(		ConfigManager::GetFloat( "MouseSpeed",		1.0f ) ) );
		m_UIManager->SetSliderValue( "ControlsOptionsScreen",	"ControllerSpeedSlider",	GetSliderValueFromControllerSpeed(	ConfigManager::GetFloat( "ControllerSpeed",	1.0f ) ) );

		OnInvertYChanged();
		OnControllerTypeChanged();
	}
	else if( EventName == sResetInputBindings )
	{
		STATICHASH( Defaults_EldKeyboard );
		STATICHASH( EldKeyboard );
		ConfigManager::PushDefaultsToContext( sDefaults_EldKeyboard, false, true, true, sEldKeyboard );

		STATICHASH( Defaults_EldMouse );
		STATICHASH( EldMouse );
		ConfigManager::PushDefaultsToContext( sDefaults_EldMouse, false, true, true, sEldMouse );

		STATICHASH( Defaults_EldController );
		STATICHASH( EldController );
		ConfigManager::PushDefaultsToContext( sDefaults_EldController, false, true, true, sEldController );

		// Tell input system we've done this
		m_InputSystem->UpdateBindingsFromConfig();
	}
	else if( EventName == sResetDisplayOptions )
	{
		STATICHASH( Defaults_Display );
		ConfigManager::PushDefaultsToContext( sDefaults_Display, false, false, true );

		// Reset slider values; this also invokes their side effects!
		m_UIManager->SetSliderValue( "DisplayOptionsScreen",	"BrightnessSlider",			GetSliderValueFromBrightness(		ConfigManager::GetFloat( "Brightness" ) ) );

		// For legacy reasons, this is still currently on its own screen
		m_UIManager->SetSliderValue( "BrightnessScreen",		"BrightnessSlider",			GetSliderValueFromBrightness(		ConfigManager::GetFloat( "Brightness",		1.0f ) ) );
	}
	else if( EventName == sResetGraphicsOptions )
	{
		STATICHASH( Defaults_Graphics );
		ConfigManager::PushDefaultsToContext( sDefaults_Graphics, false, false, true );

		// Reset slider values; this also invokes their side effects!
		m_UIManager->SetSliderValue( "GraphicsOptionsScreen",	"FOVSlider",				GetSliderValueFromFOV(				ConfigManager::GetFloat( "FOV" ) ) );

		// Recreate buckets because we may have a new render path
		CreateBuckets();
	}
	else if( EventName == sResetAudioOptions )
	{
		STATICHASH( Defaults_Audio );
		ConfigManager::PushDefaultsToContext( sDefaults_Audio, false, false, true );

		// Reset slider values; this also invokes their side effects!
		m_UIManager->SetSliderValue( "AudioOptionsScreen",		"VolumeSlider",				ConfigManager::GetFloat( "MasterVolume" ) );
		m_UIManager->SetSliderValue( "AudioOptionsScreen",		"MusicVolumeSlider",		ConfigManager::GetFloat( "MusicVolume" ) );
	}
	else if( EventName == sOnSliderChanged )
	{
		XTRACE_NAMED( OnSliderChanged );

		STATIC_HASHED_STRING( SliderName );
		const HashedString SliderName = Event.GetHash( sSliderName );

		STATIC_HASHED_STRING( SliderValue );
		const float SliderValue = Event.GetFloat( sSliderValue );

		HandleUISliderEvent( SliderName, SliderValue );
	}
	else if( EventName == sWritePrefsConfig )
	{
		XTRACE_NAMED( WritePrefsConfig );
		WritePrefsConfig();
	}
}

void EldFramework::OnHideHUDChanged()
{
	STATICHASH( HideHUD );
	const bool HideHUD = ConfigManager::GetBool( sHideHUD );

	{
		STATIC_HASHED_STRING( HUD );
		WB_MAKE_EVENT( SetScreenHidden, NULL );
		WB_SET_AUTO( SetScreenHidden, Hash, Screen, sHUD );
		WB_SET_AUTO( SetScreenHidden, Bool, Hidden, HideHUD );
		WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), SetScreenHidden, NULL );
	}

	WB_MAKE_EVENT( OnHideHUDChanged, NULL );
	WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), OnHideHUDChanged, NULL );
}

void EldFramework::OnInvertYChanged()
{
	STATICHASH( InvertY );
	const bool InvertY = ConfigManager::GetBool( sInvertY );

	STATIC_HASHED_STRING( TurnY );
	m_InputSystem->SetMouseInvert( sTurnY, InvertY );
	m_InputSystem->SetControllerInvert( sTurnY, InvertY );
}

void EldFramework::OnControllerTypeChanged()
{
	STATICHASH( EldInput );
	STATICHASH( ControllerType );
	const HashedString ControllerType = ConfigManager::GetHash( sControllerType, HashedString::NullString, sEldInput );
	m_InputSystem->SetControllerType( ControllerType );
}

void EldFramework::HandleUISliderEvent( const HashedString& SliderName, const float SliderValue )
{
	STATIC_HASHED_STRING( MouseSpeedSlider );
	STATIC_HASHED_STRING( ControllerSpeedSlider );
	STATIC_HASHED_STRING( BrightnessSlider );
	STATIC_HASHED_STRING( FOVSlider );
	STATIC_HASHED_STRING( VolumeSlider );
	STATIC_HASHED_STRING( MusicVolumeSlider );

	if( SliderName == sMouseSpeedSlider )
	{
		const float MouseSpeed = GetMouseSpeedFromSliderValue( SliderValue );

		STATIC_HASHED_STRING( TurnX );
		m_InputSystem->SetMouseScale( sTurnX, MouseSpeed );

		STATIC_HASHED_STRING( TurnY );
		m_InputSystem->SetMouseScale( sTurnY, MouseSpeed );

		// Publish config var.
		STATICHASH( MouseSpeed );
		ConfigManager::SetFloat( sMouseSpeed, MouseSpeed );
	}
	else if( SliderName == sControllerSpeedSlider )
	{
		const float ControllerSpeed = GetControllerSpeedFromSliderValue( SliderValue );

		STATICHASH( ControllerSpeedX );
		const float ControllerSpeedX = ControllerSpeed * ConfigManager::GetFloat( sControllerSpeedX );

		STATICHASH( ControllerSpeedY );
		const float ControllerSpeedY = -ControllerSpeed * ConfigManager::GetFloat( sControllerSpeedY ); // HACK! Controller Y axis is inverted from mouse.

		STATIC_HASHED_STRING( TurnX );
		m_InputSystem->SetControllerScale( sTurnX, ControllerSpeedX );

		STATIC_HASHED_STRING( TurnY );
		m_InputSystem->SetControllerScale( sTurnY, ControllerSpeedY );

		// Publish config var.
		STATICHASH( ControllerSpeed );
		ConfigManager::SetFloat( sControllerSpeed, ControllerSpeed );
	}
	else if( SliderName == sBrightnessSlider )
	{
		// TODO: Review this; in legacy Eldritch, this wasn't a divide, but it also wasn't gamma correct...
		const float Brightness = GetBrightnessFromSliderValue( SliderValue );
		//m_Game->SetGamma( 1.0f / Brightness );
		m_Game->SetGamma( Brightness );

		// Publish config var.
		STATICHASH( Brightness );
		ConfigManager::SetFloat( sBrightness, Brightness );

		// TODO: And then displayed brightness won't be 2.2-relative either?
		PublishDisplayedBrightness();
	}
	else if( SliderName == sFOVSlider )
	{
		const float FOV = GetFOVFromSliderValue( SliderValue );

		SetFOV( FOV );

		// Publish config var.
		STATICHASH( FOV );
		ConfigManager::SetFloat( sFOV, FOV );

		PublishDisplayedFOV();

		// Notify player of changed FOV to fix FOV interpolating when game is unpaused
		WB_MAKE_EVENT( OnFOVChanged, NULL );
		WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), OnFOVChanged, EldGame::GetPlayer() );

		// Notify anyone who cares that the view has changed
		WB_MAKE_EVENT( OnViewChanged, NULL );
		WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), OnViewChanged, NULL );
	}
	else if( SliderName == sVolumeSlider )
	{
		const float MasterVolume = SliderValue;

		m_AudioSystem->SetMasterVolume( MasterVolume );

		// Publish config var.
		STATICHASH( MasterVolume );
		ConfigManager::SetFloat( sMasterVolume, MasterVolume );
	}
	else if( SliderName == sMusicVolumeSlider )
	{
		const float MusicVolume = SliderValue;

		STATIC_HASHED_STRING( Music );
		m_AudioSystem->SetCategoryVolume( sMusic, MusicVolume, 0.0f );

		// Publish config var.
		STATICHASH( MusicVolume );
		ConfigManager::SetFloat( sMusicVolume, MusicVolume );
	}
}

void EldFramework::PublishDisplayedBrightness() const
{
	STATICHASH( Brightness );
	const float	Brightness			= ConfigManager::GetFloat( sBrightness );

	const float	DisplayedBrightness	= Brightness / 2.20f;

	STATICHASH( DisplayedBrightness );
	ConfigManager::SetFloat( sDisplayedBrightness, DisplayedBrightness );
}

void EldFramework::PublishDisplayedFOV() const
{
	STATICHASH( FOV );
	const float			VerticalFOV				= ConfigManager::GetFloat( sFOV );

	// Display horizontal FOV relative to vertical at a 16:9 aspect ratio
	//const float			AspectRatio				= 16.0f / 9.0f;

	// Display horizontal FOV relative to vertical at actual aspect ratio
	const float			fDisplayWidth			= static_cast<float>( m_DisplayWidth );
	const float			fDisplayHeight			= static_cast<float>( m_DisplayHeight );
	const float			AspectRatio				= fDisplayWidth / fDisplayHeight;

	const float			VerticalFOVRadians		= DEGREES_TO_RADIANS( VerticalFOV );
	const float			HorizontalFOVRadians	= 2.0f * ATan( AspectRatio * Tan( VerticalFOVRadians * 0.5f ) );
	const float			HorizontalFOV			= RADIANS_TO_DEGREES( HorizontalFOVRadians );
	const uint			iHorizontalFOV			= RoundToUInt( HorizontalFOV );
	const uint			iVerticalFOV			= RoundToUInt( VerticalFOV );

	STATICHASH( HorizontalFOV );
	ConfigManager::SetInt( sHorizontalFOV, iHorizontalFOV );

	STATICHASH( VerticalFOV );
	ConfigManager::SetInt( sVerticalFOV, iVerticalFOV );

	// LEGACY strings
	STATICHASH( DisplayedFOV );
	ConfigManager::SetInt( sDisplayedFOV, iVerticalFOV );
}

// TODO: Parameterize these functions and move to mathcore, they're pretty useful.
inline float EldFramework::GetMouseSpeedFromSliderValue( const float SliderValue )
{
	// TODO: Parameterize this so I can tune it.
	// This should scale neatly from 1/8 to 8 on a power curve, with 1 in the middle.
	static const float	sMouseScaleRange	= 8.0f;
	const float			AdjustedSliderValue	= ( SliderValue * 2.0f ) - 1.0f;
	const float			MouseSpeed			= Pow( sMouseScaleRange, AdjustedSliderValue );
	return MouseSpeed;
}

inline float EldFramework::GetSliderValueFromMouseSpeed( const float MouseSpeed )
{
	static const float	sMouseScaleRange	= 8.0f;
	const float			AdjustedSliderValue	= LogBase( MouseSpeed, sMouseScaleRange );
	const float			SliderValue			= ( AdjustedSliderValue + 1.0f ) * 0.5f;
	return SliderValue;
}

inline float EldFramework::GetControllerSpeedFromSliderValue( const float SliderValue )
{
	// TODO: Parameterize this so I can tune it.
	// This should scale neatly from 1/2 to 2 on a power curve, with 1 in the middle.
	static const float	sControllerScaleRange	= 2.0f;
	const float			AdjustedSliderValue	= ( SliderValue * 2.0f ) - 1.0f;
	const float			ControllerSpeed			= Pow( sControllerScaleRange, AdjustedSliderValue );
	return ControllerSpeed;
}

inline float EldFramework::GetSliderValueFromControllerSpeed( const float ControllerSpeed )
{
	static const float	sControllerScaleRange	= 2.0f;
	const float			AdjustedSliderValue	= LogBase( ControllerSpeed, sControllerScaleRange );
	const float			SliderValue			= ( AdjustedSliderValue + 1.0f ) * 0.5f;
	return SliderValue;
}

// TODO: Revert these to legacy behavior if needed, gamma was different in Eldritch
inline float EldFramework::GetBrightnessFromSliderValue( const float SliderValue )
{
	// TODO: Parameterize this so I can tune it.
	// This should scale neatly from 2 to 1/2 on a power curve, with 1 in the middle.
	static const float	sBrightnessScaleRange	= 2.0f;
	const float			AdjustedSliderValue		= ( SliderValue * -2.0f ) + 1.0f;
	const float			Brightness				= Pow( sBrightnessScaleRange, AdjustedSliderValue );
	return Brightness;
}

// TODO: Revert these to legacy behavior if needed, gamma was different in Eldritch
inline float EldFramework::GetSliderValueFromBrightness( const float Brightness )
{
	static const float	sBrightnessScaleRange	= 2.0f;
	const float			AdjustedSliderValue		= LogBase( Brightness, sBrightnessScaleRange );
	const float			SliderValue				= ( AdjustedSliderValue - 1.0f ) * -0.5f;
	return SliderValue;
}

inline float EldFramework::GetFOVFromSliderValue( const float SliderValue )
{
	STATICHASH( FOV_Min );
	STATICHASH( FOV_Max );

	const float FOV_Min = ConfigManager::GetFloat( sFOV_Min, 60.0f );
	const float FOV_Max = ConfigManager::GetFloat( sFOV_Max, 120.0f );
	return Lerp( FOV_Min, FOV_Max, SliderValue );
}

inline float EldFramework::GetSliderValueFromFOV( const float FOV )
{
	STATICHASH( FOV_Min );
	STATICHASH( FOV_Max );

	const float FOV_Min = ConfigManager::GetFloat( sFOV_Min, 60.0f );
	const float FOV_Max = ConfigManager::GetFloat( sFOV_Max, 120.0f );

	return InvLerp( FOV, FOV_Min, FOV_Max );
}

void EldFramework::InitializeTools()
{
#if BUILD_ELD_TOOLS
	SafeDelete( m_Tools );

	m_Tools = new EldTools;
	m_Tools->InitializeFromDefinition( "EldritchTools" );
#endif
}

void EldFramework::ShutDownWorld()
{
	XTRACE_FUNCTION;

	SafeDelete( m_World );
#if BUILD_ELD_TOOLS
	SafeDelete( m_Tools );
#endif

	m_Audio3DListener->SetWorld( NULL );

	WBWorld::DeleteInstance();
}

/*virtual*/ void EldFramework::ShutDown()
{
	XTRACE_FUNCTION;

	// HACKHACK: Clear the UI stack so everything gets popped;
	// this fixes state problems that can occur when things
	// depend on a UI screen closing to restore some state.
	GetUIManager()->GetUIStack()->Clear();

	// Shutting down game also saves the game in progress.
	m_Game->ShutDown();

	WritePrefsConfig();

	FileStream::StaticShutDown();
	PackStream::StaticShutDown();

	WBComponent::ShutDownBaseFactories();
	RodinBTNodeFactory::ShutDown();
	WBActionFactory::ShutDown();
	WBParamEvaluatorFactory::ShutDown();
	AnimEventFactory::DeleteInstance();
	UIFactory::ShutDown();
	WBActionStack::ShutDown();
	EldParticles::FlushConfigCache();
	SDPFactory::ShutDown();

	ShutDownWorld();

	SafeDelete( m_Game );
	SafeDelete( m_MainView );
	SafeDelete( m_FGView );
	SafeDelete( m_HUDView );
	SafeDelete( m_MirrorView );
	SafeDelete( m_MirrorBView );
	SafeDelete( m_MinimapView );
	SafeDelete( m_UpscaleView );
	SafeDelete( m_TargetManager );
	SafeDelete( m_InputSystem );
	SafeDelete( m_Controller );
	SafeDelete( m_Audio3DListener );
	SafeDelete( m_AchievementManager );
	SafeDelete( m_Leaderboards );

	DynamicMeshManager::DeleteInstance();

#if BUILD_STEAM
	PRINTF( "Shutting down Steam API.\n" );
	SteamAPI_Shutdown();
#endif

	Framework3D::ShutDown();

	ReverseHash::ShutDown();
}

void EldFramework::LoadPrefsConfig()
{
	const SimpleString PrefsConfigFilename = GetUserDataPath() + SimpleString( "prefs.cfg" );
	if( FileUtil::Exists( PrefsConfigFilename.CStr() ) )
	{
		const FileStream PrefsConfigStream = FileStream( PrefsConfigFilename.CStr(), FileStream::EFM_Read );
		ConfigManager::Load( PrefsConfigStream );
	}
	else
	{
		// Load legacy prefs if available
		const SimpleString LegacyPrefsConfigFilename = GetLegacyUserDataPath() + SimpleString( "prefs.cfg" );
		if( FileUtil::Exists( LegacyPrefsConfigFilename.CStr() ) )
		{
			const FileStream LegacyPrefsConfigStream = FileStream( LegacyPrefsConfigFilename.CStr(), FileStream::EFM_Read );
			ConfigManager::Load( LegacyPrefsConfigStream );
		}
	}
}

void EldFramework::WritePrefsConfig()
{
	const SimpleString PrefsConfigFilename = GetUserDataPath() + SimpleString( "prefs.cfg" );
	const FileStream PrefsConfigStream = FileStream( PrefsConfigFilename.CStr(), FileStream::EFM_Write );

	PrefsConfigStream.PrintF( "# This file is automatically generated.\n# You may delete it to restore defaults.\n\n" );

	ConfigManager::BeginWriting();

	ConfigManager::Write( PrefsConfigStream, "HideHUD" );
	ConfigManager::Write( PrefsConfigStream, "Language" );
	ConfigManager::Write( PrefsConfigStream, "PauseOnLostFocus" );
	ConfigManager::Write( PrefsConfigStream, "MuteWhenUnfocused" );
	ConfigManager::Write( PrefsConfigStream, "OpenGL" );
	ConfigManager::Write( PrefsConfigStream, "DisplayWidth" );
	ConfigManager::Write( PrefsConfigStream, "DisplayHeight" );
	ConfigManager::Write( PrefsConfigStream, "Fullscreen" );
	ConfigManager::Write( PrefsConfigStream, "UpscaleFullscreen" );
	ConfigManager::Write( PrefsConfigStream, "VSync" );
	ConfigManager::Write( PrefsConfigStream, "FXAA" );
	ConfigManager::Write( PrefsConfigStream, "Brightness" );
	ConfigManager::Write( PrefsConfigStream, "FOV" );
	ConfigManager::Write( PrefsConfigStream, "MaxAnisotropy" );
	ConfigManager::Write( PrefsConfigStream, "DirectInput" );
	ConfigManager::Write( PrefsConfigStream, "MouseSpeed" );
	ConfigManager::Write( PrefsConfigStream, "ControllerSpeed" );
	ConfigManager::Write( PrefsConfigStream, "InvertY" );
	ConfigManager::Write( PrefsConfigStream, "MasterVolume" );
	ConfigManager::Write( PrefsConfigStream, "MusicVolume" );

	ConfigManager::Write( PrefsConfigStream, "UseFixedFrameTime", "Framework" );

	m_InputSystem->WriteConfigBinds( PrefsConfigStream );
}

/*virtual*/ bool EldFramework::TickSim( const float DeltaTime )
{
	XTRACE_FUNCTION;

	m_SimTickHasRequestedRenderTick = false;

#if BUILD_STEAM
	SteamAPI_RunCallbacks();
#endif

	// Automatically pause when we lose window focus
	if( m_PauseOnLostFocus &&
		!HasFocus() &&
		!m_UIManager->GetUIStack()->PausesGame() /*&&
		!m_Game->IsInTitleScreen()*/ )
	{
		TryPause();
	}

	if( m_MuteWhenUnfocused )
	{
		m_AudioSystem->SetGlobalMute( !HasFocus() );
	}

	return Framework3D::TickSim( DeltaTime );
}

/*virtual*/ bool EldFramework::TickGame( const float DeltaTime )
{
	XTRACE_FUNCTION;

	// Oh this is horrible. But it works. Since InputSystem is a "game" system
	// (i.e., part of the realtime sim and not the UI, etc.), tick it here.
	// This way, we *do* update the input system on the same frame that the
	// game gets unpaused, even though we *don't* call TickInput (because we
	// want to ignore the Escape press). This whole system probably needs to
	// be reconsidered, but whatever. It gets this game finished.
	const bool WasBinding = m_InputSystem->IsBinding();
	m_InputSystem->Tick();
	if( WasBinding )
	{
		TryFinishInputBinding();
	}

	m_Game->Tick();

#if BUILD_ELD_TOOLS
	if( m_Tools->IsInToolMode() )
	{
		m_Tools->Tick( DeltaTime );
	}
	else
#endif
	{
		m_World->Tick( DeltaTime );
	}

	return true;
}

/*virtual*/ void EldFramework::OnUnpaused()
{
	Framework3D::OnUnpaused();

	// HACK: If we've just unpaused, tick input system again so we ignore rising inputs on the next frame.
	m_InputSystem->Tick();
}

/*virtual*/ void EldFramework::TickDevices()
{
	XTRACE_FUNCTION;

	Framework3D::TickDevices();

	m_Controller->Tick( 0.0f );

	// If we're using either controller or mouse exclusively, allow or disallow the cursor as needed.
	const bool IsUsingController	= m_Controller->ReceivedInputThisTick();
	const bool IsUsingMouse			= m_Mouse->ReceivedInputThisTick();
	if( IsUsingController != IsUsingMouse )
	{
		m_Mouse->SetAllowCursor( IsUsingMouse );
	}
}

bool EldFramework::CanPause() const
{
	// HACKHACK: Don't allow pausing while the game has queued events!
	// This should fix initialization bugs if the game loses focus and autopauses.
	if( m_Game && WBWorld::GetInstance()->GetEventManager()->HasQueuedEvents( m_Game ) )
	{
		return false;
	}

	// Don't allow pausing when dead
	if( !EldGame::IsPlayerAlive() )
	{
		return false;
	}

	// Don't allow pausing for scripted reasons
	if( EldGame::IsPlayerDisablingPause() )
	{
		return false;
	}

	// All checks passed, we can pause
	return true;
}

void EldFramework::TryPause()
{
	XTRACE_FUNCTION;

	if( CanPause() )
	{
		UIScreen* const pPauseScreen = m_UIManager->GetScreen( "PauseScreen" );
		ASSERT( pPauseScreen );
		m_UIManager->GetUIStack()->Repush( pPauseScreen );
	}
}

/*virtual*/ bool EldFramework::TickInput( const float DeltaTime, const bool UIHasFocus )
{
	XTRACE_FUNCTION;

	if( !Framework3D::TickInput( DeltaTime, UIHasFocus ) )
	{
		return false;
	}

	if( !UIHasFocus &&
		( m_Keyboard->OnRise( Keyboard::EB_Escape ) || m_Controller->OnRise( XInputController::EB_Start ) ) )
	{
		TryPause();
	}

#if BUILD_ELD_TOOLS
	if( m_Keyboard->OnRise( Keyboard::EB_Tab ) )
	{
		m_Tools->ToggleToolMode();
	}

	if( m_Tools->IsInToolMode() )
	{
		m_Tools->TickInput( DeltaTime );
		return true;
	}
#endif

	STATIC_HASHED_STRING( Quicksave );
#if BUILD_FINAL
	if( m_InputSystem->OnRise( sQuicksave ) )
	{
		m_Game->Checkpoint();
	}
#else
	{
		if( m_InputSystem->OnRise( sQuicksave ) )
		{
			m_Game->GetSaveLoad()->SaveMaster( "quick.eldmastersave" );
		}

		STATIC_HASHED_STRING( Quickload );
		if( m_InputSystem->OnRise( sQuickload ) )
		{
			PrepareForLoad();
			m_Game->GetSaveLoad()->TryLoadMaster( "quick.eldmastersave" );
		}
	}
#endif

#if BUILD_DEV

	// Alt + Q: Start/stop reverb test mode
	if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_Q ) )
	{
		m_AudioSystem->ReverbTest_Toggle();
	}

	if( m_AudioSystem->ReverbTest_IsActive() )
	{
		if( m_Keyboard->OnRise( Keyboard::EB_Up ) )
		{
			m_AudioSystem->ReverbTest_PrevSetting();
		}

		if( m_Keyboard->OnRise( Keyboard::EB_Down ) )
		{
			m_AudioSystem->ReverbTest_NextSetting();
		}

		if( m_Keyboard->OnRise( Keyboard::EB_Left ) )
		{
			const float Scalar = m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) ? 10.0f : 1.0f;
			m_AudioSystem->ReverbTest_DecrementSetting( Scalar );
		}

		if( m_Keyboard->OnRise( Keyboard::EB_Right ) )
		{
			const float Scalar = m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) ? 10.0f : 1.0f;
			m_AudioSystem->ReverbTest_IncrementSetting( Scalar );
		}
	}

	// Alt + R: Report
	// Alt + Shift + R: Skip world report
	if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_R ) )
	{
		if( m_Keyboard->IsLow( Keyboard::EB_LeftShift ) )
		{
			WBWorld::GetInstance()->Report();
			ReverseHash::ReportSize();
		}

#if 0
		// Report global stats (obtained when the game was launched)
		if( m_AchievementManager )
		{
			STATICHASH( EldritchSteamStats );

			PRINTF( "Steam global stats:\n" );
			STATICHASH( NumStats );
			const uint NumStats = ConfigManager::GetInt( sNumStats, 0, sEldritchSteamStats );
			for( uint StatIndex = 0; StatIndex < NumStats; ++StatIndex )
			{
				const SimpleString StatTag = ConfigManager::GetSequenceString( "StatTag%d", StatIndex, "", sEldritchSteamStats );
				m_AchievementManager->ReportGlobalStat( StatTag );
			}

			PRINTF( "Steam global achievement rates:\n" );
			STATICHASH( NumAchievements );
			const uint NumAchievements = ConfigManager::GetInt( sNumAchievements, 0, sEldritchSteamStats );
			for( uint AchievementIndex = 0; AchievementIndex < NumAchievements; ++AchievementIndex )
			{
				const SimpleString AchievementTag = ConfigManager::GetSequenceString( "AchievementTag%d", AchievementIndex, "", sEldritchSteamStats );
				m_AchievementManager->ReportGlobalAchievementRate( AchievementTag );
			}

			// Request updated stats for the next report
			m_AchievementManager->RequestServerUpdate();
		}
#endif
	}

	// Alt + P: play time
	if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_P ) )
	{
		PRINTF( "Play time: %.2f\n", EldGame::GetPlayTime() );
	}

	//// Alt + A: reset stats and achievements
	//if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_A ) )
	//{
	//	if( m_AchievementManager )
	//	{
	//		const bool ResetAchievements = true;
	//		m_AchievementManager->ResetAllStats( ResetAchievements );
	//	}
	//}

	//// Alt + L: post fake leaderboard scores
	//// Shift + Alt + L: get leaderboard scores
	//if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_L ) )
	//{
	//	if( m_Leaderboards )
	//	{
	//		if( m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) )
	//		{
	//			Array<SLeaderboardEntry> LeaderboardEntries;
	//			m_Leaderboards->GetRequestedScores( LeaderboardEntries );

	//			FOR_EACH_ARRAY( LeaderboardIter, LeaderboardEntries, SLeaderboardEntry )
	//			{
	//				const SLeaderboardEntry& Entry = LeaderboardIter.GetValue();
	//				PRINTF( "%d : %s : %d\n", Entry.m_Rank, Entry.m_UserName.CStr(), Entry.m_Score );
	//			}
	//		}
	//		else
	//		{
	//			m_Leaderboards->PostScore( "LB_PlayTime", 123 );
	//			m_Leaderboards->PostScore( "LB_Souls", 499 );
	//			m_Leaderboards->RequestGlobalScores( "LB_Souls", 0, 10 );
	//			m_Leaderboards->RequestGlobalScores( "LB_PlayTime", 0, 10 );
	//		}
	//	}
	//}

	//// Ctrl + Alt + Backspace: Invoke crash
	//if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->IsHigh( Keyboard::EB_LeftControl ) && m_Keyboard->OnRise( Keyboard::EB_Backspace ) )
	//{
	//	WBEntity* const pEntity = NULL;
	//	pEntity->Tick( 0.0f );
	//}

	//// Shift + Alt + Backspace: Invoke crash by allocating all the memory
	//if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) && m_Keyboard->OnRise( Keyboard::EB_Backspace ) )
	//{
	//	for(;;)
	//	{
	//		byte* pArray = new byte[ 32 ];
	//		pArray[0] = pArray[31];
	//	}
	//}

	// Alt + Backspace: Gather stats
	if( m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) && m_Keyboard->OnRise( Keyboard::EB_Backspace ) )
	{
		m_World->GatherStats();
	}

	// Shift + Ctrl + T is a full restart (new hub), like Resurrect.
	// Ctrl + T is a proxy return to hub (new worlds, same hub), like Return to Library.
	// Alt + T is a travel to next level.
	// Shift + Alt + T is a travel to prev level.
	if( m_Keyboard->OnRise( Keyboard::EB_T ) && m_Keyboard->IsHigh( Keyboard::EB_LeftControl ) )
	{
		WB_MAKE_EVENT( ReturnToHub, NULL );
		WB_SET_AUTO( ReturnToHub, Bool, Restart, true );
		WB_SET_AUTO( ReturnToHub, Bool, FlushHub, m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) );
		WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), ReturnToHub, m_Game );
	}
	else if( m_Keyboard->OnRise( Keyboard::EB_T ) && m_Keyboard->IsHigh( Keyboard::EB_LeftAlt ) )
	{
		if( m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) )
		{
			WB_MAKE_EVENT( GoToPrevLevel, NULL );
			WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), GoToPrevLevel, m_Game );
		}
		else
		{
			WB_MAKE_EVENT( GoToNextLevel, NULL );
			WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), GoToNextLevel, m_Game );
		}
	}
	else if( m_Keyboard->OnRise( Keyboard::EB_T ) )
	{
		RegenerateWorld();
	}
	else if( m_Keyboard->IsHigh( Keyboard::EB_T ) &&
		m_Keyboard->IsHigh( Keyboard::EB_LeftShift ) &&
		m_Keyboard->IsLow( Keyboard::EB_LeftControl ) &&
		m_Keyboard->IsLow( Keyboard::EB_LeftAlt ) )
	{
		RegenerateWorld();
	}
#endif	// BUILD_DEV

	return true;
}

void EldFramework::TryFinishInputBinding()
{
	// HACK: If we finished or canceled binding, pop the bind dialog.
	if( !m_InputSystem->IsBinding() )
	{
		STATIC_HASHED_STRING( BindDialog );
		if( m_UIManager->GetUIStack()->Top() == m_UIManager->GetScreen( sBindDialog ) )
		{
			m_UIManager->GetUIStack()->Pop();
		}
	}
}

/*virtual*/ void EldFramework::TickPausedInput( const float DeltaTime )
{
	XTRACE_FUNCTION;

	Framework3D::TickPausedInput( DeltaTime );

	if( m_InputSystem->IsBinding() )
	{
		m_InputSystem->Tick();
		TryFinishInputBinding();
	}
	else
	{
		m_InputSystem->UpdateIsUsingControllerExclusively();
	}
}

/*virtual*/ void EldFramework::TickRender()
{
	XTRACE_FUNCTION;

#if BUILD_ELD_TOOLS
	if( m_Tools->IsInToolMode() )
	{
		m_Tools->TickRender();
	}
	else
#endif
	{
		m_World->Render();

#if BUILD_DEV
		m_World->DebugRender();
#endif

		WBWorld::GetInstance()->Render();

#if BUILD_DEV
		WBWorld::GetInstance()->DebugRender();
#endif
	}

	m_Game->Render();

	Framework3D::TickRender();
}

void EldFramework::SetMainViewTransform( const Vector& Location, const Angles& Orientation )
{
	m_MainView->m_Location = Location;
	m_MainView->m_Rotation = Orientation;

	m_FGView->m_Location = Location;
	m_FGView->m_Rotation = Orientation;

	m_Audio3DListener->SetLocation( Location );
	m_Audio3DListener->SetRotation( Orientation );
}

void EldFramework::SetFOV( const float FOV )
{
	DEVASSERT( m_MainView );
	m_MainView->SetFOV( FOV );
}

void EldFramework::SetFGFOV( const float FGFOV )
{
	DEVASSERT( m_FGView );
	m_FGView->SetFOV( FGFOV );
}

void EldFramework::PrepareForLoad()
{
	XTRACE_FUNCTION;

	m_InputSystem->PopAllContexts();
	m_Clock->ClearMultiplierRequests();

	// HACK: Before load (and after popping all contexts), tick input system again
	// so we ignore rising inputs on the first new frame.
	m_InputSystem->Tick();

	static bool FirstLoad = true;
	if( FirstLoad )
	{
		FirstLoad = false;
	}
	else
	{
		STATIC_HASHED_STRING( HUD );
		m_UIManager->GetUIStack()->Clear();
		m_UIManager->GetUIStack()->Push( m_UIManager->GetScreen( sHUD ) );
	}
}

/*virtual*/ void EldFramework::ToggleFullscreen()
{
	PRINTF( "EldFramework::ToggleFullscreen\n" );

#if !BUILD_SWITCH
	Framework3D::ToggleFullscreen();
#endif

	// For fullscreen upscaling, we may need a new m_UpscaleView
	UpdateViews();

	ASSERT( m_Game );
	m_Game->RefreshRTDependentSystems();

	// Recreate buckets because we have new views and maybe a new render path
	CreateBuckets();
}

/*virtual*/ void EldFramework::ToggleVSync()
{
	PRINTF( "EldFramework::ToggleVSync\n" );

	Framework3D::ToggleVSync();

	ASSERT( m_Game );
	m_Game->RefreshRTDependentSystems();
}

/*virtual*/ void EldFramework::SetResolution( const uint DisplayWidth, const uint DisplayHeight )
{
	PRINTF( "EldFramework::SetResolution\n" );

	m_DisplayWidth	= DisplayWidth;
	m_DisplayHeight	= DisplayHeight;

	Framework3D::SetResolution( DisplayWidth, DisplayHeight );

	m_TargetManager->CreateTargets( DisplayWidth, DisplayHeight );

	UpdateViews();
	PublishDisplayedFOV();

	ASSERT( m_Game );
	m_Game->RefreshRTDependentSystems();

	// Recreate buckets because we have new render targets
	CreateBuckets();

	// Notify anyone else who cares that the resolution has changed
	WB_MAKE_EVENT( OnSetRes, NULL );
	WB_DISPATCH_EVENT( WBWorld::GetInstance()->GetEventManager(), OnSetRes, NULL );
}

/*virtual*/ void EldFramework::ToggleFXAA()
{
	STATICHASH( FXAA );
	const bool FXAA = !ConfigManager::GetBool( sFXAA );
	ConfigManager::SetBool( sFXAA, FXAA );

	PRINTF( "EldFramework::ToggleFXAA %s\n", FXAA ? "true" : "false" );

	// Recreate buckets because we have a new render path
	CreateBuckets();
}

void EldFramework::CreateBuckets()
{
	PRINTF( "EldFramework::CreateBuckets\n" );

	STATICHASH( FXAA );
	const bool FXAA = ConfigManager::GetBool( sFXAA );

	IRenderTarget* const pMainRT = m_TargetManager->GetRenderTarget( "Primary" );
	IRenderTarget* const pScrnRT = m_TargetManager->GetRenderTarget( "Original" );
	IRenderTarget* const pMirrRT = m_TargetManager->GetRenderTarget( "Mirror" );
	IRenderTarget* const pMrAART = m_TargetManager->GetRenderTarget( "MirrorFXAA" );
	IRenderTarget* const pMMapRT = m_TargetManager->GetRenderTarget( "Minimap" );
	IRenderTarget* const p_UI_RT = m_Display->ShouldUpscaleFullscreen() ? pMainRT : pScrnRT;	// If we're not upscaling, we can just render UI to the main backbuffer; if we are, pingpong back to the main RT
	IRenderTarget* const pPostRT = FXAA ? m_TargetManager->GetRenderTarget( "Post" ) : p_UI_RT;	// If we're not antialiasing, we can just render post to the UI target

	STATIC_HASHED_STRING( Main );
	STATIC_HASHED_STRING( Post );
	STATIC_HASHED_STRING( UI );

	m_Renderer->FreeBuckets();

#define ADDBK m_Renderer->AddBucket
#define BK new Bucket
#if BUILD_DEV
	#define DEVBK ADDBK
#else
	#define DEVBK( ... ) DoNothing
#endif

								// View			// RT		// Flags					// Filter				// Tag	// Frus	// Excl	// Clear
	ADDBK( "Main",			BK( m_MainView,		pMainRT,	MAT_WORLD,					MAT_ALPHA|MAT_DYNAMIC,	sMain,	true,	true,	CLEAR_DEPTH ) );
	ADDBK( "MainDynamic",	BK( NULL,			NULL,		MAT_WORLD|MAT_DYNAMIC,		MAT_ALPHA,				sMain,	true,	true ) );
	DEVBK( "MainDebug",		BK( NULL,			NULL,		MAT_DEBUG_WORLD,			MAT_NONE,				sMain,	true,	true ) );
	ADDBK( "MainAlpha",		BK( NULL,			NULL,		MAT_WORLD|MAT_ALPHA,		MAT_NONE,				sMain,	true,	true ) );
	ADDBK( "MainFG",		BK( m_FGView,		NULL,		MAT_FOREGROUND,				MAT_ALPHA,				sMain,	true,	true,	CLEAR_DEPTH ) );
	ADDBK( "MainFGAlpha",	BK( NULL,			NULL,		MAT_FOREGROUND|MAT_ALPHA,	MAT_NONE,				sMain,	true,	true,	CLEAR_DEPTH ) );
	ADDBK( "Mirror",		BK( m_MirrorView,	pMirrRT,	MAT_PRESCRIBED,				MAT_NONE,				sUI,	false,	true,	CLEAR_DEPTH ) );
	ADDBK( "MirrorPost",	BK( m_MirrorBView,	pMrAART,	MAT_PRESCRIBED,				MAT_NONE,				sUI,	false,	true ) );
	ADDBK( "MirrorFXAA",	BK( NULL,			pMirrRT,	MAT_PRESCRIBED,				MAT_NONE,				sUI,	false,	true ) );
	ADDBK( "Minimap",		BK( m_MinimapView,	pMMapRT,	MAT_PRESCRIBED,				MAT_NONE,				sUI,	false,	true,	CLEAR_DEPTH|CLEAR_COLOR ) );
	ADDBK( "Post",			BK( m_HUDView,		pPostRT,	MAT_PRESCRIBED,				MAT_NONE,				sPost,	false,	true ) );
	ADDBK( "FXAA",			BK( NULL,			p_UI_RT,	MAT_PRESCRIBED,				MAT_NONE,				sPost,	false,	true ) );
	ADDBK( "HUD",			BK( NULL,			NULL,		MAT_HUD,					MAT_NONE,				sUI,	false,	true ) );
	DEVBK( "HUDDebug",		BK( NULL,			NULL,		MAT_DEBUG_HUD,				MAT_NONE,				sUI,	false,	true ) );
	ADDBK( "Upscale",		BK( m_UpscaleView,	pScrnRT,	MAT_PRESCRIBED,				MAT_NONE,				sUI,	false,	true,	CLEAR_COLOR ) );	// Clear color so we get black bars when aspect doesn't match

#undef DEVBK
#undef ADDBK
#undef BK

	// Get buckets by name and set properties here because I didn't want to change that horrid bucket constructor
	m_Renderer->GetBucket( "FXAA" )->m_Enabled					= FXAA;
	m_Renderer->GetBucket( "MirrorPost" )->m_Enabled			= FXAA;
	m_Renderer->GetBucket( "MirrorFXAA" )->m_Enabled			= FXAA;
	m_Renderer->GetBucket( "Upscale" )->m_Enabled				= m_Display->ShouldUpscaleFullscreen();
	m_Renderer->GetBucket( "MainDynamic" )->m_SortByMaterial	= true;
}

void EldFramework::UpdateViews()
{
	PRINTF( "EldFramework::UpdateViews\n" );

	const float		fDisplayWidth	= static_cast<float>( m_DisplayWidth );
	const float		fDisplayHeight	= static_cast<float>( m_DisplayHeight );
	const float		AspectRatio		= fDisplayWidth / fDisplayHeight;

	m_MainView->SetAspectRatio(	AspectRatio );
	m_FGView->SetAspectRatio(	AspectRatio );

	// Recreate HUD view because it is dependent on display dimensions.
	// Some other views may not need to be rebuilt.
	CreateHUDView();
	CreateMirrorView();
	CreateMinimapView();
}

void EldFramework::CreateHUDView()
{
	PRINTF( "EldFramework::CreateHUDView\n" );

	SafeDelete( m_HUDView );
	SafeDelete( m_UpscaleView );

	const Vector	EyePosition		= Vector( 0.0f, -1.0f, 0.0f );
	const Vector	EyeDirection	= Vector( 0.0f, 1.0f, 0.0f );
	const float		NearClip		= 0.01f;
	const float		FarClip			= 2.0f;

	const float		fDisplayWidth	= static_cast<float>( m_DisplayWidth );
	const float		fDisplayHeight	= static_cast<float>( m_DisplayHeight );
	const float		fFrameWidth		= static_cast<float>( m_Display->m_FrameWidth );
	const float		fFrameHeight	= static_cast<float>( m_Display->m_FrameHeight );

	m_HUDView		= new View( EyePosition, EyeDirection, SRect( 0.0f, 0.0f, fDisplayWidth, fDisplayHeight ), NearClip, FarClip );
	m_UpscaleView	= new View( EyePosition, EyeDirection, SRect( 0.0f, 0.0f, fFrameWidth, fFrameHeight ), NearClip, FarClip );
}

void EldFramework::CreateMirrorView()
{
	PRINTF( "EldFramework::CreateMirrorView\n" );

	SafeDelete( m_MirrorView );
	SafeDelete( m_MirrorBView );

	UIScreenEldMirror* const	pMirrorScreen	= EldGame::GetMirrorScreen();
	const float					fMirrorRTWidth = static_cast<float>(pMirrorScreen->GetMirrorRTWidth());
	const float					fMirrorRTHeight = static_cast<float>(pMirrorScreen->GetMirrorRTHeight());

	{
		const float				AspectRatio = fMirrorRTWidth / fMirrorRTHeight;
		const Matrix			MirrorRotation = Matrix::CreateRotationAboutZ(pMirrorScreen->GetMirrorYaw());	// For beauty lighting with cube lights
		const Vector			EyePosition = MirrorRotation * Vector(0.0f, pMirrorScreen->GetMirrorViewDistance(), pMirrorScreen->GetMirrorViewHeight());
		const Vector			EyeDirection = MirrorRotation * Vector(0.0f, -1.0f, 0.0f);
		const float				NearClip = pMirrorScreen->GetMirrorViewNearClip();
		const float				FarClip = pMirrorScreen->GetMirrorViewFarClip();
		const float				FOV = pMirrorScreen->GetMirrorViewFOV();

		m_MirrorView	= new View( EyePosition, EyeDirection, FOV, AspectRatio, NearClip, FarClip );
	}

	{
		const Vector			EyePosition		= Vector( 0.0f, -1.0f, 0.0f );
		const Vector			EyeDirection	= Vector( 0.0f, 1.0f, 0.0f );
		const float				NearClip		= 0.01f;
		const float				FarClip			= 2.0f;

		m_MirrorBView	= new View( EyePosition, EyeDirection, SRect( 0.0f, 0.0f, fMirrorRTWidth, fMirrorRTHeight ), NearClip, FarClip );
	}
}

void EldFramework::CreateMinimapView()
{
	PRINTF( "EldFramework::CreateMinimapView\n" );

	SafeDelete( m_MinimapView );

	STATICHASH( EldMinimap );

	STATICHASH( MinimapRTWidth );
	const float		fMinimapRTWidth		= ConfigManager::GetFloat( sMinimapRTWidth, 0.0f, sEldMinimap );

	STATICHASH( MinimapRTHeight );
	const float		fMinimapRTHeight	= ConfigManager::GetFloat( sMinimapRTHeight, 0.0f, sEldMinimap );

	STATICHASH( MinimapViewDistance );
	const float		ViewDistance		= ConfigManager::GetFloat( sMinimapViewDistance, 0.0f, sEldMinimap );

	STATICHASH( MinimapViewPitch );
	const float		ViewPitch			= DEGREES_TO_RADIANS( ConfigManager::GetFloat( sMinimapViewPitch, 0.0f, sEldMinimap ) );

	STATICHASH( MinimapViewFOV );
	const float		FOV					= ConfigManager::GetFloat( sMinimapViewFOV, 0.0f, sEldMinimap );

	STATICHASH( MinimapViewNearClip );
	const float		NearClip			= ConfigManager::GetFloat( sMinimapViewNearClip, 0.0f, sEldMinimap );

	STATICHASH( MinimapViewFarClip );
	const float		FarClip				= ConfigManager::GetFloat( sMinimapViewFarClip, 0.0f, sEldMinimap );

	const Angles	EyeOrientation		= Angles( ViewPitch, 0.0f, 0.0f );
	const Vector	EyeDirection		= EyeOrientation.ToVector();
	const Vector	EyePosition			= -EyeDirection * ViewDistance;
	const float		AspectRatio			= fMinimapRTWidth / fMinimapRTHeight;

	m_MinimapView						= new View( EyePosition, EyeOrientation, FOV, AspectRatio, NearClip, FarClip );
}

/*virtual*/ void EldFramework::RefreshDisplay( const bool Fullscreen, const bool VSync, const uint DisplayWidth, const uint DisplayHeight )
{
	PRINTF( "EldFramework::RefreshDisplay\n" );

	Framework3D::RefreshDisplay( Fullscreen, VSync, DisplayWidth, DisplayHeight );

	m_TargetManager->CreateTargets( DisplayWidth, DisplayHeight );

	UpdateViews();
	PublishDisplayedFOV();

	ASSERT( m_Game );
	m_Game->RefreshRTDependentSystems();

	CreateBuckets();
}

void EldFramework::RegenerateWorld()
{
	ASSERT( m_World );
	InitializeWorld( m_World->GetCurrentWorld(), true );
}

void EldFramework::GoToLevel( const HashedString& WorldDef )
{
	InitializeWorld( WorldDef, true );
}

/*virtual*/ SimpleString EldFramework::GetUserDataPath()
{
	// HACKHACK: Hard-coded so string table *can't* override this!
	const SimpleString UserPathAppName = "Eldritch Reanimated";

#if BUILD_MAC
	// MACTODO: Make this part of FileUtil::GetUserLocalAppDataPath or whatever.
	// Changed from legacy so files are not saved in the same place!
	return ObjCJunk::GetUserDirectory( UserPathAppName );
#elif BUILD_LINUX
	const char* const XDGDataHome = getenv( "XDG_DATA_HOME" );
	if( XDGDataHome )
	{
		return SimpleString::PrintF( "%s/%s/", XDGDataHome, UserPathAppName.CStr() );
	}

	const char* const Home = getenv( "HOME" );
	if( Home )
	{
		return SimpleString::PrintF( "%s/.local/share/%s/", Home, UserPathAppName.CStr() );
	}

	// Fall back to the local directory; same as legacy, but this shouldn't happen anyway
	return SimpleString( "./" );
#elif BUILD_WINDOWS
	return SimpleString::PrintF( "%s%s/", FileUtil::GetUserLocalAppDataPath().CStr(), UserPathAppName.CStr() );
#else
	// What even platform are we running here?
	return SimpleString( "./" );
#endif
}

// Save to working directory on Windows and Linux. On Mac, save in proper location.
/*virtual*/ SimpleString EldFramework::GetSaveLoadPath()
{
	return GetUserDataPath();
}

SimpleString EldFramework::GetLegacyUserDataPath()
{
#if BUILD_MAC
	// MACTODO: Make this part of FileUtil::GetUserLocalAppDataPath or whatever.
	return ObjCJunk::GetUserDirectory( "Eldritch" );
#elif BUILD_LINUX
	// LINUXTODO: Where do user files go in an arbitrary Linux distro? Something relative to /home?
	return SimpleString( "./" );
#elif BUILD_WINDOWS
	// Eldritch just keeps files in the local directory!
	return SimpleString( "./" );
#else
	// What even platform are we running here?
	return SimpleString( "./" );
#endif
}

SimpleString EldFramework::GetLegacySaveLoadPath()
{
	return GetLegacyUserDataPath();
}
