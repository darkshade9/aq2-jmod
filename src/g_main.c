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

game_locals_t   game;
level_locals_t  level;
game_import_t   gi;
game_export_t   globals;
spawn_temp_t    st;

int     sm_meat_index;
int     snd_fry;
int meansOfDeath;
// zucc for location
int locOfDeath;
int stopAP;

edict_t         *g_edicts;

//FIREBLADE
cvar_t  *hostname;
//PaTMaN-REMOVE_TEAMPLAY cvar_t  *teamplay;
//NORADIO cvar_t  *radiolog;
cvar_t  *motd_time;
cvar_t  *actionmaps;
//cvar_t  *roundtimelimit; //V1.21 REMOVED
//cvar_t	*maxteamkills; //V1.21 REMOVED
//cvar_t	*twbanrounds; //V1.21 REMOVED
//cvar_t	*tkbanrounds; //V1.21 REMOVED
//cvar_t  *limchasecam;
//cvar_t  *roundlimit; //V1.21 REMOVED
cvar_t  *skipmotd;
cvar_t  *nohud;
//cvar_t  *noscore; //V1.21 REMOVED
cvar_t  *actionversion;
//PaTMaN - jmodversion
cvar_t  *jmodversion;
cvar_t	*needpass;
//FIREBLADE
cvar_t  *deathmatch;
cvar_t  *coop;
cvar_t  *dmflags;
//PaTMaN - weaponban
cvar_t  *weaponban;
cvar_t  *skill;
//cvar_t  *fraglimit; //V1.21 REMOVED
cvar_t  *timelimit;
cvar_t  *password;
cvar_t  *maxclients;
cvar_t  *maxentities;
cvar_t  *g_select_empty;
cvar_t  *dedicated;

cvar_t  *filterban;

cvar_t  *sv_maxvelocity;
cvar_t  *sv_gravity;

cvar_t  *sv_rollspeed;
cvar_t  *sv_rollangle;
cvar_t  *gun_x;
cvar_t  *gun_y;
cvar_t  *gun_z;

cvar_t  *run_pitch;
cvar_t  *run_roll;
cvar_t  *bob_up;
cvar_t  *bob_pitch;
cvar_t  *bob_roll;

cvar_t  *sv_cheats;

cvar_t  *flood_msgs;
cvar_t  *flood_persecond;
cvar_t  *flood_waitdelay;

//zucc server variables
//cvar_t  *unique_weapons; //V1.21 REMOVED
//cvar_t  *unique_items; //V1.21 REMOVED
cvar_t  *ir;
cvar_t  *knifelimit;
cvar_t  *markerlimit;
cvar_t  *traillimit;
//cvar_t  *tgren;
//cvar_t  *allweapon; //V1.21 REMOVED
//cvar_t  *allitem; //V1.21 REMOVED

//zucc from action
cvar_t  *sv_shelloff;
cvar_t  *bholelimit;
cvar_t  *splatlimit;

//JMod puppet demo
cvar_t	*maxpuppetdemolength;
cvar_t	*puppetdemosmoothing;

//JMod map voting
cvar_t	*mapvoting;
cvar_t	*mapvotingtime;
cvar_t	*mapvotingpercent;
//cvar_t	*killvote;

cvar_t	*notelefx;

//KBJump - PaTMaN
//KBJump-UnComment//cvar_t	*kbjump;

void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void ClientThink (edict_t *ent, usercmd_t *cmd);
qboolean ClientConnect (edict_t *ent, char *userinfo);
void ClientUserinfoChanged (edict_t *ent, char *userinfo);
void ClientDisconnect (edict_t *ent);
void ClientBegin (edict_t *ent);
void ClientCommand (edict_t *ent);
void CheckNeedPass (void);
void RunEntity (edict_t *ent);
void WriteGame (char *filename, qboolean autosave);
void ReadGame (char *filename);
void WriteLevel (char *filename);
void ReadLevel (char *filename);
void InitGame (void);
void G_RunFrame (void);


//===================================================================


void ShutdownGame (void)
{
        gi.dprintf ("==== ShutdownGame ====\n");

//        Ini_FreeIniFile(&ini_file); //PaTMaN-INI - Free up .ini memory

        gi.FreeTags (TAG_LEVEL);
        gi.FreeTags (TAG_GAME);
}


/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/
game_export_t *GetGameAPI (game_import_t *import)
{
        gi = *import;

        globals.apiversion = GAME_API_VERSION;
        globals.Init = InitGame;
        globals.Shutdown = ShutdownGame;
        globals.SpawnEntities = SpawnEntities;

        globals.WriteGame = WriteGame;
        globals.ReadGame = ReadGame;
        globals.WriteLevel = WriteLevel;
        globals.ReadLevel = ReadLevel;

        globals.ClientThink = ClientThink;
        globals.ClientConnect = ClientConnect;
        globals.ClientUserinfoChanged = ClientUserinfoChanged;
        globals.ClientDisconnect = ClientDisconnect;
        globals.ClientBegin = ClientBegin;
        globals.ClientCommand = ClientCommand;

        globals.RunFrame = G_RunFrame;

        globals.ServerCommand = ServerCommand;

        globals.edict_size = sizeof(edict_t);

        return &globals;
}

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error (char *error, ...)
{
        va_list         argptr;
        char            text[1024];

        va_start (argptr, error);
        vsprintf (text, error, argptr);
        va_end (argptr);

        gi.error (ERR_FATAL, "%s", text);
}

void Com_Printf (char *msg, ...)
{
        va_list         argptr;
        char            text[1024];

        va_start (argptr, msg);
        vsprintf (text, msg, argptr);
        va_end (argptr);

        gi.dprintf ("%s", text);
}

#endif

//======================================================================


/*
=================
ClientEndServerFrames
=================
*/
void ClientEndServerFrames (void)
{
        int             i;
        edict_t *ent;

        // calc the player views now that all pushing
        // and damage has been added
        for (i=0 ; i<maxclients->value ; i++)
        {
                ent = g_edicts + 1 + i;
                if (!ent->inuse || !ent->client)
                        continue;
                ClientEndServerFrame (ent);
        }

}

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/

//AZEROV
extern void UnBan_TeamKillers (void);
//AZEROV

void EndDMLevel (void)
{
        edict_t *ent;
        char *nextmapname = NULL;

        // stay on same level flag
        if ((int)dmflags->value & DF_SAME_LEVEL)
        {
                ent = G_Spawn ();
                ent->classname = "target_changelevel";
                nextmapname = ent->map = level.mapname;
        }
//FIREBLADE
                else if (!actionmaps->value || num_maps < 1 && !level.nextmap)
//FIREBLADE
        {
                if (level.nextmap[0])
                {       // go to a specific map
                        ent = G_Spawn ();
                        ent->classname = "target_changelevel";
                        nextmapname = ent->map = level.nextmap;
                }
                else
                {       // search for a changelevel
                        ent = G_Find (NULL, FOFS(classname), "target_changelevel");
                        if (!ent)
                        {       // the map designer didn't include a changelevel,
                                // so create a fake ent that goes back to the same level
                                ent = G_Spawn ();
                                ent->classname = "target_changelevel";
                                nextmapname = ent->map = level.mapname;
                        }
                }
        }
//FIREBLADE
                else if (!level.nextmap)
        {
                cur_map++;
                if (cur_map >= num_maps)
                  cur_map = 0;
                ent = G_Spawn();
                ent->classname = "target_changelevel";
                Com_sprintf(level.nextmap, sizeof(level.nextmap), "%s", map_rotation[cur_map]);
                nextmapname = ent->map = level.nextmap;
        }

        if (level.nextmap != NULL)
        {
                gi.bprintf(PRINT_HIGH, "Next map in rotation is %s.\n",
                        level.nextmap);
        }
//FIREBLADE

        ReadMOTDFile();
        BeginIntermission (ent);

//AZEROV
        UnBan_TeamKillers ();
//AZEROV
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules (void)
{
    //    int                     i;
    //    gclient_t       *cl;

        if (level.intermissiontime)
                return;

   /*     if (!deathmatch->value)
                return;
*/
//FIREBLADE
//PaTMaN-REMOVE_TEAMPLAY         if (teamplay->value)
//PaTMaN-REMOVE_TEAMPLAY 			teamplay->value = 0;
			/*
        {
            CheckTeamRules();
        }
                else
		*/
    //    {
		
		/* //PaTMaN - V1.23 - Timelimit removed
                if (timelimit->value)
                {
                        if (level.time >= timelimit->value*60)
                        {
                                gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
                                EndDMLevel ();
                                return;
                        }
                }
		*/

				if (level.votemaptimedown)
				{
					level.votemaptimedown -= .1;
					if (level.votemaptimedown < .1)
					{
						//gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
						ChangeLevelMapVote();
						return;
					}
				}

//FIREBLADE
   //     }
/*
        if (fraglimit->value)
        {
                for (i=0 ; i<maxclients->value ; i++)
                {
                        cl = game.clients + i;
                        if (!g_edicts[i+1].inuse)
                                continue;

                        if (cl->resp.score >= fraglimit->value)
                        {
                                gi.bprintf (PRINT_HIGH, "Fraglimit hit.\n");
                                EndDMLevel ();
                                return;
                        }
                }
        }
		*/
}


/*
=============
ExitLevel
=============
*/
void ExitLevel (void)
{
        int             i;
        edict_t *ent;
        char    command [256];

        Com_sprintf (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
        gi.AddCommandString (command);
        level.changemap = NULL;
        level.exitintermission = 0;
        level.intermissiontime = 0;
        ClientEndServerFrames ();

        // clear some things before going to next level
        for (i=0 ; i<maxclients->value ; i++)
        {
                ent = g_edicts + 1 + i;
                if (!ent->inuse)
                        continue;
                if (ent->health > ent->client->pers.max_health)
                        ent->health = ent->client->pers.max_health;
        }

		level.clientcount = 0;	//recount when players enter next map

//FIREBLADE
/*        if (teamplay->value)
        {
                team1_score = 0;
                team2_score = 0;
        }
*/
  //FIREBLADE
}

extern float mapvotetimedown;

/*
================
G_RunFrame

Advances the world by 0.1 seconds
================
*/
void G_RunFrame (void)
{
        int             i;
        edict_t *ent;

        level.framenum++;
        level.time = level.framenum*FRAMETIME;

        // choose a client for monsters to target this frame
//PaTMaN-NOGAI        AI_SetSightClient ();

        // exit intermissions

        if (level.exitintermission)
        {
                ExitLevel ();
                return;
        }

        //
        // treat each object in turn
        // even the world gets a chance to think
        //
        ent = &g_edicts[0];
        for (i=0 ; i<globals.num_edicts ; i++, ent++)
        {
                if (!ent->inuse)
                        continue;

                level.current_entity = ent;

                VectorCopy (ent->s.origin, ent->s.old_origin);

                // if the ground entity moved, make sure we are still on it
                if ((ent->groundentity) && (ent->groundentity->linkcount != ent->groundentity_linkcount))
                {
                        ent->groundentity = NULL;
                       if ( !(ent->flags & (FL_SWIM|FL_FLY)) && (ent->svflags & SVF_MONSTER) )
                        {
                                M_CheckGround (ent);
                        }
                }

                if (i > 0 && i <= maxclients->value)
                {
                        ClientBeginServerFrame (ent);
                        continue;
                }

                G_RunEntity (ent);
        }

        // see if it is time to end a deathmatch
        CheckDMRules ();

//FIREBLADE
	CheckNeedPass();
//FIREBLADE

        // build the playerstate_t structures for all players
        ClientEndServerFrames ();
/*		if (killvote->value != 0)
		{
			if (level.voting < 1)
			{
				gi.dprintf("No vote is in progress.\n");
				killvote->value = 0;
				gi.cvar_set ("killvote", "0");
			}
			else
			{
				if (level.votemaptimedown > 0)
					gi.bprintf(PRINT_HIGH,"Map change has been killed.\n");
				else
					gi.bprintf(PRINT_HIGH,"Map voting has been killed.\n");
				UnsetAllMapVote();
				killvote->value = 0;
				gi.cvar_set ("killvote", "0");
			}
		}
*/
		/*
		if (level.votemaptimedown > 0)
		{
			level.votemaptimedown -= .1;
			if (level.votemaptimedown < .1)
			{
				level.votemaptimedown = 0;
				ChangeLevelMapVote();
			}
		}
		*/
}

//ADDED FROM 3.20 SOURCE -FB
//Commented out spectator_password stuff since we don't have that now.
/*
=================
CheckNeedPass
=================
*/
void CheckNeedPass (void)
{
	int need;

	// if password or spectator_password has changed, update needpass
	// as needed
	if (password->modified /*|| spectator_password->modified*/) 
	{
		password->modified = /*spectator_password->modified = */ false;

		need = 0;

		if (*password->string && Q_stricmp(password->string, "none"))
			need |= 1;
		/*
		if (*spectator_password->string && Q_stricmp(spectator_password->string, "none"))
			need |= 2;
		*/

		gi.cvar_set("needpass", va("%d", need));
	}
}
//FROM 3.20 END
/*
void Com_Printf (char *msg, ...)
{
        va_list         argptr;
        char            text[1024];

        va_start (argptr, msg);
        vsprintf (text, msg, argptr);
        va_end (argptr);

        gi.dprintf ("%s", text);
}
*/
void debug(char *msg,...)
{
	va_list         argptr;
	char            text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.bprintf (PRINT_HIGH,"DEBUGCMD: %s", text);
}

void warn(char *msg,...)
{
	va_list         argptr;
	char            text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	gi.bprintf (PRINT_HIGH,"WARNING: %s", text);
}

int p_mkdir(const char *path)
{
#ifdef _WIN32
	return _mkdir(path);
#else
	char newpath[512]="";
	FILE *r = fopen(path,"r");
	if (r)
		fclose(r);
	else
	{
		sprintf(newpath,"mkdir %s",path);
		system(newpath);
	}
	return 0; //return value doesn't matter for what I want
#endif
}

