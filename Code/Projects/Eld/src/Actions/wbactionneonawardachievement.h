#ifndef WBACTIONNEONAWARDACHIEVEMENT_H
#define WBACTIONNEONAWARDACHIEVEMENT_H

#include "wbaction.h"
#include "simplestring.h"

class WBActionNeonAwardAchievement : public WBAction
{
public:
	WBActionNeonAwardAchievement();
	virtual ~WBActionNeonAwardAchievement();

	DEFINE_WBACTION_FACTORY( NeonAwardAchievement );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	Execute();

private:
	SimpleString	m_AchievementTag;
};

#endif // WBACTIONNEONAWARDACHIEVEMENT_H
