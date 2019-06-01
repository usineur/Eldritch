#ifndef WBACTIONELDLAUNCHUSERID_H
#define WBACTIONELDLAUNCHUSERID_H

#include "wbaction.h"
#include "simplestring.h"

class WBActionEldLaunchUserID : public WBAction
{
public:
	WBActionEldLaunchUserID();
	virtual ~WBActionEldLaunchUserID();

	DEFINE_WBACTION_FACTORY( EldLaunchUserID );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

	virtual void	Execute();

private:
	uint			m_Row;
};

#endif // WBACTIONELDLAUNCHUSERID_H
