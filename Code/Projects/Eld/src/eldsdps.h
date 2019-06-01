#ifndef ADDSDPFACTORY

#include "SDPs/sdpeldhud.h"
#include "SDPs/sdpeldhudcalibration.h"
#include "SDPs/sdpeldpost.h"
#include "SDPs/sdpeldworld.h"
#include "SDPs/sdpeldlit.h"
#include "SDPs/sdpelddropshadow.h"
#include "SDPs/sdpeldfxaa.h"

#else

ADDSDPFACTORY( EldHUD );
ADDSDPFACTORY( EldHUDCalibration );
ADDSDPFACTORY( EldPost );
ADDSDPFACTORY( EldWorld );
ADDSDPFACTORY( EldLit );
ADDSDPFACTORY( EldDropShadow );
ADDSDPFACTORY( EldFXAA );

#endif
