#ifndef WBCOMPELDHARDLISTENER_H
#define WBCOMPELDHARDLISTENER_H

#include "wbeldcomponent.h"

class WBCompEldHardListener : public WBEldComponent
{
public:
	WBCompEldHardListener();
	virtual ~WBCompEldHardListener();

	DEFINE_WBCOMP( EldHardListener, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

private:
	void			RegisterForEvents();
	void			UnregisterForEvents();
};

#endif // WBCOMPELDHARDLISTENER_H
