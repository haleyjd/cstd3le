#include "../idlib/precompiled.h"
#pragma hdrstop

#ifdef _D3XP

#include "Game_local.h"
#include "Misc.h"

#define MAX_DRAG_TRACE_DISTANCE			384.0f
#define TRACE_BOUNDS_SIZE				3.f
#define HOLD_DISTANCE					72.f
#define FIRING_DELAY					1000.0f
#define DRAG_FAIL_LEN					64.f
#define	THROW_SCALE						1000
#define MAX_PICKUP_VELOCITY				1500 * 1500
#define MAX_PICKUP_SIZE					96
//#modified-fva; BEGIN
#define CST_GRABBED_GRAVITY				idVec3(0.f, 0.f, -0.001f)
//#modified-fva; END

/*
===============================================================================

	Allows entities to be dragged through the world with physics.

===============================================================================
*/

CLASS_DECLARATION( idEntity, idGrabber )
END_CLASS

/*
==============
idGrabber::idGrabber
==============
*/
idGrabber::idGrabber( void ) {
	dragEnt = NULL;
	owner = NULL;
	beam = NULL;
	beamTarget = NULL;
	oldUcmdFlags = 0;
	shakeForceFlip = false;
	holdingAF = false;
	endTime = 0;
	lastFiredTime = -FIRING_DELAY;
	dragFailTime = 0;
	startDragTime = 0;
	warpId = -1;
	dragTraceDist = MAX_DRAG_TRACE_DISTANCE;

	//#modified-fva; BEGIN
	id = 0;
	//#modified-fva; END

	//#modified-fva; BEGIN
	cstOrigin = vec3_origin;
	cstAngles = ang_zero;
	cstStart = false;
	cstDrop = false;
	cstDistanceToGoal = 0.f;
	//#modified-fva; END
}

/*
==============
idGrabber::~idGrabber
==============
*/
idGrabber::~idGrabber( void ) {
	//#modified-fva; BEGIN
	//StopDrag( true );
	if (!gameLocal.isMultiplayer) {
		StopDrag(true);
	} else if (gameLocal.isServer) {
		CstServerStopDrag(true);
	} else {
		CstClientStopDrag();
	}
	//#modified-fva; END
	if ( beam ) {
		delete beam;
	}
	if ( beamTarget ) {
		delete beamTarget;
	}
}

/*
==============
idGrabber::Save
==============
*/
void idGrabber::Save( idSaveGame *savefile ) const {

	dragEnt.Save( savefile );
	savefile->WriteStaticObject( drag );

	savefile->WriteVec3( saveGravity );
	savefile->WriteInt( id );

	savefile->WriteVec3( localPlayerPoint );

	owner.Save( savefile );

	savefile->WriteBool( holdingAF );
	savefile->WriteBool( shakeForceFlip );

	savefile->WriteInt( endTime );
	savefile->WriteInt( lastFiredTime );
	savefile->WriteInt( dragFailTime );
	savefile->WriteInt( startDragTime );
	savefile->WriteFloat( dragTraceDist );
	savefile->WriteInt( savedContents );
	savefile->WriteInt( savedClipmask );

	savefile->WriteObject( beam );
	savefile->WriteObject( beamTarget );

	savefile->WriteInt( warpId );

	//#modified-fva; BEGIN
	savefile->WriteVec3(cstOrigin);
	savefile->WriteAngles(cstAngles);
	savefile->WriteBool(cstStart);
	savefile->WriteBool(cstDrop);
	savefile->WriteFloat(cstDistanceToGoal);
	//#modified-fva; END
}

/*
==============
idGrabber::Restore
==============
*/
void idGrabber::Restore( idRestoreGame *savefile ) {
	//Spawn the beams
	Initialize();

	dragEnt.Restore( savefile );
	savefile->ReadStaticObject( drag );

	savefile->ReadVec3( saveGravity );
	savefile->ReadInt( id );

	// Restore the drag force's physics object
	if ( dragEnt.IsValid() ) {
		drag.SetPhysics( dragEnt.GetEntity()->GetPhysics(), id, dragEnt.GetEntity()->GetPhysics()->GetOrigin() );
	}

	savefile->ReadVec3( localPlayerPoint );

	owner.Restore( savefile );

	savefile->ReadBool( holdingAF );
	savefile->ReadBool( shakeForceFlip );

	savefile->ReadInt( endTime );
	savefile->ReadInt( lastFiredTime );
	savefile->ReadInt( dragFailTime );
	savefile->ReadInt( startDragTime );
	savefile->ReadFloat( dragTraceDist );
	savefile->ReadInt( savedContents );
	savefile->ReadInt( savedClipmask );

	savefile->ReadObject( reinterpret_cast<idClass *&>(beam) );
	savefile->ReadObject( reinterpret_cast<idClass *&>(beamTarget) );

	savefile->ReadInt( warpId );

	//#modified-fva; BEGIN
	savefile->ReadVec3(cstOrigin);
	savefile->ReadAngles(cstAngles);
	savefile->ReadBool(cstStart);
	savefile->ReadBool(cstDrop);
	savefile->ReadFloat(cstDistanceToGoal);
	//#modified-fva; END
}

/*
==============
idGrabber::Initialize
==============
*/
void idGrabber::Initialize( void ) {
	if ( !gameLocal.isMultiplayer ) {
		idDict args;

		if ( !beamTarget ) {
			args.SetVector( "origin", vec3_origin );
			args.SetBool( "start_off", true );
			beamTarget = ( idBeam * )gameLocal.SpawnEntityType( idBeam::Type, &args );
		}

		if ( !beam ) {
			args.Clear();
			args.Set( "target", beamTarget->name.c_str() );
			args.SetVector( "origin", vec3_origin );
			args.SetBool( "start_off", true );
			args.Set( "width", "6" );
			args.Set( "skin", "textures/smf/flareSizeable" );
			args.Set( "_color", "0.0235 0.843 0.969 0.2" );
			beam = ( idBeam * )gameLocal.SpawnEntityType( idBeam::Type, &args );
			beam->SetShaderParm( 6, 1.0f );
		}

		endTime = 0;
		dragTraceDist = MAX_DRAG_TRACE_DISTANCE;
	}
	else {
		beam = NULL;
		beamTarget = NULL;
		endTime = 0;
		dragTraceDist = MAX_DRAG_TRACE_DISTANCE;
	};

	//#modified-fva; BEGIN
	id = 0;
	//#modified-fva; END

	//#modified-fva; BEGIN
	cstOrigin = vec3_origin;
	cstAngles = ang_zero;
	cstStart = false;
	cstDrop = false;
	cstDistanceToGoal = 0.f;
	//#modified-fva; END
}

/*
==============
idGrabber::SetDragDistance
==============
*/
void idGrabber::SetDragDistance( float dist ) {
	dragTraceDist = dist;
}

/*
==============
idGrabber::StartDrag
==============
*/
void idGrabber::StartDrag( idEntity *grabEnt, int id ) {
	int clipModelId = id;
	idPlayer *thePlayer = owner.GetEntity();

	//#modified-fva; BEGIN
	cstDrop = false;
	//#modified-fva; END

	holdingAF = false;
	dragFailTime = gameLocal.slow.time;
	startDragTime = gameLocal.slow.time;

	//#modified-fva; BEGIN
	//oldUcmdFlags = thePlayer->usercmd.flags;
	//#modified-fva; END

	// set grabbed state for networking
	grabEnt->SetGrabbedState( true );

	// This is the new object to drag around
	dragEnt = grabEnt;

	// Show the beams!
	UpdateBeams();
	if ( beam ) {
		beam->Show();
	}
	if ( beamTarget ) {
		beamTarget->Show();
	}

	// Move the object to the fast group (helltime)
	grabEnt->timeGroup = TIME_GROUP2;

	// Handle specific class types
	if ( grabEnt->IsType( idProjectile::Type ) ) {
		idProjectile* p = (idProjectile*)grabEnt;

		p->CatchProjectile( thePlayer, "_catch" );

		// Make the projectile non-solid to other projectiles/enemies (special hack for helltime hunter)
		if ( !idStr::Cmp( grabEnt->GetEntityDefName(), "projectile_helltime_killer" ) ) {
			savedContents = CONTENTS_PROJECTILE;
			savedClipmask = MASK_SHOT_RENDERMODEL|CONTENTS_PROJECTILE;
		} else {
			savedContents = grabEnt->GetPhysics()->GetContents();
			savedClipmask = grabEnt->GetPhysics()->GetClipMask();
		}
		grabEnt->GetPhysics()->SetContents( 0 );
		grabEnt->GetPhysics()->SetClipMask( CONTENTS_SOLID|CONTENTS_BODY );

		//#modified-fva; BEGIN
		if (!gameLocal.isClient) {
			if (grabEnt->IsType(idBFGProjectile::Type)) {
				idBFGProjectile* bp = static_cast<idBFGProjectile*>(grabEnt);
				bp->CstStopBeams();
			}

			if (grabEnt->spawnArgs.GetFloat("fuse") > 0) {
				if (grabEnt->spawnArgs.GetBool("detonate_on_fuse")) {
					grabEnt->CancelEvents(&EV_Explode);
				} else {
					grabEnt->CancelEvents(&EV_Fizzle);
				}
			}
		}
		//#modified-fva; END

	} else if ( grabEnt->IsType( idExplodingBarrel::Type ) ) {
		idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(grabEnt);

		ebarrel->StartBurning();

	} else if ( grabEnt->IsType( idAFEntity_Gibbable::Type ) ) {
		holdingAF = true;
		clipModelId = 0;

		if ( grabbableAI( grabEnt->spawnArgs.GetString( "classname" ) ) ) {
			idAI *aiEnt = static_cast<idAI*>(grabEnt);

			aiEnt->StartRagdoll();
		}
	} else if ( grabEnt->IsType( idMoveableItem::Type ) ) {
		grabEnt->PostEventMS( &EV_Touch, 250, thePlayer, NULL );
	}

	// Get the current physics object to manipulate
	idPhysics *phys = grabEnt->GetPhysics();

	// Turn off gravity on object
	saveGravity = phys->GetGravity();
	//#modified-fva; BEGIN
	//phys->SetGravity( vec3_origin );
	phys->SetGravity(CST_GRABBED_GRAVITY);
	//#modified-fva; END

	// hold it directly in front of player
	localPlayerPoint = ( thePlayer->firstPersonViewAxis[0] * HOLD_DISTANCE ) * thePlayer->firstPersonViewAxis.Transpose();

	// Set the ending time for the hold
	endTime = gameLocal.time + g_grabberHoldSeconds.GetFloat() * 1000;

	// Start up the Force_Drag to bring it in
	drag.Init( g_grabberDamping.GetFloat() );
	drag.SetPhysics( phys, clipModelId, thePlayer->firstPersonViewOrigin + localPlayerPoint * thePlayer->firstPersonViewAxis);

	// start the screen warp
	warpId = thePlayer->playerView.AddWarp( phys->GetOrigin(), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 160, 2000 );

	//#modified-fva; BEGIN
	this->id = clipModelId;
	//#modified-fva; END
}

/*
==============
idGrabber::StopDrag
==============
*/
void idGrabber::StopDrag( bool dropOnly ) {
	idPlayer *thePlayer = owner.GetEntity();

	//#modified-fva; BEGIN
	cstDrop = false;
	//#modified-fva; END

	if ( beam ) {
		beam->Hide();
	}
	if ( beamTarget ) {
		beamTarget->Hide();
	}

	if ( dragEnt.IsValid() ) {
		idEntity *ent = dragEnt.GetEntity();

		// set grabbed state for networking
		ent->SetGrabbedState( false );

		// If a cinematic has started, allow dropped object to think in cinematics
		if ( gameLocal.inCinematic ) {
			ent->cinematic = true;
		}

		// Restore Gravity
		ent->GetPhysics()->SetGravity( saveGravity );

		// Move the object back to the slow group (helltime)
		ent->timeGroup = TIME_GROUP1;

		if ( holdingAF ) {
			idAFEntity_Gibbable *af = static_cast<idAFEntity_Gibbable *>(ent);
			idPhysics_AF	*af_Phys = static_cast<idPhysics_AF*>(af->GetPhysics());

			if ( grabbableAI( ent->spawnArgs.GetString( "classname" ) ) ) {
				idAI *aiEnt = static_cast<idAI*>(ent);

				//#modified-fva; BEGIN
				// Stopping the ragdoll here solves the problem of monsters getting stuck after being resurrected by
				// the archvile. The ragdoll is restarted by the damage call below (the monster is killed).
				aiEnt->StopRagdoll();
				// This solves issues with temporary ai invulnerability (for instance, when ticks use anim 16hole).
				aiEnt->fl.takedamage = true;
				//#modified-fva; END

				aiEnt->Damage( thePlayer, thePlayer, vec3_origin, "damage_suicide", 1.0f, INVALID_JOINT );
			}
			
			af->SetThrown( !dropOnly );

			// Reset timers so that it isn't forcibly put to rest in mid-air
			af_Phys->PutToRest();
			af_Phys->Activate();

			af_Phys->SetTimeScaleRamp( MS2SEC(gameLocal.slow.time) - 1.5f, MS2SEC(gameLocal.slow.time) + 1.0f );
		}

		// If the object isn't near its goal, just drop it in place.
		if ( !ent->IsType( idProjectile::Type ) && ( dropOnly || drag.GetDistanceToGoal() > DRAG_FAIL_LEN ) ) {
			ent->GetPhysics()->SetLinearVelocity( vec3_origin );
			thePlayer->StartSoundShader( declManager->FindSound( "grabber_maindrop" ), SND_CHANNEL_WEAPON, 0, false, NULL );

			if ( ent->IsType( idExplodingBarrel::Type ) ) {
				idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(ent);

				ebarrel->SetStability( true );
				ebarrel->StopBurning();
			}
		} else {
			// Shoot the object forward
			//#modified-fva; BEGIN
			int cstStableThrow = cst_grabberStableThrowSP.GetInteger();
			if (cstStableThrow > 0) {
				// remove any linear movement that is not in the throw direction
				idPhysics *entPhys = ent->GetPhysics();
				idVec3 entVel = entPhys->GetLinearVelocity(id);
				idVec3 baseVel;
				if (cstStableThrow == 1) {
					// remove relative to the player
					baseVel = thePlayer->GetPlayerPhysics()->GetLinearVelocity();
				} else {
					// remove relative to the world
					baseVel = vec3_origin;
				}
				float speed = (entVel - baseVel) * thePlayer->firstPersonViewAxis[0];
				if (speed < 0.f) {
					speed = 0.f;
				}
				entVel = baseVel + (speed * thePlayer->firstPersonViewAxis[0]);
				int num = entPhys->GetNumClipModels();
				for (int i = 0; i < num; i++) {
					entPhys->SetLinearVelocity(entVel, i);
				}
			}
			//#modified-fva; END
			ent->ApplyImpulse( thePlayer, 0, ent->GetPhysics()->GetOrigin(), thePlayer->firstPersonViewAxis[0] * THROW_SCALE * ent->GetPhysics()->GetMass() );
			thePlayer->StartSoundShader( declManager->FindSound( "grabber_release" ), SND_CHANNEL_WEAPON, 0, false, NULL );

			// Orient projectiles away from the player
			if ( ent->IsType( idProjectile::Type ) ) {
				idPlayer *player = owner.GetEntity();
				idAngles ang = player->firstPersonViewAxis[0].ToAngles();

				ang.pitch += 90.f;
				ent->GetPhysics()->SetAxis( ang.ToMat3() );
				ent->GetPhysics()->SetAngularVelocity( vec3_origin );

				// Restore projectile contents
				ent->GetPhysics()->SetContents( savedContents );
				ent->GetPhysics()->SetClipMask( savedClipmask );

				//added for LM
				idProjectile *projectile = static_cast< idProjectile* >( ent );
				if ( projectile != NULL ) {
					projectile->SetLaunchedFromGrabber( true );
				}

				//#modified-fva; BEGIN
				if (!gameLocal.isClient) {
					if (ent->IsType(idBFGProjectile::Type)) {
						idBFGProjectile* bp = static_cast<idBFGProjectile*>(ent);
						bp->CstStartBeams();
					}

					float fuse = ent->spawnArgs.GetFloat("fuse");
					if (fuse > 0) {
						if (ent->spawnArgs.GetBool("detonate_on_fuse")) {
							ent->PostEventSec(&EV_Explode, fuse);
						} else {
							ent->PostEventSec(&EV_Fizzle, fuse);
						}
					}
				}
				//#modified-fva; END

				//#modified-fva; BEGIN
				idProjectile *p = static_cast<idProjectile*>(ent);
				p->CstReleaseProjectile(thePlayer);
				//#modified-fva; END

			} else if ( ent->IsType( idMoveable::Type ) ) {
				// Turn on damage for this object
				idMoveable *obj = static_cast<idMoveable*>(ent);
				obj->EnableDamage( true, 2.5f );
				obj->SetAttacker( thePlayer );

				if ( ent->IsType( idExplodingBarrel::Type ) ) {
					idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(ent);
					ebarrel->SetStability( false );
				}

			} else if ( ent->IsType( idMoveableItem::Type ) ) {
				ent->GetPhysics()->SetClipMask( MASK_MONSTERSOLID );
			}
		}

		// Remove the Force_Drag's control of the entity
		drag.RemovePhysics( ent->GetPhysics() );
	}

	if ( warpId != -1 ) {
		thePlayer->playerView.FreeWarp( warpId );
		warpId = -1;
	}

	lastFiredTime = gameLocal.time;
	dragEnt = NULL;
	endTime = 0;
}

/*
==============
idGrabber::Update
==============
*/
int idGrabber::Update( idPlayer *player, bool hide ) {
	trace_t trace;
	idEntity *newEnt;

	//#modified-fva; BEGIN
	if (gameLocal.isMultiplayer) {
		if (gameLocal.isServer) {
			return CstServerUpdate(player, hide);
		} else {
			CstClientUpdate(player, hide);
			return 0; // clients return a dummy value
		}
	}
	//#modified-fva; END

	// pause before allowing refire
	if ( lastFiredTime + FIRING_DELAY > gameLocal.time ) {
		return 3;
	}

	// Dead players release the trigger
	if ( hide || player->health <= 0 ) {
		StopDrag( true );
		if ( hide ) {
			lastFiredTime = gameLocal.time - FIRING_DELAY + 250;
		}
		return 3;
	}

	// Check if object being held has been removed (dead demon, projectile, etc.)
	//#modified-fva; BEGIN
	//if ( endTime > gameLocal.time ) {
	if ((cst_grabberUnlimitedTimeSP.GetBool() && endTime != 0) || endTime > gameLocal.time) {
	//#modified-fva; END
		bool abort = !dragEnt.IsValid();

		if ( !abort && dragEnt.GetEntity()->IsType( idProjectile::Type ) ) {
			idProjectile *proj = (idProjectile *)dragEnt.GetEntity();

			if ( proj->GetProjectileState() >= 3 ) {
				abort = true;
			}
		}
		if ( !abort && dragEnt.GetEntity()->IsHidden() ) {
			abort = true;
		}
		//#modified-fva; BEGIN
		/*
		// Not in multiplayer :: Pressing "reload" lets you carefully drop an item
		if ( !gameLocal.isMultiplayer && !abort && (( player->usercmd.flags & UCF_IMPULSE_SEQUENCE ) != ( oldUcmdFlags & UCF_IMPULSE_SEQUENCE )) && (player->usercmd.impulse == IMPULSE_13) ) {
			abort = true;
		}
		*/
		// Dropping an item with "reload" is now signaled via idGrabber::CstReload and idWeapon::Reload. Also works in multiplayer.
		if (cstDrop) {
			abort = true;
		}
		//#modified-fva; END
        
		if ( abort ) {
			StopDrag( true );
			return 3;
		}
	}

	owner = player;

	// if no entity selected for dragging
    if ( !dragEnt.GetEntity() ) {
		idBounds bounds;
		idVec3 end = player->firstPersonViewOrigin + player->firstPersonViewAxis[0] * dragTraceDist;

		bounds.Zero();
		bounds.ExpandSelf( TRACE_BOUNDS_SIZE );

		gameLocal.clip.TraceBounds( trace, player->firstPersonViewOrigin, end, bounds, MASK_SHOT_RENDERMODEL|CONTENTS_PROJECTILE|CONTENTS_MOVEABLECLIP, player );
		// If the trace hit something
		if ( trace.fraction < 1.0f ) {
			newEnt = gameLocal.entities[ trace.c.entityNum ];

			// if entity is already being grabbed then bypass
			if ( gameLocal.isMultiplayer && newEnt->IsGrabbed() ) {
				return 0;
			}

			// Check if this is a valid entity to hold
			if ( newEnt && ( newEnt->IsType( idMoveable::Type ) ||
					newEnt->IsType( idMoveableItem::Type ) ||
					newEnt->IsType( idProjectile::Type ) ||
					newEnt->IsType( idAFEntity_Gibbable::Type ) ) &&
					newEnt->noGrab == false &&
					newEnt->GetPhysics()->GetBounds().GetRadius() < MAX_PICKUP_SIZE &&
					newEnt->GetPhysics()->GetLinearVelocity().LengthSqr() < MAX_PICKUP_VELOCITY ) {

				bool validAF = true;

				if ( newEnt->IsType( idAFEntity_Gibbable::Type ) ) {
					idAFEntity_Gibbable *afEnt = static_cast<idAFEntity_Gibbable*>(newEnt);

					if ( grabbableAI( newEnt->spawnArgs.GetString( "classname" ) ) ) {
						// Make sure it's also active
						if ( !afEnt->IsActive() ) {
							validAF = false;
						}
					} else if ( !afEnt->IsActiveAF() ) {
						validAF = false;
					}
				}

				if ( validAF && player->usercmd.buttons & BUTTON_ATTACK ) {
					// Grab this entity and start dragging it around
					StartDrag( newEnt, trace.c.id );
				} else if ( validAF ) {
					// A holdable object is ready to be grabbed
					return 1;
				}
			}
		}
	}

	// check backwards server time in multiplayer
	bool allow = true;

	if ( gameLocal.isMultiplayer ) {

		// if we've marched backwards
		if ( gameLocal.slow.time < startDragTime ) {
			allow = false;
		}
	}


	// if there is an entity selected for dragging
	if ( dragEnt.GetEntity() && allow ) {
		idPhysics *entPhys = dragEnt.GetEntity()->GetPhysics();
		idVec3 goalPos;

		// If the player lets go of attack, or time is up
		if ( !( player->usercmd.buttons & BUTTON_ATTACK ) ) {
			StopDrag( false );
			return 3;
		}
		//#modified-fva; BEGIN
		//if ( gameLocal.time > endTime ) {
		if (!cst_grabberUnlimitedTimeSP.GetBool() && gameLocal.time > endTime) {
		//#modified-fva; END
			StopDrag( true );
			return 3;
		}

		// Check if the player is standing on the object
		if ( !holdingAF ) {
			idBounds	playerBounds;
			idBounds	objectBounds = entPhys->GetAbsBounds();
			idVec3		newPoint = player->GetPhysics()->GetOrigin();

			// create a bounds at the players feet
			playerBounds.Clear();
			playerBounds.AddPoint( newPoint );
			newPoint.z -= 1.f;
			playerBounds.AddPoint( newPoint );
			playerBounds.ExpandSelf( 8.f );

			// If it intersects the object bounds, then drop it
			if ( playerBounds.IntersectsBounds( objectBounds ) ) {
				StopDrag( true );
				return 3;
			}
		}

		// Shake the object at the end of the hold
		//#modified-fva; BEGIN
		//if ( g_grabberEnableShake.GetBool() && !gameLocal.isMultiplayer ) {
		if (!cst_grabberUnlimitedTimeSP.GetBool() && g_grabberEnableShake.GetBool() && !gameLocal.isMultiplayer) {
		//#modified-fva; END
			ApplyShake();
		}

		// Set and evaluate drag force
		goalPos = player->firstPersonViewOrigin + localPlayerPoint * player->firstPersonViewAxis;

		drag.SetGoalPosition( goalPos );
		drag.Evaluate( gameLocal.time );

		// If an object is flying too fast toward the player, stop it hard
		if ( g_grabberHardStop.GetBool() ) {
			idPlane theWall;
			idVec3 toPlayerVelocity, objectCenter;
			float toPlayerSpeed;

			toPlayerVelocity = -player->firstPersonViewAxis[0];
			toPlayerSpeed = entPhys->GetLinearVelocity() * toPlayerVelocity;

			if ( toPlayerSpeed > 64.f ) {
				objectCenter = entPhys->GetAbsBounds().GetCenter();

				theWall.SetNormal( player->firstPersonViewAxis[0] );
				theWall.FitThroughPoint( goalPos );

				if ( theWall.Side( objectCenter, 0.1f ) == PLANESIDE_BACK ) {
					int i, num;

					num = entPhys->GetNumClipModels();
					for ( i=0; i<num; i++ ) {
						entPhys->SetLinearVelocity( vec3_origin, i );
					}
				}
			}

			// Make sure the object isn't spinning too fast
			const float MAX_ROTATION_SPEED = 12.f;

			idVec3	angVel = entPhys->GetAngularVelocity();
			float	rotationSpeed = angVel.LengthFast();

			if ( rotationSpeed > MAX_ROTATION_SPEED ) {
				angVel.NormalizeFast();
				angVel *= MAX_ROTATION_SPEED;
				entPhys->SetAngularVelocity( angVel );
			}
		}

		// Orient projectiles away from the player
		if ( dragEnt.GetEntity()->IsType( idProjectile::Type ) ) {
			idAngles ang = player->firstPersonViewAxis[0].ToAngles();
			ang.pitch += 90.f;
			entPhys->SetAxis( ang.ToMat3() );
		}

		// Some kind of effect from gun to object?
		UpdateBeams();

		// If the object is stuck away from its intended position for more than 500ms, let it go.
		if ( drag.GetDistanceToGoal() > DRAG_FAIL_LEN ) {
			if ( dragFailTime < (gameLocal.slow.time - 500) ) {
				StopDrag( true );
				return 3;
			}
		} else {
			dragFailTime = gameLocal.slow.time;
		}

		// Currently holding an object
		return 2;
	}

	// Not holding, nothing to hold
	return 0;
}

/*
======================
idGrabber::UpdateBeams
======================
*/
void idGrabber::UpdateBeams( void ) {
	jointHandle_t	muzzle_joint;
	idVec3	muzzle_origin;
	idMat3	muzzle_axis;
	renderEntity_t *re;

	if ( !beam ) {
		return;
	}

	if ( dragEnt.IsValid() ) {
		idPlayer *thePlayer = owner.GetEntity();

		if ( beamTarget ) {
			beamTarget->SetOrigin( dragEnt.GetEntity()->GetPhysics()->GetAbsBounds().GetCenter() );
		}

		muzzle_joint = thePlayer->weapon.GetEntity()->GetAnimator()->GetJointHandle( "particle_upper" );
		if ( muzzle_joint != INVALID_JOINT ) {
			thePlayer->weapon.GetEntity()->GetJointWorldTransform( muzzle_joint, gameLocal.time, muzzle_origin, muzzle_axis );
		} else {
			muzzle_origin = thePlayer->GetPhysics()->GetOrigin();
		}

		beam->SetOrigin( muzzle_origin );
		re = beam->GetRenderEntity();
		re->origin = muzzle_origin;

		beam->UpdateVisuals();
		beam->Present();
	}
}

/*
==============
idGrabber::ApplyShake
==============
*/
void idGrabber::ApplyShake( void ) {
	float u = 1 - (float)( endTime - gameLocal.time ) / ( g_grabberHoldSeconds.GetFloat() * 1000 );

	if ( u >= 0.8f ) {
		idVec3 point, impulse;
		float shakeForceMagnitude = 450.f;
		float mass = dragEnt.GetEntity()->GetPhysics()->GetMass();

		shakeForceFlip = !shakeForceFlip;

		// get point to rotate around
		point = dragEnt.GetEntity()->GetPhysics()->GetOrigin();
		point.y += 1;

		// Articulated figures get less violent shake
		if ( holdingAF ) {
			shakeForceMagnitude = 120.f;
		}

		// calc impulse
		if ( shakeForceFlip ) {
			impulse.Set( 0, 0, shakeForceMagnitude * u * mass );
		}
		else {
			impulse.Set( 0, 0, -shakeForceMagnitude * u * mass );
		}

		dragEnt.GetEntity()->ApplyImpulse( NULL, 0, point, impulse );
	}
}

/*
==============
idGrabber::grabbableAI
==============
*/
bool idGrabber::grabbableAI( const char *aiName ) {
	//#modified-fva; BEGIN
	if (idStr::Cmpn(aiName, "monster_", 8)) {
		return false;
	}
	//#modified-fva; END

	// skip "monster_"
	aiName += 8;

	if (!idStr::Cmpn( aiName, "flying_lostsoul", 15 ) ||
		!idStr::Cmpn( aiName, "demon_trite", 11 ) ||
		!idStr::Cmp( aiName, "flying_forgotten" ) ||
		!idStr::Cmp( aiName, "demon_cherub" ) ||
		!idStr::Cmp( aiName, "demon_tick" )) {

		return true;
	}

	return false;
}

//#modified-fva; BEGIN
int idGrabber::CstServerUpdate(idPlayer *player, bool hide) {
	// code below is from idGrabber::Update (with changes)

	trace_t trace;
	idEntity *newEnt;

	// pause before allowing refire
	if ( lastFiredTime + FIRING_DELAY > gameLocal.time ) {
		return 3;
	}

	// Dead players release the trigger
	if ( hide || player->health <= 0 ) {
		//StopDrag( true );
		CstServerStopDrag(true);
		if ( hide ) {
			lastFiredTime = gameLocal.time - FIRING_DELAY + 250;
		}
		return 3;
	}

	// Check if object being held has been removed (dead demon, projectile, etc.)
	//if ( endTime > gameLocal.time ) {
	if ((gameLocal.cstSiGrabberUnlimitedTimeMP && endTime != 0) || endTime > gameLocal.time) {
		bool abort = !dragEnt.IsValid();

		if ( !abort && dragEnt.GetEntity()->IsType( idProjectile::Type ) ) {
			idProjectile *proj = (idProjectile *)dragEnt.GetEntity();

			if ( proj->GetProjectileState() >= 3 ) {
				abort = true;
			}
		}
		if ( !abort && dragEnt.GetEntity()->IsHidden() ) {
			abort = true;
		}

		// Dropping an item with "reload" is now signaled via idGrabber::CstReload and idWeapon::Reload. Also works in multiplayer.
		if (cstDrop) {
			abort = true;
		}
        
		if ( abort ) {
			//StopDrag( true );
			CstServerStopDrag(true);
			return 3;
		}
	}

	owner = player;

	// if no entity selected for dragging
    if ( !dragEnt.GetEntity() ) {
		idBounds bounds;
		//idVec3 end = player->firstPersonViewOrigin + player->firstPersonViewAxis[0] * dragTraceDist;
		idVec3 cstViewOrigin;
		idMat3 cstViewAxis;
		player->CstGetViewPosForGrabberMP(cstViewOrigin, cstViewAxis);
		idVec3 end = cstViewOrigin + cstViewAxis[0] * dragTraceDist;

		bounds.Zero();
		bounds.ExpandSelf( TRACE_BOUNDS_SIZE );

		//gameLocal.clip.TraceBounds( trace, player->firstPersonViewOrigin, end, bounds, MASK_SHOT_RENDERMODEL|CONTENTS_PROJECTILE|CONTENTS_MOVEABLECLIP, player );
		gameLocal.clip.TraceBounds(trace, cstViewOrigin, end, bounds, MASK_SHOT_RENDERMODEL | CONTENTS_PROJECTILE | CONTENTS_MOVEABLECLIP, player);

		// If the trace hit something
		if ( trace.fraction < 1.0f ) {
			newEnt = gameLocal.entities[ trace.c.entityNum ];

			// if entity is already being grabbed then bypass
			if ( gameLocal.isMultiplayer && newEnt->IsGrabbed() ) {
				return 0;
			}

			// Check if this is a valid entity to hold
			if ( newEnt && ( newEnt->IsType( idMoveable::Type ) ||
					// In multiplayer, only grab idMoveable's and idProjectile's (except idGuidedProjectile's)
					//newEnt->IsType( idMoveableItem::Type ) ||
					//newEnt->IsType( idProjectile::Type ) ||
					//newEnt->IsType( idAFEntity_Gibbable::Type ) ) &&
					(newEnt->IsType(idProjectile::Type) && !newEnt->IsType(idGuidedProjectile::Type))) &&
					newEnt->noGrab == false &&
					newEnt->GetPhysics()->GetBounds().GetRadius() < MAX_PICKUP_SIZE &&
					newEnt->GetPhysics()->GetLinearVelocity().LengthSqr() < MAX_PICKUP_VELOCITY ) {

				bool validAF = true;

				if ( validAF && player->usercmd.buttons & BUTTON_ATTACK ) {
					// Grab this entity and start dragging it around
					//StartDrag( newEnt, trace.c.id );
					CstServerStartDrag(newEnt); // always grab body 0 (id = 0) in multiplayer
				} else if ( validAF ) {
					// A holdable object is ready to be grabbed
					return 1;
				}
			}
		}
	}

	// check backwards server time in multiplayer
	bool allow = true;

	if ( gameLocal.isMultiplayer ) {

		// if we've marched backwards
		if ( gameLocal.slow.time < startDragTime ) {
			allow = false;
		}
	}


	// if there is an entity selected for dragging
	if ( dragEnt.GetEntity() && allow ) {
		idPhysics *entPhys = dragEnt.GetEntity()->GetPhysics();
		idVec3 goalPos;

		// If the player lets go of attack, or time is up
		if ( !( player->usercmd.buttons & BUTTON_ATTACK ) ) {
			//StopDrag( false );
			CstServerStopDrag(false);
			return 3;
		}
		//if ( gameLocal.time > endTime ) {
		if (!gameLocal.cstSiGrabberUnlimitedTimeMP && gameLocal.time > endTime) {
			//StopDrag( true );
			CstServerStopDrag(true);
			return 3;
		}

		// Check if the player is standing on the object
		if ( !holdingAF ) {
			idBounds	playerBounds;
			idBounds	objectBounds = entPhys->GetAbsBounds();
			idVec3		newPoint = player->GetPhysics()->GetOrigin();

			// create a bounds at the players feet
			playerBounds.Clear();
			playerBounds.AddPoint( newPoint );
			newPoint.z -= 1.f;
			playerBounds.AddPoint( newPoint );
			playerBounds.ExpandSelf( 8.f );

			// If it intersects the object bounds, then drop it
			if ( playerBounds.IntersectsBounds( objectBounds ) ) {
				//StopDrag( true );
				CstServerStopDrag(true);
				return 3;
			}
		}

		cstDistanceToGoal = CstEvalMP();

		//if ( drag.GetDistanceToGoal() > DRAG_FAIL_LEN ) {
		if (cstDistanceToGoal > DRAG_FAIL_LEN) {
			if ( dragFailTime < (gameLocal.slow.time - 500) ) {
				//StopDrag( true );
				CstServerStopDrag(true);
				return 3;
			}
		} else {
			dragFailTime = gameLocal.slow.time;
		}

		// Currently holding an object
		return 2;
	}

	// Not holding, nothing to hold
	return 0;
}

// ===========
void idGrabber::CstServerStartDrag(idEntity *grabEnt) {
	// code below is from idGrabber::StartDrag (with changes)

	idPlayer *thePlayer = owner.GetEntity();

	cstDrop = false;

	holdingAF = false;
	dragFailTime = gameLocal.slow.time;
	startDragTime = gameLocal.slow.time;

	// set grabbed state for networking
	grabEnt->SetGrabbedState( true );

	grabEnt->fl.cstGrabbedNoPhysicsMP = true;
	grabEnt->fl.cstGrabbedNoThinkMP = true;

	// This is the new object to drag around
	dragEnt = grabEnt;

	// Move the object to the fast group (helltime)
	grabEnt->timeGroup = TIME_GROUP2;

	// Handle specific class types
	if ( grabEnt->IsType( idProjectile::Type ) ) {
		idProjectile* p = (idProjectile*)grabEnt;

		p->CatchProjectile( thePlayer, "_catch" );

		// Make the projectile non-solid to other projectiles/enemies (special hack for helltime hunter)
		if ( !idStr::Cmp( grabEnt->GetEntityDefName(), "projectile_helltime_killer" ) ) {
			savedContents = CONTENTS_PROJECTILE;
			savedClipmask = MASK_SHOT_RENDERMODEL|CONTENTS_PROJECTILE;
		} else {
			savedContents = grabEnt->GetPhysics()->GetContents();
			savedClipmask = grabEnt->GetPhysics()->GetClipMask();
		}
		grabEnt->GetPhysics()->SetContents( 0 );
		grabEnt->GetPhysics()->SetClipMask( CONTENTS_SOLID|CONTENTS_BODY );

		if (!gameLocal.isClient) {
			if (grabEnt->IsType(idBFGProjectile::Type)) {
				idBFGProjectile* bp = static_cast<idBFGProjectile*>(grabEnt);
				bp->CstStopBeams();
			}

			if (grabEnt->spawnArgs.GetFloat("fuse") > 0) {
				if (grabEnt->spawnArgs.GetBool("detonate_on_fuse")) {
					grabEnt->CancelEvents(&EV_Explode);
				} else {
					grabEnt->CancelEvents(&EV_Fizzle);
				}
			}
		}

	} else if ( grabEnt->IsType( idExplodingBarrel::Type ) ) {
		idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(grabEnt);

		ebarrel->StartBurning();

	}

	// Get the current physics object to manipulate
	idPhysics *phys = grabEnt->GetPhysics();

	// Turn off gravity on object
	saveGravity = phys->GetGravity();
	//phys->SetGravity( vec3_origin );
	phys->SetGravity(CST_GRABBED_GRAVITY);

	// Set the ending time for the hold
	endTime = gameLocal.time + g_grabberHoldSeconds.GetFloat() * 1000;

	// start the screen warp
	warpId = thePlayer->playerView.AddWarp( phys->GetOrigin(), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 160, 2000 );
}

// ===========
void idGrabber::CstServerStopDrag(bool dropOnly) {
	// code below is from idGrabber::StopDrag (with changes)

	idPlayer *thePlayer = owner.GetEntity();

	cstDrop = false;

	if ( dragEnt.IsValid() ) {
		idEntity *ent = dragEnt.GetEntity();

		// set grabbed state for networking
		ent->SetGrabbedState( false );

		ent->fl.cstGrabbedNoPhysicsMP = false;
		ent->fl.cstGrabbedNoThinkMP = false;

		// If a cinematic has started, allow dropped object to think in cinematics
		if ( gameLocal.inCinematic ) {
			ent->cinematic = true;
		}

		// Restore Gravity
		ent->GetPhysics()->SetGravity( saveGravity );

		// Move the object back to the slow group (helltime)
		ent->timeGroup = TIME_GROUP1;

		// If the object isn't near its goal, just drop it in place.
		//if ( !ent->IsType( idProjectile::Type ) && ( dropOnly || drag.GetDistanceToGoal() > DRAG_FAIL_LEN ) ) {
		if (!ent->IsType(idProjectile::Type) && (dropOnly || cstDistanceToGoal > DRAG_FAIL_LEN)) {
			ent->GetPhysics()->SetLinearVelocity( vec3_origin );
			//thePlayer->StartSoundShader( declManager->FindSound( "grabber_maindrop" ), SND_CHANNEL_WEAPON, 0, false, NULL );
			thePlayer->StartSoundShader(declManager->FindSound("grabber_maindrop"), SND_CHANNEL_WEAPON, 0, true, NULL);

			if ( ent->IsType( idExplodingBarrel::Type ) ) {
				idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(ent);

				ebarrel->SetStability( true );
				ebarrel->StopBurning();
			}
		} else {
			// Shoot the object forward
			idVec3 cstViewOrigin;
			idMat3 cstViewAxis;
			thePlayer->CstGetViewPosForGrabberMP(cstViewOrigin, cstViewAxis);

			int cstStableThrow = gameLocal.cstSiGrabberStableThrowMP;
			if (cstStableThrow > 0) {
				// remove any linear movement that is not in the throw direction
				idPhysics *entPhys = ent->GetPhysics();
				idVec3 entVel = entPhys->GetLinearVelocity(id);
				idVec3 baseVel;
				if (cstStableThrow == 1) {
					// remove relative to the player
					baseVel = thePlayer->GetPlayerPhysics()->GetLinearVelocity();
				} else {
					// remove relative to the world
					baseVel = vec3_origin;
				}
				//float speed = (entVel - baseVel) * thePlayer->firstPersonViewAxis[0];
				float speed = (entVel - baseVel) * cstViewAxis[0];
				if (speed < 0.f) {
					speed = 0.f;
				}
				//entVel = baseVel + (speed * thePlayer->firstPersonViewAxis[0]);
				entVel = baseVel + (speed * cstViewAxis[0]);
				int num = entPhys->GetNumClipModels();
				for (int i = 0; i < num; i++) {
					entPhys->SetLinearVelocity(entVel, i);
				}
			}

			//ent->ApplyImpulse( thePlayer, 0, ent->GetPhysics()->GetOrigin(), thePlayer->firstPersonViewAxis[0] * THROW_SCALE * ent->GetPhysics()->GetMass() );
			ent->ApplyImpulse(thePlayer, 0, ent->GetPhysics()->GetOrigin(), cstViewAxis[0] * THROW_SCALE * ent->GetPhysics()->GetMass());

			//thePlayer->StartSoundShader( declManager->FindSound( "grabber_release" ), SND_CHANNEL_WEAPON, 0, false, NULL );
			thePlayer->StartSoundShader(declManager->FindSound("grabber_release"), SND_CHANNEL_WEAPON, 0, true, NULL);

			// Orient projectiles away from the player
			if ( ent->IsType( idProjectile::Type ) ) {
				idPlayer *player = owner.GetEntity();
				//idAngles ang = player->firstPersonViewAxis[0].ToAngles();
				idAngles ang = cstViewAxis[0].ToAngles();

				ang.pitch += 90.f;
				ent->GetPhysics()->SetAxis( ang.ToMat3() );
				ent->GetPhysics()->SetAngularVelocity( vec3_origin );

				// Restore projectile contents
				ent->GetPhysics()->SetContents( savedContents );
				ent->GetPhysics()->SetClipMask( savedClipmask );

				if (!gameLocal.isClient) {
					if (ent->IsType(idBFGProjectile::Type)) {
						idBFGProjectile* bp = static_cast<idBFGProjectile*>(ent);
						bp->CstStartBeams();
					}

					float fuse = ent->spawnArgs.GetFloat("fuse");
					if (fuse > 0) {
						if (ent->spawnArgs.GetBool("detonate_on_fuse")) {
							ent->PostEventSec(&EV_Explode, fuse);
						} else {
							ent->PostEventSec(&EV_Fizzle, fuse);
						}
					}
				}

				idProjectile *p = static_cast<idProjectile*>(ent);
				p->CstReleaseProjectile(thePlayer);

			} else if ( ent->IsType( idMoveable::Type ) ) {
				// Turn on damage for this object
				idMoveable *obj = static_cast<idMoveable*>(ent);
				obj->EnableDamage( true, 2.5f );
				obj->SetAttacker( thePlayer );

				if ( ent->IsType( idExplodingBarrel::Type ) ) {
					idExplodingBarrel *ebarrel = static_cast<idExplodingBarrel*>(ent);
					ebarrel->SetStability( false );
				}

			}
		}
	}

	if ( warpId != -1 ) {
		thePlayer->playerView.FreeWarp( warpId );
		warpId = -1;
	}

	lastFiredTime = gameLocal.time;
	dragEnt = NULL;
	endTime = 0;
}

// ===========
void idGrabber::CstClientUpdate(idPlayer *player, bool hide) {
	// code below is from idGrabber::Update (with changes)

	owner = player;

	// Dead players release the trigger
	//if (hide || player->health <= 0) {
	if (hide || !owner.IsValid() || player->health <= 0) {
		CstClientStopDrag();
		return;
	}

	// Check if object being held has been removed (dead demon, projectile, etc.)
	//bool abort = !dragEnt.IsValid();
	if (!dragEnt.IsValid()) {
		return;
	}
	bool abort = false;

	if (!abort && dragEnt.GetEntity()->IsType(idProjectile::Type)) {
		idProjectile *proj = (idProjectile *)dragEnt.GetEntity();

		if (proj->GetProjectileState() >= 3) {
			abort = true;
		}
	}
	if (!abort && dragEnt.GetEntity()->IsHidden()) {
		abort = true;
	}
	if (abort) {
		CstClientStopDrag();
		return;
	}

	CstEvalMP();
}

// ===========
void idGrabber::CstClientStartDrag(idEntity *grabEnt) {
	// code below is from idGrabber::StartDrag (with changes)

	if (!owner.IsValid()) {
		return;
	}
	cstStart = true;

	idPlayer *thePlayer = owner.GetEntity();
	dragEnt = grabEnt;

	// set grabbed state for networking
	grabEnt->SetGrabbedState(true);

	grabEnt->fl.cstGrabbedNoPhysicsMP = true;
	grabEnt->fl.cstGrabbedNoThinkMP = true;

	// Move the object to the fast group (helltime)
	grabEnt->timeGroup = TIME_GROUP2;

	// Handle specific class types
	if (grabEnt->IsType(idProjectile::Type)) {
		idProjectile* p = (idProjectile*)grabEnt;

		p->CatchProjectile(thePlayer, "_catch");

		// Make the projectile non-solid to other projectiles/enemies (special hack for helltime hunter)
		if (!idStr::Cmp(grabEnt->GetEntityDefName(), "projectile_helltime_killer")) {
			savedContents = CONTENTS_PROJECTILE;
			savedClipmask = MASK_SHOT_RENDERMODEL | CONTENTS_PROJECTILE;
		} else {
			savedContents = grabEnt->GetPhysics()->GetContents();
			savedClipmask = grabEnt->GetPhysics()->GetClipMask();
		}
		grabEnt->GetPhysics()->SetContents(0);
		grabEnt->GetPhysics()->SetClipMask(CONTENTS_SOLID | CONTENTS_BODY);
	}

	// Get the current physics object to manipulate
	idPhysics *phys = grabEnt->GetPhysics();

	// Turn off gravity on object
	saveGravity = phys->GetGravity();
	//phys->SetGravity(vec3_origin);
	phys->SetGravity(CST_GRABBED_GRAVITY);

	// start the screen warp
	warpId = thePlayer->playerView.AddWarp(phys->GetOrigin(), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 160, 2000);
}

// ===========
void idGrabber::CstClientStopDrag() {
	// code below is from idGrabber::StopDrag (with changes)

	if (dragEnt.IsValid()) {
		idEntity *ent = dragEnt.GetEntity();

		// set grabbed state for networking
		ent->SetGrabbedState(false);

		ent->fl.cstGrabbedNoPhysicsMP = false;
		ent->fl.cstGrabbedNoThinkMP = false;

		// If a cinematic has started, allow dropped object to think in cinematics
		if (gameLocal.inCinematic) {
			ent->cinematic = true;
		}

		// Restore Gravity
		ent->GetPhysics()->SetGravity(saveGravity);

		// Move the object back to the slow group (helltime)
		ent->timeGroup = TIME_GROUP1;

		if (ent->IsType(idProjectile::Type)) {
			idProjectile *p = static_cast<idProjectile*>(ent);
			p->CstReleaseProjectile(owner.GetEntity());
		}
	}

	if (warpId != -1) {
		if (owner.IsValid()) {
			idPlayer *thePlayer = owner.GetEntity();
			thePlayer->playerView.FreeWarp(warpId);
		}
		warpId = -1;
	}

	dragEnt = NULL;
}

// ============
float idGrabber::CstEvalMP() {
	if (!owner.IsValid() || !dragEnt.IsValid()) {
		return 0; // dummy return value; check the conditions above before calling the function
	}

	// Note: in multiplayer, the think and physics functions of grabbed entities are controlled/ran by the grabber

	idPlayer* player = static_cast<idPlayer*>(owner.GetEntity());
	idPhysics* physics = dragEnt.GetEntity()->GetPhysics();

	idVec3 cstViewOrigin;
	idMat3 cstViewAxis;
	player->CstGetViewPosForGrabberMP(cstViewOrigin, cstViewAxis);

	//idVec3 goalPosition = player->firstPersonViewOrigin + player->firstPersonViewAxis[0] * HOLD_DISTANCE;
	idVec3 goalPosition = cstViewOrigin + cstViewAxis[0] * HOLD_DISTANCE;
	float distanceToGoal;

	// code block below is adapted from idForce_Grab::Evaluate (neo\game\physics\Force_Grab.cpp)
	{
		// in multiplayer, the grabbed body is always body 0 (id = 0)

		idVec3			forceDir, v, objectCenter;
		float			forceAmt;
		//float			mass = physics->GetMass(id);
		float			mass = physics->GetMass(0);

		//objectCenter = physics->GetAbsBounds(id).GetCenter();
		objectCenter = physics->GetAbsBounds(0).GetCenter();

		forceDir = goalPosition - objectCenter;
		distanceToGoal = forceDir.Normalize();

		float temp = distanceToGoal;
		if (temp > 12.f && temp < 32.f) {
			temp = 32.f;
		}
		forceAmt = (1000.f * mass) + (500.f * temp * mass);

		if (forceAmt / mass > 120000.f) {
			forceAmt = 120000.f * mass;
		}
		//physics->AddForce(id, objectCenter, forceDir * forceAmt);
		physics->AddForce(0, objectCenter, forceDir * forceAmt);

		if (distanceToGoal < 196.f) {
			//v = physics->GetLinearVelocity(id);
			v = physics->GetLinearVelocity(0);
			//physics->SetLinearVelocity(v * damping, id);
			physics->SetLinearVelocity(v * g_grabberDamping.GetFloat(), 0);
		}
		if (distanceToGoal < 16.f) {
			//v = physics->GetAngularVelocity(id);
			v = physics->GetAngularVelocity(0);
			if (v.LengthSqr() > Square(8)) {
				//physics->SetAngularVelocity(v * 0.99999f, id);
				physics->SetAngularVelocity(v * 0.99999f, 0);
			}
		}
	}

	// code block below is adapted from idGrabber::Update
	{
		idPhysics* entPhys = physics;
		idVec3 goalPos = goalPosition;

		// If an object is flying too fast toward the player, stop it hard
		if (g_grabberHardStop.GetBool()) {
			idPlane theWall;
			idVec3 toPlayerVelocity, objectCenter;
			float toPlayerSpeed;

			//toPlayerVelocity = -player->firstPersonViewAxis[0];
			toPlayerVelocity = -cstViewAxis[0];
			toPlayerSpeed = entPhys->GetLinearVelocity() * toPlayerVelocity;

			if (toPlayerSpeed > 64.f) {
				objectCenter = entPhys->GetAbsBounds().GetCenter();

				//theWall.SetNormal(player->firstPersonViewAxis[0]);
				theWall.SetNormal(cstViewAxis[0]);
				theWall.FitThroughPoint(goalPos);

				if (theWall.Side(objectCenter, 0.1f) == PLANESIDE_BACK) {
					int i, num;

					num = entPhys->GetNumClipModels();
					for (i = 0; i < num; i++) {
						entPhys->SetLinearVelocity(vec3_origin, i);
					}
				}
			}

			// Make sure the object isn't spinning too fast
			const float MAX_ROTATION_SPEED = 12.f;

			idVec3	angVel = entPhys->GetAngularVelocity();
			float	rotationSpeed = angVel.LengthFast();

			if (rotationSpeed > MAX_ROTATION_SPEED) {
				angVel.NormalizeFast();
				angVel *= MAX_ROTATION_SPEED;
				entPhys->SetAngularVelocity(angVel);
			}
		}

		// Orient projectiles away from the player
		if (dragEnt.GetEntity()->IsType(idProjectile::Type)) {
			//idAngles ang = player->firstPersonViewAxis[0].ToAngles();
			idAngles ang = cstViewAxis[0].ToAngles();
			ang.pitch += 90.f;
			entPhys->SetAxis(ang.ToMat3());
		}
	}

	idEntity *ent = dragEnt.GetEntity();

	if (gameLocal.isClient && gameLocal.localClientNum != player->entityNumber) {
		if (!gameLocal.isNewFrame) {
			// think with physics enabled
			ent->fl.cstGrabbedNoPhysicsMP = false;
			ent->ClientPredictionThink();
			ent->fl.cstGrabbedNoPhysicsMP = true;
		} else {
			// run physics, but don't think
			ent->fl.cstGrabbedNoPhysicsMP = false;
			ent->RunPhysics();
			ent->fl.cstGrabbedNoPhysicsMP = true;

			// smooth results
			if (cstStart) {
				cstStart = false;
				cstOrigin = physics->GetOrigin();
				cstAngles = physics->GetAxis().ToAngles().Normalize180();
			}

			idVec3 org = physics->GetOrigin();
			idMat3 axis = physics->GetAxis();
			idAngles angles = axis.ToAngles().Normalize180();

			idVec3 orgDiff = cstOrigin - org;
			if (orgDiff.LengthSqr() < Square(100.0f)) {
				cstOrigin = org + orgDiff * gameLocal.clientSmoothing;

				idAngles angDiff = cstAngles - angles;
				angDiff.Normalize180();
				cstAngles = angles + angDiff * gameLocal.clientSmoothing;
				cstAngles.Normalize180();

			} else {
				cstOrigin = org;
				cstAngles = angles;
			}

			// use smoothed results for visuals
			physics->SetOrigin(cstOrigin);
			physics->SetAxis(cstAngles.ToMat3());

			ent->ClientPredictionThink(); // think with physics disabled
			ent->UpdateVisuals();
			ent->Present();

			// restore non-smoothed results
			physics->SetOrigin(org);
			physics->SetAxis(axis);
		}
		return 0; // dummy return value
	}

	// think with physics enabled
	ent->fl.cstGrabbedNoPhysicsMP = false;
	if (gameLocal.isClient) {
		ent->ClientPredictionThink();
	} else {
		ent->Think();
	}
	ent->fl.cstGrabbedNoPhysicsMP = true;

	return distanceToGoal;
}

// ===========
void idGrabber::CstWriteToSnapshot(idBitMsgDelta &msg) const {
	msg.WriteBits(dragEnt.GetSpawnId(), 32);
}

// ===========
void idGrabber::CstReadFromSnapshot(int grabberState, const idBitMsgDelta &msg) {
	idEntityPtr<idEntity> newEnt;
	newEnt = NULL;

	int newSpawnId = msg.ReadBits(32); // reading msg bits

	if (grabberState == 2) {
		newEnt.SetSpawnId(newSpawnId);
	}

	if (!dragEnt.IsValid() && !newEnt.IsValid()) {
		// nothing to do; early return
		return;
	} else if (!dragEnt.IsValid() && newEnt.IsValid()) {
		CstClientStartDrag(newEnt.GetEntity());
	} else if (dragEnt.IsValid() && !newEnt.IsValid()) {
		CstClientStopDrag();
	} else if (dragEnt.IsValid() && newEnt.IsValid() && dragEnt.GetSpawnId() != newEnt.GetSpawnId()) {
		CstClientStopDrag();
		CstClientStartDrag(newEnt.GetEntity());
	}
}

// ===========
void idGrabber::CstReload(int grabberState) {
	if (!gameLocal.isClient && grabberState == 2) {
		cstDrop = true;
	}
}

//#modified-fva; END

#endif
