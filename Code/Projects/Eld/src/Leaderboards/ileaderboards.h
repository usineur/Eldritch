#ifndef ILEADERBOARDS_H
#define ILEADERBOARDS_H

#include "eldframework.h"
#include "simplestring.h"

#if BUILD_STEAM
#include "steam/steam_api.h"
#endif

#if 1 || BUILD_FINAL
#define POST_SCORE( board, score ) \
	ILeaderboards* const pLeaderboards = EldFramework::GetInstance()->GetLeaderboards(); \
	ASSERTDESC( NULL != pLeaderboards, "Leaderboards did not exist when score was posted!" ); \
	if( pLeaderboards ) { pLeaderboards->PostScore( ( board ), ( score ) ); }
#else
#define POST_SCORE( board, score ) DoNothing
#endif

// HACKHACK: The size of this structure changes iff BUILD_STEAM. It's not serialized, so whatever, it shouldn't matter.
struct SLeaderboardEntry
{
	SLeaderboardEntry()
	:	m_UserName()
	,	m_Rank( 0 )
	,	m_Score( 0 )
#if BUILD_STEAM
	,	m_SteamID()
#endif
	{
	}

	SimpleString	m_UserName;
	int				m_Rank;
	int				m_Score;
#if BUILD_STEAM
	CSteamID		m_SteamID;
#endif
};

typedef void ( *LeaderboardCallback )( void* );
struct SLeaderboardCallback
{
	bool operator==( const SLeaderboardCallback& Other ) const
	{
		return ( m_Void == Other.m_Void && m_Callback == Other.m_Callback );
	}
	LeaderboardCallback	m_Callback;
	void*				m_Void;
};

class ILeaderboards
{
public:
	virtual ~ILeaderboards()
	{
	}

	// Used to enumerate leaderboards at the start of the game. No notification when it is found.
	virtual void	FindLeaderboard( const SimpleString& LeaderboardName ) = 0;

	virtual void	PostScore( const HashedString& LeaderboardName, c_int32 Score ) = 0;

	virtual void	RequestGlobalScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries ) = 0;
	virtual void	RequestLocalScores( const HashedString& LeaderboardName, const int StartOffset, const int NumEntries ) = 0;
	virtual void	RequestFriendScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries ) = 0;

	virtual void	GetRequestedScores( Array<SLeaderboardEntry>& OutEntries ) const = 0;

	virtual void	RegisterUploadCallback( const SLeaderboardCallback& Callback ) = 0;
	virtual void	UnregisterUploadCallback( const SLeaderboardCallback& Callback ) = 0;
	virtual void	RegisterDownloadCallback( const SLeaderboardCallback& Callback ) = 0;
	virtual void	UnregisterDownloadCallback( const SLeaderboardCallback& Callback ) = 0;
};

#endif // ILEADERBOARDS_H