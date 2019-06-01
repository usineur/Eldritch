#ifndef WBCOMPELDTHINKER_H
#define WBCOMPELDTHINKER_H

#include "wbeldcomponent.h"

class WBCompEldThinker : public WBEldComponent
{
public:
	WBCompEldThinker();
	virtual ~WBCompEldThinker();

	virtual void	HandleEvent( const WBEvent& Event );
};

#endif // WBCOMPELDTHINKER_H
