#ifndef ACHIEVEMENTMANAGER_STEAM_H
#define ACHIEVEMENTMANAGER_STEAM_H

#include "iachievementmanager.h"
#include "hashedstring.h"
#include "map.h"

class ISteamUserStats;

#if BUILD_STEAM
#endif

class AchievementManager_Steam : public IAchievementManager
{
public:
	AchievementManager_Steam();
	virtual ~AchievementManager_Steam();

	virtual void	RequestServerUpdate();

	virtual void	AwardAchievement( const SimpleString& AchievementTag );
	virtual void	IncrementStat( const SimpleString& StatTag, const int Amount );
	virtual void	SetStat( const SimpleString& StatTag, const int Value );
	virtual void	Store();

#if BUILD_DEV
	virtual void	ResetAllStats( const bool ResetAchievements );
#endif

	virtual void	ReportGlobalStat( const SimpleString& StatTag );
	virtual void	ReportGlobalAchievementRate( const SimpleString& AchievementTag );

protected:
	void				RequestUserStats();
	void				RequestGlobalStats();
	void				RequestGlobalAchievementRates();

	ISteamUserStats*	m_UserStats;

public:
#if BUILD_STEAM
#endif
};

#endif // ACHIEVEMENTMANAGER_STEAM_H