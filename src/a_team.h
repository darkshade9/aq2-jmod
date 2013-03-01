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
 * Include for Action team-related things
 */

//#define NOTEAM          0
//#define TEAM1           1
//#define TEAM2           2

#define MAX_TEAMS       2
#define TEAM_TOP        (MAX_TEAMS+1)

//#define WINNER_NONE     0
//#define WINNER_TEAM1    1
//#define WINNER_TEAM2    2
//#define WINNER_TIE      3

// Pre- and post-trace code for our teamplay anti-stick stuff.  If there are
// still "transparent" (SOLID_TRIGGER) players, they need to be set to
// SOLID_BBOX before a trace is performed, then changed back again
// afterwards.  PRETRACE() and POSTTRACE() should be called before and after
// traces in all places where combat is taking place (ie "transparent" players
// should be detected), ie shots being traced etc.  
// FB 6/1/99: Now crouching players will have their bounding box adjusted here
// too, for better shot areas. (there has to be a better way to do this?)
/*
#define PRETRACE() \
        if (transparent_list) && (int)teamplay->value && !lights_camera_action) \
                TransparentListSet(SOLID_BBOX)

#define POSTTRACE() \
        if (transparent_list) && (int)teamplay->value && !lights_camera_action) \
                TransparentListSet(SOLID_TRIGGER)
*/

#define PRETRACE() if (transparent_list) TransparentListSet(SOLID_BBOX)

#define POSTTRACE() if (transparent_list) TransparentListSet(SOLID_TRIGGER)

edict_t *SelectTeamplaySpawnPoint(edict_t *); 
qboolean FallingDamageAmnesty(edict_t *targ);
//PaTMaN-NOMENUS void OpenJoinMenu(edict_t *);
//PaTMaN-NOMENUS void OpenWeaponMenu(edict_t *);
//PaTMaN-NOMENUS void OpenItemMenu(edict_t *ent);

void OpenMainMenu(edict_t *ent,int cur);
void OpenTeleMenu(edict_t *ent);
void OpenMiscMenu(edict_t *ent);
void OpenItemMenu(edict_t *ent);
void OpenPuppetMenu(edict_t *ent,int cur);
void OpenPuppetMenu_Demo(edict_t *ent);
void OpenPModeMenu(edict_t *ent);
void OpenMarkerMenu(edict_t *ent);
void OpenTrailMenu(edict_t *ent,int);
void OpenTrailMenu_Cleaning(edict_t *ent);
void OpenTrailMenu_Delete(edict_t *ent);
void OpenFBMenu(edict_t *ent, int cur);
void OpenFBSelMenu(edict_t *ent, int cur);
void OpenFBMenu(edict_t *ent, int cur);
void OpenHUDMenu(edict_t *ent, int cur);

void JoinTeam(edict_t *ent, int desired_team, int skip_menuclose);
edict_t *FindOverlap(edict_t *ent, edict_t *last_overlap);
edict_t *FindMarkerOverlap(edict_t *ent);
void CheckTeamRules(void);
//void A_Scoreboard(edict_t *ent); // V1.21 REMOVED
void Team_f(edict_t *ent);
qboolean StartClient(edict_t *ent);
void AssignSkin(edict_t *, char *);
void TallyEndOfLevelTeamScores(void);
void CheckForUnevenTeams(void);
//void SetupTeamSpawnPoints(); // V1.21 REMOVED
//void GetSpawnPoints(); // V1.21 REMOVED
void CleanBodies(); // from p_client.c, removes all current dead bodies from map
void LeaveTeam(edict_t *);
//int newrand(int top); // V1.21 REMOVED
void InitTransparentList();
void AddToTransparentList(edict_t *);
void RemoveFromTransparentList(edict_t *);
void PrintTransparentList();

typedef struct spawn_distances_s
{
        float distance;
        edict_t *s;
} spawn_distances_t;

typedef struct transparent_list_s
{
        edict_t *ent;
        struct transparent_list_s *next;
} transparent_list_t;


//extern qboolean team_game_going; // V1.21 REMOVED
//extern qboolean team_round_going; // V1.21 REMOVED
//extern int team1_score; // V1.21 REMOVED
//extern int team2_score; // V1.21 REMOVED
//extern int team1_total; // V1.21 REMOVED
//extern int team2_total; // V1.21 REMOVED
//extern int lights_camera_action;
extern int holding_on_tie_check;
extern int team_round_countdown;
extern transparent_list_t *transparent_list;
extern trace_t trace_t_temp;

