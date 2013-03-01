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

#include "g_local.h"
#include "m_player.h"

int i;

//updated
char* NUMtoLOCWORD (int num)
{
	switch (num)
	{
	case 1:
		return "head\0";
	case 2:
		return "chest\0";
	case 3:
		return "stomach\0";
	case 4:
		return "legs\0";
	case 5:
		return "random\0";
	default:
		return NULL;
	}
}

void LOCWORDtoNUM (edict_t *ent)
{
	int val;
	char *loc;

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH, "\"puppetshootarea\" is \"%s\"\n",NUMtoLOCWORD(ent->client->resp.puppetshootarea));
		return;
	}
	
	loc = strdup(strtok(gi.args()," "));

	if		(Q_stricmp(loc,"head") == 0)	val = 1;
	else if (Q_stricmp(loc,"chest") == 0)	val = 2;
	else if (Q_stricmp(loc,"vest") == 0)	val = 2;
	else if (Q_stricmp(loc,"stomach") == 0)	val = 3;
	else if (Q_stricmp(loc,"leg") == 0)		val = 4;
	else if (Q_stricmp(loc,"legs") == 0)	val = 4;
	else if (Q_stricmp(loc,"random") == 0)	val = 5;
	else									val = 99;
	
	if (val == 99)
	{
		gi.cprintf(ent,PRINT_HIGH,"Unknown shoot area specified: \"%s\"; Shoot areas are: head, chest, stomach, legs, and random\n",loc);	
		return;
	}

	ent->client->resp.puppetshootarea = val;
}

#define MK23_SPREAD		140
#define MP5_SPREAD		250
#define M4_SPREAD		300
#define SNIPER_SPREAD	425
#define DUAL_SPREAD		300

void Puppet_Pistol_Fire(edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	start[2] += 8;
	fire_bullet (self, start, forward, 90, 150, MK23_SPREAD, MK23_SPREAD,MOD_MK23);
	
	if (ent->client->resp.toggles & TG_SILENCER)
		gi.sound(self, CHAN_WEAPON, gi.soundindex("misc/silencer.wav"), 1, ATTN_NORM, 0);        
	else
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/mk23fire.wav"), 1, ATTN_NORM, 0);
	
}

void Puppet_MP5_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	
	start[2] += 8;
	fire_bullet (self, start, forward, 55, 90, MP5_SPREAD, MP5_SPREAD,MOD_MP5);
	
	if (ent->client->resp.toggles & TG_SILENCER)
		gi.sound(self, CHAN_WEAPON, gi.soundindex("misc/silencer.wav"), 1, ATTN_NORM, 0);        
	else
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/mp5fire1.wav"), 1, ATTN_NORM, 0);
}

void Puppet_M4_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	start[2] += 8;
	fire_bullet_sparks (self, start, forward, 90, 90, M4_SPREAD, M4_SPREAD,MOD_M4);
	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/m4a1fire.wav"), 1, ATTN_NORM, 0);
	
}
void Puppet_M3_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	start[2] += 8;
	
	fire_shotgun (self, start, forward, 17, 20, 800, 800, 12, MOD_M3);
	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/shotgf1b.wav"), 1, ATTN_NORM, 0);
	
}
void Puppet_HC_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	vec3_t v, v2;
	start[2] += 8;
	v[0] = self->s.angles[0];
	v[1] = self->s.angles[1] - 5;
	v[2] = forward[2];
	AngleVectors (v, v2, NULL, NULL);
	fire_shotgun (self, start, forward, 20, 40, 1000*4, 500*4, 34/2, MOD_HC);
	
	v[0] = self->s.angles[0];
	v[1] = self->s.angles[1] + 5;
	v[2] = forward[2];
	AngleVectors (v, v2, NULL, NULL);
	fire_shotgun (self, start, forward, 20, 40, 1000*4, 500*4, 34/2, MOD_HC);
	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/cannon_fire.wav"), 1, ATTN_NORM, 0);
	
}

void Puppet_Sniper_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
	int spread;
	spread = SNIPER_SPREAD;
	if ( ent->client->resp.sniper_mode == SNIPER_2X )
		spread = 0;
	else if ( ent->client->resp.sniper_mode == SNIPER_4X )
		spread = 0;
	else if ( ent->client->resp.sniper_mode == SNIPER_6X )
		spread = 0;

	start[2] += 8;
	fire_bullet_sniper (self, start, forward, 250, 200, spread, spread,MOD_SNIPER);

	if (ent->client->resp.toggles & TG_SILENCER)
		gi.sound(self, CHAN_WEAPON, gi.soundindex("misc/silencer.wav"), 1, ATTN_NORM, 0);        
	else
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/ssgfire.wav"), 1, ATTN_NORM, 0);
	
}

void Puppet_Dual_Fire (edict_t *self, edict_t *ent, vec3_t start, vec3_t forward)
{
		start[2] += 8;
		if (ent->client->ps.gunframe == 8)
		{
			fire_bullet (self, start, forward, 90, 90, DUAL_SPREAD, DUAL_SPREAD,MOD_DUAL);
			gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/mk23fire.wav"), 1, ATTN_NORM, 0);
			return;
		}
			fire_bullet (self, start, forward, 90, 90, DUAL_SPREAD, DUAL_SPREAD,MOD_DUAL);
			gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/mk23fire.wav"), 1, ATTN_NORM, 0);

}

void Puppet_LookAtLastOwner (edict_t *self, edict_t *lastowner)
{
	vec3_t		dir, start;
	float		pitch, forward;
	int			mod;

	if (lastowner && lastowner != world && lastowner != self)
		VectorSubtract (lastowner->s.origin, self->s.origin, dir);
/*
	if (dir[0])
	{
		self->s.angles[1] = 180/M_PI*atan2(dir[1], dir[0]);

		forward = sqrt (dir[0]*dir[0] + dir[1]*dir[1]);
		pitch = (atan2(dir[2], forward) * -180 / M_PI);

		if (pitch < 0)
			pitch += 360;

		self->pos1[0] = pitch;

		if ((pitch <= 340) && (pitch > 180))
			pitch = 340;
		else if ((pitch >= 20) && (pitch < 180))
			pitch = 20;

		self->s.angles[0] = pitch;
	}
	else {
		self->s.angles[1] = 0;
		if (dir[1] > 0)
			self->s.angles[1] = 90;
		else if (dir[1] < 0)
			self->s.angles[1] = -90;
	}
	if (self->s.angles[1] < 0)
		self->s.angles[1] += 360;

	self->s.angles[2] = 0;

	self->pos1[1] = self->s.angles[1];
	self->pos1[2] = 0;
*/
	
	if (dir[0])
	{
		self->s.angles[1] = 180/M_PI*atan2(dir[1], dir[0]);

		forward = sqrt (dir[0]*dir[0] + dir[1]*dir[1]);
		pitch = (atan2(dir[2], forward) * -180 / M_PI);

		if (pitch < 0)
			pitch += 360;

		self->pos1[PITCH] = pitch;

		if (pitch > 180)
			self->s.angles[PITCH] = (-360 + pitch)/3;
		else
			self->s.angles[PITCH] = pitch/3;
	}
	else {
		self->s.angles[1] = 0;
		if (dir[1] > 0)
			self->s.angles[1] = 90;
		else if (dir[1] < 0)
			self->s.angles[1] = -90;
	}
	if (self->s.angles[1] < 0)
		self->s.angles[1] += 360;

	self->s.angles[2] = 0;

	self->pos1[1] = self->s.angles[1];
	self->pos1[2] = 0;

	//here for puppetshootarea var settings
	/*
	#define LOC_HDAM 1 // head
	#define LOC_CDAM 2 // chest
	#define LOC_SDAM 3 // stomach
	#define LOC_LDAM 4 // legs
	#define	LOC_RAND 5 // random //PaTMaN
	*/

	VectorCopy(self->s.origin,start);

	mod = lastowner->client->resp.puppetshootarea;
	if	(mod == LOC_RAND)	mod = rand()%4+1;
	if		(mod == LOC_HDAM)	mod = -2;
	else if (mod == LOC_CDAM)	mod = -16;
	else if (mod == LOC_SDAM)	mod = -28;
	else if (mod == LOC_LDAM)	mod = -36;
	start[2] += lastowner->viewheight + mod;

	//Puppet Line of Sight laser
	if (lastowner->client->resp.wmodes & OS_PUPPET_SIGHT)
	{
		vec3_t S,E;
		VectorCopy(self->s.origin,S);
		VectorCopy(self->lastowner->s.origin,E);

		S[2] += self->maxs[2] - 10;
		E[2] += lastowner->viewheight + mod;

		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BFG_LASER);
		gi.WritePosition (S);
		gi.WritePosition (E);
		gi.multicast (S, MULTICAST_PHS);
	}


	if (self->lastowner->client->resp.toggles & TG_PUPPETSHOOT)
	{
		switch (self->lastowner->client->curr_weap)
		{
		case MK23_NUM:
			if (self->lastowner->client->ps.gunframe == 11)
				Puppet_Pistol_Fire (self,self->lastowner,start,dir);
			break;
		case MP5_NUM:
			if ((self->lastowner->client->ps.gunframe == 11) ||
				(self->lastowner->client->ps.gunframe == 12) ||
				(self->lastowner->client->ps.gunframe == 72) ||
				(self->lastowner->client->ps.gunframe == 73) ||
				(self->lastowner->client->ps.gunframe == 74))
				Puppet_MP5_Fire (self,self->lastowner,start,dir);
			break;
		case M4_NUM:
			if ((self->lastowner->client->ps.gunframe == 11) ||
				(self->lastowner->client->ps.gunframe == 12) ||
				(self->lastowner->client->ps.gunframe == 66) ||
				(self->lastowner->client->ps.gunframe == 67) ||
				(self->lastowner->client->ps.gunframe == 68))
				Puppet_M4_Fire (self,self->lastowner,start,dir);
			break;
		case M3_NUM:
			if (self->lastowner->client->ps.gunframe == 9)
				Puppet_M3_Fire (self,self->lastowner,start,dir);
			break;
		case HC_NUM:
			if (self->lastowner->client->ps.gunframe == 8)
				Puppet_HC_Fire (self,self->lastowner,start,dir);
			break;
		case SNIPER_NUM:
			if (self->lastowner->client->ps.gunframe == 10)
				Puppet_Sniper_Fire (self,self->lastowner,start,dir);
			if (self->lastowner->client->ps.gunframe == 13)
				gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/ssgbolt.wav"), 1, ATTN_NORM, 0);
			break;
		case DUAL_NUM:
			if ((self->lastowner->client->ps.gunframe == 8) ||
				(self->lastowner->client->ps.gunframe == 9))
				Puppet_Dual_Fire (self,self->lastowner,start,dir);
			break;
		default:
			break;
		}
	}

//	VectorCopy(self->s.angles,self->pos1);
}

qboolean FollowPuppetDemoFile (edict_t *ent,qboolean reverse);
extern edict_t *FindOverlap_InsidePuppet(edict_t *ent, edict_t *last_overlap);

void Puppet_Think (edict_t *ent)
{
	edict_t *other=NULL;
	int numcl=0;

	if (ent->lastowner->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING)
	{
		if ((ent->lastowner->client->resp.wmodes & OS_PUPPET_SOLID_PPLAY) &&
			(ent->solid != SOLID_BBOX))
		{
			ent->solid = SOLID_BBOX;
			gi.linkentity(ent);
			RemoveFromTransparentList(ent);
		}
		else if ((ent->lastowner->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING) &&
			!(ent->lastowner->client->resp.wmodes & OS_PUPPET_SOLID_PPLAY) &&
			(ent->solid != SOLID_TRIGGER))
		{
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
			AddToTransparentList(ent);
		}
	}
	else
	{
		if ((ent->lastowner->client->resp.wmodes & OS_PUPPET_SOLID) && (ent->solid != SOLID_BBOX))
		{
			edict_t *overlap;
			if ((overlap = FindOverlap(ent, NULL)) == NULL)
			{
				ent->clipmask = MASK_PLAYERSOLID;
				ent->solid = SOLID_BBOX;
				gi.linkentity(ent);
				RemoveFromTransparentList(ent);
			}
			else
			{
				do
				{
					overlap = FindOverlap(ent, overlap);
				} while (overlap != NULL);
			}
		}
		else if (!(ent->lastowner->client->resp.wmodes & OS_PUPPET_SOLID) && (ent->solid != SOLID_TRIGGER))
		{
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
			AddToTransparentList(ent);
		}
	}

	if (FindOverlap_InsidePuppet(ent, NULL) != NULL)
	{
		ent->solid = SOLID_TRIGGER;
		gi.linkentity(ent);
	}

	//This shit is an old mess but it works... I ain't touching it
	if ((ent->lastowner->client->resp.puppetdemo_state == PUPPET_NEW_NOTHING) || 
		(ent->lastowner->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING))
	{
		qboolean CanSee = false;
		ent->lastowner->client->resp.puppetdemo_frame = 0;

		if (!ent->lastowner->client->pers.spectator)
		{
			if ((loc_CanSee(ent->lastowner, ent)) &&
				((ent->lastowner->s.origin[0] != ent->s.origin[0]) || 
				(ent->lastowner->s.origin[1] != ent->s.origin[1])))
			{
				Puppet_LookAtLastOwner(ent,ent->lastowner);
				CanSee = true;
			}
			else if ((loc_CanSee(ent->lastowner, ent)) && 
				(!VectorCompare(ent->lastowner->s.origin,ent->s.origin)))
				ent->s.angles[1] = ent->lastowner->s.angles[1];
		}
		
		if (ent->lastowner->client->resp.toggles & TG_PUPPETSHOOT)
			ent->s.skinnum = ent->lastowner->s.skinnum;
		
		if (ent->maxs[2] == 32)
		{
			if ((((ent->lastowner->s.frame >= 160) && (ent->lastowner->s.frame <= 167)) &&
				(ent->lastowner->client->resp.toggles & TG_PUPPETSHOOT)) && (CanSee))
			{
				if ((ent->lastowner->client->curr_weap != KNIFE_NUM) && (ent->lastowner->client->curr_weap != MARKER_NUM))
				{
					ent->s.frame = ent->lastowner->s.frame - 114;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
			else if ((((ent->lastowner->s.frame >= 46) && (ent->lastowner->s.frame <= 52)) &&
				(ent->lastowner->client->resp.toggles & TG_PUPPETSHOOT)) && (CanSee))
			{
				if ((ent->lastowner->client->curr_weap != KNIFE_NUM) && (ent->lastowner->client->curr_weap != MARKER_NUM))
				{
					ent->s.frame = ent->lastowner->s.frame;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
			else
			{
				
				if (ent->s.frame > 39)
					ent->s.frame = 0;
				if (++ent->s.frame < 40)
					ent->nextthink = level.time + FRAMETIME;
				else
				{               
					ent->s.frame = 0;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
		}
		else if (ent->maxs[2] == 16)
		{
			if ((((ent->lastowner->s.frame >= 160) && (ent->lastowner->s.frame <= 168)) &&
				(ent->lastowner->client->resp.toggles & TG_PUPPETSHOOT)) && (CanSee))
			{
				if ((ent->lastowner->client->curr_weap != KNIFE_NUM) && (ent->lastowner->client->curr_weap != MARKER_NUM))
				{
					ent->s.frame = ent->lastowner->s.frame;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
			else if ((((ent->lastowner->s.frame >= 46) && (ent->lastowner->s.frame <= 53)) &&
				(ent->lastowner->client->resp.toggles & TG_PUPPETSHOOT)) && (CanSee))
			{
				if ((ent->lastowner->client->curr_weap != KNIFE_NUM) && (ent->lastowner->client->curr_weap != MARKER_NUM))
				{
					ent->s.frame = ent->lastowner->s.frame + 114;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
			else
			{
				if ((ent->s.frame > 153) || (ent->s.frame < 135))
					ent->s.frame = 135;
				if (++ent->s.frame < 154)
					ent->nextthink = level.time + FRAMETIME;
				else
				{               
					ent->s.frame = 135;
					ent->nextthink = level.time + FRAMETIME;
				}
			}
		}
	}
	else //puppetdemo_state == PUPPET_NEW_PLAYING
	{
		if (ent->lastowner->client->resp.puppetdemo_pause == false)
		{

			if (FollowPuppetDemoFile(ent,false))
			{
				ent->s.angles[2] = 0;
				if (ent->maxs[2] == 32)
				{
					if (ent->s.frame > 39)
						ent->s.frame = 0;
					if (++ent->s.frame < 40)
						ent->nextthink = level.time + FRAMETIME;
					else
					{
						ent->s.frame = 0;
						ent->nextthink = level.time + FRAMETIME;
					}
				}
				else if (ent->maxs[2] == 16)
				{

					if ((ent->s.frame > 153) || (ent->s.frame < 135))
						ent->s.frame = 135;
					if (++ent->s.frame < 154)
						ent->nextthink = level.time + FRAMETIME;
					else
					{
						ent->s.frame = 135;
						ent->nextthink = level.time + FRAMETIME;
					}
				}

			}
		}

		if (ent->lastowner->client->resp.wmodes & OS_PUPPET_SIGHT)
		{
			vec3_t start, forward, right, end, up, angles, offset;
			trace_t tr;

			VectorCopy(ent->pos1,angles);
			AngleVectors (angles, forward, right, up);
			VectorSet(offset,0 , 0, ent->maxs[2] - 10);
			G_ProjectSource (ent->s.origin, offset, forward, right, start);
			VectorMA(start,8192,forward,end);

			PRETRACE();
			tr = gi.trace (start,NULL,NULL, end,ent,CONTENTS_SOLID);
			POSTTRACE();

			gi.WriteByte (svc_temp_entity);
			gi.WriteByte (TE_BFG_LASER);
			gi.WritePosition (start);
			gi.WritePosition (tr.endpos);
			gi.multicast (start, MULTICAST_PHS);
		}
	}

	ent->s.modelindex = 255;
	// update chase cam if being followed
	for (i = 1; i <= maxclients->value; i++) {
		other = g_edicts + i;
		if (other->inuse)
		{
			if ((other->client->chase_mode) && (other->client->chase_target == ent))
			{
				if ((other == ent->lastowner) && (other->client->chase_mode == SPEC_EYES))
				{
					if (level.clientcount == 1)
						ent->s.modelindex = 0;
				}
				Spec_UpdateChaseCam(other);
			}
		}
	}
	if (level.clientcount > 1)
		ent->s.modelindex = 255;

	ent->nextthink = level.time + .1;
}

void Puppet_Spawn (edict_t *self,qboolean skip,qboolean nomsg)
{
	edict_t *puppet=NULL;	

	if (!skip)
	{
		if (self->client->pers.spectator)
		{
			if (self->client->puppet)
			{
				G_FreeEdict(self->client->puppet);
				self->client->puppet = NULL;
				self->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
				return; //avoid the message if they just went spec
			}
			if (!nomsg)
				gi.cprintf(self,PRINT_HIGH,"This command cannot be used to spawn a puppet when spectating\n");
			return;
		}
	}

	if (self->client->puppet)
	{  
		G_FreeEdict(self->client->puppet);
		self->client->puppet = NULL;

		if (self->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
			self->client->resp.puppetdemo_pup_recorded = false;
		else
			self->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		if (!nomsg)
			gi.centerprintf (self,"Puppet removed\n");
		return;
	}

	puppet = G_Spawn ();
	puppet->classname = "puppet";
	puppet->classnum = 0;
	puppet->takedamage = DAMAGE_AIM;
	puppet->movetype = MOVETYPE_TOSS;
	
	puppet->solid = SOLID_TRIGGER;
	puppet->deadflag = DEAD_NO;
	puppet->clipmask = (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW);

	if (self->client->pers.spectator)
	{
		puppet->model = "players/male/tris.md2";
		puppet->s.modelindex = 255;
		puppet->s.modelindex2 = 255;
		puppet->s.skinnum = 256;
	}
	else
	{
		puppet->model = self->model;
		puppet->s.modelindex = 255;
		puppet->s.modelindex2 = self->s.modelindex2;
		puppet->s.skinnum |= self->s.skinnum;
	}

	if (self->client->resp.wmodes & OS_PUPPET_TRANS)
		puppet->s.renderfx |= RF_TRANSLUCENT;
	else
		puppet->s.renderfx = 0;
	   
	puppet->think = Puppet_Think;
	puppet->nextthink = level.time + .1;
	puppet->owner = world;
	puppet->lastowner = self;
	puppet->viewheight = 22;
	VectorSet (puppet->mins, -16, -16, -24);
	VectorSet (puppet->maxs, 16, 16, 32);

	if (self->movetype == MOVETYPE_NOCLIP)
	{
		puppet->maxs[2] = 32;
		puppet->s.frame = 0;
	}
	else
	{
		puppet->maxs[2] = self->maxs[2];
		puppet->s.frame = self->s.frame;
	}

	if (!self->client->pers.spectator)
	{
		VectorCopy(self->s.origin,puppet->s.origin);
		puppet->s.angles[1] = self->s.angles[1];
	}

	VectorClear (puppet->velocity);

	puppet->movetarget = NULL;

	self->client->puppet = puppet;
	self->client->resp.puppetdemo_pause = false;
	self->client->resp.puppetdemo_frame = 0;

	gi.linkentity(puppet);
	AddToTransparentList(puppet);

	if (!nomsg)
		gi.centerprintf (self,"Puppet created\n");
}

void Pup_LookAtOwner (edict_t *self, edict_t *lastowner)
{
	vec3_t          dir;
	float pitch, forward;
	
	if (lastowner && lastowner != world && lastowner != self)
		VectorSubtract (lastowner->s.origin, self->s.origin, dir);
	
	if (dir[0])
	{
		self->s.angles[1] = 180/M_PI*atan2(dir[1], dir[0]);

		forward = sqrt (dir[0]*dir[0] + dir[1]*dir[1]);
		pitch = (atan2(dir[2], forward) * -180 / M_PI);

		if (pitch < 0)
			pitch += 360;

		self->pos1[PITCH] = pitch;

		if (pitch > 180)
			self->s.angles[PITCH] = (-360 + pitch)/3;
		else
			self->s.angles[PITCH] = pitch/3;
	}
	else {
		self->s.angles[1] = 0;
		if (dir[1] > 0)
			self->s.angles[1] = 90;
		else if (dir[1] < 0)
			self->s.angles[1] = -90;
	}
	if (self->s.angles[1] < 0)
		self->s.angles[1] += 360;

	self->s.angles[2] = 0;

	self->pos1[1] = self->s.angles[1];
	self->pos1[2] = 0;
}

void Pup_Think (edict_t *ent)
{
	edict_t *other=NULL;

	if (!Q_stricmp(ent->owner->classname,"freed"))
	{
		ent->owner = world;
		ent->lastowner->client->puppet = ent;
		ent->classname = "puppet";
		ent->think = Puppet_Think;
	}
	else
	{
		if ((ent->owner->lastowner->client->resp.wmodes & OS_PUPPET_SOLID) && (ent->solid != SOLID_BBOX))
		{
			edict_t *overlap;
			if ((overlap = FindOverlap(ent, NULL)) == NULL)
			{
				ent->clipmask = MASK_PLAYERSOLID;
				ent->solid = SOLID_BBOX;
				gi.linkentity(ent);
			}
			else
			{
				do
				{
					overlap = FindOverlap(ent, overlap);
				} while (overlap != NULL);
			}
		}
		else if (!(ent->owner->lastowner->client->resp.wmodes & OS_PUPPET_SOLID) && (ent->solid != SOLID_TRIGGER))
		{
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
		}
		
		if ((loc_CanSee(ent->owner, ent)) &&
			((ent->owner->s.origin[0] != ent->s.origin[0]) || 
			(ent->owner->s.origin[1] != ent->s.origin[1])))
			Pup_LookAtOwner(ent,ent->owner);
		else if ((loc_CanSee(ent->owner, ent)) && 
			(!VectorCompare(ent->owner->s.origin,ent->s.origin)))
			ent->s.angles[1] = ent->owner->s.angles[1];
		
		
		ent->s.effects = ent->owner->s.effects;
		ent->s.renderfx = ent->owner->s.renderfx;
		if (ent->maxs[2] == 32)
		{
			if (ent->s.frame > 39)
				ent->s.frame = 0;
			if (++ent->s.frame < 40)
				ent->nextthink = level.time + FRAMETIME;
			else
			{               
				ent->s.frame = 0;
				ent->nextthink = level.time + FRAMETIME;
			}
		}
		else if (ent->maxs[2] == 16)
		{
			if ((ent->s.frame > 153) || (ent->s.frame < 135))
				ent->s.frame = 135;
			if (++ent->s.frame < 154)
				ent->nextthink = level.time + FRAMETIME;
			else
			{               
				ent->s.frame = 135;
				ent->nextthink = level.time + FRAMETIME;
			}
		}
	}

	ent->s.modelindex = 255;
	// update chase cam if being followed
	for (i = 1; i <= maxclients->value; i++) {
		other = g_edicts + i;
		if (other->inuse)
		{
			if ((other->client->chase_mode) && (other->client->chase_target == ent))
			{
				if ((other == ent->owner->lastowner) && (other->client->chase_mode == SPEC_EYES))
				{
					if (level.clientcount == 1)
						ent->s.modelindex = 0;
				}
				Spec_UpdateChaseCam(other);
			}
		}
	}
	if (level.clientcount > 1)
		ent->s.modelindex = 255;

	ent->nextthink = level.time + .1;
}

void Pup_Spawn (edict_t *self,vec3_t origin,vec3_t angles,float max)
{
	edict_t *Pup;

	//If a client was sent, switch to the client's puppet if any
	if (self->client)
	{
		if (self->client->puppet)
			self = self->client->puppet;
		else return;
	}

	//always respawn to avoid seeing pup move to origin
	if (self->movetarget)
		G_FreeEdict(self->movetarget);

	Pup = G_Spawn ();
	self->movetarget = Pup;

	Pup->classname = "pup";
	Pup->takedamage = DAMAGE_AIM;
	Pup->movetype = MOVETYPE_TOSS;
	Pup->solid = SOLID_TRIGGER;
	Pup->deadflag = DEAD_NO;
	Pup->clipmask = (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW);
	Pup->model = self->model;
	Pup->s.modelindex = 255;
	Pup->s.modelindex2 = self->s.modelindex2;
	Pup->s.skinnum |= self->s.skinnum;
	Pup->s.renderfx = self->s.renderfx;
	if (max == 16) Pup->viewheight = 8;
	else Pup->viewheight = 22;
	Pup->think = Pup_Think;
	Pup->nextthink = level.time + .1;
	Pup->owner = self;
	Pup->lastowner = self->lastowner;

	VectorSet (Pup->mins, -16, -16, -24);
	VectorSet (Pup->maxs, 16, 16, max);
	VectorCopy(origin,Pup->s.origin);
	VectorCopy(angles,Pup->s.angles);
	VectorClear (Pup->velocity);
}

