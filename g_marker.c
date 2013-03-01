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

#define MAX_TOTAL_JLOC_LINES    500
char jloc_lines[MAX_TOTAL_JLOC_LINES][70];
int jloc_num_lines;
static int markers = 0;

void SingleMarkerRemove (edict_t *ent);

int MarkersInt()
{  
	return markers;
}

void GetMarkerColor(edict_t *self, char *buf)
{
	switch (self->client->resp.marker_color)
	{
	case 0:
		sprintf(buf,"red");
		break;
	case 1:
		sprintf(buf,"green");
		break;
	case 2:
		sprintf(buf,"blue");
		break;
	case 3:
		sprintf(buf,"yellow");
		break;
	case 4:
		sprintf(buf,"white");
		break;
	case 5:
		sprintf(buf,"purple");
		break;
	case 6:
		sprintf(buf,"aqua");
		break;
	}
}

char* NUMtoMCOLORWORD (int num)
{
	switch (num)
	{
	case 0:
		return "red\0";
	case 1:
		return "green\0";
	case 2:
		return "blue\0";
	case 3:
		return "yellow\0";
	case 4:
		return "white\0";
	case 5:
		return "purple\0";
	case 6:
		return "aqua\0";
	default:
		return NULL;
	}
}

void MCOLORWORDtoNUM (edict_t *ent,int type)
{
	//int type: 1 = marker, 2 = jump, 3 = fall
	int newcolor;
	if (gi.argc() < 2)
	{
		if (type == 1)
			gi.cprintf(ent,PRINT_HIGH, "\"markercolor\" is \"%s\"\n",NUMtoMCOLORWORD(ent->client->resp.marker_color));
		else if (type == 2)
			gi.cprintf(ent,PRINT_HIGH, "\"jumpmarkercolor\" is \"%s\"\n",NUMtoMCOLORWORD(ent->client->resp.marker_color_jump));
		else if (type == 3)
			gi.cprintf(ent,PRINT_HIGH, "\"fallmarkercolor\" is \"%s\"\n",NUMtoMCOLORWORD(ent->client->resp.marker_color_fall));
		return;
	}

	if (Q_stricmp(gi.args(),"red") == 0)
		newcolor = 0;
	else if (Q_stricmp(gi.args(),"green") == 0)
		newcolor = 1;
	else if (Q_stricmp(gi.args(),"blue") == 0)
		newcolor = 2;
	else if (Q_stricmp(gi.args(),"yellow") == 0)
		newcolor = 3;
	else if (Q_stricmp(gi.args(),"white") == 0)
		newcolor = 4;
	else if (Q_stricmp(gi.args(),"purple") == 0)
		newcolor = 5;
	else if (Q_stricmp(gi.args(),"aqua") == 0)
		newcolor = 6;
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"Unknown color specified: \"%s\"; Marker colors are: red, green, blue, yellow, white, purple, and aqua\n",gi.args());	
		return;
	}

	if (type == 1)
		ent->client->resp.marker_color = newcolor;
	else if (type == 2)
		ent->client->resp.marker_color_jump = newcolor;
	else if (type == 3)
		ent->client->resp.marker_color_fall = newcolor;
}

int Marker_FXtoCOLOR (edict_t *ent)
{
		 if (ent->s.renderfx == (RF_GLOW|RF_SHELL_RED))					return 0;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_GREEN))				return 1;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_BLUE))				return 2;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_DOUBLE))				return 3;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_HALF_DAM))			return 4;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_RED|RF_SHELL_BLUE))	return 5;
	else if (ent->s.renderfx == (RF_GLOW|RF_SHELL_BLUE|RF_SHELL_GREEN))	return 6;
	else																return 0;
}

void Marker_COLORtoFX (edict_t *ent,int color)
{
	     if (color == 0)	ent->s.renderfx = RF_GLOW|RF_SHELL_RED;
	else if (color == 1)	ent->s.renderfx = RF_GLOW|RF_SHELL_GREEN;
	else if (color == 2)	ent->s.renderfx = RF_GLOW|RF_SHELL_BLUE;
	else if (color == 3)	ent->s.renderfx = RF_GLOW|RF_SHELL_DOUBLE;
	else if (color == 4)	ent->s.renderfx = RF_GLOW|RF_SHELL_HALF_DAM;
	else if (color == 5)	ent->s.renderfx = RF_GLOW|RF_SHELL_RED|RF_SHELL_BLUE;
	else if (color == 6)	ent->s.renderfx = RF_GLOW|RF_SHELL_BLUE|RF_SHELL_GREEN;
	else					ent->s.renderfx = RF_GLOW|RF_SHELL_RED;
}

edict_t *FindMarkerOverlap(edict_t *ent)
{
	int i;
	vec3_t diff;
	edict_t *marker=NULL;
	int markers;
	
	markers = MarkersInt();
	
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum ("Jump Marker", i);
		
		if (marker)
		{
			vec3_t v;
			VectorCopy(ent->s.origin,v);
			if (v[2]>0)
				v[2] += ent->viewheight - 24;
			else
				v[2] -= ent->viewheight - 24;
			VectorSubtract(v, marker->s.origin, diff);
			
			if (diff[0] >= -16.2 && diff[0] <= 16.2 &&
				diff[1] >= -16.2 && diff[1] <= 16.2 &&
				diff[2] >= -33 && diff[2] <= 25) return marker;
		}
	}
	return NULL;
}

void Cmd_NameMarker (edict_t *ent)
{
    vec3_t  forward, dir;
	float highest=0.9,d;
	int i;
	edict_t *marker=NULL;
	edict_t *best=NULL;
	char msg[256];
	int markers = MarkersInt();
	
	if (strlen(gi.args()) > 40)
		gi.cprintf(ent,PRINT_HIGH,"Message is too long and has been truncated to 40 characters.\n");
	
	strcpy(msg,gi.args());
	
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum("Jump Marker", i);
		if (marker)
			if (loc_CanSee(ent,marker))
			{
				AngleVectors(ent->client->v_angle, forward, NULL, NULL);
				VectorSubtract(marker->s.origin, ent->s.origin, dir);
				VectorNormalize(dir);
				d = DotProduct(forward, dir);
				
				if (d > 0.9)
					if (d > highest)
					{
						highest = d;
						best = marker;
					}
			}
	}
	if (best != NULL)
		strcpy(best->mmsg,msg);
}

void Marker_Think_Effects (edict_t *ent)
{
	ent->s.effects = 0;
	ent->think = NULL;
}

void T_HighlightJumpMarkerRadius (edict_t *ent, float radius, qboolean remove)
{
	edict_t *cur=NULL;
	vec3_t end, forward, start;
	trace_t tr;

	VectorCopy(ent->s.origin,start);
	start[2] += ent->viewheight-8;

	AngleVectors (ent->client->v_angle, forward, NULL, NULL);
	VectorNormalize(forward);
	VectorMA(start, 8192, forward, end);

	tr = gi.trace (start,NULL,NULL, end,ent,CONTENTS_SOLID);

	cur = NULL;
	while ((cur = findradius(cur, tr.endpos, radius)) != NULL)
	{
		if (!Q_stricmp(cur->classname,"Jump Marker"))
		{
			if (remove)
				SingleMarkerRemove(cur);
			else
			{
				cur->s.effects = EF_YELLOWSHELL;
				cur->think = Marker_Think_Effects;
				cur->nextthink = level.time + FRAMETIME;
			}
		}
	}
}

//use the above code again for removal of markers... remove the flying remover

void T_RemoveJumpMarkerRadius (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod)
{
	edict_t *ent = NULL;
	cplane_t *plane = NULL;
	while ((ent = findradius(ent, inflictor->s.origin, radius)) != NULL)
	{
		if (CanDamage (ent, inflictor))
		{
			if (!Q_stricmp(ent->classname,"Jump Marker"))
			{
				gi.WriteByte (svc_temp_entity);    
				gi.WriteByte (TE_SPARKS);
				gi.WritePosition (ent->s.origin);    
				gi.WriteDir (plane->normal);
				gi.multicast (ent->s.origin, MULTICAST_PVS);
				SingleMarkerRemove(ent);
			}
		}
	}
}

static void Marker_Explode (edict_t *ent)
{
	T_RemoveJumpMarkerRadius(ent, ent->owner, 10000, ent->owner, 100, TE_GRENADE_EXPLOSION);
}

void RemoveAllMarkers(edict_t *ent)
{
	edict_t *marker=NULL;
	int i, remcount=0;
	
if ((int)weaponban->value & WB_MARKER) return;

	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum("Jump Marker", i);
		marker->nextthink = level.time + .1;
		remcount++;
	}
	if (remcount>0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Removed %i marker",remcount);
		if (remcount != 1)
			gi.cprintf(ent,PRINT_HIGH,"s\n");
		else
			gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

void RemoveMarkersInView(edict_t *ent)
{  
	
	edict_t *marker=NULL;
	int i, remcount=0;
	vec3_t forward,dir;
	float d;
	
	if ((int)weaponban->value & WB_MARKER) return;
	
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum("Jump Marker", i);
		if (loc_CanSee(ent,marker))
		{
			
			AngleVectors(ent->client->v_angle, forward, NULL, NULL);
			VectorSubtract(marker->s.origin, ent->s.origin, dir);
			VectorNormalize(dir);
			d = DotProduct(forward, dir);
			
			if (d > 0.7)
			{
				marker->nextthink = level.time + .1;
				remcount++;
			}
		}
	}
	if (remcount>0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Removed %i marker",remcount);
		if (remcount != 1)
			gi.cprintf(ent,PRINT_HIGH,"s\n");
		else
			gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

void RelinkMarkers()
{
	edict_t *marker=NULL;
	int i,mod=0;
	
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum ("Jump Marker", i);
		if (marker)
			marker->classnum = i-mod;
		else
			mod++;
	}
}

void SingleMarkerRemove (edict_t *ent)
{
	G_FreeEdict(ent);

	RelinkMarkers();
	
	markers--;
	
	if (markers < 0)
	{
		markers=0;
		gi.bprintf (PRINT_HIGH,"REPORT AS BUG! MARKER COUNT LESS THAN 0 (%d)\nTHIS OCCURRED WHILE DELETING A MARKER\n",markers);
	}
}

void marker_touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	
	edict_t         *dropped=NULL;
	edict_t         *marker=NULL;
	vec3_t          move_angles;
	int				fx = ent->s.renderfx;

	if (other->takedamage)
	{
		G_FreeEdict(ent);
		return;
	}
	
	dropped = G_Spawn();
		
	dropped->s.renderfx = fx;

	/* // V1.21 REMOVED
	if (ent->owner->client->resp.wmodes & WM_MARKER)//marker_mode)
		dropped->s.renderfx = RF_FULLBRIGHT;
	else
		Marker_COLORtoFX (dropped,ent->owner->client->resp.marker_color);
	*/ // V1.21 REMOVED

	VectorSet (dropped->mins, -15, -15, -15);
	VectorSet (dropped->maxs, 15, 15, 15);
	if (!(ent->owner->client->resp.wmodes & WM_MARKER))//marker_mode == 0)
		gi.setmodel (dropped, "models/objects/laser/tris.md2");
	dropped->solid = SOLID_TRIGGER;
	dropped->movetype = MOVETYPE_NONE;  
	dropped->owner = ent;
	dropped->gravity = 0;
	dropped->classname = "Jump Marker";
	dropped->classnum = markers+1;
	dropped->takedamage = DAMAGE_YES;
	
	vectoangles (ent->velocity, move_angles);
	
	VectorCopy (ent->s.origin, dropped->s.origin);
	VectorCopy (move_angles, dropped->s.angles);

	if (dropped->s.renderfx != RF_FULLBRIGHT)
	{
		dropped->think = SingleMarkerRemove;
		
		if (!markerlimit->value) markerlimit->value = 80;
		
		markers++;
		
		if (markers > markerlimit->value)
			markers = markerlimit->value;
		
		marker = FindEdictByClassnum ("Jump Marker", markers+1);
		
		if (marker)
		{          
			markers++;
			marker->nextthink = level.time + .1;
		}
		
	}
	else
		dropped->think = G_FreeEdict;
	
	if (ent->owner->client->resp.wmodes & WM_MARKER)//marker_mode == 1)
	{
		Marker_Explode (dropped);
		
	}
	gi.linkentity (dropped);
	G_FreeEdict (ent);
}

void marker_float (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	
	edict_t         *dropped=NULL;
	edict_t         *marker=NULL;
	vec3_t          move_angles;
	
	dropped = G_Spawn();
	
	dropped->s.renderfx = ent->s.renderfx;
	//Marker_COLORtoFX (dropped,ent->owner->client->resp.marker_color);
	
	VectorSet (dropped->mins, -15, -15, -15);
	VectorSet (dropped->maxs, 15, 15, 15);
	
	gi.setmodel (dropped, "models/objects/laser/tris.md2");
	dropped->solid = SOLID_TRIGGER;
	dropped->movetype = MOVETYPE_NONE;  
	dropped->owner = ent;
	dropped->gravity = 0;
	dropped->classname = "Jump Marker";
	dropped->classnum = markers+1;
	dropped->takedamage = DAMAGE_YES;
	
	vectoangles (ent->velocity, move_angles);
	
	VectorCopy (ent->s.origin, dropped->s.origin);
	dropped->s.origin[2] -= 24;
	VectorCopy (move_angles, dropped->s.angles);
	
	dropped->think = SingleMarkerRemove;
	
	if (!markerlimit->value) markerlimit->value = 100;
	
	markers++;
	
	if (markers > markerlimit->value)
		markers = markerlimit->value;
	
	marker = FindEdictByClassnum ("Jump Marker", markers+1);
	
	if (marker)
	{
		markers++;
		marker->nextthink = level.time + .1;
	}
	
	gi.linkentity (dropped);
	G_FreeEdict (ent);
}

void marker_throw (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed , qboolean floating, int color, qboolean realthrow)
{
	edict_t *marker=NULL;
	
	if ((int)weaponban->value & WB_MARKER) return;
	
	if ((self->client->resp.wmodes & WM_MARKER) && (realthrow))
	{
		T_HighlightJumpMarkerRadius(self,100,true);
		return;
	}

	marker = G_Spawn();
	
	VectorNormalize (dir);
	VectorCopy (start, marker->s.origin);
	VectorCopy (start, marker->s.old_origin);
	vectoangles (dir, marker->s.angles);
	VectorScale (dir, speed, marker->velocity);
	
	marker->movetype = MOVETYPE_FLY;
	marker->clipmask = MASK_SHOT;
	marker->solid = SOLID_BBOX;
	marker->s.effects = 0;

//	if ((self->client->resp.wmodes & WM_MARKER) && (realthrow))//marker_mode)
//		marker->s.renderfx = RF_TRANSLUCENT;
//	else
	Marker_COLORtoFX (marker,color);

	gi.setmodel (marker, "models/objects/laser/tris.md2");

	VectorClear (marker->mins);
	VectorClear (marker->maxs);

	marker->owner = self;
	marker->think = G_FreeEdict;
	marker->dmg = damage;
	marker->classname = "floating_marker";

	if (floating)
		marker->touch = NULL;
	else
		marker->touch = marker_touch;

	gi.linkentity (marker);

	if (floating)
	{
		if (markers < markerlimit->value)
			marker_float (marker, NULL, NULL, NULL);
		else
			G_FreeEdict(marker);
	}
}

void marker_spawn (vec3_t start,vec3_t angles,int color,char message[256])
{
	
	edict_t         *ms=NULL;
	edict_t         *marker=NULL;
	
	if (markers >= markerlimit->value)
		return;
	
	ms = G_Spawn();
	
	ms->s.renderfx = color;
	
	VectorSet (ms->mins, -15, -15, -15);
	VectorSet (ms->maxs, 15, 15, 15);
	
	gi.setmodel (ms, "models/objects/laser/tris.md2");
	ms->solid = SOLID_TRIGGER;
	ms->movetype = MOVETYPE_NONE;  
	ms->owner = world;
	ms->gravity = 0;
	ms->classname = "Jump Marker";
	ms->classnum = markers+1;
	ms->takedamage = DAMAGE_YES;
	strcpy(ms->mmsg,message);
	
	VectorCopy (start, ms->s.origin);
	VectorCopy (angles, ms->s.angles);
	
	ms->think = SingleMarkerRemove;
	
	if (!markerlimit->value) markerlimit->value = 100;
	
	markers++;
	
	if (markers > markerlimit->value)
		markers = markerlimit->value;
	
	marker = FindEdictByClassnum ("Jump Marker", markers+1);
	
	if (marker)
	{
		markers++;
		marker->nextthink = level.time + .1;
	}
	
	gi.linkentity (ms);
}

void SaveJLOC_all(edict_t *ent,char *fn)
{

	FILE *jloc_file;
	edict_t *marker=NULL;
	int markers = MarkersInt();
	int i, savecount=0;

if ((int)weaponban->value & WB_MARKER) return;

	jloc_file = fopen(fn, "r");
	if (jloc_file != NULL)
	{
		fclose(jloc_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",fn);
		return;
	}

	jloc_file = fopen(fn, "w");
		
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum("Jump Marker", i);

		fprintf(jloc_file,"%f %f %f %f %f %i %s\n",
			marker->s.origin[0],marker->s.origin[1],marker->s.origin[2],
			marker->s.angles[0],marker->s.angles[1],Marker_FXtoCOLOR(marker),
			marker->mmsg);
		savecount++;
	}

	if (savecount>0)
	{
		/*
		char msg[500];
		char buffer[4];
			
		strcpy(msg,"Saved ");
		//LINUX PORT ITOA//itoa(savecount,buffer,10);
		sprintf(buffer,"%i",savecount);
		strcat(msg,buffer);
		strcat(msg," marker");
		if (savecount != 1) strcat(msg,"s ");
		else strcat(msg," ");
		strcat(msg,"to ");
		strcat(msg,fn);

		gi.bprintf(PRINT_HIGH,"%s: %s\n",ent->client->pers.netname,msg);
		*/

		gi.bprintf(PRINT_HIGH,"%s: Marker file saved to %s (%i marker%s)\n",ent->client->pers.netname,fn,savecount,(savecount != 1) ? "s" : "");
	}
	
	fclose(jloc_file);

}

void SaveJLOC_view(edict_t *ent,char *fn)
{

	FILE *jloc_file;
	edict_t *marker=NULL;
	int markers = MarkersInt();
	int i, savecount=0;
		
	if ((int)weaponban->value & WB_MARKER) return;

	jloc_file = fopen(fn, "r");
	if (jloc_file != NULL)
	{
		fclose(jloc_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",fn);
		return;
	}
	
	jloc_file = fopen(fn, "w");
		
	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum("Jump Marker", i);
		if (loc_CanSee(ent,marker))
		{
			vec3_t forward,dir;
			float d;

			AngleVectors(ent->client->v_angle, forward, NULL, NULL);
			VectorSubtract(marker->s.origin, ent->s.origin, dir);
			VectorNormalize(dir);
			d = DotProduct(forward, dir);
						
			if (d > 0.7)
			{
				fprintf(jloc_file,"%f %f %f %f %f %i %s\n",
					marker->s.origin[0],marker->s.origin[1],marker->s.origin[2],
					marker->s.angles[0],marker->s.angles[1],Marker_FXtoCOLOR(marker),
					marker->mmsg);
				savecount++;
			}
		}
	}

	if (savecount>0)
	{
		/*
		char msg[500];
		char buffer[4];
		
		strcpy(msg,"Saved ");
		//LINUX PORT ITOA//itoa(savecount,buffer,10);
		sprintf(buffer,"%i",savecount);
		strcat(msg,buffer);
		strcat(msg," marker");
		if (savecount != 1) strcat(msg,"s ");
		else strcat(msg," ");
		strcat(msg,"to ");
		strcat(msg,fn);

		gi.bprintf(PRINT_HIGH,"%s: %s\n",ent->client->pers.netname,msg);
		*/

		gi.bprintf(PRINT_HIGH,"%s: Marker file saved to %s (%i marker%s)\n",ent->client->pers.netname,fn,savecount,(savecount != 1) ? "s" : "");
	}
	
	fclose(jloc_file);

}

void SaveJLOC(edict_t *ent, int cmd)
{
	char *opt;
	char JLOCFile[256];
	int markers = MarkersInt();
	char path[256];
	
    if ((int)weaponban->value & WB_MARKER) return;

	if (markers < 1)
	{
		gi.cprintf(ent,PRINT_HIGH,"No markers to save!\n");
		return;
	}

	if (gi.argc() != 3)
	{
		if (cmd == 1)
			gi.cprintf(ent,PRINT_HIGH, "Wrong syntax: jlocsave <all/view> <filename>\n");
		else
			gi.cprintf(ent,PRINT_HIGH, "Wrong syntax: msave <all/view> <filename>\n");
		return;
	}

	if (BadFileName(gi.args()))
	{
		gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in filename. Markers not saved.\n");
		return;
	}

	opt = strtok(gi.args()," ");

	//sprintf (path, "%s\\jloc", GAMEVERSION); //V1.21 CHANGE
	sprintf (path, GAMEVERSION "/jloc");
	p_mkdir(path);
	sprintf (JLOCFile, "%s/jloc/%s-%s.jloc", GAMEVERSION, level.mapname, strtok(NULL," "));

	if (!Q_stricmp(opt,"all")) SaveJLOC_all(ent,JLOCFile);
	else if (!Q_stricmp(opt,"view")) SaveJLOC_view(ent,JLOCFile);
	else gi.cprintf(ent,PRINT_HIGH, "Unknown save option \"%s\"\n",opt);
	
}

void DelayMarkerLoad_Think (edict_t *marker)
{
	marker_spawn(marker->s.origin,marker->s.angles,marker->s.renderfx,marker->mmsg);
	G_FreeEdict(marker);
	
}

void DeleteJLOC(edict_t *ent, char filename[MAX_STR_LEN],char *ren)
{
	FILE *jloc_file;
	char JLOCFile[MAX_STR_LEN];

	sprintf(JLOCFile,GAMEVERSION "/jloc/%s-%s.jloc",level.mapname,filename);
	
	jloc_file = fopen(JLOCFile, "r");
	if (jloc_file)
	{
		fclose(jloc_file);

		if (ren != NULL)
		{
			char JLF_ren[MAX_STR_LEN]="";
			sprintf(JLF_ren,GAMEVERSION "/jloc/%s-%s.jloc",level.mapname,ren);
			jloc_file = fopen(JLF_ren, "r");
			if (jloc_file == NULL)
			{
				rename(JLOCFile,JLF_ren);
				gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jloc\" was renamed to \"%s-%s.jloc\"\n",
					level.mapname,filename,level.mapname,ren);
			}
			else
			{
				fclose(jloc_file);
				gi.cprintf(ent,PRINT_HIGH,"Destination file exists. Can not rename!\n");
			}
		}
		else
		{
			remove(JLOCFile);
			gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jloc\" was deleted\n",level.mapname,filename);
		}
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jloc\" was not found!\n",level.mapname,filename);
}

void LoadJLOC(edict_t *ent, char filename[MAX_STR_LEN])
{
	
	FILE *jloc_file;
	char *token="\0";
	char buf2[1000];
	char message[256];
	int lbuf2,color;
	edict_t *DML=NULL;
	vec3_t start,angles;
	char JLOCFile[256];
	
	if ((int)weaponban->value & WB_MARKER) return;

	/*if (gi.argc() != 2)
	{
		gi.cprintf(ent,PRINT_HIGH, "Wrong syntax: jlocload filename\n");
		return;
	}*/
	
	sprintf(JLOCFile,GAMEVERSION "/jloc/%s-%s.jloc",level.mapname,filename);
	
	jloc_file = fopen(JLOCFile, "r");
	if (jloc_file == NULL)
	{
		gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jloc\" was not found!\n",level.mapname,filename);
		return;
	}
	
	jloc_num_lines = 0;
	
	while (fgets(buf2, 900, jloc_file) != NULL)
	{
		double time;
		message[0]='\0';
		lbuf2 = strlen(buf2);
		while (buf2[lbuf2-1] == '\r' || buf2[lbuf2-1] == '\n')
		{
			buf2[lbuf2-1] = 0;
			lbuf2--;
		}
		
		start[0] = start[1] = start[2] = angles[0] = angles[1] = angles[2] = color = 0;
		
		DML = G_Spawn();
		DML->owner = ent;
		
		//Load Coords
		token = strtok( buf2, " " );
		start[0] = atoi(token);
		token = strtok( NULL, " " );
		start[1] = atoi(token);
		token = strtok( NULL, " " );
		start[2] = atoi(token);
		
		VectorCopy(start,DML->s.origin);
		
		//Load Angles
		token = strtok( NULL, " " );
		angles[0] = atoi(token);
		token = strtok( NULL, " " );
		angles[1] = atoi(token);
		angles[2] = 0;
		
		VectorCopy(angles,DML->s.angles);
		
		//Load Color
		token = strtok( NULL, " " );
		color = atoi(token);
		
		Marker_COLORtoFX(DML,color);
		
		//Load message
		while ((token = strtok( NULL, " " )) != NULL)
		{
			strcat(message,token);
			strcat(message," ");
		}
		
		strcpy(DML->mmsg,message);

		time = level.time + (jloc_num_lines/10);
		
		DML->think = DelayMarkerLoad_Think;
		DML->nextthink = time;
		
		gi.linkentity(DML);
		
		jloc_num_lines++;
	}
	
	fclose(jloc_file);
	
	gi.bprintf(PRINT_HIGH,"%s: \"%s\" was loaded successfully! (%i marker",ent->client->pers.netname,JLOCFile,jloc_num_lines);
	if (jloc_num_lines != 1) gi.bprintf(PRINT_HIGH,"s");
	if (jloc_num_lines > 10) gi.bprintf(PRINT_HIGH," (%i in queue))\n",jloc_num_lines-10);
	else					 gi.bprintf(PRINT_HIGH,")\n");
}

