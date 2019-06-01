#ifndef ELDFRAMEWORK_H
#define ELDFRAMEWORK_H

#include "framework3d.h"

#if BUILD_STEAM
#include "steam/steam_api.h"
#endif

class EldGame;
class EldWorld;
class WBEventManager;
class View;
class InputSystem;
class Vector;
class Angles;
class EldTools;
class IDataStream;
class EldSound3DListener;
class Mesh;
class EldTargetManager;
class XInputController;
class IAchievementManager;
class ILeaderboards;

class EldFramework : public Framework3D
{
public:
	EldFramework();
	virtual ~EldFramework();

	// IWBEventObserver
	virtual void	HandleEvent( const WBEvent& Event );

	EldWorld*				GetWorld() const				{ return m_World; }
	EldGame*				GetGame() const					{ return m_Game; }
	EldTargetManager*		GetTargetManager() const		{ return m_TargetManager; }
	XInputController*		GetController() const			{ return m_Controller; }
	IAchievementManager*	GetAchievementManager() const	{ return m_AchievementManager; }
	ILeaderboards*			GetLeaderboards() const			{ return m_Leaderboards; }

#if BUILD_DEV
	EldTools*				GetTools() const				{ return m_Tools; }
#endif

	virtual InputSystem*	GetInputSystem() const			{ return m_InputSystem; }

	virtual SimpleString	GetUserDataPath();
	virtual SimpleString	GetSaveLoadPath();

	SimpleString			GetLegacyUserDataPath();
	SimpleString			GetLegacySaveLoadPath();

	View*			GetMainView() const				{ return m_MainView; }
	View*			GetFGView() const				{ return m_FGView; }
	void			SetMainViewTransform( const Vector& Location, const Angles& Orientation );

	// This strictly sets the FOV for the views; it does not set the config var or publish the FOV.
	void			SetFOV( const float FOV );
	void			SetFGFOV( const float FGFOV );

	void			RegenerateWorld();
	void			GoToLevel( const HashedString& WorldDef );

	void			PrepareForLoad();
	void			InitializeTools();

	// Singleton accessor
	static EldFramework*	GetInstance();
	static void				SetInstance( EldFramework* const pFramework );

	void			RequestRenderTick() { m_SimTickHasRequestedRenderTick = true; }

protected:
	virtual void	Initialize();
	virtual void	ShutDown();

	void			InitializePackagesAndConfig();	// Can be called again to reinit and hotload
	void			InitializePackages();
	void			InitializeDLC();

	void			InitializeWorld( const HashedString& WorldDef, const bool CreateWorld );
	void			ShutDownWorld();

	void			LoadPrefsConfig();
	void			WritePrefsConfig();

	void			PushDefaultOptions();

	void			RegisterForEvents();

	void			HandleUISliderEvent( const HashedString& SliderName, const float SliderValue );

	bool			CanPause() const;
	void			TryPause();

	void			TryFinishInputBinding();

	float			GetMouseSpeedFromSliderValue( const float SliderValue );
	float			GetSliderValueFromMouseSpeed( const float MouseSpeed );
	float			GetControllerSpeedFromSliderValue( const float SliderValue );
	float			GetSliderValueFromControllerSpeed( const float ControllerSpeed );
	float			GetBrightnessFromSliderValue( const float SliderValue );
	float			GetSliderValueFromBrightness( const float MouseSpeed );
	float			GetFOVFromSliderValue( const float SliderValue );
	float			GetSliderValueFromFOV( const float FOV );

	virtual bool	TickSim( const float DeltaTime );
	virtual bool	TickGame( const float DeltaTime );
	virtual void	OnUnpaused();
	virtual void	TickDevices();
	virtual bool	TickInput( const float DeltaTime, const bool UIHasFocus );
	virtual void	TickPausedInput( const float DeltaTime );
	virtual void	TickRender();

	virtual bool	SimTickHasRequestedRenderTick() const { return m_SimTickHasRequestedRenderTick; }

	virtual void	GetInitialWindowTitle( SimpleString& WindowTitle );
	virtual void	GetInitialWindowIcon( uint& WindowIcon );
	virtual void	GetUIManagerDefinitionName( SimpleString& DefinitionName );
	virtual void	InitializeUIInputMap();
	virtual bool	ShowWindowASAP() { return false; }
	virtual void	InitializeAudioSystem();

	void			CreateBuckets();
	void			UpdateViews();
	void			CreateHUDView();
	void			CreateMirrorView();
	void			CreateMinimapView();
	virtual void	ToggleFullscreen();
	virtual void	ToggleVSync();
	void			ToggleFXAA();
	virtual void	SetResolution( const uint DisplayWidth, const uint DisplayHeight );
	virtual void	RefreshDisplay( const bool Fullscreen, const bool VSync, const uint DisplayWidth, const uint DisplayHeight );

	void			PublishDisplayedBrightness() const;
	void			PublishDisplayedFOV() const;

	void			OnHideHUDChanged();

	void			OnInvertYChanged();
	void			OnControllerTypeChanged();

	static void		OnSetRes( void* pUIElement, void* pVoid );
	static void		OnFadeFinished( void* pUIElement, void* pVoid );	// TODO: This was used for... something?

private:
	EldGame*				m_Game;
	EldWorld*				m_World;
#if BUILD_DEV
	EldTools*				m_Tools;
#endif
	XInputController*		m_Controller;
	InputSystem*			m_InputSystem;

	uint					m_DisplayWidth;
	uint					m_DisplayHeight;

	EldTargetManager*		m_TargetManager;

	View*					m_MainView;
	View*					m_FGView;
	View*					m_HUDView;
	View*					m_MirrorView;
	View*					m_MirrorBView;
	View*					m_MinimapView;
	View*					m_UpscaleView;

	EldSound3DListener*		m_Audio3DListener;

	IAchievementManager*	m_AchievementManager;
	ILeaderboards*			m_Leaderboards;

	// HACKHACK to force loading screen to display (TODO: add a loading screen to Eldritch!)
	bool					m_SimTickHasRequestedRenderTick;

	bool					m_PauseOnLostFocus;
	bool					m_MuteWhenUnfocused;
};

#endif // ELDFRAMEWORK_H