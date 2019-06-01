#ifndef UIINPUTMAPELD_H
#define UIINPUTMAPELD_H

#include "iuiinputmap.h"

class EldFramework;

class UIInputMapEld : public IUIInputMap
{
public:
	UIInputMapEld( EldFramework* const pFramework );
	virtual ~UIInputMapEld();

	virtual bool OnNext();
	virtual bool OnPrevious();
	virtual bool OnUp();
	virtual bool OnDown();
	virtual bool OnLeft();
	virtual bool OnRight();

	virtual bool OnPrevPage();
	virtual bool OnNextPage();

	virtual bool OnAccept();
	virtual bool OnCancel();

protected:
	EldFramework*	m_Framework;
};

#endif // UIINPUTMAPELD_H
