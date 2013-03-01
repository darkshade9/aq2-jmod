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

// This source file comes from the 3.20 source originally.
// 
// Added through-the-eyes cam mode, as well as the ability to spin around the player
// when in regular chase cam mode, among other Axshun-related mods.
// -Fireblade

#include "g_local.h"

void Spec_UpdateChaseCam (edict_t *ent)
{
	int chase_mode=ent->client->chase_mode;
	vec3_t o, ownerv, goal;
	edict_t *targ;
	vec3_t forward, right;
	trace_t trace;
	int i;
	vec3_t angles;

	if (chase_mode != SPEC_FREE)
	{
		if (Spec_ChaseTargetGone(ent))
			return;
	}
	else
	{
		ent->client->chase_target = NULL;
		return;
	}

	if (!ent->client->chase_target)
		return;

	ent->client->resp.last_chase_target = ent->client->chase_target;
	chase_mode=ent->client->chase_mode;

	targ = ent->client->chase_target;

	if (targ->client)
	{
		if (chase_mode == SPEC_EYES)
		{
			VectorCopy(targ->s.origin, ownerv);

			VectorCopy(targ->client->v_angle, angles);

			AngleVectors (angles, forward, right, NULL);
			VectorNormalize(forward);
			VectorMA(ownerv, 8, forward, o);

			o[2] += targ->viewheight;

			VectorCopy(o, ent->s.origin);

			ent->client->ps.fov = targ->client->ps.fov;
			ent->client->desired_fov = targ->client->ps.fov;

			for (i=0 ; i<3 ; i++)
				ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(targ->client->v_angle[i] - ent->client->resp.cmd_angles[i]);


			if (targ->deadflag)
			{
				ent->client->ps.viewangles[ROLL] = 40;
				ent->client->ps.viewangles[PITCH] = -15;
				ent->client->ps.viewangles[YAW] = targ->client->killer_yaw;
			}
			else 
			{
				VectorAdd(targ->client->v_angle,
					targ->client->ps.kick_angles,
					angles);
				VectorCopy(angles, ent->client->ps.viewangles);
				VectorCopy(angles, ent->client->v_angle);
			}
		}
		else if (chase_mode == SPEC_AROUND)
		{
			ent->client->desired_fov = 90;
			ent->client->ps.fov = 90;

			if (ent->client->resp.cmd_angles[PITCH] > 89)
				ent->client->resp.cmd_angles[PITCH] = 89;
			if (ent->client->resp.cmd_angles[PITCH] < -89)
				ent->client->resp.cmd_angles[PITCH] = -89;

			VectorCopy(targ->s.origin, ownerv);

			ownerv[2] += targ->viewheight;

			VectorCopy(ent->client->ps.viewangles, angles);
			AngleVectors (angles, forward, right, NULL);
			VectorNormalize(forward);
			VectorMA(ownerv, ent->client->resp.spec_dist, forward, o);

			// not sure if this should be left in... -FB
			//              if (o[2] < targ->s.origin[2] + 20)  
			//                      o[2] = targ->s.origin[2] + 20;

			// jump animation lifts
			if (!targ->groundentity)
				o[2] += 16;

			PRETRACE();
			trace = gi.trace(ownerv, vec3_origin, vec3_origin, o, targ, MASK_SOLID);        
			POSTTRACE();

			VectorCopy(trace.endpos, goal);

			VectorMA(goal, 2, forward, goal);

			// pad for floors and ceilings
			VectorCopy(goal, o);
			o[2] += 6;
			PRETRACE();
			trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
			POSTTRACE();
			if (trace.fraction < 1) {
				VectorCopy(trace.endpos, goal);
				goal[2] -= 6;
			}

			VectorCopy(goal, o);
			o[2] -= 6;
			PRETRACE();
			trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
			POSTTRACE();
			if (trace.fraction < 1) {
				VectorCopy(trace.endpos, goal);
				goal[2] += 6;
			}

			if (targ->deadflag)
				ent->client->ps.pmove.pm_type = PM_DEAD;
			else
				ent->client->ps.pmove.pm_type = PM_FREEZE;              

			VectorCopy(goal, ent->s.origin);

			for (i=0 ; i<3 ; i++)
				ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

			VectorCopy(ent->client->resp.cmd_angles, ent->client->ps.viewangles);
		}
	}
	else if (!targ->client)
	{
		if (chase_mode == SPEC_EYES)
		{
			edict_t *puppet = ent->client->chase_target;
			
			ent->client->desired_fov = 90;
			ent->client->ps.fov = 90;
			
			if (ent->client->resp.cmd_angles[PITCH] > 89)
				ent->client->resp.cmd_angles[PITCH] = 89;
			if (ent->client->resp.cmd_angles[PITCH] < -89)
				ent->client->resp.cmd_angles[PITCH] = -89;
			
			VectorCopy(puppet->s.origin, ownerv);
			
			ownerv[2] += puppet->viewheight;
			
			VectorCopy(puppet->pos1, angles);
			
			AngleVectors (angles, forward, right, NULL);
			VectorNormalize(forward);
			VectorMA(ownerv, 8, forward, o);
			
			VectorCopy(o, ent->s.origin);
			
			for (i=0 ; i<3 ; i++)
				ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(puppet->pos1[i] - ent->client->resp.cmd_angles[i]);

			VectorCopy(puppet->pos1, ent->client->ps.viewangles);
			VectorCopy(puppet->pos1, ent->client->v_angle);

		}
		else if (chase_mode == SPEC_AROUND)
		{
			ent->client->desired_fov = 90;
			ent->client->ps.fov = 90;

			if (ent->client->resp.cmd_angles[PITCH] > 89)
				ent->client->resp.cmd_angles[PITCH] = 89;
			if (ent->client->resp.cmd_angles[PITCH] < -89)
				ent->client->resp.cmd_angles[PITCH] = -89;

			VectorCopy(targ->s.origin, ownerv);

			ownerv[2] += targ->viewheight;

			VectorCopy(ent->client->ps.viewangles, angles);
			AngleVectors (angles, forward, right, NULL);
			VectorNormalize(forward);
			VectorMA(ownerv, ent->client->resp.spec_dist, forward, o);

			if (!targ->groundentity)
				o[2] += 16;

			PRETRACE();
			trace = gi.trace(ownerv, vec3_origin, vec3_origin, o, targ, MASK_SOLID);        
			POSTTRACE();

			VectorCopy(trace.endpos, goal);

			VectorMA(goal, 2, forward, goal);

			VectorCopy(goal, o);
			o[2] += 6;
			PRETRACE();
			trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
			POSTTRACE();
			if (trace.fraction < 1) {
				VectorCopy(trace.endpos, goal);
				goal[2] -= 6;
			}

			VectorCopy(goal, o);
			o[2] -= 6;
			PRETRACE();
			trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
			POSTTRACE();
			if (trace.fraction < 1) {
				VectorCopy(trace.endpos, goal);
				goal[2] += 6;
			}

			if (targ->deadflag)
				ent->client->ps.pmove.pm_type = PM_DEAD;
			else
				ent->client->ps.pmove.pm_type = PM_FREEZE;              

			VectorCopy(goal, ent->s.origin);

			for (i=0 ; i<3 ; i++)
				ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

			VectorCopy(ent->client->resp.cmd_angles, ent->client->ps.viewangles);
		}
	}
	else
	{
		if (ent->client->chase_target)
			warn("Client chasing a %s\n",ent->client->chase_target->classname);
		else
			warn("Client chasing but no target!\n");
	}

	ent->viewheight = 0;
	ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
	gi.linkentity(ent);
}

void Spec_CycleMode_Forward (edict_t *ent)
{
	gclient_t *client=ent->client;

	client->latched_buttons = 0;
	if (client->chase_mode != SPEC_FREE)
	{
		if (client->chase_mode == SPEC_AROUND)
		{
			client->desired_fov = 90;
			client->ps.fov = 90;
			client->chase_mode = SPEC_EYES;
		}
		else
		{
			if (client->chase_target != NULL)
				if (!Q_stricmp(client->chase_target->classname,"player"))
					gi.cprintf(client->chase_target,PRINT_HIGH,"%s left spectating you\n",client->pers.netname);
			client->chase_target = NULL;
			client->desired_fov = 90;
			client->ps.fov = 90;
			client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			client->chase_mode = SPEC_FREE;
		}
	}
	else
	{
		client->chase_target = NULL;
		Spec_GetChaseTarget(ent);
		client->latched_buttons = 0;
		if (client->chase_target != NULL)
		{
			client->chase_mode = SPEC_AROUND;
			Spec_UpdateChaseCam(ent);
		}
	}
}

void Spec_CycleMode_Backward (edict_t *ent)
{
	ent->client->chase_mode++;
	if (ent->client->chase_mode > SPEC_EYES)
		ent->client->chase_mode = SPEC_FREE;
	Spec_CycleMode_Forward(ent);
}

int Spec_ChaseTargetGone (edict_t *ent)
{
	if ((!ent->client->chase_target) || (!ent->client->chase_target->inuse))
	{
		ent->client->chase_target = ent->client->resp.last_chase_target;
		if ((!ent->client->chase_target) || (!ent->client->chase_target->inuse))
		{
			ent->client->desired_fov = 90;
			ent->client->ps.fov = 90;
			ent->client->chase_mode = SPEC_FREE;
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			ent->client->resp.last_chase_target = NULL;
			return 1;
		}
		else
			return 0;
	}
	if ((!ent->client->chase_target->inuse) ||
		(ent->client->chase_target->solid == SOLID_NOT))
	{
		Spec_GetChaseTarget_Next(ent);
		if (!ent->client->chase_target)
		{
			ent->client->desired_fov = 90;
			ent->client->ps.fov = 90;
			ent->client->chase_mode = SPEC_FREE;
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			return 1;
		}
	}
	return 0;
}

void Spec_GetChaseTarget (edict_t *ent)
{
	if (ent->client->resp.last_chase_target != NULL)
		ent->client->chase_target = ent->client->resp.last_chase_target;
	else if (ent->client->puppet)
		ent->client->chase_target = ent->client->puppet;
	else
	{
		ent->client->chase_target = ent++;
		if (ent->client->chase_target)
		{
			if (!ent->client->chase_target->client)
				ent->client->chase_target = g_edicts + 1;
		}
		else
			ent->client->chase_target = g_edicts + 1;
	}
}

void Spec_GetChaseTarget_Next (edict_t *ent)
{
	int search_loop_count=0;	//number of loops taken for break precaution
	edict_t *cur=NULL;			//current target
	edict_t *start=NULL;		//target loop started on

	start = cur = ent->client->chase_target;	//start at current chase target
	if (start == NULL)							//if no chase target
		start = cur = ent;						//start at self

	do											//loop time
	{
		if (cur)								//if we have a current target
		{
			if (cur->client)					//if cur is a client
			{
				if (cur->client->puppet)		//if client has a puppet
					cur = cur->client->puppet;
				else							//else if client has no puppet
				{
					do							//ahh... loops...
					{
						cur = cur++;			//go to the next client
						if (!cur->client)		//if not a client (> g_edicts + maxclients)
							cur = g_edicts+1;	//go back to the first client
					} while (!cur->inuse);		//do until client hit
				}
			}
			else if (!Q_stricmp(cur->classname,"puppet"))	//if cur is a puppet
			{
				if (!cur->movetarget)			//if puppet has no pup
				{
					cur = cur->lastowner;		//set cur to the puppet's client
					do
					{
						cur = cur++;			//go to the next client
						if (!cur->client)		//if not a client (> g_edicts + maxclients)
							cur = g_edicts+1;	//go back to the first client
					} while (!cur->inuse);		//do until client hit
				}
				else
					cur = cur->movetarget;		//set cur to the puppet's pup
			}
			else if (!Q_stricmp(cur->classname,"pup"))	//if cur is a pup
			{
				cur = cur->lastowner;		//set cur to the pup's client
				do
				{
					cur = cur++;			//go to the next client
					if (!cur->client)		//if not a client (> g_edicts + maxclients)
						cur = g_edicts+1;	//go back to the first client
				} while (!cur->inuse);		//do until client hit
			}
		}

		if ((cur != NULL) && (cur != ent))	//if we have a new target and it isn't yourself
			break;							//break out of the search loop

		if (++search_loop_count > 200)		//loop break precaution
		{
			warn("Broke out of a chase search loop (next)\n");		//warn message
			cur = NULL;						//unset the chase target
			break;							//stop looping
		}
	} while (cur != start);					//stop loop if we have done a full loop

	if ((cur == start) && (cur == ent))		//don't want to spec self
		cur = NULL;							//so NULL it
	else if (cur->client)
		gi.cprintf(cur,PRINT_HIGH,"%s began spectating you\n",ent->client->pers.netname);

	if ((start != cur) && (start->client) && (start != ent))
		gi.cprintf(start,PRINT_HIGH,"%s left spectating you\n",ent->client->pers.netname);

	ent->client->chase_target = cur;		//finally give the client what we came up with
}

//This is basically the Next code but it does a full loop and takes the last in the loop
void Spec_GetChaseTarget_Prev (edict_t *ent)
{
	int search_loop_count=0;	//number of loops taken for break precaution
	edict_t *cur=NULL;			//current target
	edict_t *start=NULL;		//target loop started on
	edict_t *prev=NULL;			//target previous to cur

	start = cur = ent->client->chase_target;	//start at current chase target
	if (start == NULL)							//if no chase target
		start = cur = ent;						//start at self

	do											//loop time
	{
		if (cur)								//if we have a current target
		{
			if (cur != ent)						//if not yourself
				prev = cur;						//save prev as current target for later

			if (cur->client)					//if cur is a client
			{
				if (cur->client->puppet)		//if client has a puppet
					cur = cur->client->puppet;
				else							//else if client has no puppet
				{
					do							//ahh... loops...
					{
						cur = cur++;			//go to the next client
						if (!cur->client)		//if not a client (> g_edicts + maxclients)
							cur = g_edicts+1;	//go back to the first client
					} while (!cur->inuse);		//do until client hit
				}
			}
			else if (!Q_stricmp(cur->classname,"puppet"))	//if cur is a puppet
			{
				if (!cur->movetarget)			//if puppet has no pup
				{
					cur = cur->lastowner;		//set cur to the puppet's client
					do
					{
						cur = cur++;			//go to the next client
						if (!cur->client)		//if not a client (> g_edicts + maxclients)
							cur = g_edicts+1;	//go back to the first client
					} while (!cur->inuse);		//do until client hit
				}
				else
					cur = cur->movetarget;		//set cur to the puppet's pup
			}
			else if (!Q_stricmp(cur->classname,"pup"))	//if cur is a pup
			{
				cur = cur->lastowner;		//set cur to the pup's client
				do
				{
					cur = cur++;			//go to the next client
					if (!cur->client)		//if not a client (> g_edicts + maxclients)
						cur = g_edicts+1;	//go back to the first client
				} while (!cur->inuse);		//do until client hit
			}
		}

		//Skip next 2 lines to prevent stopping until a full loop is done
		//if ((cur != NULL) && (cur != ent))	//if we have a new target and it isn't yourself
		//	break;							//break out of the search loop

		if (++search_loop_count > 200)		//loop break precaution
		{
			warn("Broke out of a chase search loop (next)\n");		//warn message
			cur = NULL;						//unset the chase target
			break;							//stop looping
		}
	} while (cur != start);					//stop loop if we have done a full loop

	if ((prev == start) && (prev == ent))	//don't want to spec self
		prev = NULL;						//so NULL it
	else if (prev->client)
		gi.cprintf(prev,PRINT_HIGH,"%s began spectating you\n",ent->client->pers.netname);

	if ((start != prev) && (start->client))
		gi.cprintf(start,PRINT_HIGH,"%s left spectating you\n",ent->client->pers.netname);

	ent->client->chase_target = prev;		//finally give the client what we came up with
}

