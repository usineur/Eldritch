#ifndef UISCREENELDCREDITS_H
#define UISCREENELDCREDITS_H

#include "uiscreen.h"

class UIScreenEldCredits : public UIScreen
{
public:
	UIScreenEldCredits();
	virtual ~UIScreenEldCredits();

	DEFINE_UISCREEN_FACTORY( EldCredits );

	virtual void		InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual ETickReturn	Tick( const float DeltaTime, bool HasFocus );
	virtual void		Pushed();

private:
	bool			m_Repeat;
	HashedString	m_TextWidget;
};

#endif // UISCREENELDCREDITS_H
