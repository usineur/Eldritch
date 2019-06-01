#ifndef UISCREENELDLEADERBOARDS_H
#define UISCREENELDLEADERBOARDS_H

#include "uiscreen.h"
#include "Leaderboards/ileaderboards.h"
#include "iwbeventobserver.h"

// I'm going to hard-code a lot of this for my Eldritch use cases,
// because it will simplify things here and I don't expect to use
// "generalized" leaderboards any time soon.

class UIScreenEldLeaderboards : public UIScreen, public IWBEventObserver
{
public:
	UIScreenEldLeaderboards();
	virtual ~UIScreenEldLeaderboards();

	DEFINE_UISCREEN_FACTORY( EldLeaderboards );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

	virtual void	RegisterForEvents();
	virtual void	HandleEvent( const WBEvent& Event );

	virtual void	Pushed();
	virtual void	Popped();

	static void		LeaderboardDownloadCallback( void* pVoid );

	void			LaunchUserID( const uint Row );

protected:
	enum ELeaderboard
	{
		ELB_PlayTime,
		ELB_Souls,
		ELB_COUNT
	};

	enum EScoreMode
	{
		ESM_Global,
		ESM_Local,
		ESM_Friends,
		ESM_COUNT
	};

	ILeaderboards*	GetLeaderboards() const;
	void			MakeLeaderboardRequest();
	void			OnDownload();
	void			Publish();	// Push values from m_CurrentEntries into widgets

	SimpleString	GetLeaderboardName( const ELeaderboard Leaderboard ) const;
	SimpleString	GetScoreModeName( const EScoreMode ScoreMode ) const;

	void			InitializeRulesDefinition( const SimpleString& RulesDefinitionName );
	void			CreateRankWidgetDefinition();
	void			CreateUserNameWidgetDefinition();
	void			CreateScoreWidgetDefinition();
	void			CreateUserIDActionDefinition();
	void			CreateCompositeWidgetDefinition();
	void			CreateCompositeWidget();

	ELeaderboard	m_Leaderboard;
	EScoreMode		m_ScoreMode;
	int				m_ScoreIndex;	// (or offset, for ESM_Local)

	uint			m_NumRows;

	Array<SLeaderboardEntry>	m_CurrentEntries;

	// Stuff for initializing widgets
	uint			m_Row;
	float			m_Y;
	float			m_YBase;
	float			m_YStep;
	float			m_Column0X;
	float			m_Column1X;
	float			m_Column2X;
	SimpleString	m_ArchetypeName;
	SimpleString	m_Parent;
	SimpleString	m_ScoreOrigin;
	SimpleString	m_RankWidgetDefinitionName;
	SimpleString	m_UserNameWidgetDefinitionName;
	SimpleString	m_ScoreWidgetDefinitionName;
	SimpleString	m_UserIDActionDefinitionName;
	SimpleString	m_CompositeWidgetDefinitionName;
};

#endif // UISCREENELDLEADERBOARDS_H