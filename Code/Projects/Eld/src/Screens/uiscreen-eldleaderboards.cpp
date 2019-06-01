#include "core.h"
#include "uiscreen-eldleaderboards.h"
#include "Leaderboards/ileaderboards.h"
#include "eldframework.h"
#include "uiwidget.h"
#include "configmanager.h"
#include "wbworld.h"
#include "wbeventmanager.h"
#include "uifactory.h"

#if BUILD_STEAM
#include "steam/steam_api.h"
#endif

UIScreenEldLeaderboards::UIScreenEldLeaderboards()
:	m_Leaderboard( ELB_PlayTime )
,	m_ScoreMode( ESM_Global )
,	m_ScoreIndex( 1 )
,	m_NumRows( 0 )
,	m_CurrentEntries()
,	m_Row( 0 )
,	m_Y( 0.0f )
,	m_YBase( 0.0f )
,	m_YStep( 0.0f )
,	m_Column0X( 0.0f )
,	m_Column1X( 0.0f )
,	m_Column2X( 0.0f )
,	m_ArchetypeName()
,	m_Parent()
,	m_ScoreOrigin()
,	m_RankWidgetDefinitionName()
,	m_UserNameWidgetDefinitionName()
,	m_ScoreWidgetDefinitionName()
,	m_UserIDActionDefinitionName()
,	m_CompositeWidgetDefinitionName()
{
}

UIScreenEldLeaderboards::~UIScreenEldLeaderboards()
{
}

/*virtual*/ void UIScreenEldLeaderboards::RegisterForEvents()
{
	STATIC_HASHED_STRING( LeaderboardsPrevBoard );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsPrevBoard, this, NULL );

	STATIC_HASHED_STRING( LeaderboardsNextBoard );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsNextBoard, this, NULL );

	STATIC_HASHED_STRING( LeaderboardsPrevMode );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsPrevMode, this, NULL );

	STATIC_HASHED_STRING( LeaderboardsNextMode );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsNextMode, this, NULL );

	STATIC_HASHED_STRING( LeaderboardsGoToPlayTime );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsGoToPlayTime, this, NULL );

	STATIC_HASHED_STRING( LeaderboardsGoToSouls );
	WBWorld::GetInstance()->GetEventManager()->AddObserver( sLeaderboardsGoToSouls, this, NULL );
}

/*virtual*/ void UIScreenEldLeaderboards::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	FlushWidgets();

	UIScreen::InitializeFromDefinition( DefinitionName );

	MAKEHASH( DefinitionName );

	STATICHASH( Rules );
	const SimpleString RulesDefinitionName = ConfigManager::GetString( sRules, "", sDefinitionName );
	InitializeRulesDefinition( RulesDefinitionName );

	for( m_Row = 0; m_Row < m_NumRows; ++m_Row )
	{
		m_Y = m_YBase + m_Row * m_YStep;

		CreateRankWidgetDefinition();
		CreateUserNameWidgetDefinition();
		CreateScoreWidgetDefinition();
		CreateUserIDActionDefinition();
		CreateCompositeWidgetDefinition();
		CreateCompositeWidget();
	}

	UpdateRender();
}

void UIScreenEldLeaderboards::InitializeRulesDefinition( const SimpleString& RulesDefinitionName )
{
	MAKEHASH( RulesDefinitionName );

	STATICHASH( Archetype );
	m_ArchetypeName = ConfigManager::GetString( sArchetype, "", sRulesDefinitionName );

	STATICHASH( Parent );
	m_Parent = ConfigManager::GetString( sParent, "", sRulesDefinitionName );

	STATICHASH( ScoreOrigin );
	m_ScoreOrigin = ConfigManager::GetString( sScoreOrigin, "", sRulesDefinitionName );

	STATICHASH( PixelYBase );
	m_YBase = ConfigManager::GetFloat( sPixelYBase, 0.0f, sRulesDefinitionName );

	STATICHASH( PixelYStep );
	m_YStep = ConfigManager::GetFloat( sPixelYStep, 0.0f, sRulesDefinitionName );

	STATICHASH( Column0PixelX );
	m_Column0X = ConfigManager::GetFloat( sColumn0PixelX, 0.0f, sRulesDefinitionName );

	STATICHASH( Column1PixelX );
	m_Column1X = ConfigManager::GetFloat( sColumn1PixelX, 0.0f, sRulesDefinitionName );

	STATICHASH( Column2PixelX );
	m_Column2X = ConfigManager::GetFloat( sColumn2PixelX, 0.0f, sRulesDefinitionName );

	STATICHASH( NumRows );
	m_NumRows = ConfigManager::GetInt( sNumRows, 0, sRulesDefinitionName );
}

void UIScreenEldLeaderboards::CreateRankWidgetDefinition()
{
	m_RankWidgetDefinitionName		= SimpleString::PrintF( "_LB_Rank%d", m_Row );
	const SimpleString	RankString	= SimpleString::PrintF( "#{i:Leaderboards:Rank%d}", m_Row );

	MAKEHASH( m_RankWidgetDefinitionName );

	STATICHASH( UIWidgetType );
	ConfigManager::SetString( sUIWidgetType, "Text", sm_RankWidgetDefinitionName );

	STATICHASH( Extends );
	ConfigManager::SetString( sExtends, m_ArchetypeName.CStr(), sm_RankWidgetDefinitionName );

	STATICHASH( Parent );
	ConfigManager::SetString( sParent, m_Parent.CStr(), sm_RankWidgetDefinitionName );

	STATICHASH( IsLiteral );
	ConfigManager::SetBool( sIsLiteral, true, sm_RankWidgetDefinitionName );

	STATICHASH( DynamicString );
	ConfigManager::SetString( sDynamicString, RankString.CStr(), sm_RankWidgetDefinitionName );

	STATICHASH( PixelX );
	ConfigManager::SetFloat( sPixelX, m_Column0X, sm_RankWidgetDefinitionName );

	STATICHASH( PixelY );
	ConfigManager::SetFloat( sPixelY, m_Y, sm_RankWidgetDefinitionName );
}

void UIScreenEldLeaderboards::CreateUserNameWidgetDefinition()
{
	m_UserNameWidgetDefinitionName		= SimpleString::PrintF( "_LB_UserName%d", m_Row );
	const SimpleString	UserNameString	= SimpleString::PrintF( "#{s:Leaderboards:UserName%d}", m_Row );

	MAKEHASH( m_UserNameWidgetDefinitionName );

	STATICHASH( UIWidgetType );
	ConfigManager::SetString( sUIWidgetType, "Text", sm_UserNameWidgetDefinitionName );

	STATICHASH( Extends );
	ConfigManager::SetString( sExtends, m_ArchetypeName.CStr(), sm_UserNameWidgetDefinitionName );

	STATICHASH( Parent );
	ConfigManager::SetString( sParent, m_Parent.CStr(), sm_UserNameWidgetDefinitionName );

	STATICHASH( IsLiteral );
	ConfigManager::SetBool( sIsLiteral, true, sm_UserNameWidgetDefinitionName );

	STATICHASH( DynamicString );
	ConfigManager::SetString( sDynamicString, UserNameString.CStr(), sm_UserNameWidgetDefinitionName );

	STATICHASH( PixelX );
	ConfigManager::SetFloat( sPixelX, m_Column1X, sm_UserNameWidgetDefinitionName );

	STATICHASH( PixelY );
	ConfigManager::SetFloat( sPixelY, m_Y, sm_UserNameWidgetDefinitionName );
}

void UIScreenEldLeaderboards::CreateScoreWidgetDefinition()
{
	m_ScoreWidgetDefinitionName		= SimpleString::PrintF( "_LB_Score%d", m_Row );
	const SimpleString	ScoreString	= SimpleString::PrintF( "#{s:Leaderboards:Score%d}", m_Row );

	MAKEHASH( m_ScoreWidgetDefinitionName );

	STATICHASH( UIWidgetType );
	ConfigManager::SetString( sUIWidgetType, "Text", sm_ScoreWidgetDefinitionName );

	STATICHASH( Extends );
	ConfigManager::SetString( sExtends, m_ArchetypeName.CStr(), sm_ScoreWidgetDefinitionName );

	STATICHASH( Parent );
	ConfigManager::SetString( sParent, m_Parent.CStr(), sm_ScoreWidgetDefinitionName );

	STATICHASH( IsLiteral );
	ConfigManager::SetBool( sIsLiteral, true, sm_ScoreWidgetDefinitionName );

	STATICHASH( DynamicString );
	ConfigManager::SetString( sDynamicString, ScoreString.CStr(), sm_ScoreWidgetDefinitionName );

	STATICHASH( Origin );
	ConfigManager::SetString( sOrigin, m_ScoreOrigin.CStr(), sm_ScoreWidgetDefinitionName );

	STATICHASH( PixelX );
	ConfigManager::SetFloat( sPixelX, m_Column2X, sm_ScoreWidgetDefinitionName );

	STATICHASH( PixelY );
	ConfigManager::SetFloat( sPixelY, m_Y, sm_ScoreWidgetDefinitionName );
}

void UIScreenEldLeaderboards::CreateUserIDActionDefinition()
{
	m_UserIDActionDefinitionName = SimpleString::PrintF( "_LB_UserIDAction%d", m_Row );

	MAKEHASH( m_UserIDActionDefinitionName );

	STATICHASH( ActionType );
	ConfigManager::SetString( sActionType, "EldLaunchUserID", sm_UserIDActionDefinitionName );

	STATICHASH( Row );
	ConfigManager::SetInt( sRow, m_Row, sm_UserIDActionDefinitionName );
}

void UIScreenEldLeaderboards::CreateCompositeWidgetDefinition()
{
	m_CompositeWidgetDefinitionName = SimpleString::PrintF( "_LB_Composite%d", m_Row );

	MAKEHASH( m_CompositeWidgetDefinitionName );

	STATICHASH( UIWidgetType );
	ConfigManager::SetString( sUIWidgetType, "Composite", sm_CompositeWidgetDefinitionName );

	STATICHASH( Focus );
	ConfigManager::SetBool( sFocus, true, sm_CompositeWidgetDefinitionName );

	STATICHASH( NumChildren );
	ConfigManager::SetInt( sNumChildren, 3, sm_CompositeWidgetDefinitionName );

	STATICHASH( Child0 );
	ConfigManager::SetString( sChild0, m_RankWidgetDefinitionName.CStr(), sm_CompositeWidgetDefinitionName );

	STATICHASH( Child1 );
	ConfigManager::SetString( sChild1, m_UserNameWidgetDefinitionName.CStr(), sm_CompositeWidgetDefinitionName );

	STATICHASH( Child2 );
	ConfigManager::SetString( sChild2, m_ScoreWidgetDefinitionName.CStr(), sm_CompositeWidgetDefinitionName );

	STATICHASH( NumActions );
	ConfigManager::SetInt( sNumActions, 1, sm_CompositeWidgetDefinitionName );

	STATICHASH( Action0 );
	ConfigManager::SetString( sAction0, m_UserIDActionDefinitionName.CStr(), sm_CompositeWidgetDefinitionName );
}

void UIScreenEldLeaderboards::CreateCompositeWidget()
{
	UIWidget* const pCompositeWidget = UIFactory::CreateWidget( m_CompositeWidgetDefinitionName, this, NULL );
	DEVASSERT( pCompositeWidget );

	AddWidget( pCompositeWidget );
}

/*virtual*/ void UIScreenEldLeaderboards::HandleEvent( const WBEvent& Event )
{
	XTRACE_FUNCTION;

	STATIC_HASHED_STRING( LeaderboardsPrevBoard );
	STATIC_HASHED_STRING( LeaderboardsNextBoard );
	STATIC_HASHED_STRING( LeaderboardsPrevMode );
	STATIC_HASHED_STRING( LeaderboardsNextMode );
	STATIC_HASHED_STRING( LeaderboardsGoToPlayTime );
	STATIC_HASHED_STRING( LeaderboardsGoToSouls );

	const HashedString EventName = Event.GetEventName();
	if( EventName == sLeaderboardsPrevBoard )
	{
		m_Leaderboard	= static_cast<ELeaderboard>( ( m_Leaderboard + ELB_COUNT - 1 ) % ELB_COUNT );
		m_ScoreMode		= ESM_Local;
		m_ScoreIndex	= -4;
		MakeLeaderboardRequest();
	}
	else if( EventName == sLeaderboardsNextBoard )
	{
		m_Leaderboard	= static_cast<ELeaderboard>( ( m_Leaderboard + 1 ) % ELB_COUNT );
		m_ScoreMode		= ESM_Local;
		m_ScoreIndex	= -4;
		MakeLeaderboardRequest();
	}
	else if( EventName == sLeaderboardsPrevMode )
	{
		m_ScoreMode		= static_cast<EScoreMode>( ( m_ScoreMode + ESM_COUNT - 1 ) % ESM_COUNT );
		m_ScoreIndex	= ( m_ScoreMode == ESM_Local ) ? -4 : 1;
		MakeLeaderboardRequest();
	}
	else if( EventName == sLeaderboardsNextMode )
	{
		m_ScoreMode		= static_cast<EScoreMode>( ( m_ScoreMode + 1 ) % ESM_COUNT );
		m_ScoreIndex	= ( m_ScoreMode == ESM_Local ) ? -4 : 1;
		MakeLeaderboardRequest();
	}
	else if( EventName == sLeaderboardsGoToPlayTime )
	{
		m_Leaderboard	= ELB_PlayTime;
		m_ScoreMode		= ESM_Local;
		m_ScoreIndex	= -4;
		MakeLeaderboardRequest();
	}
	else if( EventName == sLeaderboardsGoToSouls )
	{
		m_Leaderboard	= ELB_Souls;
		m_ScoreMode		= ESM_Local;
		m_ScoreIndex	= -4;
		MakeLeaderboardRequest();
	}
}

ILeaderboards* UIScreenEldLeaderboards::GetLeaderboards() const
{
	EldFramework* const pFramework = EldFramework::GetInstance();
	DEVASSERT( pFramework );

	ILeaderboards* const pLeaderboards = pFramework->GetLeaderboards();
	DEVASSERT( pLeaderboards );

	return pLeaderboards;
}

SimpleString UIScreenEldLeaderboards::GetLeaderboardName( const ELeaderboard Leaderboard ) const
{
	if( Leaderboard == ELB_PlayTime )
	{
		return "LB_PlayTime";
	}
	else
	{
		DEVASSERT( Leaderboard == ELB_Souls );
		return "LB_Souls";
	}
}

SimpleString UIScreenEldLeaderboards::GetScoreModeName( const EScoreMode ScoreMode ) const
{
	if( ScoreMode == ESM_Global )
	{
		return "LBM_Global";
	}
	else if( ScoreMode == ESM_Local )
	{
		return "LBM_Local";
	}
	else
	{
		DEVASSERT( ScoreMode == ESM_Friends );
		return "LBM_Friends";
	}
}

void UIScreenEldLeaderboards::MakeLeaderboardRequest()
{
	m_CurrentEntries.Clear();
	Publish();

	const HashedString LeaderboardName = GetLeaderboardName( m_Leaderboard );

	if( m_ScoreMode == ESM_Global )
	{
		GetLeaderboards()->RequestGlobalScores( LeaderboardName, m_ScoreIndex, m_NumRows );
	}
	else if( m_ScoreMode == ESM_Local )
	{
		GetLeaderboards()->RequestLocalScores( LeaderboardName, m_ScoreIndex, m_NumRows );
	}
	else
	{
		DEVASSERT( m_ScoreMode == ESM_Friends );
		GetLeaderboards()->RequestFriendScores( LeaderboardName, m_ScoreIndex, m_NumRows );
	}
}

/*virtual*/ void UIScreenEldLeaderboards::Pushed()
{
	UIScreen::Pushed();

	SLeaderboardCallback Callback;
	Callback.m_Callback	= LeaderboardDownloadCallback;
	Callback.m_Void		= this;
	GetLeaderboards()->RegisterDownloadCallback( Callback );

	MakeLeaderboardRequest();
}

/*virtual*/ void UIScreenEldLeaderboards::Popped()
{
	UIScreen::Popped();

	SLeaderboardCallback Callback;
	Callback.m_Callback	= LeaderboardDownloadCallback;
	Callback.m_Void		= this;
	GetLeaderboards()->UnregisterDownloadCallback( Callback );
}

/*static*/ void UIScreenEldLeaderboards::LeaderboardDownloadCallback( void* pVoid )
{
	UIScreenEldLeaderboards* const pScreen = static_cast<UIScreenEldLeaderboards*>( pVoid );
	DEVASSERT( pScreen );

	pScreen->OnDownload();
}

void UIScreenEldLeaderboards::OnDownload()
{
	GetLeaderboards()->GetRequestedScores( m_CurrentEntries );
	DEVASSERT( m_CurrentEntries.Size() <= m_NumRows );

	Publish();
}

void UIScreenEldLeaderboards::Publish()
{
	STATICHASH( Leaderboards );
	STATICHASH( Header );
	ConfigManager::SetString( sHeader, GetLeaderboardName( m_Leaderboard ).CStr(), sLeaderboards );

	STATICHASH( Footer );
	ConfigManager::SetString( sFooter, GetScoreModeName( m_ScoreMode ).CStr(), sLeaderboards );

	FOR_EACH_ARRAY( LeaderboardIter, m_CurrentEntries, SLeaderboardEntry )
	{
		const SLeaderboardEntry&	Entry	= LeaderboardIter.GetValue();
		const uint					Index	= LeaderboardIter.GetIndex();

		const SimpleString RankString = SimpleString::PrintF( "Rank%d", Index );
		MAKEHASH( RankString );
		ConfigManager::SetInt( sRankString, Entry.m_Rank, sLeaderboards );

		const SimpleString UserNameString = SimpleString::PrintF( "UserName%d", Index );
		MAKEHASH( UserNameString );
		ConfigManager::SetString( sUserNameString, Entry.m_UserName.CStr(), sLeaderboards );

		const SimpleString	ScoreString		= SimpleString::PrintF( "Score%d", Index );
		const SimpleString	ScoreAsNumber	= SimpleString::PrintF( "%d", Entry.m_Score );
		const SimpleString	ScoreAsTime		= SimpleString::PrintF( "%dm %ds", Entry.m_Score / 60, Entry.m_Score % 60 );
		const SimpleString&	UsingScore		= ( m_Leaderboard == ELB_PlayTime ) ? ScoreAsTime : ScoreAsNumber;
		MAKEHASH( ScoreString );
		ConfigManager::SetString( sScoreString, UsingScore.CStr(), sLeaderboards );
	}

	for( uint Row = 0; Row < m_NumRows; ++Row )
	{
		const bool			ShouldHide						= Row >= m_CurrentEntries.Size();
		const HashedString	CompositeWidgetDefinitionName	= SimpleString::PrintF( "_LB_Composite%d", Row );
		UIWidget* const		pCompositeWidget				= GetWidget( CompositeWidgetDefinitionName );
		pCompositeWidget->SetHidden( ShouldHide );
	}
}

void UIScreenEldLeaderboards::LaunchUserID( const uint Row )
{
	Unused( Row );

#if BUILD_STEAM
	if( Row >= m_CurrentEntries.Size() )
	{
		return;
	}

	const SLeaderboardEntry& Entry = m_CurrentEntries[ Row ];
	if( !Entry.m_SteamID.IsValid() )
	{
		return;
	}

	if( !SteamFriends() )
	{
		return;
	}

	SteamFriends()->ActivateGameOverlayToUser( "steamid", Entry.m_SteamID );
#endif
}