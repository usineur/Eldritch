#ifndef WBCOMPELDICICLES_H
#define WBCOMPELDICICLES_H

#include "wbeldcomponent.h"

class WBCompEldIcicles : public WBEldComponent
{
public:
	WBCompEldIcicles();
	virtual ~WBCompEldIcicles();

	DEFINE_WBCOMP( EldIcicles, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_TickDefault; }
	virtual void	Tick( const float DeltaTime );

	virtual void	HandleEvent( const WBEvent& Event );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	float	m_CheckDistance;	// Config
};

#endif // WBCOMPELDICICLES_H
