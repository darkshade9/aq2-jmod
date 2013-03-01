//^J Mod for Action Quake2 - Jumping practice and teaching utilities
//Copyright (C) 2006 Patrick Stage
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//version 2 as published by the Free Software Foundation
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//If you wish to contact the author, you may email him at patman_q@hotmail.com
//or check his website for contacts at http://patman.has.it
//

// zucc
// File for our new commands.

// laser sight patch, by Geza Beladi

#include "g_local.h"

extern void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);

/*----------------------------------------
SP_LaserSight

  Create/remove the laser sight entity
-----------------------------------------*/

#define lss self->lasersight
//int lsi;

void SP_LaserSight(edict_t *self, gitem_t *item )
{
	vec3_t  start,forward,right,end;
	int laser_on = 1;

	AngleVectors (self->client->v_angle, forward, right, NULL);

	VectorSet(end,100 , 0, 0);
	G_ProjectSource (self->s.origin, end, forward, right, start);

	lss = G_Spawn ();
	lss->owner = self;
	lss->movetype = MOVETYPE_NOCLIP;
	lss->solid = SOLID_NOT;
	lss->classname = "lasersight";
	lss->s.modelindex = 0;
	lss->s.renderfx = RF_TRANSLUCENT;
	lss->think = LaserSightThink;
	lss->nextthink = level.time + 0.01;
}

/*---------------------------------------------
LaserSightThink

  Updates the sights position, angle, and shape
  is the lasersight entity
---------------------------------------------*/

void LaserSightThink (edict_t *self)
{
	if ((!(self->owner->client->resp.toggles & TG_LASER)) || (self->owner->client->pers.spectator)
		|| ( self->owner->lasersight != self ))
		self->think = G_FreeEdict;

	if (self->owner->client->resp.toggles & TG_LASER)
	{
		vec3_t start,end,endp,offset;
		vec3_t forward,right,up;
		vec3_t angles;
		trace_t tr;
		int height = -1;

		// zucc compensate for weapon ride up
		VectorAdd (self->owner->client->v_angle, self->owner->client->kick_angles, angles);
		AngleVectors (angles, forward, right, up);

		VectorSet(offset,24 , 8, self->owner->viewheight- height);

		P_ProjectSource (self->owner->client, self->owner->s.origin, offset, forward, right, start);

		VectorMA(start,8192,forward,end);

		PRETRACE();
		tr = gi.trace (start,NULL,NULL, end,self->owner,CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);
		POSTTRACE();

		if (tr.fraction != 1) {
			VectorMA(tr.endpos,-4,forward,endp);
			VectorCopy(endp,tr.endpos);
		}

		vectoangles(tr.plane.normal,self->s.angles);
		VectorCopy(tr.endpos,self->s.origin);

		gi.linkentity (self);

		self->s.modelindex = gi.modelindex ("sprites/lsight.sp2");
	}

	self->nextthink = level.time + 0.1;
}

/*//PaTMaN-NORELOAD
void Cmd_New_Reload_f( edict_t *ent )
{
//FB 6/1/99 - refuse to reload during LCA
        if ((int)teamplay->value && lights_camera_action)
                return;
//FB 6/1/99
                
        ent->client->reload_attempts++;
}
*/


//+BD ENTIRE CODE BLOCK NEW
// Cmd_Reload_f()
// Handles weapon reload requests
/*//PaTMaN-NORELOAD
void Cmd_Reload_f (edict_t *ent)
{
//      int rds_left;           //+BD - Variable to handle rounds left


        //+BD - If the player is dead, don't bother
        if(ent->deadflag == DEAD_DEAD)
        {
                //gi.centerprintf(ent, "I know you're a hard ass,\nBUT YOU'RE FUCKING DEAD!!\n");
                return;
        }

/*        if(ent->client->weaponstate == WEAPON_BANDAGING
                                                || ent->client->bandaging == 1
                                                || ent->client->bandage_stopped == 1
                                                || ent->client->weaponstate == WEAPON_ACTIVATING
                                                || ent->client->weaponstate == WEAPON_DROPPING
                                                                                                || ent->client->weaponstate == WEAPON_FIRING )
        {
                return;
        }
*/                
//PaTMaN-NORELOAD                if (!ent->client->fast_reload)
//PaTMaN-NORELOAD                        ent->client->reload_attempts--;
//PaTMaN-NORELOAD        if ( ent->client->reload_attempts < 0 )
//PaTMaN-NORELOAD                        ent->client->reload_attempts = 0;
/*//PaTMaN-NORELOAD
        //First, grab the current magazine max count...
        if ( ( ent->client->curr_weap == MK23_NUM  )
                || ( ent->client->curr_weap == MP5_NUM ) 
                || ( ent->client->curr_weap == M4_NUM ) 
                || ( ent->client->curr_weap == M3_NUM )
                || ( ent->client->curr_weap == HC_NUM )
                || ( ent->client->curr_weap == SNIPER_NUM )
                || ( ent->client->curr_weap == DUAL_NUM ) )
        {
        }
                else    //We should never get here, but...
                //BD 5/26 - Actually we get here quite often right now. Just exit for weaps that we
                //          don't want reloaded or that never reload (grenades)
        {
                //gi.centerprintf(ent,"Where'd you train?\nYou can't reload that!\n");
                return;
        }
        
        if(ent->client->pers.inventory[ent->client->ammo_index])
        {       
                //Set the weaponstate...
                if ( ent->client->curr_weap == M3_NUM )
                {
                        if (ent->client->shot_rds >= ent->client->shot_max)
                        {
                                return;
                        }
                        // already in the process of reloading!
//PaTMaN-NORELOAD                        if ( ent->client->weaponstate == WEAPON_RELOADING && (ent->client->shot_rds < (ent->client->shot_max -1)) && !(ent->client->fast_reload) && ((ent->client->pers.inventory[ent->client->ammo_index] -1) > 0 ))
//PaTMaN-NORELOAD                        {
//PaTMaN-NORELOAD                        	// don't let them start fast reloading until far enough into the firing sequence
//PaTMaN-NORELOAD				// this gives them a chance to break off from reloading to fire the weapon - zucc
//PaTMaN-NORELOAD				if ( ent->client->ps.gunframe >=  48 )
//PaTMaN-NORELOAD				{
//PaTMaN-NORELOAD	                                ent->client->fast_reload = 1;
//PaTMaN-NORELOAD        	                        (ent->client->pers.inventory[ent->client->ammo_index])--;
//PaTMaN-NORELOAD				}
//PaTMaN-NORELOAD					else
//PaTMaN-NORELOAD				{
//PaTMaN-NORELOAD					ent->client->reload_attempts++;
//PaTMaN-NORELOAD                                                       
//PaTMaN-NORELOAD				}
//PaTMaN-NORELOAD                        }
                }                       
                if ( ent->client->curr_weap == HC_NUM )
                {
                        if (ent->client->cannon_rds >= ent->client->cannon_max)
                        {
                        	return;
                        }
                                         
                        if (!(ent->client->pers.inventory[ent->client->ammo_index] >= 2))
                                return;
                }
                if ( ent->client->curr_weap == SNIPER_NUM )
                {
                        if (ent->client->sniper_rds >= ent->client->sniper_max)
                        {
                                return;
                        }
                        // already in the process of reloading!
//PaTMaN-NOBANDAGE                        if ( ent->client->weaponstate == WEAPON_RELOADING && (ent->client->sniper_rds < (ent->client->sniper_max -1)) && !(ent->client->fast_reload) && ((ent->client->pers.inventory[ent->client->ammo_index] -1) > 0 ) )
//PaTMaN-NOBANDAGE                        {
				// don't let them start fast reloading until far enough into the firing sequence
				// this gives them a chance to break off from reloading to fire the weapon - zucc
//PaTMaN-NORELOAD				if ( ent->client->ps.gunframe >= 72 )
//PaTMaN-NORELOAD				{
//PaTMaN-NORELOAD					ent->client->fast_reload = 1;
//PaTMaN-NORELOAD					(ent->client->pers.inventory[ent->client->ammo_index])--;
//PaTMaN-NORELOAD				}
//PaTMaN-NORELOAD					else
//PaTMaN-NORELOAD				{
//PaTMaN-NORELOAD					ent->client->reload_attempts++;
//PaTMaN-NORELOAD				}
//PaTMaN-NOBANDAGE                        }
//PaTMaN-NOBANDAGE                        ent->client->ps.fov = 90;
//PaTMaN-NOBANDAGE                        if ( ent->client->pers.weapon )
//PaTMaN-NOBANDAGE                                ent->client->ps.gunindex = gi.modelindex( ent->client->pers.weapon->view_model );
                }                       
                if ( ent->client->curr_weap == DUAL_NUM )
                {
                        if (!(ent->client->pers.inventory[ent->client->ammo_index] >= 2))
                                return;
//FIREBLADE 7/11/1999 - stop reloading when weapon already full
			if (ent->client->dual_rds == ent->client->dual_max)
				return;
                }
		if (ent->client->curr_weap == MP5_NUM)
		{
			if (ent->client->mp5_rds == ent->client->mp5_max)
				return;
		}
		if (ent->client->curr_weap == M4_NUM)
		{
			if (ent->client->m4_rds == ent->client->m4_max)
				return;
		}
		if (ent->client->curr_weap == MK23_NUM)
		{
			if (ent->client->mk23_rds == ent->client->mk23_max)
				return;
		}
//FIREBLADE
                
//PaTMaN-NORELOADING                ent->client->weaponstate = WEAPON_RELOADING;
                //(ent->client->pers.inventory[ent->client->ammo_index])--;
        }
        else
                gi.cprintf (ent, PRINT_HIGH, "Out of ammo\n");
                //gi.centerprintf(ent,"Pull your head out-\nYou've got NO AMMO!\n");
}
//+BD END CODE BLOCK
*///PaTMaN-NORELOAD
void Cmd_New_Weapon_f( edict_t *ent )
{
        ent->client->weapon_attempts++;
        if ( ent->client->weapon_attempts == 1 )
                Cmd_Weapon_f(ent);
}


// function to change the firing mode of weapons (when appropriate) 

void Cmd_Weapon_f ( edict_t *ent )
{
        int dead;

        dead = (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD);

        ent->client->weapon_attempts--;
        if ( ent->client->weapon_attempts < 0 )
                        ent->client->weapon_attempts = 0;
                
     /*           if ( ent->client->bandaging || ent->client->bandage_stopped )
		{
                
                                gi.cprintf(ent, PRINT_HIGH, "You'll get to your weapon when your done bandaging!\n");
                                gi.cprintf(ent, PRINT_HIGH, "REPORT AS BANDAGE BUG!\n"); //PaTMaN
                ent->client->weapon_attempts++;
                                return;
        }
*/

//weapons are reloading slow (sniper/shotgun) i think due to bandaging on every endframe

                if ( ent->client->weaponstate == WEAPON_FIRING /*|| ent->client->weaponstate == WEAPON_BUSY*/ )
                {
                        //gi.cprintf(ent, PRINT_HIGH, "Try again when you aren't using your weapon.\n");
                        ent->client->weapon_attempts++;
                        return;
                }
        
                if ( ent->client->curr_weap == MK23_NUM )
        {
                if (!dead)
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
				if (ent->client->resp.wmodes & WM_MK23) ent->client->resp.wmodes -= WM_MK23;
				else ent->client->resp.wmodes += WM_MK23;
                //ent->client->resp.mk23_mode = !(ent->client->resp.mk23_mode);
                if (ent->client->resp.wmodes & WM_MK23)//ent->client->resp.mk23_mode )
                        gi.cprintf (ent, PRINT_HIGH, "MK23 Pistol set for semi-automatic action\n");
                else
                        gi.cprintf (ent, PRINT_HIGH, "MK23 Pistol set for automatic action\n");
        }
        if ( ent->client->curr_weap == MP5_NUM )
        {
                if (!dead)
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
				if (ent->client->resp.wmodes & WM_MP5) ent->client->resp.wmodes -= WM_MP5;
				else ent->client->resp.wmodes += WM_MP5;
             //   ent->client->resp.mp5_mode = !(ent->client->resp.mp5_mode);
                if ( ent->client->resp.wmodes & WM_MP5 )//ent->client->resp.mp5_mode )
                        gi.cprintf (ent, PRINT_HIGH, "MP5 set to 3 Round Burst mode\n");
                else
                        gi.cprintf (ent, PRINT_HIGH, "MP5 set to Full Automatic mode\n");
        }
        if ( ent->client->curr_weap == M4_NUM )
        {
                if (!dead)
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
				if (ent->client->resp.wmodes & WM_M4) ent->client->resp.wmodes -= WM_M4;
				else ent->client->resp.wmodes += WM_M4;
                //ent->client->resp.m4_mode = !(ent->client->resp.m4_mode);
                if (ent->client->resp.wmodes & WM_M4)//ent->client->resp.m4_mode )
                        gi.cprintf (ent, PRINT_HIGH, "M4 set to 3 Round Burst mode\n");
                else
                        gi.cprintf (ent, PRINT_HIGH, "M4 set to Full Automatic mode\n");
        }

        if ( ent->client->curr_weap == SNIPER_NUM )
        {

                if (dead)
                   return;
                if ( ent->client->resp.sniper_mode == SNIPER_1X )
                {
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);                    
                        ent->client->resp.sniper_mode = SNIPER_2X;
                        ent->client->desired_fov = 45;
//PaTMaN-NORELOADING                        if ( ent->client->weaponstate != WEAPON_RELOADING )
//PaTMaN-NORELOADING                                                {       
//PaTMaN-NORELOADING                                                        ent->client->idle_weapon = 6; // 6 frames of idleness
//PaTMaN-NORELOADING                                                        ent->client->ps.gunframe = 22;
//PaTMaN-NORELOADING                                                        ent->client->weaponstate = WEAPON_BUSY;
//PaTMaN-NORELOADING                                                }
                }
                else if ( ent->client->resp.sniper_mode == SNIPER_2X )
                {
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
                        ent->client->resp.sniper_mode = SNIPER_4X;
                        ent->client->desired_fov = 20;
                }
                else if ( ent->client->resp.sniper_mode == SNIPER_4X )
                {
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
                        ent->client->resp.sniper_mode = SNIPER_6X;
                        ent->client->desired_fov = 10;
                }
                else
                {
                        gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
                        ent->client->resp.sniper_mode = SNIPER_1X;
                        ent->client->desired_fov = 90;
                        if ( ent->client->pers.weapon )
                                ent->client->ps.gunindex = gi.modelindex( ent->client->pers.weapon->view_model );
                }
        }
        if ( ent->client->curr_weap == KNIFE_NUM )
        {
                if (dead)
                        return;
                if ( ent->client->weaponstate == WEAPON_READY )
                {
					if (ent->client->resp.wmodes & WM_KNIFE) ent->client->resp.wmodes -= WM_KNIFE;
					else ent->client->resp.wmodes += WM_KNIFE;
                    //    ent->client->resp.knife_mode = !(ent->client->resp.knife_mode);
                        ent->client->weaponstate = WEAPON_ACTIVATING;
                        if ( ent->client->resp.wmodes & WM_KNIFE )//knife_mode )
                                                {
                                                        gi.cprintf(ent, PRINT_HIGH, "Switching to throwing\n");
                                                        ent->client->ps.gunframe = 0;
                                                }
                        else
                                                {
                                                        gi.cprintf(ent, PRINT_HIGH, "Switching to slashing\n");
                                                        ent->client->ps.gunframe = 106;
                                                }

                }
        }
//PaTMaN - Marker Weapon Start
		        if ( ent->client->curr_weap == MARKER_NUM )
        {
                if (dead)
                        return;
          //      if ( ent->client->weaponstate == WEAPON_READY )
          //      {
					if (ent->client->resp.wmodes & WM_MARKER) ent->client->resp.wmodes -= WM_MARKER;
					else ent->client->resp.wmodes += WM_MARKER;
                        //ent->client->resp.marker_mode = !(ent->client->resp.marker_mode);
                        ent->client->weaponstate = WEAPON_ACTIVATING;
                        if ( !(ent->client->resp.wmodes & WM_MARKER))//marker_mode == 0 )
                                                {
                                                        gi.cprintf(ent, PRINT_HIGH, "Marker Placement Mode\n");
                                                        ent->client->ps.gunframe = 0;
														if (ent->client->resp.wmodes & WM_MARKER)
															ent->client->resp.wmodes -= WM_MARKER;
														//ent->client->resp.marker_mode = 0;
                                                }
                        else
                                                {
                                                        gi.cprintf(ent, PRINT_HIGH, "Marker Deletion Mode\n");
                                                        ent->client->ps.gunframe = 106;//106;
														if (!(ent->client->resp.wmodes & WM_MARKER))
															ent->client->resp.wmodes += WM_MARKER;
													//	ent->client->resp.marker_mode = 1;
                                                }

           //     }
        }
//PaTMaN - Marker Weapon End

        if ( ent->client->curr_weap == GRENADE_NUM )
        {
                if ( ent->client->resp.grenade_mode == 0 )
                {
                        gi.cprintf (ent, PRINT_HIGH, "Prepared to make a medium range throw\n");
                        ent->client->resp.grenade_mode = 1;
                }
                else if ( ent->client->resp.grenade_mode == 1 )
                {
                        gi.cprintf (ent, PRINT_HIGH, "Prepared to make a long range throw\n");
                        ent->client->resp.grenade_mode = 2;
                }
                else
                {
                        gi.cprintf (ent, PRINT_HIGH, "Prepared to make a short range throw\n");
                        ent->client->resp.grenade_mode = 0;
                }

        }

}


// sets variable to toggle nearby door status
void Cmd_OpenDoor_f (edict_t *ent )
{

        ent->client->doortoggle = 1;

        return;
}

/*
void Cmd_Bandage_f ( edict_t *ent )
{
        gitem_t *item;


                if ( (ent->client->bleeding != 0 || ent->client->leg_damage != 0) && ent->client->bandaging != 1 )
                        ent->client->reload_attempts = 0; // prevent any further reloading


                
                if ( (ent->client->weaponstate == WEAPON_READY || ent->client->weaponstate == WEAPON_END_MAG )
                && (ent->client->bleeding != 0 || ent->client->leg_damage != 0 ) 
                && ent->client->bandaging != 1 )
        {       
 
                            // zucc - check if they have a primed grenade

                if ( ent->client->curr_weap == GRENADE_NUM
                        && ( ( ent->client->ps.gunframe >= GRENADE_IDLE_FIRST
                        && ent->client->ps.gunframe <= GRENADE_IDLE_LAST )
                                                || ( ent->client->ps.gunframe >= GRENADE_THROW_FIRST
                                                && ent->client->ps.gunframe <= GRENADE_THROW_LAST ) ) )
                {
                        ent->client->ps.gunframe = 0;
                        fire_grenade2 (ent, ent->s.origin, tv(0,0,0), GRENADE_DAMRAD, 0, 2, GRENADE_DAMRAD*2, false);
                        item = FindItem(GRENADE_NAME);
                        ent->client->pers.inventory[ITEM_INDEX(item)]--;
                        if ( ent->client->pers.inventory[ITEM_INDEX(item)] <= 0 )
                        {
                                ent->client->newweapon = FindItem( MK23_NAME );

                        }
                }               

                ent->client->bandaging = 1;
                                ent->client->resp.sniper_mode = SNIPER_1X;
                ent->client->ps.fov = 90;
                ent->client->desired_fov = 90;
                if ( ent->client->pers.weapon )
                        ent->client->ps.gunindex = gi.modelindex( ent->client->pers.weapon->view_model );
        
        }
        else if ( ent->client->bandaging == 1 )
                gi.cprintf (ent, PRINT_HIGH, "Already bandaging\n");         
//FIREBLADE 12/26/98 - fix inappropriate message
        else if (ent->client->bleeding == 0 && ent->client->leg_damage == 0)
                gi.cprintf (ent, PRINT_HIGH, "No need to bandage\n");
        else
                gi.cprintf(ent, PRINT_HIGH, "Can't bandage now\n");
//FIREBLADE



}
// function called in generic_weapon function that does the bandaging

void Bandage( edict_t* ent )
{
        ent->client->leg_noise = 0;
        ent->client->leg_damage = 0;
        ent->client->leghits = 0;
                ent->client->bleeding = 0;
                ent->client->bleed_remain = 0;
//        ent->client->bleedcount = 0;
//        ent->client->bleeddelay = 0;    
                ent->client->bandaging = 0;
                ent->client->leg_dam_count = 0;
                ent->client->attacker = NULL;
        ent->client->bandage_stopped = 1;
        ent->client->idle_weapon = BANDAGE_TIME;
}
*/
/*void Cmd_ID_f (edict_t *ent )
{

        if (!ent->client->resp.id) {
                gi.cprintf(ent, PRINT_HIGH, "Disabling player identification display.\n");
                ent->client->resp.id = 1;
        } else {
                gi.cprintf(ent, PRINT_HIGH, "Activating player identification display.\n");
                ent->client->resp.id = 0;
        }
        return;
}
*/

static void loc_buildboxpoints(vec3_t p[8], vec3_t org, vec3_t mins, vec3_t maxs)
{
        VectorAdd(org, mins, p[0]);
        VectorCopy(p[0], p[1]);
        p[1][0] -= mins[0];
        VectorCopy(p[0], p[2]);
        p[2][1] -= mins[1];
        VectorCopy(p[0], p[3]);
        p[3][0] -= mins[0];
        p[3][1] -= mins[1];
        VectorAdd(org, maxs, p[4]);
        VectorCopy(p[4], p[5]);
        p[5][0] -= maxs[0];
        VectorCopy(p[0], p[6]);
        p[6][1] -= maxs[1];
        VectorCopy(p[0], p[7]);
        p[7][0] -= maxs[0];
        p[7][1] -= maxs[1];
}

qboolean loc_CanSee (edict_t *targ, edict_t *inflictor)
{
        trace_t trace;
        vec3_t  targpoints[8];
        int i;
        vec3_t viewpoint;

// bmodels need special checking because their origin is 0,0,0
        if (targ->movetype == MOVETYPE_PUSH)
                return false; // bmodels not supported

        loc_buildboxpoints(targpoints, targ->s.origin, targ->mins, targ->maxs);
        
        VectorCopy(inflictor->s.origin, viewpoint);
        viewpoint[2] += inflictor->viewheight;

        for (i = 0; i < 8; i++) {
                PRETRACE();
                trace = gi.trace (viewpoint, vec3_origin, vec3_origin, targpoints[i], inflictor, MASK_SOLID);
                POSTTRACE();
                if (trace.fraction == 1.0)
                        return true;
        }

        return false;
}

// originally from Zoid's CTF
void SetIDView(edict_t *ent)
{
	vec3_t  forward, dir;
	trace_t tr;
	edict_t *who, *best;
	//FIREBLADE, suggested by hal[9k]  3/11/1999
	float bd = 0.9;
	//FIREBLADE
	float d;
	int i;

	if (ent->client->resp.toggles & TG_MTEXT)
	{
		float highest=0.9;
		int best1=0;
		edict_t *marker;
		char msg[256];

		int markers = MarkersInt();

		for (i=1;i<markers+1;i++)
		{

			marker = FindEdictByClassnum("Jump Marker", i);
			if (marker)
				if (Q_stricmp(marker->mmsg,""))
					if (loc_CanSee(ent,marker))
					{
						AngleVectors(ent->client->v_angle, forward, NULL, NULL);
						VectorSubtract(marker->s.origin, ent->s.origin, dir);
						VectorNormalize(dir);
						d = DotProduct(forward, dir);

						if (d > 0.963)
							if (d > highest)
							{
								highest = d;
								best1 = i;
								sprintf(msg,marker->mmsg);
							}
					}
		}
		if ((best1 != 0) && (msg != NULL))
		{
			if ((Q_stricmp(ent->client->resp.lastmmsg,msg)) || (level.time-2 > ent->client->resp.lastmmsgtime))
			{
				gi.centerprintf(ent,"%s",msg);
				strcpy(ent->client->resp.lastmmsg,msg);
				ent->client->resp.lastmmsgtime=level.time;
			}
		}
	}

	if (ent->client->resp.toggles & TG_HUD_ID)
	{

		if ((ent->client->chase_mode) && (ent->client->chase_target))
		{

			if ((!Q_stricmp(ent->client->chase_target->classname,"player")) &&
				ent->client->chase_target->inuse)
			{
				int playernum;
				playernum = ent->client->chase_target-g_edicts-1;
				ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
				ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PLAYER;
			}
			else if (!Q_stricmp(ent->client->chase_target->classname,"puppet"))
			{
				int playernum;
				playernum = ent->client->chase_target->lastowner-g_edicts-1;
				ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
				ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PUPPET;
			}
			else if (!Q_stricmp(ent->client->chase_target->classname,"pup"))
			{
				int playernum;
				playernum = ent->client->chase_target->owner->lastowner-g_edicts-1;
				ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
				ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PUP;
			}
			return;
		}

		AngleVectors(ent->client->v_angle, forward, NULL, NULL);
		VectorScale(forward, 8192, forward);
		VectorAdd(ent->s.origin, forward, forward);
		PRETRACE();
		tr = gi.trace(ent->s.origin, NULL, NULL, forward, ent, MASK_PLAYERSOLID);
		POSTTRACE();

		AngleVectors(ent->client->v_angle, forward, NULL, NULL);
		best = NULL;
		for (i = 1; i <= maxclients->value; i++) {

			who = g_edicts + i;
			if (!who->inuse)
				continue;

			//check for player

			VectorSubtract(who->s.origin, ent->s.origin, dir);
			VectorNormalize(dir);
			d = DotProduct(forward, dir);

			if (d > bd && loc_CanSee(ent, who))// && 
			{
				//FIREBLADE
				bd = d;
				best = who;
			}

			//check for puppet

			if (who->client->puppet)
			{
				VectorSubtract(who->client->puppet->s.origin, ent->s.origin, dir);
				VectorNormalize(dir);
				d = DotProduct(forward, dir);

				if (d > bd && loc_CanSee(ent, who->client->puppet))// && 
				{
					//FIREBLADE
					bd = d;
					best = who->client->puppet;
				}

				if (who->client->puppet->movetarget)
				{
					VectorSubtract(who->client->puppet->movetarget->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);

					if (d > bd && loc_CanSee(ent, who->client->puppet->movetarget))// && 
					{
						//FIREBLADE
						bd = d;
						best = who->client->puppet->movetarget;
					}
				}

			}
		}
		if (best != NULL)
		{

			if (bd > 0.90)
			{
				if (!Q_stricmp(best->classname,"puppet"))
				{
					int playernum;
					playernum = best->lastowner-g_edicts-1;
					ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
					ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PUPPET;
				}
				else if (!Q_stricmp(best->classname,"pup"))
				{
					int playernum;
					playernum = best->owner->lastowner-g_edicts-1;
					ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
					ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PUP;
				}
				else if (!Q_stricmp(best->classname,"player"))
				{
					if (!best->client->pers.spectator)
					{
						int playernum;
						playernum = best-g_edicts-1;
						ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS+playernum;
						ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_PLAYER;
					}
				}
			}
		}
	}
}
/*
void Cmd_IR_f (edict_t *ent )
{
	
	if ( ent->client->resp.ir == 0 )
	{
		ent->client->resp.ir = 1;
		gi.cprintf(ent, PRINT_HIGH, "IR vision disabled.\n");
		
	}
	else
	{
		ent->client->resp.ir = 0;
		gi.cprintf(ent, PRINT_HIGH, "IR vision enabled.\n");
	}
	
	
}
*/

// zucc choose command, avoids using the menus in teamplay

/*
void Cmd_Choose_f (edict_t *ent)
{
     
        char             *s;

        s = gi.args();

                // only works in teamplay
                if (!teamplay->value)
                        return;
                
                // convert names a player might try
                if (!stricmp(s, "A 2nd pistol") || !stricmp(s, "railgun"))
                        s = DUAL_NAME;
                if (!stricmp(s, "shotgun"))
                        s = M3_NAME;
                if (!stricmp(s, "machinegun"))
                        s = HC_NAME;
                if (!stricmp(s, "super shotgun"))
                        s = MP5_NAME;
                if (!stricmp(s, "chaingun"))
                        s = SNIPER_NAME;
                if (!stricmp(s, "bfg10k"))
                        s = KNIFE_NAME;
                if (!stricmp(s, "grenade launcher"))
                        s = M4_NAME;

        if ( stricmp(s, MP5_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(MP5_NAME);
                else if ( stricmp(s, M3_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(M3_NAME);
                else if ( stricmp(s, M4_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(M4_NAME);
                else if ( stricmp(s, HC_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(HC_NAME);
                else if ( stricmp(s, SNIPER_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(SNIPER_NAME);
                else if ( stricmp(s, KNIFE_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(KNIFE_NAME);
                else if ( stricmp(s, DUAL_NAME) == 0 )
                        ent->client->resp.weapon = FindItem(DUAL_NAME);
                else if ( stricmp(s, KEV_NAME) == 0 )
                        ent->client->resp.item = FindItem(KEV_NAME);
                else if ( stricmp(s, LASER_NAME) == 0 )
                        ent->client->resp.item = FindItem(LASER_NAME);
                else if ( stricmp(s, SLIP_NAME) == 0 )
                        ent->client->resp.item = FindItem(SLIP_NAME);
                else if ( stricmp(s, SIL_NAME) == 0 )
                        ent->client->resp.item = FindItem(SIL_NAME);
                else if ( stricmp(s, BAND_NAME) == 0 )
                        ent->client->resp.item = FindItem(BAND_NAME);
                else
                {
                        gi.cprintf(ent, PRINT_HIGH, "Invalid weapon or item choice.\n");
                        return;
                }
                gi.cprintf(ent, PRINT_HIGH, "Weapon selected: %s\nItem selected: %s\n", (ent->client->resp.weapon)->pickup_name, (ent->client->resp.item)->pickup_name);

}
*/

void Cmd_PMLCA_f(edict_t *ent)
{  
	
	if (ent->client->pers.spectator)
	{
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
		ent->client->resp.toggle_lca = 0;
		return;
	}
	
	if (!ent->client->resp.toggle_lca)
	{
		gi.centerprintf (ent,"LIGHTS...\n");
		gi.sound(ent, CHAN_VOICE,// | CHAN_NO_PHS_ADD, 
			gi.soundindex("atl/lights.wav"), 1, ATTN_STATIC, 0);
		ent->client->resp.toggle_lca = 41;
	}
	else if (ent->client->resp.toggle_lca == 21)
	{
		gi.centerprintf (ent,"CAMERA...\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("atl/camera.wav"), 1, ATTN_STATIC, 0);
	}
    else if (ent->client->resp.toggle_lca == 1)
	{ 
		gi.centerprintf (ent,"ACTION!\n");
		gi.sound(ent, CHAN_VOICE, gi.soundindex("atl/action.wav"), 1, ATTN_STATIC, 0);
	}
	ent->client->resp.toggle_lca--;
}

void Cmd_DemoMode_f(edict_t *ent)
{
	ent->client->resp.highspeed = 0;
	ent->client->ps.stats[STAT_SPEEDX] = 0;
	ent->client->resp.fallcount = 0;
	ent->client->resp.falldmg = 0;
	ent->client->resp.falldmglast = 0;
}

//PaTMaN - New Toggle Command
void Cmd_Toggle(edict_t *ent)
{
	
	char	*s;
	char	ACT  [2][12] = { "Deactivated\0","Activated\0" };
	char	ENA  [2][9]  = { "Disabled\0","Enabled\0" };
	char	TRANS[2][12] = { "Opaque\0","Transparent\0" };
	int		spec, tokens=0,val=0;
	
	s = strtok(gi.args()," ");

	if ((gi.argc() == 1) && (s == NULL))
	{
			gi.cprintf(ent,PRINT_HIGH,"Options to toggle: laser, vest, slippers, silencer, ir, id, range, shootable, "
				"kickable, teamjump, grenadejump, markerflashes, markertext, markercolor, markjumps, markfalls, "
				"jumpmarkercolor, fallmarkercolor, trailing, trailcolor, trailflashes, puppettrans, puppetshoot, "
				"puppetshootarea, puppetsolid, puppetsight, spectator...\n");
			gi.cprintf(ent,PRINT_HIGH,"hud, hud_speed, hud_highspeed, hud_lastfalldamage, and hud_gravity\n");
			return;
	}

	if (!Q_stricmp(s,"togglecode"))
		s = strtok(NULL," ");
	else
		s = strtok(gi.args()," ");

	spec = ent->client->pers.spectator;

	if ( stricmp(s, "laser") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_LASER) ent->client->resp.toggles -= TG_LASER;
		else { ent->client->resp.toggles += TG_LASER; val=1; SP_LaserSight(ent,NULL); }
		gi.cprintf(ent, PRINT_HIGH, "Laser %s\n",ACT[val]);
	}
	else if ( stricmp(s, "vest") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_VEST) ent->client->resp.toggles -= TG_VEST;
		else { ent->client->resp.toggles += TG_VEST; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Vest %s\n",ACT[val]);
	}
	else if ( stricmp(s, "ir") == 0 )
	{
		if (ent->client->resp.toggles & TG_IR) ent->client->resp.toggles -= TG_IR;
		else { ent->client->resp.toggles += TG_IR; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "IR Vision %s\n",ACT[val]);
	}
	else if ( stricmp(s, "markjumps") == 0 )
	{
		if (ent->client->resp.toggles & TG_MARKJUMPS) ent->client->resp.toggles -= TG_MARKJUMPS;
		else { ent->client->resp.toggles += TG_MARKJUMPS; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Jump Marking %s\n",ENA[val]);
	}
	else if ( stricmp(s, "markfalls") == 0 )
	{
		if (ent->client->resp.toggles & TG_MARKFALLS) ent->client->resp.toggles -= TG_MARKFALLS;
		else { ent->client->resp.toggles += TG_MARKFALLS; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Fall Marking %s\n",ENA[val]);
	}

	else if ( stricmp(s, "slippers") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_SLIPPERS) ent->client->resp.toggles -= TG_SLIPPERS;
		else { ent->client->resp.toggles += TG_SLIPPERS; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Slippers %s\n",ACT[val]);
	}
	else if ( stricmp(s, "silencer") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_SILENCER) ent->client->resp.toggles -= TG_SILENCER;
		else { ent->client->resp.toggles += TG_SILENCER; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Silencer %s\n",ACT[val]);
	}
	else if ( stricmp(s, "kickable") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_KICKABLE)
		{
			ent->client->resp.toggles -= TG_KICKABLE;
			gi.cprintf(ent,PRINT_HIGH, "You are now NOT KICKABLE\n");
		}
		else
		{
			ent->client->resp.toggles += TG_KICKABLE;
			gi.cprintf(ent,PRINT_HIGH, "You are now KICKABLE\n");
		}
	}
	else if ( stricmp(s, "teamjump") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_TEAMJUMP)
		{
			ent->client->resp.toggles -= TG_TEAMJUMP;
			if (ent->solid == SOLID_BBOX)
				AddToTransparentList(ent);
		}
		else
		{
			ent->client->resp.toggles += TG_TEAMJUMP;
			if (ent->solid == SOLID_TRIGGER)
				RemoveFromTransparentList(ent);
			val=1;
		}
		gi.cprintf(ent, PRINT_HIGH, "Team Jumping %s\n",ENA[val]);
	}
	else if ( stricmp(s, "grenadejump") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_GRENJUMP) ent->client->resp.toggles -= TG_GRENJUMP;
		else { ent->client->resp.toggles += TG_GRENJUMP; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Grenade Jumping (for own grenades) %s\n",ENA[val]);
	}
/*	else if ( stricmp(s, "grenadejumpother") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_GRENJUMPOTHER) ent->client->resp.toggles -= TG_GRENJUMPOTHER;
		else { ent->client->resp.toggles += TG_GRENJUMPOTHER; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Grenade Jumping (for others' grenades) %s\n",ENA[val]);
	}
*/	else if ( stricmp(s, "grenadejumps") == 0 )
	{
		if (spec) goto spec;
		if (!(ent->client->resp.toggles & TG_GRENJUMP) || !(ent->client->resp.toggles & TG_GRENJUMPOTHER)) //if at least 1 is off, turn both on
		{
			ent->client->resp.toggles |= TG_GRENJUMP;
			ent->client->resp.toggles |= TG_GRENJUMPOTHER;
			val = 1;
		}
		else //if both are on, turn both off
		{
			ent->client->resp.toggles &= ~TG_GRENJUMP;
			ent->client->resp.toggles &= ~TG_GRENJUMPOTHER;
			val = 0;
		}
		gi.cprintf(ent, PRINT_HIGH, "Grenade Jumping (for own and others' grenades) %s\n",ENA[val]);
	}
	else if ( stricmp(s, "shootable") == 0 )
	{
		if (spec) goto spec;
		if (ent->client->resp.toggles & TG_SHOOTABLE) ent->client->resp.toggles -= TG_SHOOTABLE;
		else { ent->client->resp.toggles += TG_SHOOTABLE; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "You are now %s\n",val ? "shootable" : "not shootable");
	}
	else if ( stricmp(s, "id") == 0 )
	{
		if (ent->client->resp.toggles & TG_HUD_ID) ent->client->resp.toggles -= TG_HUD_ID;
		else { ent->client->resp.toggles += TG_HUD_ID; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Player ID Display %s\n",ENA[val]);
	}
	else if ( stricmp(s, "range") == 0 )
	{
		if (ent->client->resp.toggles & TG_HUD_RANGE) ent->client->resp.toggles -= TG_HUD_RANGE;
		else { ent->client->resp.toggles += TG_HUD_RANGE; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Range Finder %s\n",ENA[val]);
		ent->client->ps.stats[STAT_HUD_RANGEFINDER] = 0;
	}
	else if ( stricmp(s, "markerflashes") == 0 )
	{
		if (ent->client->resp.toggles & TG_MFLASH) ent->client->resp.toggles -= TG_MFLASH;
		else { ent->client->resp.toggles += TG_MFLASH; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Marker Flashes %s\n",ENA[val]);
	}
	else if ( stricmp(s, "trailflashes") == 0 )
	{
		if (ent->client->resp.toggles & TG_TFLASH) ent->client->resp.toggles -= TG_TFLASH;
		else { ent->client->resp.toggles += TG_TFLASH; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Trail Flashes %s\n",ENA[val]);
	}
	else if ( stricmp(s, "markertext") == 0 )
	{
		if (ent->client->resp.toggles & TG_MTEXT) ent->client->resp.toggles -= TG_MTEXT;
		else { ent->client->resp.toggles += TG_MTEXT; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Marker Text %s\n",ENA[val]);
	}
	else if ( stricmp(s, "markercolor") == 0 )
	{
		char colors[7][7] = { "RED\0","GREEN\0","BLUE\0","YELLOW\0","WHITE\0","PURPLE\0","AQUA\0" };
		int col;
		col = ent->client->resp.marker_color+1;
		if (col > 6)
			col = 0;
		gi.cprintf(ent, PRINT_HIGH, "Thrown markers are now %s\n",colors[col]);
		ent->client->resp.marker_color = col;
	}
	else if ( stricmp(s, "jumpmarkercolor") == 0 )
	{
		char colors[7][7] = { "RED\0","GREEN\0","BLUE\0","YELLOW\0","WHITE\0","PURPLE\0","AQUA\0" };
		int col;
		col = ent->client->resp.marker_color_jump+1;
		if (col > 6)
			col = 0;
		gi.cprintf(ent, PRINT_HIGH, "Jump markers are now %s\n",colors[col]);
		ent->client->resp.marker_color_jump = col;
	}
	else if ( stricmp(s, "fallmarkercolor") == 0 )
	{
		char colors[7][7] = { "RED\0","GREEN\0","BLUE\0","YELLOW\0","WHITE\0","PURPLE\0","AQUA\0" };
		int col;
		col = ent->client->resp.marker_color_fall+1;
		if (col > 6)
			col = 0;
		gi.cprintf(ent, PRINT_HIGH, "Fall markers are now %s\n",colors[col]);
		ent->client->resp.marker_color_fall = col;
	}
	else if ( stricmp(s, "puppetsight") == 0 )
	{
		if (ent->client->resp.wmodes & OS_PUPPET_SIGHT) ent->client->resp.wmodes -= OS_PUPPET_SIGHT;
		else { ent->client->resp.wmodes += OS_PUPPET_SIGHT; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "Puppet Sight %s\n",ENA[val]);
	}
	else if ( stricmp(s, "puppetshootarea") == 0 )
	{
		char areas[5][13] = { "your head\0","your chest\0","your stomach\0","your legs\0","random areas\0" };
		ent->client->resp.puppetshootarea++;
		if (ent->client->resp.puppetshootarea > 5)
			ent->client->resp.puppetshootarea = 1;
		gi.cprintf(ent, PRINT_HIGH, "Puppet will aim for %s\n",areas[ent->client->resp.puppetshootarea - 1]);
	}
	else if ( stricmp(s, "puppettrans") == 0 )
	{
		if (ent->client->puppet)
		{
			if (ent->client->puppet->s.renderfx & RF_TRANSLUCENT)
			{
				ent->client->puppet->s.renderfx -= RF_TRANSLUCENT;
				ent->client->resp.wmodes -= OS_PUPPET_TRANS;
			}
			else
			{
				ent->client->puppet->s.renderfx += RF_TRANSLUCENT;
				ent->client->resp.wmodes += OS_PUPPET_TRANS;
				val=1;
			}
			gi.cprintf(ent, PRINT_HIGH, "Your puppet is now %s\n",TRANS[val]);
		}
	}
	else if ( stricmp(s, "puppetshoot") == 0 )
	{
		if (ent->client->resp.toggles & TG_PUPPETSHOOT) ent->client->resp.toggles -= TG_PUPPETSHOOT;
		else { ent->client->resp.toggles += TG_PUPPETSHOOT; val=1; }
		if (val)
			gi.cprintf(ent, PRINT_HIGH, "Your puppet will now shoot\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "Your puppet will not shoot\n");
	}
	else if ( stricmp(s, "puppetsolid") == 0 )
	{
		if (ent->client->resp.wmodes & OS_PUPPET_SOLID)
		{
			ent->client->resp.wmodes -= OS_PUPPET_SOLID;
			gi.cprintf(ent,PRINT_HIGH,"Your puppet is now not solid\n");
		}
		else
		{
			ent->client->resp.wmodes += OS_PUPPET_SOLID;
			gi.cprintf(ent,PRINT_HIGH,"Your puppet is now solid\n");
		}
	}
	else if ( stricmp(s, "puppetsolidpplay") == 0 )
	{
		if (ent->client->resp.wmodes & OS_PUPPET_SOLID_PPLAY)
		{
			ent->client->resp.wmodes -= OS_PUPPET_SOLID_PPLAY;
			gi.cprintf(ent,PRINT_HIGH,"Your puppet is now not solid during a puppetdemo playback\n");
		}
		else
		{
			ent->client->resp.wmodes += OS_PUPPET_SOLID_PPLAY;
			gi.cprintf(ent,PRINT_HIGH,"Your puppet is now solid during puppetdemo playback\n");
		}
	}
	else if ( stricmp(s, "trailing") == 0 )
	{
		if (ent->client->resp.toggles & TG_TRAIL) ent->client->resp.toggles -= TG_TRAIL;
		else
		{
			ent->client->resp.toggles += TG_TRAIL;
			val=1;
			AddTrailCount(1);
			ent->client->resp.trailcounter=1; //current link count
			ent->client->resp.trail_cur = LaserTrailsInt(); //current global trail number
		}
		gi.cprintf(ent, PRINT_HIGH, "Trailing %s\n",ENA[val]);
	}
	else if ( stricmp(s, "trailcolor") == 0 )
	{
		char colors[7][7] = { "RED\0","GREEN\0","BLUE\0","YELLOW\0","WHITE\0","ORANGE\0","BLACK\0" };
		int col;
		col = ent->client->resp.trail_color+1;
		if (col > 6)
			col = 0;
		gi.cprintf(ent, PRINT_HIGH, "Trails are now %s\n",colors[col]);
		ent->client->resp.trail_color = col;
	}
	else if ( stricmp(s, "spectator") == 0 )
	{
		if (ent->client->pers.spectator == 1) stuffcmd(ent, "spectator 0");
		else stuffcmd(ent, "spectator 1");
	}
	else if ( stricmp(s, "hud") == 0 )
	{
		if (ent->client->resp.hud_toggles & TGH_NO_HUD) ent->client->resp.hud_toggles -= TGH_NO_HUD;
		else { ent->client->resp.hud_toggles += TGH_NO_HUD; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "HUD %s\n",ENA[!val]);
	}
	else if ( stricmp(s, "hud_speed") == 0 )
	{
		if (ent->client->resp.hud_toggles & TGH_NO_SPEED) ent->client->resp.hud_toggles -= TGH_NO_SPEED;
		else { ent->client->resp.hud_toggles += TGH_NO_SPEED; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "HUD Speed %s\n",ENA[!val]);
	}
	else if ( stricmp(s, "hud_highspeed") == 0 )
	{
		if (ent->client->resp.hud_toggles & TGH_NO_HIGH_SPEED) ent->client->resp.hud_toggles -= TGH_NO_HIGH_SPEED;
		else { ent->client->resp.hud_toggles += TGH_NO_HIGH_SPEED; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "HUD High Speed %s\n",ENA[!val]);
	}
	else if (( stricmp(s, "hud_lastfalldamage") == 0 ) ||
		( stricmp(s, "hud_lastfd") == 0 ))
	{
		if (ent->client->resp.hud_toggles & TGH_NO_LAST_FALL_DAMAGE) ent->client->resp.hud_toggles -= TGH_NO_LAST_FALL_DAMAGE;
		else { ent->client->resp.hud_toggles += TGH_NO_LAST_FALL_DAMAGE; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "HUD Last Fall Damage %s\n",ENA[!val]);
	}
	else if ( stricmp(s, "hud_gravity") == 0 )
	{
		if (ent->client->resp.hud_toggles & TGH_NO_GRAVITY) ent->client->resp.hud_toggles -= TGH_NO_GRAVITY;
		else { ent->client->resp.hud_toggles += TGH_NO_GRAVITY; val=1; }
		gi.cprintf(ent, PRINT_HIGH, "HUD Gravity %s\n",ENA[!val]);
	}
	else
		gi.cprintf(ent, PRINT_HIGH, "\"%s\" isn't a valid toggle option\n",s);
	return;
spec:
	gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
}

