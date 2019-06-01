#ifndef WBCOMPELDCLIMBABLE_H
#define WBCOMPELDCLIMBABLE_H

#include "wbeldcomponent.h"
#include "plane.h"

class WBCompEldClimbable : public WBEldComponent
{
public:
	WBCompEldClimbable();
	virtual ~WBCompEldClimbable();

	DEFINE_WBCOMP( EldClimbable, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );
	virtual void	AddContextToEvent( WBEvent& Event ) const;

	virtual uint	GetSerializationSize();
	virtual void	Save( const IDataStream& Stream );
	virtual void	Load( const IDataStream& Stream );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			InitializeSnapPlane();

	bool			m_UseSnapPlane;	// Config
	Plane			m_SnapPlane;	// Serialized
};

#endif // WBCOMPELDCLIMBABLE_H
