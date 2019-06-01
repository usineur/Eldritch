#ifndef WBCOMPELDAZATHOTH_H
#define WBCOMPELDAZATHOTH_H

#include "wbeldcomponent.h"
#include "wbeventmanager.h"

class WBCompEldAzathoth : public WBEldComponent
{
public:
	WBCompEldAzathoth();
	virtual ~WBCompEldAzathoth();

	DEFINE_WBCOMP( EldAzathoth, WBEldComponent );

	virtual void	Tick( const float DeltaTime );
	virtual int		GetTickOrder() { return ETO_TickDefault; }

	virtual void	HandleEvent( const WBEvent& Event );

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			PublishToHUD() const;

	float			m_AzathothSpawnTime;
	TEventUID		m_AzathothSpawnEventUID;	// Serialized
};

#endif // WBCOMPELDAZATHOTH_H
