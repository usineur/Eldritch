#ifndef WBCOMPELDHARD_H
#define WBCOMPELDHARD_H

#include "wbeldcomponent.h"

class WBCompEldHard : public WBEldComponent
{
public:
	WBCompEldHard();
	virtual ~WBCompEldHard();

	DEFINE_WBCOMP( EldHard, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

	bool			IsHard() const { return m_Hard; }

private:
	void			PushPersistence() const;
	void			PullPersistence();

	bool			m_Hard;
};

#endif // WBCOMPELDHARD_H
