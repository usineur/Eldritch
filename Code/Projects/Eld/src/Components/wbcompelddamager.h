#ifndef WBCOMPELDDAMAGER_H
#define WBCOMPELDDAMAGER_H

#include "wbeldcomponent.h"
#include "simplestring.h"

class WBCompEldDamager : public WBEldComponent
{
public:
	WBCompEldDamager();
	virtual ~WBCompEldDamager();

	DEFINE_WBCOMP( EldDamager, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	SimpleString	GetDamagerName() const { return m_DamagerName; }

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	SimpleString	m_DamagerName;
};

#endif // WBCOMPELDDAMAGER_H
