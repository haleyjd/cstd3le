// Copyright (C) 2004 Id Software, Inc.
//

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

#if defined( _DEBUG )
	#define	BUILD_DEBUG	"-debug"
#else
	//#modified-fva; BEGIN
	//#define	BUILD_DEBUG "-release"
	#define	BUILD_DEBUG ""
	//#modified-fva; END
#endif

/*

All game cvars should be defined here.

*/

#ifdef CTF
const char *si_gameTypeArgs[]		= { "singleplayer", "deathmatch", "Tourney", "Team DM", "Last Man", "CTF", NULL };
#else
const char *si_gameTypeArgs[]		= { "singleplayer", "deathmatch", "Tourney", "Team DM", "Last Man", NULL };
#endif

const char *si_readyArgs[]			= { "Not Ready", "Ready", NULL }; 
const char *si_spectateArgs[]		= { "Play", "Spectate", NULL };

#ifdef _D3XP
const char *ui_skinArgs[]			= { "skins/characters/player/marine_mp", "skins/characters/player/marine_mp_red", "skins/characters/player/marine_mp_blue", "skins/characters/player/marine_mp_green", "skins/characters/player/marine_mp_yellow", "skins/characters/player/marine_mp_purple", "skins/characters/player/marine_mp_grey", "skins/characters/player/marine_mp_orange", NULL };
#else
const char *ui_skinArgs[]			= { "skins/characters/player/marine_mp", "skins/characters/player/marine_mp_red", "skins/characters/player/marine_mp_blue", "skins/characters/player/marine_mp_green", "skins/characters/player/marine_mp_yellow", NULL };
#endif

const char *ui_teamArgs[]			= { "Red", "Blue", NULL }; 

struct gameVersion_s {
	//#modified-fva; BEGIN
	//gameVersion_s( void ) { sprintf( string, "%s.%d%s %s %s", ENGINE_VERSION, BUILD_NUMBER, BUILD_DEBUG, BUILD_STRING, __DATE__, __TIME__ ); }
	gameVersion_s(void) { sprintf(string, "%s.%d%s %s %s %s", ENGINE_VERSION, BUILD_NUMBER, BUILD_DEBUG, BUILD_STRING, __DATE__, __TIME__); }
	//#modified-fva; END
	char	string[256];
} gameVersion;

idCVar g_version(					"g_version",				gameVersion.string,	CVAR_GAME | CVAR_ROM, "game version" );

// noset vars
idCVar gamename(					"gamename",					GAME_VERSION,	CVAR_GAME | CVAR_SERVERINFO | CVAR_ROM, "" );
idCVar gamedate(					"gamedate",					__DATE__,		CVAR_GAME | CVAR_ROM, "" );

// server info
idCVar si_name(						"si_name",					"DOOM Server",	CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE, "name of the server" );

#ifdef CTF
idCVar si_gameType(					"si_gameType",		si_gameTypeArgs[ 0 ],	CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE, "game type - singleplayer, deathmatch, Tourney, Team DM, Last Man or CTF", si_gameTypeArgs, idCmdSystem::ArgCompletion_String<si_gameTypeArgs> );
#else
idCVar si_gameType(					"si_gameType",		si_gameTypeArgs[ 0 ],	CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE, "game type - singleplayer, deathmatch, Tourney, Team DM or Last Man", si_gameTypeArgs, idCmdSystem::ArgCompletion_String<si_gameTypeArgs> );
#endif

idCVar si_map(						"si_map",					"game/mp/d3dm1",CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE, "map to be played next on server", idCmdSystem::ArgCompletion_MapName );
//#modified-fva; BEGIN
//idCVar si_maxPlayers(				"si_maxPlayers",			"8",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "max number of players allowed on the server", 1, 8 );
#define CST_QUOTES_AUX(x) #x
#define CST_QUOTES(x) CST_QUOTES_AUX(x)
idCVar si_maxPlayers("si_maxPlayers", CST_QUOTES(CST_MAX_PLAYERS), CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "max number of players allowed on the server", 1, CST_MAX_PLAYERS);
#undef CST_QUOTES
#undef CST_QUOTES_AUX
//#modified-fva; END
idCVar si_fragLimit(				"si_fragLimit",				"10",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "frag limit", 1, MP_PLAYER_MAXFRAGS );
idCVar si_timeLimit(				"si_timeLimit",				"10",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "time limit in minutes", 0, 60 );
idCVar si_teamDamage(				"si_teamDamage",			"0",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "enable team damage" );
idCVar si_warmup(					"si_warmup",				"0",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "do pre-game warmup" );
idCVar si_usePass(					"si_usePass",				"0",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "enable client password checking" );
idCVar si_pure(						"si_pure",					"1",			CVAR_GAME | CVAR_SERVERINFO | CVAR_BOOL, "server is pure and does not allow modified data" );
idCVar si_spectators(				"si_spectators",			"1",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "allow spectators or require all clients to play" );
idCVar si_serverURL(				"si_serverURL",				"",				CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE, "where to reach the server admins and get information about the server" );

#ifdef CTF
//idCVar si_pointLimit(				"si_pointlimit",			"8",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "team points limit to win in CTF" );
idCVar si_flagDropTimeLimit(		"si_flagDropTimeLimit",		"30",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "seconds before a dropped CTF flag is returned" );
idCVar si_midnight(                 "si_midnight",              "0",            CVAR_GAME | CVAR_INTEGER | CVAR_SERVERINFO, "Start the game up in midnight CTF (completely dark)" );
#endif


// user info
idCVar ui_name(						"ui_name",					"Player",		CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE, "player name" );
idCVar ui_skin(						"ui_skin",				ui_skinArgs[ 0 ],	CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE, "player skin", ui_skinArgs, idCmdSystem::ArgCompletion_String<ui_skinArgs> );
idCVar ui_team(						"ui_team",				ui_teamArgs[ 0 ],	CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE, "player team", ui_teamArgs, idCmdSystem::ArgCompletion_String<ui_teamArgs> ); 
idCVar ui_autoSwitch(				"ui_autoSwitch",			"1",			CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE | CVAR_BOOL, "auto switch weapon" );
idCVar ui_autoReload(				"ui_autoReload",			"1",			CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE | CVAR_BOOL, "auto reload weapon" );
idCVar ui_showGun(					"ui_showGun",				"1",			CVAR_GAME | CVAR_USERINFO | CVAR_ARCHIVE | CVAR_BOOL, "show gun" );
idCVar ui_ready(					"ui_ready",				si_readyArgs[ 0 ],	CVAR_GAME | CVAR_USERINFO, "player is ready to start playing", idCmdSystem::ArgCompletion_String<si_readyArgs> );
idCVar ui_spectate(					"ui_spectate",		si_spectateArgs[ 0 ],	CVAR_GAME | CVAR_USERINFO, "play or spectate", idCmdSystem::ArgCompletion_String<si_spectateArgs> );
idCVar ui_chat(						"ui_chat",					"0",			CVAR_GAME | CVAR_USERINFO | CVAR_BOOL | CVAR_ROM | CVAR_CHEAT, "player is chatting" );

// change anytime vars
idCVar developer(					"developer",				"0",			CVAR_GAME | CVAR_BOOL, "" );

//#modified-fva; BEGIN
//idCVar r_aspectRatio( 				"r_aspectRatio",			"0",			CVAR_RENDERER | CVAR_INTEGER | CVAR_ARCHIVE, "aspect ratio of view:\n0 = 4:3\n1 = 16:9\n2 = 16:10", 0, 2 );
idCVar cst_aspectRatio("cst_aspectRatio", "-1", CVAR_RENDERER | CVAR_INTEGER | CVAR_NOCHEAT, "aspect ratio of view:\n-1 = Automatic\n0 = 4:3\n1 = 16:9\n2 = 16:10", -1, 2);
//#modified-fva; END

idCVar g_cinematic(					"g_cinematic",				"1",			CVAR_GAME | CVAR_BOOL, "skips updating entities that aren't marked 'cinematic' '1' during cinematics" );
idCVar g_cinematicMaxSkipTime(		"g_cinematicMaxSkipTime",	"600",			CVAR_GAME | CVAR_FLOAT, "# of seconds to allow game to run when skipping cinematic.  prevents lock-up when cinematic doesn't end.", 0, 3600 );

idCVar g_muzzleFlash(				"g_muzzleFlash",			"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show muzzle flashes" );
idCVar g_projectileLights(			"g_projectileLights",		"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show dynamic lights on projectiles" );
idCVar g_bloodEffects(				"g_bloodEffects",			"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show blood splats, sprays and gibs" );
idCVar g_doubleVision(				"g_doubleVision",			"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show double vision when taking damage" );
idCVar g_monsters(					"g_monsters",				"1",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_decals(					"g_decals",					"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show decals such as bullet holes" );
idCVar g_knockback(					"g_knockback",				"1000",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar g_skill(						"g_skill",					"1",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar g_nightmare(					"g_nightmare",				"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "if nightmare mode is allowed" );
idCVar g_gravity(					"g_gravity",		DEFAULT_GRAVITY_STRING, CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_skipFX(					"g_skipFX",					"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_skipParticles(				"g_skipParticles",			"0",			CVAR_GAME | CVAR_BOOL, "" );

idCVar g_disasm(					"g_disasm",					"0",			CVAR_GAME | CVAR_BOOL, "disassemble script into base/script/disasm.txt on the local drive when script is compiled" );
idCVar g_debugBounds(				"g_debugBounds",			"0",			CVAR_GAME | CVAR_BOOL, "checks for models with bounds > 2048" );
idCVar g_debugAnim(					"g_debugAnim",				"-1",			CVAR_GAME | CVAR_INTEGER, "displays information on which animations are playing on the specified entity number.  set to -1 to disable." );
idCVar g_debugMove(					"g_debugMove",				"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugDamage(				"g_debugDamage",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugWeapon(				"g_debugWeapon",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugScript(				"g_debugScript",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugMover(				"g_debugMover",				"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugTriggers(				"g_debugTriggers",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_debugCinematic(			"g_debugCinematic",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_stopTime(					"g_stopTime",				"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_damageScale(				"g_damageScale",			"1",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "scale final damage on player by this factor" );
idCVar g_armorProtection(			"g_armorProtection",		"0.3",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "armor takes this percentage of damage" );
idCVar g_armorProtectionMP(			"g_armorProtectionMP",		"0.6",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "armor takes this percentage of damage in mp" );
idCVar g_useDynamicProtection(		"g_useDynamicProtection",	"1",			CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "scale damage and armor dynamically to keep the player alive more often" );
idCVar g_healthTakeTime(			"g_healthTakeTime",			"5",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how often to take health in nightmare mode" );
idCVar g_healthTakeAmt(				"g_healthTakeAmt",			"5",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how much health to take in nightmare mode" );
idCVar g_healthTakeLimit(			"g_healthTakeLimit",		"25",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how low can health get taken in nightmare mode" );



idCVar g_showPVS(					"g_showPVS",				"0",			CVAR_GAME | CVAR_INTEGER, "", 0, 2 );
idCVar g_showTargets(				"g_showTargets",			"0",			CVAR_GAME | CVAR_BOOL, "draws entities and thier targets.  hidden entities are drawn grey." );
idCVar g_showTriggers(				"g_showTriggers",			"0",			CVAR_GAME | CVAR_BOOL, "draws trigger entities (orange) and thier targets (green).  disabled triggers are drawn grey." );
idCVar g_showCollisionWorld(		"g_showCollisionWorld",		"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_showCollisionModels(		"g_showCollisionModels",	"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_showCollisionTraces(		"g_showCollisionTraces",	"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_maxShowDistance(			"g_maxShowDistance",		"128",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_showEntityInfo(			"g_showEntityInfo",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_showviewpos(				"g_showviewpos",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_showcamerainfo(			"g_showcamerainfo",			"0",			CVAR_GAME | CVAR_ARCHIVE, "displays the current frame # for the camera when playing cinematics" );
idCVar g_showTestModelFrame(		"g_showTestModelFrame",		"0",			CVAR_GAME | CVAR_BOOL, "displays the current animation and frame # for testmodels" );
idCVar g_showActiveEntities(		"g_showActiveEntities",		"0",			CVAR_GAME | CVAR_BOOL, "draws boxes around thinking entities.  dormant entities (outside of pvs) are drawn yellow.  non-dormant are green." );
idCVar g_showEnemies(				"g_showEnemies",			"0",			CVAR_GAME | CVAR_BOOL, "draws boxes around monsters that have targeted the the player" );

idCVar g_frametime(					"g_frametime",				"0",			CVAR_GAME | CVAR_BOOL, "displays timing information for each game frame" );
idCVar g_timeentities(				"g_timeEntities",			"0",			CVAR_GAME | CVAR_FLOAT, "when non-zero, shows entities whose think functions exceeded the # of milliseconds specified" );

#ifdef _D3XP
idCVar g_testPistolFlashlight(		"g_testPistolFlashlight",	"1",			CVAR_GAME | CVAR_BOOL, "Test out having a flashlight out with the pistol" );
idCVar g_debugShockwave(			"g_debugShockwave",			"0",			CVAR_GAME | CVAR_BOOL, "Debug the shockwave" );

idCVar g_enableSlowmo(				"g_enableSlowmo",			"0",			CVAR_GAME | CVAR_BOOL, "for testing purposes only" );
idCVar g_slowmoStepRate(			"g_slowmoStepRate",			"0.02",			CVAR_GAME | CVAR_FLOAT, "" );

idCVar g_enablePortalSky(			"g_enablePortalSky",		"1",			CVAR_GAME | CVAR_BOOL, "enables the portal sky" );
idCVar g_testFullscreenFX(			"g_testFullscreenFX",		"-1",			CVAR_GAME | CVAR_INTEGER, "index will activate specific fx, -2 is for all on, -1 is off" );
idCVar g_testHelltimeFX(			"g_testHelltimeFX",			"-1",			CVAR_GAME | CVAR_INTEGER, "set to 0, 1, 2 to test helltime, -1 is off" );
idCVar g_testMultiplayerFX(			"g_testMultiplayerFX",		"-1",			CVAR_GAME | CVAR_INTEGER, "set to 0, 1, 2 to test multiplayer, -1 is off" );
idCVar g_lowresFullscreenFX(		"g_lowresFullscreenFX",		"0",			CVAR_GAME | CVAR_BOOL, "enable lores mode for fx" );

idCVar g_moveableDamageScale(		"g_moveableDamageScale",	"0.1",			CVAR_GAME | CVAR_FLOAT, "scales damage wrt mass of object in multiplayer" );

idCVar g_testBloomSpeed(			"g_testBloomSpeed",			"1",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_testBloomIntensity(		"g_testBloomIntensity",		"-0.01",		CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_testBloomNumPasses(		"g_testBloomNumPasses",		"30",			CVAR_GAME | CVAR_INTEGER, "" );
#endif

idCVar ai_debugScript(				"ai_debugScript",			"-1",			CVAR_GAME | CVAR_INTEGER, "displays script calls for the specified monster entity number" );
idCVar ai_debugMove(				"ai_debugMove",				"0",			CVAR_GAME | CVAR_BOOL, "draws movement information for monsters" );
idCVar ai_debugTrajectory(			"ai_debugTrajectory",		"0",			CVAR_GAME | CVAR_BOOL, "draws trajectory tests for monsters" );
idCVar ai_testPredictPath(			"ai_testPredictPath",		"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar ai_showCombatNodes(			"ai_showCombatNodes",		"0",			CVAR_GAME | CVAR_BOOL, "draws attack cones for monsters" );
idCVar ai_showPaths(				"ai_showPaths",				"0",			CVAR_GAME | CVAR_BOOL, "draws path_* entities" );
idCVar ai_showObstacleAvoidance(	"ai_showObstacleAvoidance",	"0",			CVAR_GAME | CVAR_INTEGER, "draws obstacle avoidance information for monsters.  if 2, draws obstacles for player, as well", 0, 2, idCmdSystem::ArgCompletion_Integer<0,2> );
idCVar ai_blockedFailSafe(			"ai_blockedFailSafe",		"1",			CVAR_GAME | CVAR_BOOL, "enable blocked fail safe handling" );

#ifdef _D3XP
idCVar ai_showHealth(				"ai_showHealth",			"0",			CVAR_GAME | CVAR_BOOL, "Draws the AI's health above its head" );
#endif

idCVar g_dvTime(					"g_dvTime",					"1",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_dvAmplitude(				"g_dvAmplitude",			"0.001",		CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_dvFrequency(				"g_dvFrequency",			"0.5",			CVAR_GAME | CVAR_FLOAT, "" );

idCVar g_kickTime(					"g_kickTime",				"1",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_kickAmplitude(				"g_kickAmplitude",			"0.0001",		CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_blobTime(					"g_blobTime",				"1",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_blobSize(					"g_blobSize",				"1",			CVAR_GAME | CVAR_FLOAT, "" );

idCVar g_testHealthVision(			"g_testHealthVision",		"0",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_editEntityMode(			"g_editEntityMode",			"0",			CVAR_GAME | CVAR_INTEGER,	"0 = off\n"
																											"1 = lights\n"
																											"2 = sounds\n"
																											"3 = articulated figures\n"
																											"4 = particle systems\n"
																											"5 = monsters\n"
																											"6 = entity names\n"
																											"7 = entity models", 0, 7, idCmdSystem::ArgCompletion_Integer<0,7> );
idCVar g_dragEntity(				"g_dragEntity",				"0",			CVAR_GAME | CVAR_BOOL, "allows dragging physics objects around by placing the crosshair over them and holding the fire button" );
idCVar g_dragDamping(				"g_dragDamping",			"0.5",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_dragShowSelection(			"g_dragShowSelection",		"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_dropItemRotation(			"g_dropItemRotation",		"",				CVAR_GAME, "" );

#ifdef CTF
// Note: These cvars do not necessarily need to be in the shipping game. 
idCVar g_flagAttachJoint( "g_flagAttachJoint", "Chest", CVAR_GAME | CVAR_CHEAT, "player joint to attach CTF flag to" );
idCVar g_flagAttachOffsetX( "g_flagAttachOffsetX", "8", CVAR_GAME | CVAR_CHEAT, "X offset of CTF flag when carried" );
idCVar g_flagAttachOffsetY( "g_flagAttachOffsetY", "4", CVAR_GAME | CVAR_CHEAT, "Y offset of CTF flag when carried" );
idCVar g_flagAttachOffsetZ( "g_flagAttachOffsetZ", "-12", CVAR_GAME | CVAR_CHEAT, "Z offset of CTF flag when carried" );
idCVar g_flagAttachAngleX( "g_flagAttachAngleX", "90", CVAR_GAME | CVAR_CHEAT, "X angle of CTF flag when carried" );
idCVar g_flagAttachAngleY( "g_flagAttachAngleY", "25", CVAR_GAME | CVAR_CHEAT, "Y angle of CTF flag when carried" );
idCVar g_flagAttachAngleZ( "g_flagAttachAngleZ", "-90", CVAR_GAME | CVAR_CHEAT, "Z angle of CTF flag when carried" );
#endif


idCVar g_vehicleVelocity(			"g_vehicleVelocity",		"1000",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleForce(				"g_vehicleForce",			"50000",		CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleSuspensionUp(		"g_vehicleSuspensionUp",	"32",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleSuspensionDown(		"g_vehicleSuspensionDown",	"20",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleSuspensionKCompress("g_vehicleSuspensionKCompress","200",		CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleSuspensionDamping(	"g_vehicleSuspensionDamping","400",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_vehicleTireFriction(		"g_vehicleTireFriction",	"0.8",			CVAR_GAME | CVAR_FLOAT, "" );
#ifdef _D3XP
idCVar g_vehicleDebug(				"g_vehicleDebug",			"0",			CVAR_GAME | CVAR_BOOL, "" );
#endif

idCVar ik_enable(					"ik_enable",				"1",			CVAR_GAME | CVAR_BOOL, "enable IK" );
idCVar ik_debug(					"ik_debug",					"0",			CVAR_GAME | CVAR_BOOL, "show IK debug lines" );

idCVar af_useLinearTime(			"af_useLinearTime",			"1",			CVAR_GAME | CVAR_BOOL, "use linear time algorithm for tree-like structures" );
idCVar af_useImpulseFriction(		"af_useImpulseFriction",	"0",			CVAR_GAME | CVAR_BOOL, "use impulse based contact friction" );
idCVar af_useJointImpulseFriction(	"af_useJointImpulseFriction","0",			CVAR_GAME | CVAR_BOOL, "use impulse based joint friction" );
idCVar af_useSymmetry(				"af_useSymmetry",			"1",			CVAR_GAME | CVAR_BOOL, "use constraint matrix symmetry" );
idCVar af_skipSelfCollision(		"af_skipSelfCollision",		"0",			CVAR_GAME | CVAR_BOOL, "skip self collision detection" );
idCVar af_skipLimits(				"af_skipLimits",			"0",			CVAR_GAME | CVAR_BOOL, "skip joint limits" );
idCVar af_skipFriction(				"af_skipFriction",			"0",			CVAR_GAME | CVAR_BOOL, "skip friction" );
idCVar af_forceFriction(			"af_forceFriction",			"-1",			CVAR_GAME | CVAR_FLOAT, "force the given friction value" );
idCVar af_maxLinearVelocity(		"af_maxLinearVelocity",		"128",			CVAR_GAME | CVAR_FLOAT, "maximum linear velocity" );
idCVar af_maxAngularVelocity(		"af_maxAngularVelocity",	"1.57",			CVAR_GAME | CVAR_FLOAT, "maximum angular velocity" );
idCVar af_timeScale(				"af_timeScale",				"1",			CVAR_GAME | CVAR_FLOAT, "scales the time" );
idCVar af_jointFrictionScale(		"af_jointFrictionScale",	"0",			CVAR_GAME | CVAR_FLOAT, "scales the joint friction" );
idCVar af_contactFrictionScale(		"af_contactFrictionScale",	"0",			CVAR_GAME | CVAR_FLOAT, "scales the contact friction" );
idCVar af_highlightBody(			"af_highlightBody",			"",				CVAR_GAME, "name of the body to highlight" );
idCVar af_highlightConstraint(		"af_highlightConstraint",	"",				CVAR_GAME, "name of the constraint to highlight" );
idCVar af_showTimings(				"af_showTimings",			"0",			CVAR_GAME | CVAR_BOOL, "show articulated figure cpu usage" );
idCVar af_showConstraints(			"af_showConstraints",		"0",			CVAR_GAME | CVAR_BOOL, "show constraints" );
idCVar af_showConstraintNames(		"af_showConstraintNames",	"0",			CVAR_GAME | CVAR_BOOL, "show constraint names" );
idCVar af_showConstrainedBodies(	"af_showConstrainedBodies",	"0",			CVAR_GAME | CVAR_BOOL, "show the two bodies contrained by the highlighted constraint" );
idCVar af_showPrimaryOnly(			"af_showPrimaryOnly",		"0",			CVAR_GAME | CVAR_BOOL, "show primary constraints only" );
idCVar af_showTrees(				"af_showTrees",				"0",			CVAR_GAME | CVAR_BOOL, "show tree-like structures" );
idCVar af_showLimits(				"af_showLimits",			"0",			CVAR_GAME | CVAR_BOOL, "show joint limits" );
idCVar af_showBodies(				"af_showBodies",			"0",			CVAR_GAME | CVAR_BOOL, "show bodies" );
idCVar af_showBodyNames(			"af_showBodyNames",			"0",			CVAR_GAME | CVAR_BOOL, "show body names" );
idCVar af_showMass(					"af_showMass",				"0",			CVAR_GAME | CVAR_BOOL, "show the mass of each body" );
idCVar af_showTotalMass(			"af_showTotalMass",			"0",			CVAR_GAME | CVAR_BOOL, "show the total mass of each articulated figure" );
idCVar af_showInertia(				"af_showInertia",			"0",			CVAR_GAME | CVAR_BOOL, "show the inertia tensor of each body" );
idCVar af_showVelocity(				"af_showVelocity",			"0",			CVAR_GAME | CVAR_BOOL, "show the velocity of each body" );
idCVar af_showActive(				"af_showActive",			"0",			CVAR_GAME | CVAR_BOOL, "show tree-like structures of articulated figures not at rest" );
idCVar af_testSolid(				"af_testSolid",				"1",			CVAR_GAME | CVAR_BOOL, "test for bodies initially stuck in solid" );

idCVar rb_showTimings(				"rb_showTimings",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid body cpu usage" );
idCVar rb_showBodies(				"rb_showBodies",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid bodies" );
idCVar rb_showMass(					"rb_showMass",				"0",			CVAR_GAME | CVAR_BOOL, "show the mass of each rigid body" );
idCVar rb_showInertia(				"rb_showInertia",			"0",			CVAR_GAME | CVAR_BOOL, "show the inertia tensor of each rigid body" );
idCVar rb_showVelocity(				"rb_showVelocity",			"0",			CVAR_GAME | CVAR_BOOL, "show the velocity of each rigid body" );
idCVar rb_showActive(				"rb_showActive",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid bodies that are not at rest" );

// The default values for player movement cvars are set in def/player.def
idCVar pm_jumpheight(				"pm_jumpheight",			"48",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "approximate hieght the player can jump" );
idCVar pm_stepsize(					"pm_stepsize",				"16",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "maximum height the player can step up without jumping" );
idCVar pm_crouchspeed(				"pm_crouchspeed",			"80",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while crouched" );
idCVar pm_walkspeed(				"pm_walkspeed",				"140",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while walking" );
idCVar pm_runspeed(					"pm_runspeed",				"220",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while running" );
idCVar pm_noclipspeed(				"pm_noclipspeed",			"200",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while in noclip" );
idCVar pm_spectatespeed(			"pm_spectatespeed",			"450",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while spectating" );
idCVar pm_spectatebbox(				"pm_spectatebbox",			"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "size of the spectator bounding box" );
idCVar pm_usecylinder(				"pm_usecylinder",			"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "use a cylinder approximation instead of a bounding box for player collision detection" );
idCVar pm_minviewpitch(				"pm_minviewpitch",			"-89",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "amount player's view can look up (negative values are up)" );
idCVar pm_maxviewpitch(				"pm_maxviewpitch",			"89",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "amount player's view can look down" );
idCVar pm_stamina(					"pm_stamina",				"24",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "length of time player can run" );
idCVar pm_staminathreshold(			"pm_staminathreshold",		"45",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "when stamina drops below this value, player gradually slows to a walk" );
idCVar pm_staminarate(				"pm_staminarate",			"0.75",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "rate that player regains stamina. divide pm_stamina by this value to determine how long it takes to fully recharge." );
idCVar pm_crouchheight(				"pm_crouchheight",			"38",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while crouched" );
idCVar pm_crouchviewheight(			"pm_crouchviewheight",		"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while crouched" );
idCVar pm_normalheight(				"pm_normalheight",			"74",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while standing" );
idCVar pm_normalviewheight(			"pm_normalviewheight",		"68",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while standing" );
idCVar pm_deadheight(				"pm_deadheight",			"20",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while dead" );
idCVar pm_deadviewheight(			"pm_deadviewheight",		"10",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while dead" );
idCVar pm_crouchrate(				"pm_crouchrate",			"0.87",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "time it takes for player's view to change from standing to crouching" );
idCVar pm_bboxwidth(				"pm_bboxwidth",				"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "x/y size of player's bounding box" );
idCVar pm_crouchbob(				"pm_crouchbob",				"0.5",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob much faster when crouched" );
idCVar pm_walkbob(					"pm_walkbob",				"0.3",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob slowly when walking" );
idCVar pm_runbob(					"pm_runbob",				"0.4",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob faster when running" );
idCVar pm_runpitch(					"pm_runpitch",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
idCVar pm_runroll(					"pm_runroll",				"0.005",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
idCVar pm_bobup(					"pm_bobup",					"0.005",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
idCVar pm_bobpitch(					"pm_bobpitch",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
idCVar pm_bobroll(					"pm_bobroll",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
idCVar pm_thirdPersonRange(			"pm_thirdPersonRange",		"80",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "camera distance from player in 3rd person" );
idCVar pm_thirdPersonHeight(		"pm_thirdPersonHeight",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of camera from normal view height in 3rd person" );
idCVar pm_thirdPersonAngle(			"pm_thirdPersonAngle",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "direction of camera from player in 3rd person in degrees (0 = behind player, 180 = in front)" );
idCVar pm_thirdPersonClip(			"pm_thirdPersonClip",		"1",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "clip third person view into world space" );
idCVar pm_thirdPerson(				"pm_thirdPerson",			"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "enables third person view" );
idCVar pm_thirdPersonDeath(			"pm_thirdPersonDeath",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "enables third person view when player dies" );
idCVar pm_modelView(				"pm_modelView",				"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER, "draws camera from POV of player model (1 = always, 2 = when dead)", 0, 2, idCmdSystem::ArgCompletion_Integer<0,2> );
idCVar pm_airTics(					"pm_air",					"1800",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER, "how long in milliseconds the player can go without air before he starts taking damage" );

idCVar g_showPlayerShadow(			"g_showPlayerShadow",		"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables shadow of player model" );
idCVar g_showHud(					"g_showHud",				"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "" );
idCVar g_showProjectilePct(			"g_showProjectilePct",		"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables display of player hit percentage" );
idCVar g_showBrass(					"g_showBrass",				"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables ejected shells from weapon" );
//#modified-fva; BEGIN
//idCVar g_gun_x(						"g_gunX",					"0",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_gun_x("g_gunX", "0", CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "");
//#modified-fva; END
idCVar g_gun_y(						"g_gunY",					"0",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_gun_z(						"g_gunZ",					"0",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_viewNodalX(				"g_viewNodalX",				"0",			CVAR_GAME | CVAR_FLOAT, "" );
idCVar g_viewNodalZ(				"g_viewNodalZ",				"0",			CVAR_GAME | CVAR_FLOAT, "" );
//#modified-fva; BEGIN
//idCVar g_fov(						"g_fov",					"90",			CVAR_GAME | CVAR_INTEGER | CVAR_NOCHEAT, "" );
idCVar g_fov("g_fov", "90", CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "");
//#modified-fva; END
idCVar g_skipViewEffects(			"g_skipViewEffects",		"0",			CVAR_GAME | CVAR_BOOL, "skip damage and other view effects" );
idCVar g_mpWeaponAngleScale(		"g_mpWeaponAngleScale",		"0",			CVAR_GAME | CVAR_FLOAT, "Control the weapon sway in MP" );

idCVar g_testParticle(				"g_testParticle",			"0",			CVAR_GAME | CVAR_INTEGER, "test particle visualation, set by the particle editor" );
idCVar g_testParticleName(			"g_testParticleName",		"",				CVAR_GAME, "name of the particle being tested by the particle editor" );
idCVar g_testModelRotate(			"g_testModelRotate",		"0",			CVAR_GAME, "test model rotation speed" );
idCVar g_testPostProcess(			"g_testPostProcess",		"",				CVAR_GAME, "name of material to draw over screen" );
idCVar g_testModelAnimate(			"g_testModelAnimate",		"0",			CVAR_GAME | CVAR_INTEGER, "test model animation,\n"
																							"0 = cycle anim with origin reset\n"
																							"1 = cycle anim with fixed origin\n"
																							"2 = cycle anim with continuous origin\n"
																							"3 = frame by frame with continuous origin\n"
																							"4 = play anim once", 0, 4, idCmdSystem::ArgCompletion_Integer<0,4> );
idCVar g_testModelBlend(			"g_testModelBlend",			"0",			CVAR_GAME | CVAR_INTEGER, "number of frames to blend" );
idCVar g_testDeath(					"g_testDeath",				"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar g_exportMask(				"g_exportMask",				"",				CVAR_GAME, "" );
idCVar g_flushSave(					"g_flushSave",				"0",			CVAR_GAME | CVAR_BOOL, "1 = don't buffer file writing for save games." );

idCVar aas_test(					"aas_test",					"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_showAreas(				"aas_showAreas",			"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar aas_showPath(				"aas_showPath",				"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_showFlyPath(				"aas_showFlyPath",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_showWallEdges(			"aas_showWallEdges",		"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar aas_showHideArea(			"aas_showHideArea",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_pullPlayer(				"aas_pullPlayer",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_randomPullPlayer(		"aas_randomPullPlayer",		"0",			CVAR_GAME | CVAR_BOOL, "" );
idCVar aas_goalArea(				"aas_goalArea",				"0",			CVAR_GAME | CVAR_INTEGER, "" );
idCVar aas_showPushIntoArea(		"aas_showPushIntoArea",		"0",			CVAR_GAME | CVAR_BOOL, "" );

idCVar g_password(					"g_password",				"",				CVAR_GAME | CVAR_ARCHIVE, "game password" );
idCVar password(					"password",					"",				CVAR_GAME | CVAR_NOCHEAT, "client password used when connecting" );

idCVar g_countDown(					"g_countDown",				"10",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "pregame countdown in seconds", 4, 3600 );
idCVar g_gameReviewPause(			"g_gameReviewPause",		"10",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER | CVAR_ARCHIVE, "scores review time in seconds (at end game)", 2, 3600 );
idCVar g_TDMArrows(					"g_TDMArrows",				"1",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "draw arrows over teammates in team deathmatch" );
idCVar g_balanceTDM(				"g_balanceTDM",				"1",			CVAR_GAME | CVAR_BOOL, "maintain even teams" );
#ifdef CTF
idCVar g_CTFArrows(					"g_CTFArrows",				"1",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "draw arrows over teammates in CTF" );
#endif

idCVar net_clientPredictGUI(		"net_clientPredictGUI",		"1",			CVAR_GAME | CVAR_BOOL, "test guis in networking without prediction" );

idCVar g_voteFlags(					"g_voteFlags",				"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER | CVAR_ARCHIVE, "vote flags. bit mask of votes not allowed on this server\n"
																					"bit 0 (+1)   restart now\n"
																					"bit 1 (+2)   time limit\n"
																					"bit 2 (+4)   frag limit\n"
																					"bit 3 (+8)   game type\n"
																					"bit 4 (+16)  kick player\n"
																					"bit 5 (+32)  change map\n"
																					"bit 6 (+64)  spectators\n"
																					"bit 7 (+128) next map" );
idCVar g_mapCycle(					"g_mapCycle",				"mapcycle",		CVAR_GAME | CVAR_ARCHIVE, "map cycling script for multiplayer games - see mapcycle.scriptcfg" );

#ifdef _D3XP
idCVar mod_validSkins(				"mod_validSkins",			"skins/characters/player/marine_mp;skins/characters/player/marine_mp_green;skins/characters/player/marine_mp_blue;skins/characters/player/marine_mp_red;skins/characters/player/marine_mp_yellow;skins/characters/player/marine_mp_purple;skins/characters/player/marine_mp_grey;skins/characters/player/marine_mp_orange",		CVAR_GAME | CVAR_ARCHIVE, "valid skins for the game" );
#else
idCVar mod_validSkins(				"mod_validSkins",			"skins/characters/player/marine_mp;skins/characters/player/marine_mp_green;skins/characters/player/marine_mp_blue;skins/characters/player/marine_mp_red;skins/characters/player/marine_mp_yellow",		CVAR_GAME | CVAR_ARCHIVE, "valid skins for the game" );
#endif


#ifdef _D3XP
idCVar g_grabberHoldSeconds(		"g_grabberHoldSeconds",		"3",			CVAR_GAME | CVAR_FLOAT | CVAR_CHEAT, "number of seconds to hold object" );
idCVar g_grabberEnableShake(		"g_grabberEnableShake",		"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "enable the grabber shake" );
idCVar g_grabberRandomMotion(		"g_grabberRandomMotion",	"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "enable random motion on the grabbed object" );
idCVar g_grabberHardStop(			"g_grabberHardStop",		"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "hard stops object if too fast" );
idCVar g_grabberDamping(			"g_grabberDamping",			"0.5",			CVAR_GAME | CVAR_FLOAT | CVAR_CHEAT, "damping of grabber" );
#endif

#ifdef _D3XP
//#modified-fva; BEGIN
//idCVar g_xp_bind_run_once( "g_xp_bind_run_once", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "Rebind all controls once for D3XP." );
idCVar cst_bind_run_once("cst_bind_run_once", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "rebind all controls once");
//#modified-fva; END
#endif

idCVar net_serverDownload(			"net_serverDownload",		"0",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "enable server download redirects. 0: off 1: redirect to si_serverURL 2: use builtin download. see net_serverDl cvars for configuration" );
idCVar net_serverDlBaseURL(			"net_serverDlBaseURL",		"",				CVAR_GAME | CVAR_ARCHIVE, "base URL for the download redirection" );
idCVar net_serverDlTable(			"net_serverDlTable",		"",				CVAR_GAME | CVAR_ARCHIVE, "pak names for which download is provided, seperated by ;" );

//#modified-fva; BEGIN
#ifdef _D3XP
idCVar cst_grabberUnlimitedTimeSP("cst_grabberUnlimitedTimeSP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "don't limit the grabber hold time in singleplayer");
idCVar cst_grabberUnlimitedTimeMP("cst_grabberUnlimitedTimeMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "don't limit the grabber hold time in multiplayer");
idCVar cst_grabberStableThrowSP("cst_grabberStableThrowSP", "0", CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "stabilize the movement of the grabbed object before it is thrown - SP:\n 0: don't stabilize\n 1: stabilize relative to the player\n 2: stabilize relative to the world", 0, 2);
idCVar cst_grabberStableThrowMP("cst_grabberStableThrowMP", "0", CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "stabilize the movement of the grabbed object before it is thrown - MP:\n 0: don't stabilize\n 1: stabilize relative to the player\n 2: stabilize relative to the world", 0, 2);
idCVar cst_grabberDropOnHoldLayer("cst_grabberDropOnHoldLayer", "0", CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "drop the grabbed object on hold layer commands:\n 0: don't drop\n 1 to 9: drop when this layer is held\n -1: drop when any layer is held", -1, 9);
idCVar cst_grabberDropOnSwitchLayer("cst_grabberDropOnSwitchLayer", "0", CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "drop the grabbed object on switch layer commands:\n 0: don't drop\n 1 to 9: drop when switching to this layer\n -1: drop when switching to any layer", -1, 9);
idCVar cst_noSmokeFlyGrabbed("cst_noSmokeFlyGrabbed", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable fly smoke of projectiles while grabbed");
idCVar cst_noSmokeFlyGrabbedOthersMP("cst_noSmokeFlyGrabbedOthersMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable fly smoke of projectiles while grabbed by other players - multiplayer");
idCVar cst_awChainsawRoE("cst_awChainsawRoE", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "spawn the Chainsaw in RoE");
#endif
//#modified-fva; END

//#modified-fva; BEGIN
idCVar cst_noStaminaDrop("cst_noStaminaDrop", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "no stamina drop when running");
idCVar cst_headlampAlertMonsters("cst_headlampAlertMonsters", "1", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "alert monsters when the headlamp is on");
idCVar cst_headlampKeepState("cst_headlampKeepState", "1", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "preserve the headlamp on/off state between maps");
idCVar cst_headlampSound("cst_headlampSound", "1", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "play the headlamp on/off sounds");
idCVar cst_allowHeadlampMP("cst_allowHeadlampMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "allow headlamp in multiplayer");
idCVar cst_showAccessCodes("cst_showAccessCodes", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "show access codes");
idCVar cst_takeNoAmmoWeapons("cst_takeNoAmmoWeapons", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "always take no ammo weapons in singleplayer");
idCVar cst_smokeFixMP("cst_smokeFixMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "experimental fix for multiplayer smoke");
idCVar cst_allowSmokeControlMP("cst_allowSmokeControlMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "allow fly/muzzle smoke control in multiplayer");
idCVar cst_noSmokeFly("cst_noSmokeFly", "", CVAR_GAME | CVAR_ARCHIVE, "disable fly smoke for the specified projectiles");
idCVar cst_noSmokeFlyOthersMP("cst_noSmokeFlyOthersMP", "", CVAR_GAME | CVAR_ARCHIVE, "disable fly smoke for the specified projectiles of other players - multiplayer");
idCVar cst_noSmokeMuzzle("cst_noSmokeMuzzle", "", CVAR_GAME | CVAR_ARCHIVE, "disable muzzle smoke for the specified weapons");
idCVar cst_noSmokeMuzzleOthersMP("cst_noSmokeMuzzleOthersMP", "", CVAR_GAME | CVAR_ARCHIVE, "disable muzzle smoke for the specified weapons of other players - multiplayer");
idCVar cst_allowFlashlightToggleSP("cst_allowFlashlightToggleSP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "allow switching the flashlight on/off in singleplayer using the reload key");
idCVar cst_allowDamageFeedbackControlMP("cst_allowDamageFeedbackControlMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "allow damage feedback control in multiplayer");
idCVar cst_dfNoDoubleVision("cst_dfNoDoubleVision", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable double vision when damaged");
idCVar cst_dfNoTunnelVision("cst_dfNoTunnelVision", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable tunnel vision when damaged");
idCVar cst_dfNoViewAngleKick("cst_dfNoViewAngleKick", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable view angle kick when damaged");
idCVar cst_dfNoScreenBlob("cst_dfNoScreenBlob", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable screen blob when damaged");
idCVar cst_dfNoArmorPulse("cst_dfNoArmorPulse", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable armor pulse when damaged");
idCVar cst_dfNoSoundPain("cst_dfNoSoundPain", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable pain sound when damaged");
idCVar cst_dfNoSoundAirGasp("cst_dfNoSoundAirGasp", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "disable gasp sound when damaged by lack of air");
idCVar cst_hudZoom("cst_hudZoom", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "enable the HUD's zoom indicator");
idCVar cst_hudCrouch("cst_hudCrouch", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "enable the HUD's crouch indicator");
idCVar cst_hudRun("cst_hudRun", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "enable the HUD's run indicator");
idCVar cst_crosshairAlwaysShow("cst_crosshairAlwaysShow", "1", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "don't hide the crosshair when switching or reloading weapons");
idCVar cst_shotgunHalfSpreadSP("cst_shotgunHalfSpreadSP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "reduce the shotgun spread by half in singleplayer");
idCVar cst_grenadesToggle("cst_grenadesToggle", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_USERINFO, "enable the grenades key to toggle between grenades and the previous weapon");
idCVar cst_allowGrenadesToggleMP("cst_allowGrenadesToggleMP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_CST_INFOSYNC, "allow grenades toggle in multiplayer");
//#modified-fva; END

// haleyjd: CstD3LE 
idCVar cst_ammoPickupFactor("cst_ammoPickupFactor", "1.0", CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "multiply ammo pickup values by this amount", 0.25, 4.0);
idCVar cst_armorProtectionSP("cst_armorProtectionSP", "-1", CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "armor protection factor in singleplayer:\n  < 0.0 = default (0.3 on recruit/marine, 0.2 on veteran/nightmare)\n  0.0 to 1.0 = use this value on any skill", -1.0f, 1.0f);
idCVar cst_bfgPlayerSpeedSP("cst_bfgPlayerSpeedSP", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "use the player speed from Doom 3 BFG Edition (faster movement) in singleplayer");
// haleyjd: CstD3LE END
