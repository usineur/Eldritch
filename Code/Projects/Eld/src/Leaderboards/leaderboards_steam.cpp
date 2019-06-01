#include "core.h"
#include "leaderboards_steam.h"
#include "simplestring.h"
#include "mathcore.h"

Leaderboards_Steam::Leaderboards_Steam()
:	m_UploadCallbacks()
,	m_DownloadCallbacks()
#if BUILD_STEAM
#endif // BUILD_STEAM
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ Leaderboards_Steam::~Leaderboards_Steam()
{
}

void Leaderboards_Steam::FindLeaderboard( const SimpleString& LeaderboardName )
{
	Unused( LeaderboardName );

#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::PostScore( const HashedString& LeaderboardName, c_int32 Score )
{
	Unused( LeaderboardName );
	Unused( Score );

#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::RequestGlobalScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries )
{
	Unused( LeaderboardName );
	Unused( StartIndex );
	Unused( NumEntries );

#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::RequestLocalScores( const HashedString& LeaderboardName, const int StartOffset, const int NumEntries )
{
	Unused( LeaderboardName );
	Unused( StartOffset );
	Unused( NumEntries );

#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::RequestFriendScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries )
{
	Unused( LeaderboardName );
	Unused( StartIndex );
	Unused( NumEntries );

#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::GetRequestedScores( Array<SLeaderboardEntry>& OutEntries ) const
{
	// A simple array assignment just does a shallow memcpy. I need a deep copy here.
	OutEntries.Clear();
#if BUILD_STEAM
#endif
}

/*virtual*/ void Leaderboards_Steam::RegisterUploadCallback( const SLeaderboardCallback& Callback )
{
	m_UploadCallbacks.PushBack( Callback );
}

/*virtual*/ void Leaderboards_Steam::UnregisterUploadCallback( const SLeaderboardCallback& Callback )
{
	m_UploadCallbacks.RemoveItem( Callback );
}

/*virtual*/ void Leaderboards_Steam::RegisterDownloadCallback( const SLeaderboardCallback& Callback )
{
	m_DownloadCallbacks.PushBack( Callback );
}

/*virtual*/ void Leaderboards_Steam::UnregisterDownloadCallback( const SLeaderboardCallback& Callback )
{
	m_DownloadCallbacks.RemoveItem( Callback );
}

void Leaderboards_Steam::DispatchUploadCallbacks()
{
	FOR_EACH_ARRAY( CallbackIter, m_UploadCallbacks, SLeaderboardCallback )
	{
		const SLeaderboardCallback& Callback = CallbackIter.GetValue();
		Callback.m_Callback( Callback.m_Void );
	}
}

void Leaderboards_Steam::DispatchDownloadCallbacks()
{
	FOR_EACH_ARRAY( CallbackIter, m_DownloadCallbacks, SLeaderboardCallback )
	{
		const SLeaderboardCallback& Callback = CallbackIter.GetValue();
		Callback.m_Callback( Callback.m_Void );
	}
}

#if BUILD_STEAM
#endif // BUILD_STEAM