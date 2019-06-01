#ifndef ADDWBACTIONFACTORY

#include "Actions/wbactioneldplayanim.h"
#include "Actions/wbactioneldplayhandanim.h"
#include "Actions/wbactioneldgiveitem.h"
#include "Actions/wbactioneldaddblock.h"
#include "Actions/wbactioneldremoveblock.h"
#include "Actions/wbactioneldspawnentity.h"
#include "Actions/wbactioneldstopmotion.h"
#include "Actions/wbactioneldsetlight.h"
#include "Actions/wbactioneldchecksphere.h"
#include "Actions/wbactioneldcheckcone.h"
#include "Actions/wbactioneldcheckline.h"
#include "Actions/wbactioneldgotolevel.h"
#include "Actions/wbactioneldbanktransaction.h"
#include "Actions/wbactioneldbindinput.h"
#include "Actions/wbactioneldtweetrip.h"
#include "Actions/wbactioneldshowbook.h"
#include "Actions/wbactioneldsetpersistentvar.h"
#include "Actions/wbactionelddamageworld.h"
#include "Actions/wbactioneldlaunchwebsite.h"
#include "Actions/wbactionneonawardachievement.h"
#include "Actions/wbactionneonincrementstat.h"
#include "Actions/wbactionneonsetstat.h"
#include "Actions/wbactioneldlaunchuserid.h"
#include "Actions/wbactioneldpostscore.h"
#include "Actions/wbactioneldopenuserdatapath.h"

#else

ADDWBACTIONFACTORY( EldPlayAnim );
ADDWBACTIONFACTORY( EldPlayHandAnim );
ADDWBACTIONFACTORY( EldGiveItem );
ADDWBACTIONFACTORY( EldAddBlock );
ADDWBACTIONFACTORY( EldRemoveBlock );
ADDWBACTIONFACTORY( EldSpawnEntity );
ADDWBACTIONFACTORY( EldStopMotion );
ADDWBACTIONFACTORY( EldSetLight );
ADDWBACTIONFACTORY( EldCheckSphere );
ADDWBACTIONFACTORY( EldCheckCone );
ADDWBACTIONFACTORY( EldCheckLine );
ADDWBACTIONFACTORY( EldGoToLevel );
ADDWBACTIONFACTORY( EldBankTransaction );
ADDWBACTIONFACTORY( EldBindInput );
ADDWBACTIONFACTORY( EldTweetRIP );
ADDWBACTIONFACTORY( EldShowBook );
ADDWBACTIONFACTORY( EldSetPersistentVar );
ADDWBACTIONFACTORY( EldDamageWorld );
ADDWBACTIONFACTORY( EldLaunchWebSite );
ADDWBACTIONFACTORY( NeonAwardAchievement );
ADDWBACTIONFACTORY( NeonIncrementStat );
ADDWBACTIONFACTORY( NeonSetStat );
ADDWBACTIONFACTORY( EldLaunchUserID );
ADDWBACTIONFACTORY( EldPostScore );
ADDWBACTIONFACTORY( EldOpenUserDataPath );

#endif
