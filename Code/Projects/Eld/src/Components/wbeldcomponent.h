#ifndef WBCOMPELD_H
#define WBCOMPELD_H

#include "wbcomponent.h"

class EldFramework;
class EldGame;
class EldWorld;
class EldCampaign;

class WBEldComponent : public WBComponent
{
public:
	WBEldComponent();
	virtual ~WBEldComponent();

protected:
	EldFramework*	GetFramework() const;
	EldGame*		GetGame() const;
	EldWorld*		GetWorld() const;
};

#endif // WBCOMPELD_H
