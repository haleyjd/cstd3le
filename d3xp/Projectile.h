// Copyright (C) 2004 Id Software, Inc.
//

#ifndef __GAME_PROJECTILE_H__
#define __GAME_PROJECTILE_H__

/*
===============================================================================

  idProjectile
	
===============================================================================
*/

extern const idEventDef EV_Explode;
//#modified-fva; BEGIN
#ifdef _D3XP
extern const idEventDef EV_Fizzle;
#endif
//#modified-fva; END

class idProjectile : public idEntity {
public :
	CLASS_PROTOTYPE( idProjectile );

							idProjectile();
	virtual					~idProjectile();

	void					Spawn( void );

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Create( idEntity *owner, const idVec3 &start, const idVec3 &dir );
	virtual void			Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire = 0.0f, const float launchPower = 1.0f, const float dmgPower = 1.0f );
	virtual void			FreeLightDef( void );

	idEntity *				GetOwner( void ) const;
#ifdef _D3XP
	void					CatchProjectile( idEntity* o, const char* reflectName );
	int						GetProjectileState( void );
	void					Event_CreateProjectile( idEntity *owner, const idVec3 &start, const idVec3 &dir );
	void					Event_LaunchProjectile( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity );
	void					Event_SetGravity( float gravity );
#endif

	//#modified-fva; BEGIN
#ifdef _D3XP
	void					CstReleaseProjectile(idPlayer *grabberOwner);
#endif
	//#modified-fva; END

	//#modified-fva; BEGIN
	static void				CstInitSmokeFlyControl();
	//#modified-fva; END

	virtual void			Think( void );
	virtual void			Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location );
	virtual bool			Collide( const trace_t &collision, const idVec3 &velocity );
	virtual void			Explode( const trace_t &collision, idEntity *ignore );
	void					Fizzle( void );

	static idVec3			GetVelocity( const idDict *projectile );
	static idVec3			GetGravity( const idDict *projectile );

	enum {
		EVENT_DAMAGE_EFFECT = idEntity::EVENT_MAXEVENTS,
		EVENT_MAXEVENTS
	};

	//Added for LM
	void					SetLaunchedFromGrabber( bool bl ) { launchedFromGrabber = bl; }
	bool					GetLaunchedFromGrabber() { return launchedFromGrabber; }

	static void				DefaultDamageEffect( idEntity *soundEnt, const idDict &projectileDef, const trace_t &collision, const idVec3 &velocity );
	static bool				ClientPredictionCollide( idEntity *soundEnt, const idDict &projectileDef, const trace_t &collision, const idVec3 &velocity, bool addDamageEffect );
	virtual void			ClientPredictionThink( void );
	virtual void			WriteToSnapshot( idBitMsgDelta &msg ) const;
	virtual void			ReadFromSnapshot( const idBitMsgDelta &msg );
	virtual bool			ClientReceiveEvent( int event, int time, const idBitMsg &msg );

protected:
	idEntityPtr<idEntity>	owner;

	struct projectileFlags_s {
		bool				detonate_on_world			: 1;
		bool				detonate_on_actor			: 1;
		bool				randomShaderSpin			: 1;
		bool				isTracer					: 1;
		bool				noSplashDamage				: 1;
	} projectileFlags;

	//Added for LM
	bool					launchedFromGrabber;

	float					thrust;
	int						thrust_end;
	float					damagePower;

	renderLight_t			renderLight;
	qhandle_t				lightDefHandle;				// handle to renderer light def
	idVec3					lightOffset;
	int						lightStartTime;
	int						lightEndTime;
	idVec3					lightColor;

	idForce_Constant		thruster;
	idPhysics_RigidBody		physicsObj;

	const idDeclParticle *	smokeFly;
	int						smokeFlyTime;

	//Code for LM
	bool					mNoExplodeDisappear;
	bool					mTouchTriggers;

#ifdef _D3XP
	int						originalTimeGroup;
#endif

	typedef enum {
		// must update these in script/doom_defs.script if changed
		SPAWNED = 0,
		CREATED = 1,
		LAUNCHED = 2,
		FIZZLED = 3,
		EXPLODED = 4
	} projectileState_t;
	
	projectileState_t		state;

private:
	bool					netSyncPhysics;

	void					AddDefaultDamageEffect( const trace_t &collision, const idVec3 &velocity );

	void					Event_Explode( void );
	void					Event_Fizzle( void );
	void					Event_RadiusDamage( idEntity *ignore );
	void					Event_Touch( idEntity *other, trace_t *trace );
	void					Event_GetProjectileState( void );

	//#modified-fva; BEGIN
	class CstSmokeFlyControl {
	public:
		CstSmokeFlyControl();
		void							Init();
		bool							IsSmokeFlyAllowed(idProjectile *projectile, int weaponSlot = -1);
#ifdef _D3XP
		void							OnGrabberCatch(idPlayer *grabberOwner, idProjectile *projectile);
		void							OnGrabberRelease(idPlayer *grabberOwner, idProjectile *projectile);
#endif
	private:
		class ProjectileWeaponPair {
		public:
			ProjectileWeaponPair() { projectileDefNumber = -1; weaponSlot = -1; }
			int							projectileDefNumber;
			int							weaponSlot;
		};
		idList<ProjectileWeaponPair>	controlTable;
		idCVar &						GetNoSmokeFlyCVar(idPlayer &player);
		bool							IsAllowedByWeapon(int weaponSlot, idCVar &noSmokeFlyCVar);
		int								FindWeaponSlot(int projectileDefNumber);
#ifdef _D3XP
		int								grabberSlot;
		void							UpdateSmokeFly(idProjectile *projectile, int weaponSlot = -1);
		void							StopSmokeFly(idProjectile *projectile);
		void							StartSmokeFly(idProjectile *projectile);
#endif
	};
	static CstSmokeFlyControl			cstSmokeFlyControl;
	//#modified-fva; END
};

class idGuidedProjectile : public idProjectile {
public :
	CLASS_PROTOTYPE( idGuidedProjectile );

							idGuidedProjectile( void );
							~idGuidedProjectile( void );

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Spawn( void );
	virtual void			Think( void );
	virtual void			Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire = 0.0f, const float launchPower = 1.0f, const float dmgPower = 1.0f );
#ifdef _D3XP
	void					SetEnemy( idEntity *ent );
	void					Event_SetEnemy(idEntity *ent);
#endif
	//#modified-fva; BEGIN
#ifdef _D3XP
	void					CstUnGuide();
#endif
	//#modified-fva; END

protected:
	float					speed;
	idEntityPtr<idEntity>	enemy;
	virtual void			GetSeekPos( idVec3 &out );

private:
	idAngles				rndScale;
	idAngles				rndAng;
	idAngles				angles;
	int						rndUpdateTime;
	float					turn_max;
	float					clamp_dist;
	bool					burstMode;
	bool					unGuided;
	float					burstDist;
	float					burstVelocity;
};

class idSoulCubeMissile : public idGuidedProjectile {
public:
	CLASS_PROTOTYPE ( idSoulCubeMissile );
	~idSoulCubeMissile();
	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Spawn( void );
	virtual void			Think( void );
	virtual void			Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire = 0.0f, const float power = 1.0f, const float dmgPower = 1.0f );

protected:
	virtual void			GetSeekPos( idVec3 &out );
	void					ReturnToOwner( void );
	void					KillTarget( const idVec3 &dir );

private:
	idVec3					startingVelocity;
	idVec3					endingVelocity;
	float					accelTime;
	int						launchTime;
	bool					killPhase;
	bool					returnPhase;
	idVec3					destOrg;
	idVec3					orbitOrg;
	int						orbitTime;
	int						smokeKillTime;
	const idDeclParticle *	smokeKill;
};

struct beamTarget_t {
	idEntityPtr<idEntity>	target;
	renderEntity_t			renderEntity;
	qhandle_t				modelDefHandle;
	//#modified-fva; BEGIN
	bool					cstIsPlayer;
	//#modified-fva; END
};

class idBFGProjectile : public idProjectile {
public :
	CLASS_PROTOTYPE( idBFGProjectile );

							idBFGProjectile();
							~idBFGProjectile();

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Spawn( void );
	virtual void			Think( void );
	virtual void			Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire = 0.0f, const float launchPower = 1.0f, const float dmgPower = 1.0f );
	virtual void			Explode( const trace_t &collision, idEntity *ignore );

	//#modified-fva; BEGIN
	virtual void			WriteToSnapshot(idBitMsgDelta &msg) const;
	virtual void			ReadFromSnapshot(const idBitMsgDelta &msg);

	void CstStopBeams();
	void CstStartBeams();
	//#modified-fva; END

private:
	idList<beamTarget_t>	beamTargets;
	renderEntity_t			secondModel;
	qhandle_t				secondModelDefHandle;
	int						nextDamageTime;
	idStr					damageFreq;

	void					FreeBeams();
	void					Event_RemoveBeams();
	void					ApplyDamage();

	//#modified-fva; BEGIN
	static idList<idBFGProjectile*> cstBfgProjectiles;

	bool cstBfgVision[CST_MAX_PLAYERS];
	int cstTargetsMP;

	void CstClearBfgVision();
	void CstUpdateBfgVision(idPlayer* player, bool enable);
	void CstFreeSingleBeam(beamTarget_t& beamTarget);
	void CstClientStopBeams(bool stopSound = true);
	void CstClientUpdateBeams(int targets);
	//#modified-fva; END
};

//Added for LM (Lost mission)

class idHomingProjectile : public idProjectile {
public :
	CLASS_PROTOTYPE( idHomingProjectile );

	idHomingProjectile();
	~idHomingProjectile();

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Spawn();
	virtual void			Think();
	virtual void			Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire = 0.0f, const float launchPower = 1.0f, const float dmgPower = 1.0f );
	void					SetEnemy( idEntity *ent );
	void					SetSeekPos( idVec3 pos );
	void					Event_SetEnemy(idEntity *ent);

protected:
	float					speed;
	idEntityPtr<idEntity>	enemy;
	idVec3					seekPos;

private:
	idAngles				rndScale;
	idAngles				rndAng;
	idAngles				angles;
	float					turn_max;
	float					clamp_dist;
	bool					burstMode;
	bool					unGuided;
	float					burstDist;
	float					burstVelocity;
};


/*
===============================================================================

  idDebris
	
===============================================================================
*/

class idDebris : public idEntity {
public :
	CLASS_PROTOTYPE( idDebris );

							idDebris();
							~idDebris();

	// save games
	void					Save( idSaveGame *savefile ) const;					// archives object for save game file
	void					Restore( idRestoreGame *savefile );					// unarchives object from save game file

	void					Spawn( void );

	void					Create( idEntity *owner, const idVec3 &start, const idMat3 &axis );
	void					Launch( void );
	void					Think( void );
	void					Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location );
	void					Explode( void );
	void					Fizzle( void );
	virtual bool			Collide( const trace_t &collision, const idVec3 &velocity );


private:
	idEntityPtr<idEntity>	owner;
	idPhysics_RigidBody		physicsObj;
	const idDeclParticle *	smokeFly;
	int						smokeFlyTime;
	const idSoundShader *	sndBounce;


	void					Event_Explode( void );
	void					Event_Fizzle( void );
};

#endif /* !__GAME_PROJECTILE_H__ */
