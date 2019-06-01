#ifndef LEADERBOARDS_STEAM_H
#define LEADERBOARDS_STEAM_H

#include "ileaderboards.h"
#include "hashedstring.h"
#include "map.h"
#include "array.h"

#if BUILD_STEAM
#endif

// Steam made call results not assignable so my old method of using a list of them is broken.
// This *should* be fine, I don't think it's possible to spam these.
#define NUM_POOLED_CALL_RESULTS 8

class Leaderboards_Steam : public ILeaderboards
{
public:
	Leaderboards_Steam();
	virtual ~Leaderboards_Steam();

	virtual void	FindLeaderboard( const SimpleString& LeaderboardName );
	virtual void	PostScore( const HashedString& LeaderboardName, c_int32 Score );
	virtual void	RequestGlobalScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries );
	virtual void	RequestLocalScores( const HashedString& LeaderboardName, const int StartOffset, const int NumEntries );
	virtual void	RequestFriendScores( const HashedString& LeaderboardName, const int StartIndex, const int NumEntries );
	virtual void	GetRequestedScores( Array<SLeaderboardEntry>& OutEntries ) const;

	virtual void	RegisterUploadCallback( const SLeaderboardCallback& Callback );
	virtual void	UnregisterUploadCallback( const SLeaderboardCallback& Callback );
	virtual void	RegisterDownloadCallback( const SLeaderboardCallback& Callback );
	virtual void	UnregisterDownloadCallback( const SLeaderboardCallback& Callback );

protected:
	void			DispatchUploadCallbacks();
	void			DispatchDownloadCallbacks();

	Array<SLeaderboardCallback>	m_UploadCallbacks;
	Array<SLeaderboardCallback>	m_DownloadCallbacks;

#if BUILD_STEAM
#endif

public:
#if BUILD_STEAM
#endif
};

#endif // LEADERBOARDS_STEAM_H