#ifndef ELDBANK_H
#define ELDBANK_H

#include "iwbeventobserver.h"

class IDataStream;

class EldBank : public IWBEventObserver
{
public:
	EldBank();
	~EldBank();

	// IWBEventObserver
	virtual void	HandleEvent( const WBEvent& Event );

	void			RegisterForEvents();

	uint			GetBankMoney() const;
	void			SetBankMoney( const uint Money );

	bool			HasBankMoney( uint Money ) { return Money <= GetBankMoney(); }
	void			AddBankMoney( uint Money );
	void			RemoveBankMoney( uint Money );

private:
	void			TryBankTransaction( const int Amount );
	void			PublishToHUD() const;
};

#endif // ELDBANK_H