#ifndef WBCOMPELDLOCK_H
#define WBCOMPELDLOCK_H

#include "wbeldcomponent.h"

class WBCompEldLock : public WBEldComponent
{
public:
	WBCompEldLock();
	virtual ~WBCompEldLock();

	DEFINE_WBCOMP( EldLock, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );
	virtual void	AddContextToEvent( WBEvent& Event ) const;

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			Unlock();

	bool			m_Locked;
	HashedString	m_Key;
};

#endif // WBCOMPELDLOCK_H
