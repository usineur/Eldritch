#ifndef ELDGAME_H
#define ELDGAME_H

#include "iwbeventobserver.h"
#include "wbevent.h"
#include "vector2.h"

class EldSaveLoad;
class EldPersistence;
class EldBank;
class EldMusic;
class Mesh;
class ITexture;
class UIScreenEldMirror;
class UIWidgetImage;

typedef WBEvent TPersistence;

class EldGame : public IWBEventObserver
{
public:
	EldGame();
	~EldGame();

	void					Initialize();
	void					ShutDown();

	void					Tick();
	void					Render() const;

	const SimpleString&		GetCurrentLevelName() const { return m_CurrentLevelName; }
	void					SetCurrentLevelName( const SimpleString& LevelName );

	TPersistence&			GetTravelPersistence() { return m_TravelPersistence; }
	static TPersistence&	StaticGetTravelPersistence();

	EldSaveLoad*		GetSaveLoad() const { return m_SaveLoad; }
	EldPersistence*	GetPersistence() const { return m_GenerationPersistence; }
	EldBank*			GetBank() const { return m_Bank; }
	EldMusic*			GetMusic() const { return m_Music; }

	void					Checkpoint() const;

	void					RefreshRTDependentSystems();
	Mesh*					CreateFullscreenQuad( const uint Width, const uint Height, const HashedString& PrescribedBucket, const SimpleString& MaterialDef );
	void					CreatePostQuad();
	void					CreateFXAAQuad();
	void					CreateMirrorPostQuad();
	void					CreateMirrorFXAAQuad();
	void					CreateUpscaleQuad();
	void					UpdateMirror();
	void					UpdateMinimap();
	Mesh*					GetPostQuad() const { return m_PostQuad; }
	Mesh*					GetFXAAQuad() const { return m_FXAAQuad; }
	Mesh*					GetMirrorPostQuad() const { return m_MirrorPostQuad; }
	Mesh*					GetMirrorFXAAQuad() const { return m_MirrorFXAAQuad; }
	Mesh*					GetUpscaleQuad() const { return m_UpscaleQuad; }
	void					SetColorGradingTexture( const SimpleString& TextureFilename );

	void					SetGamma( const float Gamma ) { m_Gamma = Gamma; }
	float					GetGamma() const { return m_Gamma; }

	void					SetFogParams( const float FogNear, const float FogFar, const SimpleString& FogTextureFilename );
	Vector2					GetFogParams() const { return m_FogParams; }
	ITexture*				GetFogTexture() const { return m_FogTexture; }

	void					SetCurrentMusic( const SimpleString& Music ) { m_CurrentMusic = Music; }

	// IWBEventObserver
	virtual void			HandleEvent( const WBEvent& Event );

	SimpleString			GetHubLevelName() const;
	SimpleString			GetAsylumHubLevelName() const;
	bool					IsInHub() const;
	void					SetUIReturnToHubDisabled( const bool Disabled );
	void					RefreshUIReturnToHubEnabled();

	void					LaunchWebSite();
	void					OpenUserDataPath();

	void					LaunchRIPTweet();
	const SimpleString		GetRIPDamage();
	const SimpleString		GetRIPLevel();

	// Helper function, because where else would it go
	static float			GetPlayTime();
	static WBEntity*		GetPlayer();
	static Vector			GetPlayerLocation();
	static Vector			GetPlayerViewLocation();
	static Angles			GetPlayerViewOrientation();
	static bool				IsPlayerAlive();
	static bool				IsPlayerDisablingPause();
	static bool				IsPlayerVisible();

	// More helpers
	static UIScreenEldMirror*	GetMirrorScreen();
	static UIWidgetImage*		GetMinimapImage();

private:
	void					ClearTravelPersistence();

	void					RequestReturnToHub( const bool Restart, const bool FlushHub );
	void					RequestReturnToAsylumHub();	// HACKHACK
	void					RequestGoToNextLevel();
	void					RequestGoToPrevLevel();
	void					RequestGoToLevel( const SimpleString& NextLevel, const HashedString& NextWorldDef, const bool RestoreSpawnPoint );
	void					GoToLevel();

	SimpleString			DecorateWorldFileName( const SimpleString& LevelName ) const;

	bool					m_GoToLevelOnNextTick;
	bool					m_IsRestarting;
	bool					m_FlushHubOnRestart;
	bool					m_RestoreSpawnPoint;
	SimpleString			m_NextLevelName;
	HashedString			m_NextWorldDef;

	SimpleString			m_CurrentLevelName;

	TPersistence			m_TravelPersistence;		// Travel persistence propagates world state data between worlds

	EldSaveLoad*		m_SaveLoad;
	EldPersistence*	m_GenerationPersistence;	// Generation persistence saves progress beyond death
	EldBank*			m_Bank;
	EldMusic*			m_Music;

	float					m_Gamma;
	Mesh*					m_PostQuad;
	Mesh*					m_FXAAQuad;
	Mesh*					m_MirrorPostQuad;
	Mesh*					m_MirrorFXAAQuad;
	Mesh*					m_UpscaleQuad;
	SimpleString			m_ColorGradingTexture;
	ITexture*				m_FogTexture;
	Vector2					m_FogParams;	// x = near, y = 1/(far-near)
	SimpleString			m_CurrentMusic;
};

#endif // ELDGAME_H