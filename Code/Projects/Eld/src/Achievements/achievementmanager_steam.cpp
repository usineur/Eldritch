#include "core.h"
#include "achievementmanager_steam.h"
#include "simplestring.h"

AchievementManager_Steam::AchievementManager_Steam()
:	m_UserStats( NULL )
#if BUILD_STEAM
#endif // BUILD_STEAM
{
#if BUILD_STEAM
#endif // BUILD_STEAM
	RequestServerUpdate();
}

/*virtual*/ AchievementManager_Steam::~AchievementManager_Steam()
{
}

/*virtual*/ void AchievementManager_Steam::RequestServerUpdate()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

/* virtual*/ void AchievementManager_Steam::AwardAchievement( const SimpleString& AchievementTag )
{
	Unused( AchievementTag );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::IncrementStat( const SimpleString& StatTag, const int Amount )
{
	Unused( StatTag );
	Unused( Amount );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::SetStat( const SimpleString& StatTag, const int Value )
{
	Unused( StatTag );
	Unused( Value );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::Store()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

#if BUILD_DEV
/*virtual*/ void AchievementManager_Steam::ResetAllStats( const bool ResetAchievements )
{
	Unused( ResetAchievements );

#if BUILD_STEAM
#endif
}
#endif

/*virtual*/ void AchievementManager_Steam::ReportGlobalStat( const SimpleString& StatTag )
{
	Unused( StatTag );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::ReportGlobalAchievementRate( const SimpleString& AchievementTag )
{
	Unused( AchievementTag );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestUserStats()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestGlobalStats()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestGlobalAchievementRates()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

#if BUILD_STEAM
#endif // BUILD_STEAM

#if BUILD_STEAM
#endif // BUILD_STEAM

#if BUILD_STEAM
#endif // BUILD_STEAM