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



/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
        if (deathmatch->value || coop->value)
        {
                ent->client->showscores = true;
                ent->client->scoreboardnum = 1;
        }
        VectorCopy (level.intermission_origin, ent->s.origin);
        ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
        ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
        ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
        VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
        ent->client->ps.pmove.pm_type = PM_FREEZE;
        ent->client->ps.gunindex = 0;
        ent->client->ps.blend[3] = 0;
        ent->client->ps.rdflags &= ~RDF_UNDERWATER;

        // clean up powerup info
        ent->client->quad_framenum = 0;
        ent->client->invincible_framenum = 0;
        ent->client->breather_framenum = 0;
        ent->client->enviro_framenum = 0;
        ent->client->grenade_blew_up = false;
        ent->client->grenade_time = 0;

        ent->viewheight = 0;
        ent->s.modelindex = 0;
        ent->s.modelindex2 = 0;
        ent->s.modelindex3 = 0;
        ent->s.effects = 0;
        ent->s.sound = 0;
        ent->solid = SOLID_NOT;

//FIREBLADE
        ent->client->resp.sniper_mode = SNIPER_1X;
        ent->client->desired_fov = 90;
        ent->client->ps.fov = 90;
        ent->client->ps.stats[STAT_SNIPER_ICON] = 0;
//FIREBLADE

        // add the layout

        if (deathmatch->value || coop->value)
        {
                DeathmatchScoreboardMessage (ent, NULL);
                gi.unicast (ent, true);
        }

}

void BeginIntermission (edict_t *targ)
{
        int             i, n;
        edict_t *ent, *client;

        if (level.intermissiontime)
                return;         // already activated

//FIREBLADE
//        if (teamplay->value)
//                TallyEndOfLevelTeamScores();
//FIREBLADE

        game.autosaved = false;

        // respawn any dead clients
        for (i=0 ; i<maxclients->value ; i++)
        {
                client = g_edicts + 1 + i;
                if (!client->inuse)
                        continue;
                if (client->health <= 0)
                        respawn(client);
        }

        level.intermissiontime = level.time;
        level.changemap = targ->map;

		if (strstr(level.changemap, "*"))
        {
                if (coop->value)
                {
                        for (i=0 ; i<maxclients->value ; i++)
                        {
                                client = g_edicts + 1 + i;
                                if (!client->inuse)
                                        continue;
                                // strip players of all keys between units
                                for (n = 0; n < MAX_ITEMS; n++)
                                {
                                        if (itemlist[n].flags & IT_KEY)
                                                client->client->pers.inventory[n] = 0;
                                }
                        }
                }
        }
        else
        {
                if (!deathmatch->value)
                {
                        level.exitintermission = 1;             // go immediately to the next level
                        return;
                }
        }

        level.exitintermission = 0;

        // find an intermission spot
        ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
        if (!ent)
        {       // the map creator forgot to put in an intermission point...
                ent = G_Find (NULL, FOFS(classname), "info_player_start");
                if (!ent)
                        ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
        }
        else
        {       // chose one of four spots
                i = rand() & 3;
                while (i--)
                {
                        ent = G_Find (ent, FOFS(classname), "info_player_intermission");
                        if (!ent)       // wrap around the list
                                ent = G_Find (ent, FOFS(classname), "info_player_intermission");
                }
        }

        VectorCopy (ent->s.origin, level.intermission_origin);
        VectorCopy (ent->s.angles, level.intermission_angle);

        // move all clients to the intermission point
        for (i=0 ; i<maxclients->value ; i++)
        {
                client = g_edicts + 1 + i;
                if (!client->inuse)
                        continue;
                MoveClientToIntermission (client);
        }

/* //V1.21 REMOVED
// AZEROV: Clear the team kills for everyone
		//gi.cprintf(NULL,PRINT_MEDIUM,"Resetting all team kills\n");
		for (i=1; i<=maxclients->value; i++)
		{
			edict_t *temp_ent;
			temp_ent = g_edicts + i;

			if (!temp_ent->inuse || !temp_ent->client)
			{
				continue;
			}

//NOTEAM_WOUNDS			temp_ent->client->team_wounds = 0;
//NOTEAM_KILLS			temp_ent->client->team_kills = 0;
		}
// AZEROV
*/ //V1.21 REMOVED
}

void A_ScoreboardMessage (edict_t *ent, edict_t *killer);

/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer)
{
 /*       char    entry[1024];
        char    string[1400];
        int             stringlength;
        int             i, j, k;
        int             sorted[MAX_CLIENTS];
        int             sortedscores[MAX_CLIENTS];
        int             score, total;
        int             picnum;
        int             x, y;
//		int				ping;
        gclient_t       *cl;
        edict_t         *cl_ent;
        char    *tag;
*/
//FIREBLADE
//       if (teamplay->value) {
                A_ScoreboardMessage (ent, killer);
                return;
 //       }
		
//FIREBLADE
/*
        // sort the clients by score
        total = 0;
        for (i=0 ; i<game.maxclients ; i++)
        {
			//			      ping = game.clients[sorted[i]].ping;
             //           if (ping > 999)
             //                   ping = 999;
                cl_ent = g_edicts + 1 + i;
                if (!cl_ent->inuse)
                        continue;
//                score = game.clients[i].resp.score;
				score = 0;
                for (j=0 ; j<total ; j++)
                {
                        if (score > sortedscores[j])
                                break;
                }
                for (k=total ; k>j ; k--)
                {
                        sorted[k] = sorted[k-1];
                        sortedscores[k] = sortedscores[k-1];
                }
                sorted[j] = i;
                sortedscores[j] = score;
                total++;
        }

        // print level name and exit rules
        string[0] = 0;

        stringlength = strlen(string);

        // add the clients in sorted order
        if (total > 12)
                total = 12;

        for (i=0 ; i<total ; i++)
        {
                cl = &game.clients[sorted[i]];
                cl_ent = g_edicts + 1 + sorted[i];

                picnum = gi.imageindex ("i_fixme");
                x = (i>=6) ? 160 : 0;
                y = 32 + 32 * (i%6);

                // add a dogtag
                if (cl_ent == ent)
                        tag = "tag1";
                else if (cl_ent == killer)
                        tag = "tag2";
                else
                        tag = NULL;
                if (tag)
                {
                        Com_sprintf (entry, sizeof(entry),
                                "xv %i yv %i picn %s ",x+32, y, tag);
                        j = strlen(entry);
                        if (stringlength + j > 1024)
                                break;
                        strcpy (string + stringlength, entry);
                        stringlength += j;
                }

                // send the layout
                Com_sprintf (entry, sizeof(entry),
                        "client %i %i %i %i %i %i ",
                        x, y, sorted[i], 0 , cl->ping, (level.framenum - cl->resp.enterframe)/600);
                j = strlen(entry);
                if (stringlength + j > 1024)
                        break;
                strcpy (string + stringlength, entry);
                stringlength += j;
        }

        gi.WriteByte (svc_layout);
        gi.WriteString (string);
		*/
}
/*
void JumpScoreboardMessage (edict_t *ent, edict_t *killer)
{
//        char    entry[1024];
        char    string[1400];//, damage[50];
        int             stringlength;
        int             i, j, k;
        int             sorted[MAX_CLIENTS];
        int             sortedscores[MAX_CLIENTS];
        int             score, total;
//        int             picnum;
//        int             x, y;
//		int ping;
		int maxsize = 1000;
//       gclient_t       *cl;
        edict_t         *cl_ent;
//        char    *tag;

//FIREBLADE
        if (teamplay->value) {
                A_ScoreboardMessage (ent, killer);
                return;
        }
//FIREBLADE

        // sort the clients by score
        total = 0;
        for (i=0 ; i<game.maxclients ; i++)
        {

                cl_ent = g_edicts + 1 + i;
                if (!cl_ent->inuse)
                        continue;
                score = game.clients[i].resp.score;
                for (j=0 ; j<total ; j++)
                {
                        if (score > sortedscores[j])
                                break;
                }
                for (k=total ; k>j ; k--)
                {
                        sorted[k] = sorted[k-1];
                        sortedscores[k] = sortedscores[k-1];
                }
                sorted[j] = i;
                sortedscores[j] = score;
                total++;
        }

        // print level name and exit rules
        string[0] = 0;

        stringlength = strlen(string);
*/
/*Com_sprintf(entry, sizeof(entry), 
"xv 32 yv 32 string2  \"Player                PTS    FD\" " 
"xv 32 yv 40 string2  \"-------------------- ----- ------\" "); 
j = strlen(entry); 
if (stringlength + j < 1024) 
{ 
strcpy (string + stringlength, entry); 
stringlength += j; 
} 
*/
		/*
        // add the clients in sorted order
        if (total > 12)
                total = 12;

        for (i=0 ; i<total ; i++)
        {
                cl = &game.clients[sorted[i]];
                cl_ent = g_edicts + 1 + sorted[i];

                picnum = gi.imageindex ("i_fixme");
                x = (i>=6) ? 160 : 0;
                y = 32 + 32 * (i%6);

				
                // add a dogtag
                if (cl_ent == ent)
                        tag = "tag1";
                else if (cl_ent == killer)
                        tag = "tag2";
                else
                        tag = NULL;
                if (tag)
                {
                        Com_sprintf (entry, sizeof(entry),
                                "xv %i yv %i picn %s ",x+32, y, tag);
                        j = strlen(entry);
                        if (stringlength + j > 1024)
                                break;
                        strcpy (string + stringlength, entry);
                        stringlength += j;
                }

                // send the layout

                Com_sprintf (entry, sizeof(entry),
                        " client %i %i %i %i %i %i ",
                        x, y, sorted[i], cl->resp.cl_maxfps, ent->client->rate, (level.framenum - cl->resp.enterframe));
 //cl->resp.falldmg
				j = strlen(entry);
                if (stringlength + j > 1024)
                        break;
                strcpy (string + stringlength, entry);
                stringlength += j;
				
        }
*/

/*for (i = 0; i < total; i++)
                {
//                        ping = game.clients[sorted[i]].ping;
//                        if (ping > 999)
                                ping = 999;
                        if (noscore->value)
                        {
                                sprintf(string + strlen(string), 
                                        "xv 0 yv %d string \"%-15s %4d %4d\" ",
                                        48 + i * 8, 
                                        game.clients[sorted[i]].pers.netname,
                                        (level.framenum - game.clients[sorted[i]].resp.enterframe)/600,
                                        ping);                  
                        }       
                        else
//                        {
				if (game.clients[sorted[i]].resp.damage_dealt < 1000000)
					sprintf(damage, "%d", game.clients[sorted[i]].resp.damage_dealt);
					else
					strcpy(damage, "******");
                                sprintf(string + strlen(string), 
                                        "xv 0 yv %d string \"%5d %-15s %4d %4d %6s\" ",
                                        48 + i * 8, 
                                        sortedscores[i],
                                        game.clients[sorted[i]].pers.netname,
                                        (level.framenum - game.clients[sorted[i]].resp.enterframe)/600,
                                        ping, damage);                  
                        }

                        if (strlen(string) > (maxsize - 100) &&
                                i < (total - 2))
                        {
                                sprintf(string + strlen(string),
                                        "xv 0 yv %d string \"..and %d more\" ",
                                        48 + (i + 1) * 8,
                                        (total - i - 1));
                                break;
                        }
                }
*/


/*

        gi.WriteByte (svc_layout);
        gi.WriteString (string);
}
*/

/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
        DeathmatchScoreboardMessage (ent, ent->enemy);
        gi.unicast (ent, true);
}

//void JumpScoreboard (edict_t *ent)
//{
//        JumpScoreboardMessage (ent, ent->enemy);
//        gi.unicast (ent, true);
//}

/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	int i;

        ent->client->showinventory = false;
        ent->client->showhelp = false;

//FIREBLADE
        if (ent->client->menu)
                PMenu_Close(ent);
//FIREBLADE

        if (!deathmatch->value && !coop->value)
                return;
	//PaTMaN  gi.cprintf(ent, PRINT_HIGH, "Scoreboardnum = %d\n",ent->client->scoreboardnum);
        if (ent->client->showscores)
        {
//FIREBLADE
/*                if (teamplay->value && ent->client->scoreboardnum < 2)  // toggle scoreboards...
                {
                        ent->client->scoreboardnum++;
                        DeathmatchScoreboard(ent);
	//PaTMaN  gi.cprintf(ent, PRINT_HIGH, "Scoreboardnum - TP = %d\n",ent->client->scoreboardnum);
                        return;
                }
//FIREBLADE


				if (teamplay->value)
				{
					i = 3;
				} else {
				*/
					i = 2;
			//	}

//remove /* */ for second scoreboard
			/*	if (ent->client->scoreboardnum < i)  // toggle scoreboards...
                {
                        ent->client->scoreboardnum++;
                        JumpScoreboard(ent);
						ent->client->scoreboard = 2;
	//PaTMaN  gi.cprintf(ent, PRINT_HIGH, "Scoreboardnum - Jump = %d\n",ent->client->scoreboardnum);
                        return;
                }
*/
				ent->client->showscores = false;
                return;

        }



//FIREBLADE


        ent->client->showscores = true;
//FIREBLADE
        ent->client->scoreboardnum = 1;
//FIREBLADE
	//PaTMaN  	gi.cprintf(ent, PRINT_HIGH, "Scoreboardnum - DM = %d\n",ent->client->scoreboardnum);
        DeathmatchScoreboard (ent);
		ent->client->scoreboard = 1;

}




/*
==================
HelpComputer

Draw help computer.
==================
*/
/*
void HelpComputer (edict_t *ent)
{
        char    string[1024];
        char    *sk;

        if (skill->value == 0)
                sk = "easy";
        else if (skill->value == 1)
                sk = "medium";
        else if (skill->value == 2)
                sk = "hard";
        else
                sk = "hard+";

        // send the layout
        Com_sprintf (string, sizeof(string),
                "xv 32 yv 8 picn help "                 // background
                "xv 202 yv 12 string2 \"%s\" "          // skill
                "xv 0 yv 24 cstring2 \"%s\" "           // level name
                "xv 0 yv 54 cstring2 \"%s\" "           // help 1
                "xv 0 yv 110 cstring2 \"%s\" "          // help 2
                "xv 50 yv 164 string2 \" kills     goals    secrets\" "
                "xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ", 
                sk,
                level.level_name,
                game.helpmessage1,
                game.helpmessage2,
                level.killed_monsters, level.total_monsters, 
                level.found_goals, level.total_goals,
                level.found_secrets, level.total_secrets);

        gi.WriteByte (svc_layout);
        gi.WriteString (string);
        gi.unicast (ent, true);
}
*/

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
        // this is for backwards compatability
//        if (deathmatch->value)
//        {
                Cmd_Score_f (ent);
//                return;
//        }
/*
        ent->client->showinventory = false;
        ent->client->showscores = false;

        if (ent->client->showhelp && (ent->client->resp.game_helpchanged == game.helpchanged))
        {
                ent->client->showhelp = false;
                return;
        }

        ent->client->showhelp = true;
        ent->client->resp.helpchanged = 0;
        HelpComputer (ent);
		*/
}


//=======================================================================

//void Cmd_ViewTrailNodeInfo(edict_t *ent);

vec3_t tempvec[40];

/*
===============
G_SetStats

Rearranged for chase cam support -FB
===============
*/
void G_SetStats (edict_t *ent)
{
//        gitem_t         *item;
//        int                     index, cells, index2;
//        int                     power_armor_type;
		vec3_t          start, forward, end, velocity;			// PaTMaN - Rangefinder
		trace_t         tr, tr2;								// PaTMaN - Rangefinder
		vec_t			 speed;

        if (!ent->client->chase_mode)
        {
			if (  ent->client->resp.sniper_mode == SNIPER_1X 
				|| ent->client->weaponstate == WEAPON_BUSY
				|| ent->client->no_sniper_display ) 
				ent->client->ps.stats[STAT_SNIPER_ICON] = 0;
			else if ( ent->client->resp.sniper_mode == SNIPER_2X )
				ent->client->ps.stats[STAT_SNIPER_ICON] = gi.imageindex ("scope2x");
			else if ( ent->client->resp.sniper_mode == SNIPER_4X )
				ent->client->ps.stats[STAT_SNIPER_ICON] = gi.imageindex ("scope4x");
			else if ( ent->client->resp.sniper_mode == SNIPER_6X )
				ent->client->ps.stats[STAT_SNIPER_ICON] = gi.imageindex ("scope6x");
				
			ent->client->ps.stats[STAT_FALLDMGLAST] = ent->client->resp.falldmglast;	//PaTMaN
			ent->client->ps.stats[STAT_GRAVITY] = ent->client->resp.cgravity;
        }
		else
		{
			if (ent->client->chase_target->inuse)
			{
				if (ent->client->chase_target->client)
				{
					ent->client->ps.stats[STAT_FALLDMGLAST] = ent->client->chase_target->client->resp.falldmglast;
					ent->client->ps.stats[STAT_SPEEDX] = ent->client->chase_target->client->ps.stats[STAT_SPEEDX];
					ent->client->ps.stats[STAT_HIGHSPEED] = ent->client->chase_target->client->resp.highspeed;
					ent->client->ps.stats[STAT_GRAVITY] = ent->client->chase_target->client->resp.cgravity;
				}
			}
		}

		VectorClear(velocity);
		if (strstr(ent->client->pers.hud_speed_source,"x"))
			velocity[0] = ent->velocity[0];
		if (strstr(ent->client->pers.hud_speed_source,"y"))
			velocity[1] = ent->velocity[1];
		if (strstr(ent->client->pers.hud_speed_source,"z"))
			velocity[2] = ent->velocity[2];

		speed = VectorNormalize(velocity);

		//testing  - jump prediction - strafe jump equation testing
	//	if (ent->classnum == 0)
	//		ent->classnum = 1;

//		if (ent->client->ps.stats[STAT_SPEEDX] < speed)
//		{
		//	ent->classnum *= 2;
		//	ent->client->ps.pmove.delta_angles[YAW] = ent->classnum;
		//	50 mod = .25 yaw
		//		+200 delta[yaw] = +1 angles[yaw]
//			ent->client->ps.pmove.delta_angles[YAW] = (-0.0901*(level.time*level.time*800)) + (6.0198*level.time*800);// + 35.694
		//	gi.dprintf("INFO: %d        %i        %i",
		//		ent->client->ps.pmove.delta_angles[YAW],
		//		ent->classnum,speed);
//			ent->linkcount = speed;
//		}
//		else
//			ent->linkcount = 0;


		ent->client->ps.stats[STAT_SPEEDX] = speed;

		ent->client->ps.stats[STAT_GRAVITY] = ent->client->resp.cgravity;
		if (speed /*ent->client->ps.stats[STAT_SPEEDX]*/ > ent->client->resp.highspeed)
			ent->client->resp.highspeed = speed; //ent->client->ps.stats[STAT_SPEEDX];
		ent->client->ps.stats[STAT_HIGHSPEED] = ent->client->resp.highspeed; 

        // PaTMaN - Rangefinder
		if (ent->client->resp.toggles & TG_HUD_RANGE)
		{
			VectorCopy(ent->s.origin, start);
			start[2] += ent->viewheight;
			AngleVectors(ent->client->v_angle, forward, NULL, NULL);
			VectorMA(start, 8192, forward, end);
			tr = gi.trace(start, NULL, NULL, end, ent, MASK_SHOT|CONTENTS_SLIME|CONTENTS_LAVA);
			tr2 = gi.trace(start, NULL, NULL, end, ent, MASK_SOLID|MASK_SHOT);
	
			{
				//Min x/y velocity needed to slide up a ramp at (deg) degrees
				//float deg=0.0;
				//deg = tr2.plane.normal[2];
				//deg = (acos(deg)/(M_PI/2))*90;
				//deg = 12500*(1/pow(deg,1.2))+268;
				//ent->client->ps.stats[STAT_HUD_RANGEFINDER] = (int)deg;
			}

			{
				//x/y speed transfer to z when hitting slant
				//needs tweaked
				//float dv=0.0;
				//dv = tr2.plane.normal[2];
				//dv = (acos(dv)/(M_PI/2))*90;
				//dv = -0.10594951*(pow(dv,2))-(6.0282522*dv)+486.27451 + speedxy;
				//ent->client->ps.stats[STAT_HUD_RANGEFINDER] = (int)dv;
				//debug("%f\n",speedz);
			}
			

			

			if ( tr2.plane.normal[2] > 0.7 && !tr.startsolid)
				ent->client->ps.stats[STAT_HUD_RANGEFINDER] = (int)(tr.fraction * 8192);
			else
				ent->client->ps.stats[STAT_HUD_RANGEFINDER] = (int)(tr.fraction * -8192);
/*
//testing - jump prediction
//forget if i was in the middle of adding in strafe jumping
//if so... might be screwed up
			if (ent->groundentity)
			{
				float target_height=0;
				float inctime=0;
				float speed=0;
				float distance=0;
				float trailnum=0;
				vec3_t E;
				vec3_t S;

				target_height = tr.endpos[2] - ent->s.origin[2];
				for (inctime=0;inctime < 20;inctime += 0.1)
				{
					if ((-4*((inctime*10)*(inctime*10))+32.425*(inctime*10)) < target_height)
						break;

				}
				VectorSubtract(tr.endpos,ent->s.origin,E);
				distance = VectorLength(E);
				speed = distance / inctime;

				for (trailnum=0;trailnum<inctime;trailnum+=0.1)
				{
					VectorClear(S);
					VectorClear(E);
					S[2] = (-4*((trailnum*10)*(trailnum*10))+32.425*(trailnum*10));
					E[2] = (-4*(((trailnum+0.1)*10)*((trailnum+0.1)*10))+32.425*((trailnum+0.1)*10));
					S[2] += ent->s.origin[2];
					E[2] += ent->s.origin[2];

					//gi.dprintf("%.3f %.3f\n",S[2],E[2]);
					//VectorAdd(S,ent->s.origin,S);
					//VectorAdd(E,ent->s.origin,E);
					S[1] = ent->s.origin[1] + (speed * (trailnum / inctime));
					E[1] = ent->s.origin[1] + (speed * ((trailnum+0.1) / inctime));

					//first test for strafe jump calculation
			//		S[0] = ent->s.origin[0] + (-0.0901*(trailnum*trailnum*16)) + (6.0198*trailnum*16);
			//		E[0] = ent->s.origin[0] + (-0.0901*((trailnum+0.1)*(trailnum+0.1)*16)) + (6.0198*(trailnum+0.1)*16);

					VectorCopy(S,tempvec[(int)(trailnum*10)]);
					VectorCopy(E,tempvec[(int)(trailnum*10)+20]);
					gi.WriteByte (svc_temp_entity);
					gi.WriteByte (TE_BFG_LASER);
					gi.WritePosition (S);
					gi.WritePosition (E);
					gi.multicast (start, MULTICAST_PHS);
				}
				tempvec[(int)(trailnum*10)][0] = 999;

				ent->client->ps.stats[STAT_HUD_RANGEFINDER] = speed;
				ent->classnum = speed;

			}
			else
			{
				vec3_t E;
				vec3_t S;
					float trailnum=0;
					while (tempvec[(int)(trailnum*10)][0] != 999)
					{
						VectorCopy(tempvec[(int)(trailnum*10)],S);
						VectorCopy(tempvec[(int)(trailnum*10)+20],E);
						gi.WriteByte (svc_temp_entity);
						gi.WriteByte (TE_BFG_LASER);
						gi.WritePosition (S);
						gi.WritePosition (E);
						gi.multicast (start, MULTICAST_PHS);
						trailnum += 0.1;
					}

				ent->client->ps.stats[STAT_HUD_RANGEFINDER] = ent->classnum;

			}
			*/
		}
        
        //
        // layouts
        //

			ent->client->ps.stats[STAT_HIGHSPEED_ZERO] = 0;
			ent->client->ps.stats[STAT_SPEEDX_ZERO] = 0;
			//ent->client->ps.stats[STAT_FALLDMG_ZERO] = 0;
			ent->client->ps.stats[STAT_FALLDMGLAST_ZERO] = 0;
			//ent->client->ps.stats[STAT_FALLCOUNT_ZERO] = 0;
			ent->client->ps.stats[STAT_GRAVITY_ZERO] = 0;

		if (ent->client->resp.hud_toggles & TGH_NO_HUD)
		{
			ent->client->ps.stats[STAT_SPEEDX] = 0;
			ent->client->ps.stats[STAT_HIGHSPEED] = 0;
			//ent->client->ps.stats[STAT_FALLDMG] = 0;
			ent->client->ps.stats[STAT_FALLDMGLAST] = 0;
			//ent->client->ps.stats[STAT_FALLCOUNT] = 0;
			ent->client->ps.stats[STAT_GRAVITY] = 0;

	/*		ent->client->ps.stats[STAT_HIGHSPEED_ZERO] = 0;
			ent->client->ps.stats[STAT_SPEEDX_ZERO] = 0;
			//ent->client->ps.stats[STAT_FALLDMG_ZERO] = 0;
			ent->client->ps.stats[STAT_FALLDMGLAST_ZERO] = 0;
			//ent->client->ps.stats[STAT_FALLCOUNT_ZERO] = 0;
			ent->client->ps.stats[STAT_GRAVITY_ZERO] = 0;
	*/
		}
		else
		{
			if (ent->client->resp.hud_toggles & TGH_NO_SPEED)
			{
				ent->client->ps.stats[STAT_SPEEDX] = 0;
				ent->client->ps.stats[STAT_SPEEDX_ZERO] = 0;
			}
			else if (ent->client->ps.stats[STAT_SPEEDX] == 0)
				ent->client->ps.stats[STAT_SPEEDX_ZERO] = 1;

			if (ent->client->resp.hud_toggles & TGH_NO_HIGH_SPEED)
			{
				ent->client->ps.stats[STAT_HIGHSPEED] = 0;
				ent->client->ps.stats[STAT_HIGHSPEED_ZERO] = 0;
			}
			else if (ent->client->ps.stats[STAT_HIGHSPEED] == 0)
				ent->client->ps.stats[STAT_HIGHSPEED_ZERO] = 1;

			if (ent->client->resp.hud_toggles & TGH_NO_LAST_FALL_DAMAGE)
			{
				ent->client->ps.stats[STAT_FALLDMGLAST] = 0;
				ent->client->ps.stats[STAT_FALLDMGLAST_ZERO] = 0;
			}
			else if (ent->client->ps.stats[STAT_FALLDMGLAST] == 0)
				ent->client->ps.stats[STAT_FALLDMGLAST_ZERO] = 1;

			if (ent->client->resp.hud_toggles & TGH_NO_GRAVITY)
			{
				ent->client->ps.stats[STAT_GRAVITY] = 0;
				ent->client->ps.stats[STAT_GRAVITY_ZERO] = 0;
			}
			else if (ent->client->ps.stats[STAT_GRAVITY] == 0)
				ent->client->ps.stats[STAT_GRAVITY_ZERO] = 1;
		}

      ent->client->ps.stats[STAT_LAYOUTS] = 0;

     //   if (deathmatch->value)
     //   {
                if ( /*ent->client->pers.health <= 0 ||*/ level.intermissiontime
                        || ent->client->showscores)
                        ent->client->ps.stats[STAT_LAYOUTS] |= 1;
                if (ent->client->showinventory && ent->client->pers.health > 0)
                        ent->client->ps.stats[STAT_LAYOUTS] |= 2;
      /*  }
        else
        {
                if (ent->client->showscores || ent->client->showhelp)
                        ent->client->ps.stats[STAT_LAYOUTS] |= 1;
                if (ent->client->showinventory && ent->client->pers.health > 0)
                        ent->client->ps.stats[STAT_LAYOUTS] |= 2;
        }
*/
		ent->client->ps.stats[STAT_ID_VIEW] = 21;
		ent->client->ps.stats[STAT_ID_VIEW_CLASS] = CLASS_NONE;
		ent->client->ps.stats[STAT_ID_VIEW_NUM] = 0;
//		ent->client->ps.stats[STAT_ID_VIEW_SPECIAL] = 0;

		//if (ent->client->resp.toggles & TG_HUD_ID) //v1.23 handled in setidview now
			SetIDView(ent);

//		Cmd_ViewTrailNodeInfo(ent);
	   

//FIREBLADE
//        if (teamplay->value)
//              A_Scoreboard(ent);
//FIREBLADE

}

