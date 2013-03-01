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

/*
 * Include for base Action game-related things
 */

#define ACTION_VERSION	"1.52"
#define JMOD_VERSION	"1.23"
#define	JMOD_REVISION	".04"

//Full version string should look like: 

//extern char team1_name[];
//extern char team2_name[];
//extern char team1_skin[];
//extern char team2_skin[];
//extern char team1_skin_index[];
//extern char team2_skin_index[];
extern char *map_rotation[];
extern int num_maps, cur_map;
extern char *tnames[];
extern int *took_damage;

void ReadConfigFile();
void ReadMOTDFile();
void PrintMOTD(edict_t *);
void stuffcmd(edict_t *, char *);
int KickDoor( trace_t *tr_old, edict_t *ent, vec3_t forward );

// Prototypes of base Q2 functions that weren't included in any Q2 header
qboolean loc_CanSee(edict_t *, edict_t *);
qboolean IsNeutral(edict_t *);
qboolean IsFemale(edict_t *);
void ParseSayText(edict_t *, char *);

// Firing styles (where shots originate from)
#define ACTION_FIRING_CENTER            0
#define ACTION_FIRING_CLASSIC           1
#define ACTION_FIRING_CLASSIC_HIGH      2

// maxs[2] of a player when crouching (we modify it from the normal 4)
// ...also the modified viewheight -FB 7/18/99
#define CROUCHING_MAXS2                 16
#define CROUCHING_VIEWHEIGHT		8

