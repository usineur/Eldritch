#ifndef SDPELDHUDCALIBRATION_H
#define SDPELDHUDCALIBRATION_H

#include "SDPs/sdpeldhud.h"

class SDPEldHUDCalibration : public SDPEldHUD
{
public:
	SDPEldHUDCalibration();
	virtual ~SDPEldHUDCalibration();

	DEFINE_SDP_FACTORY( EldHUDCalibration );

	virtual void	SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const;
};

#endif // SDPELDHUDCALIBRATION_H
