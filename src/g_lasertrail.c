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

//updated
//means that the function is not being worked on and is working properly
//remove "updated" before modifying a function

void SV_AddBlend (float r, float g, float b, float a, float *v_blend);
void LaserLinkOn (edict_t *laser);
void LaserLinkOff (edict_t *laser);
void SV_CalcBlend(edict_t *ent);

int ltnodes = 1;	//laser trail nodes	(global)
int ltrails = 0;	//laser trails		(global)

//updated
qboolean TrailsBanned()
{
	if ((int)weaponban->value & WB_JTRAIL) return true;
	else return false;
}

//updated
void SetNodeCount (int val)
{
	ltnodes = val;
}

//updated
void AddTrailCount (int val)
{
	ltrails += val;
}

//updated
int LaserTrailsInt ()
{
	return ltrails;
}

//updated
int TrailsInt()
{  
	return ltnodes;
}

//updated
edict_t *FindEdictByClassnumAndMass (char *classname, int classnum,int mass)
{
	int i;
	edict_t *it = NULL;

	for (i=0 ; i<globals.num_edicts ; i++)
	{
		it = &g_edicts[i];
		if (!it->classname)
			continue;
		if (!it->classnum)
			continue;
		if (!it->mass)
			continue;
		if (Q_stricmp(it->classname, classname) == 0)
		{
			if (it->mass == mass)
				if (it->classnum == classnum)
					return it;
		}

	}
	return NULL;
}

//updated
void TrailBlendScreen(edict_t *ent)
{

	if (ent->client->resp.toggles & TG_TFLASH)
	{
		int val = ent->client->resp.intblend;

	/*
	1 - Red		 - 0xf0f0f0f0;
	2 - Green	 - 0xd0d0d0d0;
	3 - Blue	 - 0xf3f3f3f3;
	4 - Yellow	 - 0xdddddddd;
	5 - White	 - 0xd7d7d7d7;
	6 - Orange	 - 0xe0e0e0e0;
	7 - Black	 - 0x00000000;
	*/

		if (val == 0xf0f0f0f0)
			SV_AddBlend (.3,0,0,100,ent->client->ps.blend);
		else if (val == 0xd0d0d0d0)
			SV_AddBlend (0,.3,0,100,ent->client->ps.blend);
		else if (val == 0xf3f3f3f3)
			SV_AddBlend (0,0,.3,100,ent->client->ps.blend);
		else if (val == 0xdddddddd)
			SV_AddBlend (.3,.3,0,100,ent->client->ps.blend);
		else if (val == 0xd7d7d7d7)
			SV_AddBlend (.3,.3,.3,100,ent->client->ps.blend);
		else if (val == 0xe0e0e0e0)
			SV_AddBlend (.7,.5,.2,100,ent->client->ps.blend);
		else if (val == 0x01010101)
			SV_AddBlend (0,0,0,100,ent->client->ps.blend);
	}
}

//updated
void LaserLinkThink (edict_t *laser)
{
	edict_t *node = NULL;
	vec3_t  start;
	vec3_t  end;
	trace_t tr;

	if (!laser->enemy)
	{
		node = FindEdictByClassnumAndMass("LaserTrailLink", laser->classnum-1, laser->mass);
		if (node)
			laser->enemy = node;
		else
			laser->enemy = laser;
	}

	VectorCopy (laser->s.origin, start);
	VectorMA (start, 0, laser->movedir, end);

	PRETRACE();
	tr = gi.trace (start, NULL, NULL, laser->enemy->s.origin, NULL /*ignore*/ , /*MASK_SOLID);//*/ MASK_PLAYERSOLID);
	POSTTRACE();

	//HERE FOR TRAIL FLASHES
	if (tr.ent->client)
	{
		tr.ent->client->resp.intblend = laser->s.skinnum;
		tr.ent->client->resp.trailtouch = laser->mass;
	}

	VectorCopy (tr.endpos, laser->s.old_origin);

	laser->nextthink = level.time + FRAMETIME;
}


//updated
void LaserLinkOn (edict_t *laser)
{
        if (!laser->activator)
                laser->activator = laser;
        laser->spawnflags |= 0x80000001;
        laser->svflags &= ~SVF_NOCLIENT;
        LaserLinkThink (laser);
}

//updated
int NUMtoCOLOR (int num)
{
	int color;
	
	/* //these might be wrong, so test them
	0 - Red		 - 0xf0f0f0f0;
	1 - Green	 - 0xd0d0d0d0;
	2 - Blue	 - 0xf3f3f3f3;
	3 - Yellow	 - 0xdddddddd;
	4 - White	 - 0xd7d7d7d7;
	5 - Orange	 - 0xe0e0e0e0;
	6 - Black	 - 0x01010101;
	*/
	
	switch (num)
	{
	case 0:
		color = 0xf0f0f0f0;
		break;
	case 1:
		color = 0xd0d0d0d0;
		break;
	case 2:
		color = 0xf3f3f3f3;
		break;
	case 3:
		color = 0xdddddddd;
		break;
	case 4:
		color = 0xd7d7d7d7;
		break;
	case 5:
		color = 0xe0e0e0e0;
		break;
	case 6:
		color = 0x01010101;
		break;
	default:
		color = 0x01010101;
		break;
	}
	
	return color;
}

//updated
char* NUMtoTCOLORWORD (int num)
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
		return "orange\0";
	case 6:
		return "black\0";
	default:
		return NULL;
	}
}

int TCOLORWORDtoNUM2(char *color)
{
	int val;
	if (Q_stricmp(color,"green") == 0)			val = 1;
	else if (Q_stricmp(color,"blue") == 0)		val = 2;
	else if (Q_stricmp(color,"yellow") == 0)	val = 3;
	else if (Q_stricmp(color,"white") == 0)		val = 4;
	else if (Q_stricmp(color,"orange") == 0)	val = 5;
	else if (Q_stricmp(color,"black") == 0)		val = 6;
	else if (Q_stricmp(color,"red") == 0)		val = 0;
	else										val = 99;
	return val;
}

//updated
void TCOLORWORDtoNUM (edict_t *ent)
{
	int val;
	char *color;
	qboolean change=false;

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH, "\"trailcolor\" is \"%s\"\n",NUMtoTCOLORWORD(ent->client->resp.trail_color));
		return;
	}

	if (gi.argc() > 2)
		color = strdup(strtok(gi.args()," "));

	if (Q_stricmp(gi.args(),"change") == 0)
	{
		color = strdup(strtok(NULL," "));
		change = true;
	}
	else
		color = strdup(strtok(gi.args()," "));

	if (Q_stricmp(color,"green") == 0)			val = 1;
	else if (Q_stricmp(color,"blue") == 0)		val = 2;
	else if (Q_stricmp(color,"yellow") == 0)	val = 3;
	else if (Q_stricmp(color,"white") == 0)		val = 4;
	else if (Q_stricmp(color,"orange") == 0)	val = 5;
	else if (Q_stricmp(color,"black") == 0)		val = 6;
	else if (Q_stricmp(color,"red") == 0)		val = 0;
	else										val = 99;
	
	if (val == 99)
	{
		gi.cprintf(ent,PRINT_HIGH,"Unknown color specified: \"%s\"; Trail colors are: green, blue, yellow, white, orange, black, and red\n",color);	
		return;
	}

	if (!change)
		ent->client->resp.trail_color = val;
	else
		ChangeTrailColor(ent,val);
}

//updated
void Cmd_ChangeTrailColor (int mass,int color)
{

	edict_t *jtrail = NULL;
	int i;
	
	if (TrailsBanned()) return;
	
	for (i=1;i<(int)traillimit->value+1;i++)
	{
		jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,mass);
		if (jtrail)
			jtrail->s.skinnum = NUMtoCOLOR(color);
	}

}

//updated
void PlaceLaserLink (edict_t *ent)
{
        edict_t *laser = NULL;
		
		laser = G_Spawn();
		
		laser->mass = ent->client->resp.trail_cur;//ltrails;
		laser->classname = "LaserTrailLink";
		laser->classnum = ent->client->resp.trailcounter;//ltnodes;
		laser->movetype = MOVETYPE_NONE;
        laser->solid = SOLID_NOT;
        laser->s.renderfx |= RF_BEAM|RF_TRANSLUCENT|RF_FULLBRIGHT;
        laser->s.modelindex = 1;
		
		VectorCopy(ent->s.origin,laser->s.origin);
		if (ent->maxs[2] == 16)
			laser->s.origin[2] -= 8;

        // set the beam diameter
		if (!ent->client->resp.trail_width) ent->client->resp.trail_width = 4;
        laser->s.frame = ent->client->resp.trail_width;

        // set the color
        laser->s.skinnum = NUMtoCOLOR(ent->client->resp.trail_color);

        laser->think = LaserLinkThink;

        VectorSet (laser->mins, -8, -8, -8);
        VectorSet (laser->maxs, 8, 8, 8);
        gi.linkentity (laser);

		LaserLinkOn (laser);
}

//updated
void PlaceLaserNode(edict_t *ent)
{
	if (TrailsBanned()) return;
	if (ltnodes > traillimit->value) return;
	PlaceLaserLink(ent);
	ent->client->resp.trailcounter++;
	ltnodes++;
}

//updated
void DelayLaserLinkOn (edict_t *laser)
{
	laser->movetype = MOVETYPE_NONE;
	laser->solid = SOLID_NOT;
	laser->s.renderfx |= RF_BEAM|RF_TRANSLUCENT|RF_FULLBRIGHT;
	laser->s.modelindex = 1;
	LaserLinkOn (laser);
}

int jtrail_num_lines;

void DeleteJTRAIL(edict_t *ent,char filename[MAX_STR_LEN],char *ren)
{
	FILE *jtrail_file;
	char JTRAILFile[MAX_STR_LEN];

	sprintf(JTRAILFile, GAMEVERSION "/jtrail/%s-%s.jtrail",level.mapname,filename);
	
	jtrail_file = fopen(JTRAILFile, "r");
	if (jtrail_file)
	{
		fclose(jtrail_file);

		if (ren != NULL)
		{
			char JTF_ren[MAX_STR_LEN];
			sprintf(JTF_ren, GAMEVERSION "/jtrail/%s-%s.jtrail",level.mapname,ren);

			jtrail_file = fopen(JTF_ren,"r");
			if (jtrail_file == NULL)
			{
				rename(JTRAILFile,JTF_ren);
				gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jtrail\" was renamed to \"%s-%s.jtrail\"\n",
					level.mapname,filename,level.mapname,ren);
			}
			else
			{
				fclose(jtrail_file);
				gi.cprintf(ent,PRINT_HIGH,"Destination file exists. Can not rename!\n");
			}
		}
		else
		{
			remove(JTRAILFile);
			gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jtrail\" was deleted\n",level.mapname,filename);
		}
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jtrail\" was not found!\n",level.mapname,filename);
}

void LoadJTRAIL(edict_t *ent,char filename[MAX_STR_LEN],qboolean pdm,int clean,int colorp,int sizep)
{
	
	FILE *jtrail_file;
	char *token="\0";
	char buf2[1000];
	char message[256];
	int lbuf2,color,mass;
	edict_t *laser = NULL;
	vec3_t start;
	int lasttrailnum=0;
	char JTRAILFile[MAX_STR_LEN];
	int delcount=0;
	char tok[2]="";
	int clean_count=0;
	int total_good_lines=0;
	
	if (TrailsBanned()) return;

	if (pdm)
	{
		if (!Q_stricmp(filename,"lastdemo"))
			sprintf(JTRAILFile, GAMEVERSION "/puppetdemo/%i.pdm",ent->client->resp.id);
		else
			sprintf(JTRAILFile, GAMEVERSION "/puppetdemo/%s-%s.pdm",level.mapname,filename);
	}
	else
		sprintf(JTRAILFile, GAMEVERSION "/jtrail/%s-%s.jtrail",level.mapname,filename);

	jtrail_file = fopen(JTRAILFile, "r");
	if (jtrail_file == NULL)
	{
		if (pdm)
		{
			if (!Q_stricmp(filename,"lastdemo"))
				gi.cprintf(ent,PRINT_HIGH,"\"%i.pdm\" was not found!\n",ent->client->resp.id);
			else
				gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.pdm\" was not found!\n",level.mapname,filename);
		}
		else
			gi.cprintf(ent,PRINT_HIGH,"\"%s-%s.jtrail\" was not found!\n",level.mapname,filename);
		return;
	}
	
    if (pdm)
		tok[0] = ',';	//puppet demos parsed with ','
	else
		tok[0] = ' ';	//trail files parsed with ' '
	tok[1] = '\0';

	//grab the total good lines, for new clean-on-load usage
	while (fgets(buf2, 900, jtrail_file) != NULL)
	{
		if (!(((buf2[0] > '0') && (buf2[0] < '9')) || (buf2[0] == '-')))
			continue;
		total_good_lines++;
	}
	fclose(jtrail_file);

	//reopen the file to start reading again... we know it exists so no check on that
	jtrail_file = fopen(JTRAILFile, "r");
	jtrail_num_lines = 0;
	ltrails++;
	while (fgets(buf2, 900, jtrail_file) != NULL)
	{
		double time;

		if (!(((buf2[0] > '0') && (buf2[0] < '9')) || (buf2[0] == '-')))
			continue;

		if (clean_count != clean)
		{
			if ((jtrail_num_lines != 0) && (jtrail_num_lines != total_good_lines))
			{
				clean_count++;
				continue;
			}
			clean_count++;
		}
		else
			clean_count = 0;

		message[0]='\0';
		lbuf2 = strlen(buf2);
		while (buf2[lbuf2-1] == '\r' || buf2[lbuf2-1] == '\n')
		{
			buf2[lbuf2-1] = 0;
			lbuf2--;
		}
		
		start[0] = start[1] = start[2] = color = mass = 0;

		if (ltnodes <= traillimit->value)
		{
			laser = G_Spawn();
			//		laser->owner = ent;

			ltnodes++;
			laser->classnum = jtrail_num_lines+1;//ltnodes;
			laser->classname = "LaserTrailLink";
			laser->s.frame = 4;
			VectorSet (laser->mins, -8, -8, -8);
			VectorSet (laser->maxs, 8, 8, 8);
			
			//Load Coords
			token = strtok( buf2, tok );
			start[0] = atoi(token);
			token = strtok( NULL, tok );
			if ((token == NULL) && pdm)
				break;
			start[1] = atoi(token);
			token = strtok( NULL, tok );
			start[2] = atoi(token);
			VectorCopy(start,laser->s.origin);

			if (pdm)
			{
				if (colorp == -1)
					laser->s.skinnum = NUMtoCOLOR(1); //color (green)
				else
					laser->s.skinnum = NUMtoCOLOR(colorp);

				if (sizep == -1)
					laser->s.frame = 4; //size
				else
					laser->s.frame = sizep;
				laser->mass = ltrails; //num (never gonna be multiple paths to load this way)
			}
			else
			{
				//Load Color
				token = strtok( NULL, tok );
				color = atoi(token);
				laser->s.skinnum = color;
				
				//Load Size
				token = strtok( NULL, tok );
				color = atoi(token);
				laser->s.frame = color;
				
				//Load Gravity
				token = strtok( NULL, tok );
				color = atoi(token);
				
				if (lasttrailnum != color)
				{
					ltrails++;
					lasttrailnum = color;
				}
				
				laser->mass = color+ltrails;
			}
			
			time = level.time + (jtrail_num_lines/10);
			
			laser->think = DelayLaserLinkOn;//MODIFY ME!!!WHY?!!//DelayMarkerLoad_Think;
			laser->nextthink = time;
			
			gi.linkentity(laser);
		}
		else
		{
			delcount++;
		}
		
		jtrail_num_lines++;
	}

	fclose(jtrail_file);
	
	if (delcount !=0)
	{
		if (delcount == jtrail_num_lines)
			gi.bprintf(PRINT_HIGH,"%s: \"%s\" could not be loaded due to traillimit hit!\n",ent->client->pers.netname,JTRAILFile);
		else
		{
			gi.bprintf(PRINT_HIGH,"%s: \"%s\" was partially loaded due to traillimit hit! (%i/%i link",ent->client->pers.netname,JTRAILFile,jtrail_num_lines-delcount,jtrail_num_lines);
			if (jtrail_num_lines != 1)
				gi.bprintf(PRINT_HIGH,"s");
			if (jtrail_num_lines-delcount > 10)
				gi.bprintf(PRINT_HIGH," (%i in queue))\n",jtrail_num_lines-delcount-10);
			else
				gi.bprintf(PRINT_HIGH,")\n");
		}
	}
	else
	{
		gi.bprintf(PRINT_HIGH,"%s: \"%s\" was loaded successfully! (%i link",ent->client->pers.netname,JTRAILFile,jtrail_num_lines);
		if (jtrail_num_lines != 1)
			gi.bprintf(PRINT_HIGH,"s");
		if (jtrail_num_lines > 10)
			gi.bprintf(PRINT_HIGH," (%i in queue))\n",jtrail_num_lines-10);
		else
			gi.bprintf(PRINT_HIGH,")\n");
	}
}

//updated
void SaveJTRAIL_all(edict_t *ent,char *fn)
{

	FILE *jtrail_file;
	edict_t *jtrail = NULL;
	int i,j, savecount=0;
	int lastmass=0, curmass=0;

	if (TrailsBanned()) return;

	jtrail_file = fopen(fn, "r");
	if (jtrail_file != NULL)
	{
		fclose(jtrail_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",fn);
		return;
	}

	jtrail_file = fopen(fn, "w");

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (jtrail->mass-1 != lastmass)
				{
					lastmass = jtrail->mass-1;
					if (savecount != 0)
						curmass++;
				}
				fprintf(jtrail_file,"%f %f %f %i %i %i\n",
					jtrail->s.origin[0],jtrail->s.origin[1],jtrail->s.origin[2],
					jtrail->s.skinnum,jtrail->s.frame,curmass);
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
		strcat(msg," trail link");
		if (savecount != 1) strcat(msg,"s ");
		else strcat(msg," ");
		strcat(msg,"to ");
		strcat(msg,fn);

		gi.bprintf(PRINT_HIGH,"%s: %s\n",ent->client->pers.netname,msg);
		*/

		gi.bprintf(PRINT_HIGH,"%s: Trail file saved to %s (%i link%s)\n",ent->client->pers.netname,fn,savecount,(savecount != 1) ? "s" : "");
	}
	
	fclose(jtrail_file);

}

//updated
void SaveJTRAIL_view(edict_t *ent,char *fn)
{

	FILE *jtrail_file;
	edict_t *jtrail = NULL;
	int i,j, savecount=0;
	int lastmass=0, curmass=0;

	if (TrailsBanned()) return;

	jtrail_file = fopen(fn, "r");
	if (jtrail_file != NULL)
	{
		fclose(jtrail_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",fn);
		return;
	}
	
	jtrail_file = fopen(fn, "w");


	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				
				vec3_t forward,dir;
				float d;
				
				
				AngleVectors(ent->client->v_angle, forward, NULL, NULL);
				VectorSubtract(jtrail->s.origin, ent->s.origin, dir);
				VectorNormalize(dir);
				d = DotProduct(forward, dir);
				
				if (d > 0.7)
				{
					if (jtrail->mass-1 != lastmass)
					{
						lastmass = jtrail->mass-1;
						if (savecount != 0)
							curmass++;
					}
					fprintf(jtrail_file,"%f %f %f %i %i %i\n",
						jtrail->s.origin[0],jtrail->s.origin[1],jtrail->s.origin[2],
						jtrail->s.skinnum,jtrail->s.frame,curmass);
					savecount++;
				}
				
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
		strcat(msg," trail link");
		if (savecount != 1) strcat(msg,"s ");
		else strcat(msg," ");
		strcat(msg,"to ");
		strcat(msg,fn);

		gi.bprintf(PRINT_HIGH,"%s: %s\n",ent->client->pers.netname,msg);
		*/

		gi.bprintf(PRINT_HIGH,"%s: Trail file saved to %s (%i link%s)\n",ent->client->pers.netname,fn,savecount,(savecount != 1) ? "s" : "");
	}
	
	fclose(jtrail_file);

}

//updated
void SaveJTRAIL_last(edict_t *ent,char *fn)
{

	FILE *jtrail_file;
	edict_t *jtrail = NULL;
	int i,j, savecount=0;

	if (TrailsBanned()) return;

	jtrail_file = fopen(fn, "r");
	if (jtrail_file != NULL)
	{
		fclose(jtrail_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",fn);
		return;
	}

	jtrail_file = fopen(fn, "w");

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (jtrail->mass == ent->client->resp.trail_cur)
				{
					fprintf(jtrail_file,"%f %f %f %i %i %i\n",
						jtrail->s.origin[0],jtrail->s.origin[1],jtrail->s.origin[2],
						jtrail->s.skinnum,jtrail->s.frame,0);//it will always only be 1 trail so make mass 0
					savecount++;
				}
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
		strcat(msg," trail link");
		if (savecount != 1) strcat(msg,"s ");
		else strcat(msg," ");
		strcat(msg,"to ");
		strcat(msg,fn);

		gi.bprintf(PRINT_HIGH,"%s: %s\n",ent->client->pers.netname,msg);
		*/

		gi.bprintf(PRINT_HIGH,"%s: Trail file saved to %s (%i link%s)\n",ent->client->pers.netname,fn,savecount,(savecount != 1) ? "s" : "");
	}
	
	fclose(jtrail_file);

}

//updated
void SaveJTRAIL(edict_t *ent, int cmd)
{
	char *opt;
	char JTRAILFile[256];
	char path[256];
	
	if (TrailsBanned()) return;

	if (ltnodes < 1)
	{
		gi.cprintf(ent,PRINT_HIGH,"No jump trails to save!\n");
		return;
	}

	if (gi.argc() != 3)
	{
		if (cmd == 1)
			gi.cprintf(ent,PRINT_HIGH, "Wrong syntax: jtrailsave <all/view/last> <filename>\n");
		else
			gi.cprintf(ent,PRINT_HIGH, "Wrong syntax: tsave <all/view/last> <filename>\n");
		return;
	}

	if (BadFileName(gi.args()))
	{
		gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in filename. JTrail not saved.\n");
		return;
	}

	opt = strtok(gi.args()," ");

	sprintf (path, GAMEVERSION "/jtrail");
	p_mkdir(path);
	sprintf (JTRAILFile, GAMEVERSION "/jtrail/%s-%s.jtrail", level.mapname, strtok(NULL," "));

	if (!Q_stricmp(opt,"all")) SaveJTRAIL_all(ent,JTRAILFile);
	else if (!Q_stricmp(opt,"view")) SaveJTRAIL_view(ent,JTRAILFile);
	else if (!Q_stricmp(opt,"last")) SaveJTRAIL_last(ent,JTRAILFile);
	else gi.cprintf(ent,PRINT_HIGH, "Unknown save option \"%s\". Valid options are \"view\", \"all\", and \"last\"\n",opt);
}

//updated
edict_t* GetBestNode (edict_t *ent)
{
    vec3_t  forward, dir;
	float highest=0.9,d;
	int i,j;
	edict_t *ltnode = NULL;
	edict_t *bestltnode = NULL;
	int nodes = ltnodes;
	
	if (TrailsBanned()) return NULL;
	
	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			ltnode = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			if (ltnode)
			{
				
				if (loc_CanSee(ent,ltnode))
				{
					
					AngleVectors(ent->client->v_angle, forward, NULL, NULL);
					VectorSubtract(ltnode->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);

					if (d > 0.9)
						if (d > highest)
						{
							highest = d;
							bestltnode = ltnode;
						}
				}
			}
		}
	}
	return bestltnode;
}

//updated
int GetBestTrailMassByView (edict_t *ent)
{
	edict_t *node = NULL;
	node = GetBestNode(ent);
	if (node)
		return node->mass;
	else
		return -1;
}

//updated
void Cmd_ViewTrailNodeInfo (edict_t *ent)
{
    vec3_t  forward, dir;
	float highest=0.9,d;
	int i,j;
	edict_t *ltnode = NULL;
	edict_t *bestltnode = NULL;
	int nodes = ltnodes;
	
	//dont want this now but it works perfectly
	return;

	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			//	gi.bprintf(PRINT_HIGH,"Checking: j:%i i:%i\n",j,i);
			ltnode = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (ltnode)
				if (loc_CanSee(ent,ltnode))
				{
					AngleVectors(ent->client->v_angle, forward, NULL, NULL);
					VectorSubtract(ltnode->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);
					
					if (d > 0.9)
						if (d > highest)
						{
							highest = d;
							bestltnode = ltnode;
						}
				}
		}
	}
//the above loops should be good for the rest of the needed functions

	//NOTICE THAT THIS IS A BPRINTF BELOW!
	if ((bestltnode != NULL) && (Q_stricmp(bestltnode->classname,"LaserTrailLink")==0))
		gi.bprintf(PRINT_HIGH,"Classname:%s mass:%i classnum:%i\n",bestltnode->classname,bestltnode->mass,bestltnode->classnum);
}

//updated
void RelinkAllLaserTrails()
{
	edict_t *jtrail = NULL;
	int i,j;
	
	for (j=1;j<ltrails+1;j++)
	{
		int mod=0;
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			if (jtrail)
			{
				jtrail->enemy = NULL;
				jtrail->classnum = i-mod;
			}
			else mod++;
		}
	}
}

//updated
void RelinkALaserTrail(int j)
{
	edict_t *jtrail = NULL;
	int i;
	
	int mod=0;
	for (i=1;i<(int)traillimit->value+1;i++)
	{
		jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
		if (jtrail)
		{
			jtrail->enemy = NULL;
			jtrail->classnum = i-mod;
		}
		else mod++;
	}
}

//updated
int CountLaserTrailLinks_By_Mass(int mass)
{
	//SHOULD ONLY BE CALLED AFTER A RELINK
	edict_t *link = NULL;
	int i,mod1=1,mod2=10;
	for(i=0;i<2;i++)
	{
		do
		{
			mod1 += mod2;
		} while (link = FindEdictByClassnumAndMass("LaserTrailLink", mod1,mass));
		mod1 -= mod2;
		mod2 = 1;
	}
	return mod1;
}

//updated
void DeleteLaserTrail_LinksInView(edict_t *ent)
{  
	
	edict_t *jtrail = NULL;
	int i,j, remcount=0;
	vec3_t forward,dir;
	float d;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (loc_CanSee(ent,jtrail))
				{
					
					AngleVectors(ent->client->v_angle, forward, NULL, NULL);
					VectorSubtract(jtrail->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);
					
					if (d > 0.7)
					{
						if (ent->client->resp.trail_cur == jtrail->mass)
							ent->client->resp.trailcounter--;
						G_FreeEdict(jtrail);
						remcount++;
					}
				}
			}
		}
	}

	RelinkAllLaserTrails();
	ltnodes -= remcount;
	if (remcount>0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Delete by Viewed Links: Removed %i link",remcount);
		if (remcount != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void CleanLaserTrail_LinksInView(edict_t *ent)
{  
	
	edict_t *jtrail = NULL,*nnode = NULL;
	int i,j, remcount=0,count,mass;
	vec3_t forward,dir;
	float d;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				count = CountLaserTrailLinks_By_Mass(jtrail->mass);
				if ((count > 2) && (jtrail->classnum != count-1))
				{
					nnode = FindEdictByClassnumAndMass("LaserTrailLink", i+1,j);
					if (nnode)
						if (loc_CanSee(ent,nnode))
						{
							
							AngleVectors(ent->client->v_angle, forward, NULL, NULL);
							VectorSubtract(nnode->s.origin, ent->s.origin, dir);
							VectorNormalize(dir);
							d = DotProduct(forward, dir);
							
							if (d > 0.7)
							{
								if (ent->client->resp.trail_cur == nnode->mass)
									ent->client->resp.trailcounter--;
								G_FreeEdict(nnode);
								mass = nnode->mass;
								RelinkALaserTrail(mass);
//								RelinkAllLaserTrails();
								ltnodes--;
								remcount++;
							}
						}
				}
			}
		}
	}

	if (remcount>0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Cleaning by Viewed Links: Removed %i link",remcount);
		if (remcount != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void DeleteLaserTrail_All(edict_t *ent)
{
	edict_t *jtrail = NULL;
	int i,j,removed=0;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (ent->client->resp.trail_cur == jtrail->mass)
					ent->client->resp.trailcounter--;
				G_FreeEdict(jtrail);
				ltnodes--;
				removed++;
			}
		}
	}

	if (removed > 0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Delete by All: Removed %i link",removed);
		if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void DeleteLaserTrail_By_Mass(edict_t *ent,int mass,char *message)
{
	edict_t *jtrail = NULL;
	int i,removed=0;
	
	if (TrailsBanned()) return;
	
	for (i=1;i<(int)traillimit->value+1;i++)
	{
		jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,mass);
		
		if (jtrail)
		{
			if (ent->client->resp.trail_cur == mass)
				ent->client->resp.trailcounter--;
			G_FreeEdict(jtrail);
			ltnodes--;
			removed++;
		}
	}
	if (Q_stricmp(message,""))
		if (removed > 0)
		{
			gi.cprintf(ent,PRINT_HIGH,"Jump Trail Delete by %s: Removed %i link",message,removed);
			if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
			else gi.cprintf(ent,PRINT_HIGH,"\n");
		}
}

//updated
void DeleteLaserTrail_Viewed(edict_t *ent)
{  
	edict_t *jtrail = NULL;
	int i,j, remcount=0;
	vec3_t forward,dir;
	float d;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (loc_CanSee(ent,jtrail))
				{
					
					AngleVectors(ent->client->v_angle, forward, NULL, NULL);
					VectorSubtract(jtrail->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);
					
					if (d > 0.7)
					{
						DeleteLaserTrail_By_Mass(ent,jtrail->mass,"");
						remcount++;
						i=traillimit->value;
					}
				}
			}
		}
	}

	if (remcount>0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Delete by Viewed Trails: Removed %i trail",remcount);
		if (remcount != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void CleanLaserTrail_By_Mass(edict_t *ent,int mass,char *message)
{
	edict_t *cnode = NULL,*nnode = NULL;
	int i,mod=0,removed=0;
	
	if (TrailsBanned()) return;
	
	for (i=2;i<(int)traillimit->value+1;i++)
	{
		cnode = FindEdictByClassnumAndMass("LaserTrailLink", i,mass);
		
		if (cnode)
		{
			nnode = FindEdictByClassnumAndMass ("LaserTrailLink", cnode->classnum+1,mass);
			if (nnode)
			{
				nnode->classnum = cnode->classnum;
				if (ent->client->resp.trail_cur == nnode->mass)
					ent->client->resp.trailcounter--;
				G_FreeEdict(cnode);
				ltnodes--;
				RelinkALaserTrail(mass);
//				RelinkAllLaserTrails();
				removed++;
			}
		}
	}
	if (Q_stricmp(message,""))
		if (removed > 0)
		{
			gi.cprintf(ent,PRINT_HIGH,"Jump Trail Cleaning by %s: Removed %i link",message,removed);
			if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
			else gi.cprintf(ent,PRINT_HIGH,"\n");
		}
}

//updated
void CleanLaserTrail_Viewed(edict_t *ent)
{
	edict_t *jtrail = NULL;
	int i,j, remcount=0;
	vec3_t forward,dir;
	float d;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (loc_CanSee(ent,jtrail))
				{
					
					AngleVectors(ent->client->v_angle, forward, NULL, NULL);
					VectorSubtract(jtrail->s.origin, ent->s.origin, dir);
					VectorNormalize(dir);
					d = DotProduct(forward, dir);
					
					if (d > 0.7)
					{
						if (CountLaserTrailLinks_By_Mass(jtrail->mass) > 2)
						{
							CleanLaserTrail_By_Mass(ent,jtrail->mass,"");
							remcount++;
						}
						i=traillimit->value;
					}
				}
			}
		}
	}

	if (remcount > 0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Cleaning by Viewed Trails: Cleaned %i trail",remcount);
		if (remcount != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void CleanLaserTrail_All(edict_t *ent)
{
	edict_t *cnode = NULL,*nnode = NULL;
	int i,j,mod=0,removed=0;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=2;i<(int)traillimit->value+1;i++)
		{
			cnode = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (cnode)
			{
				nnode = FindEdictByClassnumAndMass ("LaserTrailLink", cnode->classnum+1,cnode->mass);
				if (nnode)
				{
					nnode->classnum = cnode->classnum;
					if (ent->client->resp.trail_cur == nnode->mass)
						ent->client->resp.trailcounter--;
					G_FreeEdict(cnode);
					ltnodes--;
					RelinkALaserTrail(nnode->mass);
					//RelinkAllLaserTrails();
					removed++;
				}
			}
		}
	}

	if (removed > 0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Cleaning by All: Removed %i link",removed);
		if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void CleanLaserTrail_Last(edict_t *ent)
{
	edict_t *cnode = NULL,*nnode = NULL;
	int i,j,mod=0,removed=0;
	
	if (TrailsBanned()) return;
	
	for (j=1;j<ltrails+1;j++)
	{
		for (i=2;i<(int)traillimit->value+1;i++)
		{
			cnode = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (cnode)
			{
				nnode = FindEdictByClassnumAndMass ("LaserTrailLink", cnode->classnum+1,cnode->mass);
				if (nnode)
				{
					if (nnode->mass == ent->client->resp.trail_cur)
					{
						nnode->classnum = cnode->classnum;
						if (ent->client->resp.trail_cur == nnode->mass)
							ent->client->resp.trailcounter--;
						G_FreeEdict(cnode);
						ltnodes--;
						RelinkALaserTrail(nnode->mass);
						//RelinkAllLaserTrails();
						removed++;
					}
				}
			}
		}
	}
	
	if (removed > 0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Cleaning by Last: Removed %i link",removed);
		if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void DeleteLaserTrail_Last(edict_t *ent)
{
	edict_t *jtrail = NULL;
	int i,j,removed=0;
	
	if (TrailsBanned()) return;

	for (j=1;j<ltrails+1;j++)
	{
		for (i=1;i<(int)traillimit->value+1;i++)
		{
			jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
			
			if (jtrail)
			{
				if (jtrail->mass == ent->client->resp.trail_cur)
				{
					if (ent->client->resp.trail_cur == jtrail->mass)
						ent->client->resp.trailcounter--;
					G_FreeEdict(jtrail);
					ltnodes--;
					removed++;
				}
			}
		}
	}

	if (removed > 0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Jump Trail Delete by Last: Removed %i link",removed);
		if (removed != 1) gi.cprintf(ent,PRINT_HIGH,"s\n");
		else gi.cprintf(ent,PRINT_HIGH,"\n");
	}
}

//updated
void Goto_LaserTrail_Start(edict_t *ent)
{
	edict_t *jtrail = NULL;
	int j;
	
	if (TrailsBanned()) return;

	j = GetBestTrailMassByView(ent);

	if (j == -1)
	{
		gi.cprintf(ent,PRINT_HIGH,"No trail in view\n");
		return;
	}

	jtrail = FindEdictByClassnumAndMass("LaserTrailLink", 1,j);
	if (jtrail)
	{
		TeleFX(ent);
		VectorCopy(jtrail->s.origin,ent->s.origin);
		ent->s.event = EV_PLAYER_TELEPORT;
	}
}

//updated
void Goto_LaserTrail_End(edict_t *ent)
{
	edict_t *jtrail = NULL;
	int j,i=0;
	
	if (TrailsBanned()) return;
	
	j = GetBestTrailMassByView(ent);

	if (j == -1)
	{
		gi.cprintf(ent,PRINT_HIGH,"No trail in view\n");
		return;
	}

	do
	{
		i++;
		jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i,j);
	} while (jtrail);
	jtrail = FindEdictByClassnumAndMass("LaserTrailLink", i-1,j);

	if (jtrail)
	{
		TeleFX(ent);
		VectorCopy(jtrail->s.origin,ent->s.origin);
		ent->s.event = EV_PLAYER_TELEPORT;
	}
}

//updated
void ChangeTrailColor(edict_t *ent,int color)
{
	int mass=0;
	
	if (TrailsBanned()) return;
	mass = GetBestTrailMassByView(ent);
	if (mass != -1)
		Cmd_ChangeTrailColor(mass,color);
}

