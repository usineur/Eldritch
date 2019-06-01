#ifndef WBCOMPELDSOULS_H
#define WBCOMPELDSOULS_H

#include "wbeldcomponent.h"
#include "wbeventmanager.h"

class WBCompEldSouls : public WBEldComponent
{
public:
	WBCompEldSouls();
	virtual ~WBCompEldSouls();

	DEFINE_WBCOMP( EldSouls, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );
	virtual void	AddContextToEvent( WBEvent& Event ) const;

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

	uint			GetSouls() const { return m_Souls; }
	void			AddSouls( const uint UnmoddedSouls, const bool ShowPickupScreen );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			PublishToHUD() const;
	void			PushPersistence() const;
	void			PullPersistence();

	uint	m_Souls;

	// Pickup UI management
	float		m_HidePickupScreenDelay;
	TEventUID	m_HidePickupScreenUID;
};

#endif // WBCOMPELDSOULS_H
