#ifndef WBCOMPELDFACTION_H
#define WBCOMPELDFACTION_H

#include "wbeldcomponent.h"
#include "eldfactions.h"

class WBCompEldFaction : public WBEldComponent
{
public:
	WBCompEldFaction();
	virtual ~WBCompEldFaction();

	DEFINE_WBCOMP( EldFaction, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );
	virtual void	AddContextToEvent( WBEvent& Event ) const;

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

	EldFactions::EFactionCon		GetCon( const WBEntity* const pEntityB );
	EldFactions::EFactionCon		GetCon( const HashedString& FactionB );
	static EldFactions::EFactionCon	GetCon( const WBEntity* const pEntityA, const WBEntity* const pEntityB );

	HashedString	GetFaction() const { return m_Faction; }

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	HashedString	m_Faction;		// Serialized
	bool			m_Immutable;	// Config
};

#endif // WBCOMPELDFACTION_H
