#ifndef WBCOMPELDMARKUP_H
#define WBCOMPELDMARKUP_H

#include "wbeldcomponent.h"
#include "hashedstring.h"

class WBCompEldMarkup : public WBEldComponent
{
public:
	WBCompEldMarkup();
	virtual ~WBCompEldMarkup();

	DEFINE_WBCOMP( EldMarkup, WBEldComponent );

	virtual bool	BelongsInComponentArray() { return true; }

	virtual int		GetTickOrder() { return ETO_NoTick; }

	HashedString	GetMarkup() { return m_Markup; }

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	HashedString	m_Markup;
};

#endif // WBCOMPELDMARKUP_H
