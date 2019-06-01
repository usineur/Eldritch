#ifndef WBCOMPELDPOWERTELEPORT_H
#define WBCOMPELDPOWERTELEPORT_H

#include "wbeldcomponent.h"
#include "wbentityref.h"

class WBCompEldPowerTeleport : public WBEldComponent
{
public:
	WBCompEldPowerTeleport();
	virtual ~WBCompEldPowerTeleport();

	DEFINE_WBCOMP( EldPowerTeleport, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

private:
	void			TryTeleport() const;

	WBEntityRef		m_Beacon;
};

#endif // WBCOMPELDPOWERTELEPORT_H
