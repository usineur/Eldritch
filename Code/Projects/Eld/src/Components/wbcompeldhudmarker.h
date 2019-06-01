#ifndef WBCOMPELDHUDMARKER_H
#define WBCOMPELDHUDMARKER_H

#include "wbeldcomponent.h"

class WBCompEldHUDMarker : public WBEldComponent
{
public:
	WBCompEldHUDMarker();
	virtual ~WBCompEldHUDMarker();

	DEFINE_WBCOMP( EldHUDMarker, WBEldComponent );

	virtual int		GetTickOrder() { return ETO_NoTick; }

	virtual void	HandleEvent( const WBEvent& Event );

protected:
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			UpdateMarkerPosition() const;

	HashedString	m_UIScreenName;
	HashedString	m_UIWidgetName;
	HashedString	m_OccludedImage;
	HashedString	m_UnoccludedImage;
	float			m_FalloffRadius;
	float			m_OffsetZ;
};

#endif // WBCOMPELDHUDMARKER_H
