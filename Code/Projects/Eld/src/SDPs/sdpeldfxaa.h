#ifndef SDPELDFXAA_H
#define SDPELDFXAA_H

#include "SDPs/sdpbase.h"

class SDPEldFXAA : public SDPBase
{
public:
	SDPEldFXAA();
	virtual ~SDPEldFXAA();

	DEFINE_SDP_FACTORY( EldFXAA );

	virtual void	SetShaderParameters( IRenderer* const pRenderer, Mesh* const pMesh, const View& CurrentView ) const;
};

#endif // SDPELDFXAA_H
