#ifndef WBACTIONELDOPENUSERDATAPATH_H
#define WBACTIONELDOPENUSERDATAPATH_H

#include "wbaction.h"

class WBActionEldOpenUserDataPath : public WBAction
{
public:
	WBActionEldOpenUserDataPath();
	virtual ~WBActionEldOpenUserDataPath();

	DEFINE_WBACTION_FACTORY( EldOpenUserDataPath );

	virtual void	Execute();
};

#endif // WBACTIONELDOPENUSERDATAPATH_H
