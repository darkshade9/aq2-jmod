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

// g_local.h -- local definitions for game module

#include "q_shared.h"



/*
Under windows (use direct.h as include file):
int _mkdir (const char *dirname)
Under Linux (conforming to posix; using sys/stat.h and sys/types.h as include file):
int mkdir (const char *dirname, mode_t mode)


void (*think)(edict_t *self);
ent.think = temp_think_specweap;
void temp_think_specweap( edict_t* ent );

*/
//int (*p_mkdir)(const char *);

#ifdef _WIN32

#include <direct.h>
//static int (*p_mkdir)(const char *path) = _mkdir;

#else

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
////int mkdir (const char *dirname, mode_t mode)
//static int (*p_mkdir)(const char *path) = mkdir;

#endif

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define GAME_INCLUDE
#include "game.h"

//FIREBLADE
#include "a_team.h"
#include "a_game.h"
#include "a_menu.h"
//NORADIO #include "a_radio.h"
//FIREBLADE

// the "gameversion" client command will print this plus compile date
#define GAMEVERSION     "action"

#define MAX_STR_LEN             1000
#define MAX_STR_LEN_PATH        260

// protocol bytes that can be directly added to messages
#define svc_muzzleflash         1
#define svc_muzzleflash2        2
#define svc_temp_entity         3
#define svc_layout              4
#define svc_inventory           5
#define svc_stufftext           11

//==================================================================

// view pitching times
#define DAMAGE_TIME             0.5
#define FALL_TIME               0.3


// edict->spawnflags
// these are set with checkboxes on each entity in the map editor
#define SPAWNFLAG_NOT_EASY                      0x00000100
#define SPAWNFLAG_NOT_MEDIUM            0x00000200
#define SPAWNFLAG_NOT_HARD                      0x00000400
#define SPAWNFLAG_NOT_DEATHMATCH        0x00000800
#define SPAWNFLAG_NOT_COOP                      0x00001000

// edict->flags
#define FL_FLY                                  0x00000001
#define FL_SWIM                                 0x00000002      // implied immunity to drowining
#define FL_IMMUNE_LASER                 0x00000004
#define FL_INWATER                              0x00000008
#define FL_GODMODE                              0x00000010
#define FL_NOTARGET                             0x00000020
#define FL_IMMUNE_SLIME                 0x00000040
#define FL_IMMUNE_LAVA                  0x00000080
#define FL_PARTIALGROUND                0x00000100      // not all corners are valid
#define FL_WATERJUMP                    0x00000200      // player jumping out of water
#define FL_TEAMSLAVE                    0x00000400      // not the first on the team
#define FL_NO_KNOCKBACK                 0x00000800
#define FL_POWER_ARMOR                  0x00001000      // power armor (if any) is active
#define FL_RESPAWN                              0x80000000      // used for item respawning


#define FRAMETIME               0.1

// memory tags to allow dynamic memory to be cleaned up
#define TAG_GAME        765             // clear when unloading the dll
#define TAG_LEVEL       766             // clear when loading a new level


#define MELEE_DISTANCE  80

#define BODY_QUEUE_SIZE         8

typedef enum
{
        DAMAGE_NO,
        DAMAGE_YES,                     // will take damage if hit
        DAMAGE_AIM                      // auto targeting recognizes this
} damage_t;

typedef enum 
{
        WEAPON_READY, 
        WEAPON_ACTIVATING,
        WEAPON_DROPPING,
        WEAPON_FIRING,
// zucc
    WEAPON_END_MAG,
//PaTMaN-NORELOADING    WEAPON_RELOADING,
        WEAPON_BURSTING,
        WEAPON_BUSY, // used by sniper rifle when engaging zoom
                                // if I want to make laser sight toggle on/off 
                                // this could be used for that too...
//PaTMaN-NOBANDAGING        WEAPON_BANDAGING
} weaponstate_t;

typedef enum
{
        AMMO_BULLETS,
        AMMO_SHELLS,
        AMMO_ROCKETS,
        AMMO_GRENADES,
        AMMO_CELLS,
        AMMO_SLUGS
} ammo_t;


//deadflag
#define DEAD_NO                                 0
#define DEAD_DYING                              1
#define DEAD_DEAD                               2
#define DEAD_RESPAWNABLE                3

//range
#define RANGE_MELEE                             0
#define RANGE_NEAR                              1
#define RANGE_MID                               2
#define RANGE_FAR                               3

//gib types
#define GIB_ORGANIC                             0
#define GIB_METALLIC                    1

//monster ai flags
#define AI_STAND_GROUND                 0x00000001
#define AI_TEMP_STAND_GROUND    0x00000002
#define AI_SOUND_TARGET                 0x00000004
#define AI_LOST_SIGHT                   0x00000008
#define AI_PURSUIT_LAST_SEEN    0x00000010
#define AI_PURSUE_NEXT                  0x00000020
#define AI_PURSUE_TEMP                  0x00000040
#define AI_HOLD_FRAME                   0x00000080
#define AI_GOOD_GUY                             0x00000100
#define AI_BRUTAL                               0x00000200
#define AI_NOSTEP                               0x00000400
#define AI_DUCKED                               0x00000800
#define AI_COMBAT_POINT                 0x00001000
#define AI_MEDIC                                0x00002000
#define AI_RESURRECTING                 0x00004000

//monster attack state
#define AS_STRAIGHT                             1
#define AS_SLIDING                              2
#define AS_MELEE                                3
#define AS_MISSILE                              4

// armor types
#define ARMOR_NONE                              0
#define ARMOR_JACKET                    1
#define ARMOR_COMBAT                    2
#define ARMOR_BODY                              3
#define ARMOR_SHARD                             4

// power armor types
#define POWER_ARMOR_NONE                0
#define POWER_ARMOR_SCREEN              1
#define POWER_ARMOR_SHIELD              2

// handedness values
#define RIGHT_HANDED                    0
#define LEFT_HANDED                             1
#define CENTER_HANDED                   2


// game.serverflags values
#define SFL_CROSS_TRIGGER_1             0x00000001
#define SFL_CROSS_TRIGGER_2             0x00000002
#define SFL_CROSS_TRIGGER_3             0x00000004
#define SFL_CROSS_TRIGGER_4             0x00000008
#define SFL_CROSS_TRIGGER_5             0x00000010
#define SFL_CROSS_TRIGGER_6             0x00000020
#define SFL_CROSS_TRIGGER_7             0x00000040
#define SFL_CROSS_TRIGGER_8             0x00000080
#define SFL_CROSS_TRIGGER_MASK  0x000000ff


// noise types for PlayerNoise
#define PNOISE_SELF                             0
#define PNOISE_WEAPON                   1
#define PNOISE_IMPACT                   2


// edict->movetype values
typedef enum
{
MOVETYPE_NONE,                  // never moves
MOVETYPE_NOCLIP,                // origin and angles change with no interaction
MOVETYPE_PUSH,                  // no clip to world, push on box contact
MOVETYPE_STOP,                  // no clip to world, stops on box contact

MOVETYPE_WALK,                  // gravity
MOVETYPE_STEP,                  // gravity, special edge handling
MOVETYPE_FLY,
MOVETYPE_TOSS,                  // gravity
MOVETYPE_FLYMISSILE,    // extra size to monsters
MOVETYPE_BOUNCE,
MOVETYPE_BLOOD
} movetype_t;



typedef struct
{
        int             base_count;
        int             max_count;
        float   normal_protection;
        float   energy_protection;
        int             armor;
} gitem_armor_t;

typedef struct mapvote_s
{
	char name[128];
	int votes;
	struct mapvote_s *next;
} mapvote_t;

// gitem_t->flags
#define IT_WEAPON               1               // use makes active weapon
#define IT_AMMO                 2
#define IT_ARMOR                4
#define IT_STAY_COOP			8
#define IT_KEY                  16
#define IT_POWERUP              32
#define IT_ITEM                 64

typedef struct gitem_s
{
        char            *classname;     // spawning name
        qboolean        (*pickup)(struct edict_s *ent, struct edict_s *other);
        void            (*use)(struct edict_s *ent, struct gitem_s *item);
        void            (*drop)(struct edict_s *ent, struct gitem_s *item);
        void            (*weaponthink)(struct edict_s *ent);
        char            *pickup_sound;
        char            *world_model;
        int                     world_model_flags;
        char            *view_model;

        // client side info
        char            *icon;
        char            *pickup_name;   // for printing on pickup
        int                     count_width;            // number of digits to display by icon

        int                     quantity;               // for ammo how much, for weapons how much is used per shot
        char            *ammo;                  // for weapons
        int                     flags;                  // IT_* flags

        void            *info;
        int                     tag;

        char            *precaches;             // string of all models, sounds, and images this item will use
} gitem_t;

//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
typedef struct
{
        char            helpmessage1[512];
        char            helpmessage2[512];
//PaTMaN-REMOVE_HELPCHANGED        int                     helpchanged;    // flash F1 icon if non 0, play sound
                                                                // and increment only if 1, 2, or 3

        gclient_t       *clients;               // [maxclients]

        // can't store spawnpoint in level, because
        // it would get overwritten by the savegame restore
        char            spawnpoint[512];        // needed for coop respawns

        // store latched cvars here that we want to get at often
        int                     maxclients;
        int                     maxentities;

		//KBJump - PaTMaN
//KBJump-UnComment//		int						kbjump;

        // cross level triggers
        int                     serverflags;

        // items
        int                     num_items;

        qboolean        autosaved;
} game_locals_t;

#define	PDKEY_FORWARD 0
#define PDKEY_RIGHT 1
#define	PDKEY_UP 2

typedef struct puppetdemo_s
{
	//seeking/linking
	struct puppetdemo_s *prev;
	struct puppetdemo_s *next;
	int framenum;

	//Variables
	vec3_t		origin;
	vec3_t		angles;
	vec3_t		v_angle;
	vec3_t		velocity;
	vec3_t		movekeys;
	short		forwardmove;
	short		sidemove;
	short		upmove;
	float		maxs_2;
	int			weaponstate;
	int			modelframe;
	int			event;
	int			sound;
	int			falldamage;
	int			speed;
	int			highspeed;
	qboolean	groundentity;
} puppetdemo_t;

//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//

typedef struct
{
        int                     framenum;
        float					time;

        char					level_name[MAX_QPATH];  // the descriptive name (Outer Base, etc)
        char					mapname[MAX_QPATH];     // the server name (base1, etc)
        char					nextmap[MAX_QPATH];     // go here when fraglimit is hit

        // intermission state
        float					intermissiontime;       // time the intermission was started
        char					*changemap;
        int                     exitintermission;
        vec3_t					intermission_origin;
        vec3_t					intermission_angle;

        edict_t					*sight_client;			// changed once each frame for coop games

        edict_t					*sight_entity;
        int                     sight_entity_framenum;
        edict_t					*sound_entity;
        int                     sound_entity_framenum;
        edict_t					*sound2_entity;
        int                     sound2_entity_framenum;

        int                     pic_health;

//        int                     total_secrets; //V1.21 REMOVED
        int                     found_secrets;

        int                     total_goals;
        int                     found_goals;

        int                     total_monsters;
        int                     killed_monsters;

        edict_t					*current_entity;        // entity running from G_RunFrame
        int                     body_que;               // dead bodies

//PaTMaN-REMOVE_POWER_CUBES         int                 power_cubes;            // ugly necessity for coop
        int                     specspawn;				// determines if initial spawning has occured
		float					votemaptimedown;
		int						voting;
		int						clientcount;			//add on connect, subtract on disc, to avoid counting over and over for the number
} level_locals_t;


// spawn_temp_t is only used to hold entity field values that
// can be set from the editor, but aren't actualy present
// in edict_t during gameplay
typedef struct
{
        // world vars
        char            *sky;
        float           skyrotate;
        vec3_t          skyaxis;
        char            *nextmap;

        int                     lip;
        int                     distance;
        int                     height;
        char            *noise;
        float           pausetime;
        char            *item;
        char            *gravity;

        float           minyaw;
        float           maxyaw;
        float           minpitch;
        float           maxpitch;

} spawn_temp_t;


typedef struct
{
        // fixed data
        vec3_t          start_origin;
        vec3_t          start_angles;
        vec3_t          end_origin;
        vec3_t          end_angles;

        int                     sound_start;
        int                     sound_middle;
        int                     sound_end;

        float           accel;
        float           speed;
        float           decel;
        float           distance;

        float           wait;

        // state data
        int                     state;
        vec3_t          dir;
        float           current_speed;
        float           move_speed;
        float           next_speed;
        float           remaining_distance;
        float           decel_distance;
        void            (*endfunc)(edict_t *);
} moveinfo_t;


typedef struct
{
        void    (*aifunc)(edict_t *self, float dist);
        float   dist;
        void    (*thinkfunc)(edict_t *self);
} mframe_t;

typedef struct
{
        int                     firstframe;
        int                     lastframe;
        mframe_t        *frame;
        void            (*endfunc)(edict_t *self);
} mmove_t;

typedef struct
{
        mmove_t         *currentmove;
        int                     aiflags;
        int                     nextframe;
        float           scale;

        void            (*stand)(edict_t *self);
        void            (*idle)(edict_t *self);
        void            (*search)(edict_t *self);
        void            (*walk)(edict_t *self);
        void            (*run)(edict_t *self);
        void            (*dodge)(edict_t *self, edict_t *other, float eta);
        void            (*attack)(edict_t *self);
        void            (*melee)(edict_t *self);
        void            (*sight)(edict_t *self, edict_t *other);
        qboolean        (*checkattack)(edict_t *self);

        float           pausetime;
        float           attack_finished;

        vec3_t          saved_goal;
        float           search_time;
        float           trail_time;
        vec3_t          last_sighting;
        int                     attack_state;
        int                     lefty;
        float           idle_time;
        int                     linkcount;

        int                     power_armor_type;
        int                     power_armor_power;
} monsterinfo_t;



extern  game_locals_t   game;
extern  level_locals_t  level;
extern  game_import_t   gi;
extern  game_export_t   globals;
extern  spawn_temp_t    st;

extern  int     sm_meat_index;
extern  int     snd_fry;

//extern  int     jacket_armor_index;
//extern  int     combat_armor_index;
//extern  int     body_armor_index;


// means of death
#define MOD_UNKNOWN                     0
#define MOD_BLASTER                     1
#define MOD_SHOTGUN                     2
#define MOD_SSHOTGUN            3
#define MOD_MACHINEGUN          4
#define MOD_CHAINGUN            5
#define MOD_GRENADE                     6
#define MOD_G_SPLASH            7
#define MOD_ROCKET                      8
#define MOD_R_SPLASH            9
#define MOD_HYPERBLASTER        10
#define MOD_RAILGUN                     11
#define MOD_BFG_LASER           12
#define MOD_BFG_BLAST           13
#define MOD_BFG_EFFECT          14
#define MOD_HANDGRENADE         15
#define MOD_HG_SPLASH           16
#define MOD_WATER                       17
#define MOD_SLIME                       18
#define MOD_LAVA                        19
#define MOD_CRUSH                       20
#define MOD_TELEFRAG            21
#define MOD_FALLING                     22
#define MOD_SUICIDE                     23
#define MOD_HELD_GRENADE        24
#define MOD_EXPLOSIVE           25
#define MOD_BARREL                      26
#define MOD_BOMB                        27
#define MOD_EXIT                        28
#define MOD_SPLASH                      29
#define MOD_TARGET_LASER        30
#define MOD_TRIGGER_HURT        31
#define MOD_HIT                         32
#define MOD_TARGET_BLASTER      33
//zucc
#define MOD_MK23                        34
#define MOD_MP5                         35
#define MOD_M4                          36
#define MOD_M3                          37
#define MOD_HC                          38
#define MOD_SNIPER                      39
#define MOD_DUAL                        40
#define MOD_KNIFE                       41
#define MOD_KNIFE_THROWN        42
//PaTMaN-REMOVE-BLEED #define MOD_BLEEDING            43      
#define MOD_GAS                         44
#define MOD_KICK                        45
#define MOD_FRIENDLY_FIRE       0x8000000


extern  int     meansOfDeath;
// zucc for hitlocation of death
extern  int     locOfDeath;
// stop an armor piercing round that hits a vest
extern int              stopAP;

extern  edict_t                 *g_edicts;

#define FOFS(x) (int)&(((edict_t *)0)->x)
#define STOFS(x) (int)&(((spawn_temp_t *)0)->x)
#define LLOFS(x) (int)&(((level_locals_t *)0)->x)
#define CLOFS(x) (int)&(((gclient_t *)0)->x)

#define random()        ((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()       (2.0 * (random() - 0.5))

extern  cvar_t  *maxentities;
extern  cvar_t  *deathmatch;
extern  cvar_t  *coop;
extern  cvar_t  *dmflags;
//PaTMaN - weaponban
extern  cvar_t  *weaponban;
//FIREBLADE
extern	cvar_t	*needpass;
extern  cvar_t  *hostname;
//PaTMaN-REMOVE_TEAMPLAY extern  cvar_t  *teamplay;
//extern  cvar_t  *radiolog;
extern  cvar_t  *motd_time;
extern  cvar_t  *actionmaps;
//extern  cvar_t  *roundtimelimit; //V1.21 REMOVED
//extern	cvar_t	*maxteamkills; //V1.21 REMOVED
//extern	cvar_t	*tkbanrounds; //V1.21 REMOVED
//extern	cvar_t	*twbanrounds; //V.21 REMOVED
//extern  cvar_t  *limchasecam;
//extern  cvar_t  *roundlimit; //V1.21 REMOVED
extern  cvar_t  *skipmotd;
extern  cvar_t  *nohud;
//extern  cvar_t  *noscore; //V1.21 REMOVED
extern  cvar_t  *actionversion;
extern  cvar_t  *jmodversion;
//FIREBLADE
extern  cvar_t  *skill;
//extern  cvar_t  *fraglimit; //V1.21 REMOVED
extern  cvar_t  *timelimit;
extern  cvar_t  *password;
extern  cvar_t  *g_select_empty;
extern  cvar_t  *dedicated;

extern  cvar_t  *filterban;
extern  cvar_t  *flood_msgs;
extern  cvar_t  *flood_persecond;
extern  cvar_t  *flood_waitdelay;

extern  cvar_t  *sv_gravity;
extern  cvar_t  *sv_maxvelocity;

extern  cvar_t  *gun_x, *gun_y, *gun_z;
extern  cvar_t  *sv_rollspeed;
extern  cvar_t  *sv_rollangle;

extern  cvar_t  *run_pitch;
extern  cvar_t  *run_roll;
extern  cvar_t  *bob_up;
extern  cvar_t  *bob_pitch;
extern  cvar_t  *bob_roll;

extern  cvar_t  *sv_cheats;
extern  cvar_t  *maxclients;

//KBJump - PaTMaN
//KBJump-UnComment//extern	cvar_t	*kbjump;

// zucc server variables

//extern cvar_t   *unique_weapons; //V1.21 REMOVED
//extern cvar_t   *unique_items;  //V1.21 REMOVED
extern cvar_t   *ir; //toggles if bandolier works as infra-red sensor
extern cvar_t   *knifelimit;
extern cvar_t   *markerlimit;
extern cvar_t   *traillimit;
//extern cvar_t   *tgren;
//extern cvar_t   *allweapon; //V1.21 REMOVED
//extern cvar_t   *allitem; //V1.21 REMOVED

// zucc from action
extern  cvar_t  *sv_shelloff;
extern  cvar_t  *splatlimit;
extern  cvar_t  *bholelimit;

//JMod puppet demo
extern	cvar_t	*maxpuppetdemolength;
extern	cvar_t	*puppetdemosmoothing;

//JMod map voting
extern	cvar_t	*mapvoting;
extern	cvar_t	*mapvotingtime;
extern	cvar_t	*mapvotingpercent;
//extern	cvar_t	*killvote;

//JMod misc
extern	cvar_t	*notelefx;

#define world   (&g_edicts[0])

// item spawnflags
#define ITEM_TRIGGER_SPAWN              0x00000001
#define ITEM_NO_TOUCH                   0x00000002
// 6 bits reserved for editor flags
// 8 bits used as power cube id bits for coop games
#define DROPPED_ITEM                    0x00010000
#define DROPPED_PLAYER_ITEM             0x00020000
#define ITEM_TARGETS_USED               0x00040000

//
// fields are needed for spawning from the entity string
// and saving / loading games
//
#define FFL_SPAWNTEMP           1
#define FFL_NOSPAWN             2

typedef enum {
        F_INT, 
        F_FLOAT,
        F_LSTRING,                      // string on disk, pointer in memory, TAG_LEVEL
        F_GSTRING,                      // string on disk, pointer in memory, TAG_GAME
        F_VECTOR,
        F_ANGLEHACK,
        F_EDICT,                        // index on disk, pointer in memory
        F_ITEM,                         // index on disk, pointer in memory
        F_CLIENT,                       // index on disk, pointer in memory
        F_FUNCTION,
        F_MMOVE,
        F_IGNORE
} fieldtype_t;

typedef struct
{
        char    *name;
        int             ofs;
        fieldtype_t     type;
        int             flags;
} field_t;


extern  field_t fields[];
extern  gitem_t itemlist[];


//
// g_cmds.c
//
void Cmd_Help_f (edict_t *ent);
void Cmd_Score_f (edict_t *ent);

//
// g_items.c
//
void PrecacheItem (gitem_t *it);
void InitItems (void);
void SetItemNames (void);
gitem_t *FindItem (char *pickup_name);
gitem_t *FindItemByClassname (char *classname);
#define ITEM_INDEX(x) ((x)-itemlist)
edict_t *Drop_Item (edict_t *ent, gitem_t *item);
void SetRespawn (edict_t *ent, float delay);
void ChangeWeapon (edict_t *ent);
void SpawnItem (edict_t *ent, gitem_t *item);
void Think_Weapon (edict_t *ent);
int ArmorIndex (edict_t *ent);
int PowerArmorType (edict_t *ent);
gitem_t *GetItemByIndex (int index);
//qboolean Add_Ammo (edict_t *ent, gitem_t *item, int count);
void Touch_Item (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);

//
// g_utils.c
//
qboolean        KillBox (edict_t *ent);
void    G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
edict_t *G_Find (edict_t *from, int fieldofs, char *match);
edict_t *findradius (edict_t *from, vec3_t org, float rad);
edict_t *G_PickTarget (char *targetname);
void    G_UseTargets (edict_t *ent, edict_t *activator);
void    G_SetMovedir (vec3_t angles, vec3_t movedir);

void    G_InitEdict (edict_t *e);
edict_t *G_Spawn (void);
void    G_FreeEdict (edict_t *e);

void    G_TouchTriggers (edict_t *ent);
void    G_TouchSolids (edict_t *ent);

char    *G_CopyString (char *in);

float   *tv (float x, float y, float z);
char    *vtos (vec3_t v);

float vectoyaw (vec3_t vec);
void vectoangles (vec3_t vec, vec3_t angles);

//
// g_combat.c
//
qboolean OnSameTeam (edict_t *ent1, edict_t *ent2);
qboolean CanDamage (edict_t *targ, edict_t *inflictor);
void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t normal, int damage, int knockback, int dflags, int mod);
void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod);
void T_RemoveJumpMarkerRadius (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod);

// damage flags
#define DAMAGE_RADIUS                   0x00000001      // damage was indirect
#define DAMAGE_NO_ARMOR                 0x00000002      // armour does not protect from this damage
#define DAMAGE_ENERGY                   0x00000004      // damage is from an energy based weapon
#define DAMAGE_NO_KNOCKBACK             0x00000008      // do not affect velocity, just view angles
#define DAMAGE_BULLET                   0x00000010  // damage is from a bullet (used for ricochets)
#define DAMAGE_NO_PROTECTION    0x00000020  // armor, shields, invulnerability, and godmode have no effect

#define DEFAULT_BULLET_HSPREAD  300
#define DEFAULT_BULLET_VSPREAD  500
#define DEFAULT_SHOTGUN_HSPREAD 1000
#define DEFAULT_SHOTGUN_VSPREAD 500
#define DEFAULT_DEATHMATCH_SHOTGUN_COUNT        12
#define DEFAULT_SHOTGUN_COUNT   12
#define DEFAULT_SSHOTGUN_COUNT  20

//
// g_monster.c
//
void monster_fire_bullet (edict_t *self, vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype);
void monster_fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype);
void monster_fire_blaster (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect);
void monster_fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype);
void monster_fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype);
void monster_fire_railgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype);
void monster_fire_bfg (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype);
void M_droptofloor (edict_t *ent);
void monster_think (edict_t *self);
void walkmonster_start (edict_t *self);
void swimmonster_start (edict_t *self);
void flymonster_start (edict_t *self);
void AttackFinished (edict_t *self, float time);
void monster_death_use (edict_t *self);
void M_CatagorizePosition (edict_t *ent);
qboolean M_CheckAttack (edict_t *self);
void M_FlyCheck (edict_t *self);
void M_CheckGround (edict_t *ent);

//
// g_misc.c
//
void ThrowHead (edict_t *self, char *gibname, int damage, int type);
void ThrowClientHead (edict_t *self, int damage);
void ThrowGib (edict_t *self, char *gibname, int damage, int type);
void BecomeExplosion1(edict_t *self);

//
// g_ai.c
//
//void AI_SetSightClient (void);

void ai_stand (edict_t *self, float dist);
void ai_move (edict_t *self, float dist);
void ai_walk (edict_t *self, float dist);
void ai_turn (edict_t *self, float dist);
//void ai_run (edict_t *self, float dist);
void ai_charge (edict_t *self, float dist);
//int range (edict_t *self, edict_t *other);

void FoundTarget (edict_t *self);
qboolean infront (edict_t *self, edict_t *other);
qboolean visible (edict_t *self, edict_t *other);
qboolean FacingIdeal(edict_t *self);

//
// g_weapon.c
//
void ThrowDebris (edict_t *self, char *modelname, float speed, vec3_t origin);
qboolean fire_hit (edict_t *self, vec3_t aim, int damage, int kick);
void fire_bullet (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
void fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod);
void fire_blaster (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int effect, qboolean hyper);
void fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius);
void fire_grenade2 (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held);
void fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage);
void fire_rail (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick);
void fire_bfg (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius);

// zucc
int knife_attack ( edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick);
void knife_throw (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed );
void marker_throw (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed , qboolean floating, int color, qboolean realthrow);
void fire_bullet_sparks (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
void fire_bullet_sniper (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
void fire_grenade3 (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed );

/*
//
// g_ptrail.c
//
void PlayerTrail_Init (void);
void PlayerTrail_Add (vec3_t spot);
void PlayerTrail_New (vec3_t spot);
edict_t *PlayerTrail_PickFirst (edict_t *self);
edict_t *PlayerTrail_PickNext (edict_t *self);
edict_t *PlayerTrail_LastSpot (void);
*/

//
// g_client.c
//
void respawn (edict_t *ent);
void BeginIntermission (edict_t *targ);
void PutClientInServer (edict_t *ent);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
void InitBodyQue (void);
void ClientBeginServerFrame (edict_t *ent);

//
// g_player.c
//
void player_pain (edict_t *self, edict_t *other, float kick, int damage);
void player_die_spec (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

//
// g_svcmds.c
//
void    ServerCommand (void);
qboolean SV_FilterPacket (char *from);

//
// p_view.c
//
void ClientEndServerFrame (edict_t *ent);

//
// p_hud.c
//
void MoveClientToIntermission (edict_t *client);
void G_SetStats (edict_t *ent);
void G_SetSpectatorStats (edict_t *ent);
void G_CheckChaseStats (edict_t *ent);
void ValidateSelectedItem (edict_t *ent);
void DeathmatchScoreboardMessage (edict_t *client, edict_t *killer);
//void JumpScoreboardMessage (edict_t *client, edict_t *killer);

//
// g_pweapon.c
//
void PlayerNoise(edict_t *who, vec3_t where, int type);

//
// m_move.c
//
qboolean M_CheckBottom (edict_t *ent);
qboolean M_walkmove (edict_t *ent, float yaw, float dist);
void M_MoveToGoal (edict_t *ent, float dist);
void M_ChangeYaw (edict_t *ent);

//
// g_phys.c
//
void G_RunEntity (edict_t *ent);

//
// g_main.c
//
void SaveClientData (void);
//PaTMaN-REMOVE_FETCHCLIENTENTDATA void FetchClientEntData (edict_t *ent);

//
// g_chase.c
//
//void UpdateChaseCam(edict_t *ent);
//int ChaseTargetGone(edict_t *ent);
//void ChaseNext(edict_t *ent);
//void ChasePrev(edict_t *ent);
//void GetChaseTarget(edict_t *ent);

#define	SPEC_FREE		0
#define	SPEC_AROUND		1
#define	SPEC_EYES		2

void Spec_GetChaseTarget (edict_t *ent);		//get chase target 1 after ent, or pickup if last target
void Spec_GetChaseTarget_Next (edict_t *ent);	//get next chase target
void Spec_GetChaseTarget_Prev (edict_t *ent);	//get previous chase target
void Spec_CycleMode_Forward (edict_t *ent);		//cycle view mode forward
void Spec_CycleMode_Backward (edict_t *ent);	//cycle view mode backward
void Spec_UpdateChaseCam (edict_t *ent);		//update spec chase cam
int Spec_ChaseTargetGone (edict_t *ent);		//check if target exists and adjust if not


//============================================================================

// client_t->anim_priority
#define ANIM_BASIC              0               // stand / run
#define ANIM_WAVE               1
#define ANIM_JUMP               2
#define ANIM_PAIN               3
#define ANIM_ATTACK             4
#define ANIM_DEATH              5
// in 3.20 there is #define ANIM_REVERSE 6    -FB
// zucc vwep - based on info from Hentai
#define ANIM_REVERSE    -1

// client data that stays across multiple level loads
typedef struct
{
        char            userinfo[MAX_INFO_STRING];
        char            netname[16];
        int				hand;

        qboolean        connected;                      // a loadgame will leave valid entities that
                                                                        // just don't have a connection yet

        // values saved and restored from edicts when changing levels
        int				health;
        int				max_health;
//PaTMaN-REMOVE_PERS.POWERARMORACTIVE        qboolean        powerArmorActive;

        int				selected_item;
        int				inventory[MAX_ITEMS];

        // ammo capacities
//PaTMaN-NOAMMO        int                     max_bullets;
//PaTMaN-NOAMMO        int                     max_shells;
//PaTMaN-NOAMMO        int                     max_rockets;
//PaTMaN-NOAMMO        int                     max_grenades;
//PaTMaN-NOAMMO        int                     max_cells;
//PaTMaN-NOAMMO        int                     max_slugs;



        gitem_t         *weapon;
        gitem_t         *lastweapon;

//PaTMaN-REMOVE_POWER_CUBES         int                     power_cubes;    // used for tracking the cubes in coop games
//        int             score;                  // for calculating total unit score in coop games

//FIREBLADE
        qboolean        spectator;
        int				firing_style;
		char			hud_speed_source[4];

//FIREBLADE
} client_persistant_t;

// client data that stays across deathmatch respawns
typedef struct //resp.
{
		int						eqdebug; //Debug for movement equation creation
//PaTMaN-REMOVE_RESP.COOP_RESPAWN        client_persistant_t     coop_respawn;   // what to set client->pers to on a respawn
        int                     enterframe;                     // level.framenum the client entered the game
//      int                     score;							// frags, etc
		int						id;

//		int						teleporting;
		int						spectating;
//		int						ducking;
		int						falldmg;
		int						falldmglast;
		int						fallcount;
		int						speedx;
		int						highspeed;
		int						cgravity;
		int						marker_color;
		int						marker_color_jump;
		int						marker_color_fall;
		int						trail_color;
		int						trail_width;
		int						trail_cur;				//cur mass of output
		int						trailcounter;			//cur number of output
		int						intblend;
		int						trailtouch;
		int						puppetshootarea;
		char					votemap[101];
		vec3_t					movekeys;

//KBJump Vars
//KBJump-UnComment// int		runtime;				//current run time
//KBJump-UnComment// int		besttime;				//best time the player achieved
//KBJump-UnComment// int		runstatus;				//status of the player's run (ok,teleported,cheated,finished)
//KBJump-UnComment// int		teleports;				//number of teleports used
//KBJump-UnComment// int		teleporttime;			//player's time from last teleport save

		
		int						toggles;				//holding 20 vars	(TG_*)
		int						wmodes;					//holding 9 vars	(WM_* and OS_*)
		int						hud_toggles;			//holding 7 vars	(TGH_*)
		puppetdemo_t			*puppetdemo_head;
		puppetdemo_t			*puppetdemo_last;
		puppetdemo_t			*puppetdemo_current;
		int						puppetdemo_state;
		qboolean				puppetdemo_pause;
		int						puppetdemo_frame;
		int						puppetdemo_subframe;
		int						puppetdemo_speed;
		qboolean				puppetdemo_not_mine;
		qboolean				puppetdemo_bad_grav;
		qboolean				puppetdemo_pup_recorded;
		
//		char					pdfile[256];
		int						pdfile;
//		FILE					*pdfile_file;
		int						toggle_lca;
		char					lastmmsg[256];
		int						lastmmsgtime;
		float					precord_starttime;
//PUPPET SPEC UNCOMMENT		qboolean				pspectating;

//								int						toggle_laser;
//								int						toggle_slippers;
//								int						toggle_silencer;
//								int						toggle_kickable;
//								int						toggle_solid;
//								int						toggle_hud_id;
//								int						toggle_hud_rangefinder;
//								int						toggle_grenjump;
//								int						toggle_puppet;
//								int						toggle_mflash;
//								int						toggle_mtext;
//								int						toggle_markjumps;
//PaTMaN-REMOVE_LASTWEAPON2     gitem_t					*lastweapon2;
//PaTMaN-REMOVE_CURR_WEAP2 		int						curr_weap2;


		//V1.23 new spec stuff
		int			spec_dist;	//Spectating around distance

		//PaTMaN - teleport storage variables
		qboolean	teleport_stored;
		vec3_t		teleport_origin;
		vec3_t		teleport_v_angle;

        vec3_t		cmd_angles;									// angles sent over in the last command
        int         sniper_mode;								//level of zoom

//PaTMaN-REMOVE_GAME_HELPCHANGED        int                     game_helpchanged;
//PaTMaN-REMOVE_HELPCHANGED				int                     helpchanged;

    //PaTMaN-REMOVE    int                     kills; // real kills
    //PaTMaN-REMOVE    int                     damage_dealt; // keep track of damage dealt by player to other players
    //PaTMaN-REMOVE    int                     streak; // kills in a row
    //PaTMaN-REMOVE_RESP.ITEM            gitem_t                                 *item; // item for teamplay
    //PaTMaN-REMOVE_RESP.WEAPON            gitem_t                                 *weapon; // weapon for teamplay
//FIREBLADE
//        int                     team;  // team the player is on
//        int                     joined_team;    // last frame # at which the player joined a team

//NORADIO radio/partners stuff...
//NORADIO        int                     radio_delay;
//NORADIO        radio_queue_entry_t     radio_queue[MAX_RADIO_QUEUE_SIZE];
//NORADIO        int                     radio_queue_size;
//NORADIO        edict_t                 *radio_partner;                 // current partner
//NORADIO        edict_t                 *partner_last_offered_to;       // last person I offered a partnership to
//NORADIO        edict_t                 *partner_last_offered_from;     // last person I received a partnership offer from
//NORADIO        edict_t                 *partner_last_denied_from;      // last person I denied a partnership offer from
//NORADIO // end of radio/partners stuff...
                
        int                     motd_refreshes;
        int                     last_motd_refresh;
        edict_t					*last_chase_target;   // last person they chased, to resume at the same place later...
//FIREBLADE
//Action
//      int                     mk23_mode; // firing mode, semi or auto
//      int                     mp5_mode;
//      int                     m4_mode;
//      int                     knife_mode;
        int                     grenade_mode;
		char					grenadejumplist[101];
		int						cvar_stuff;
//		int						marker_mode;
//      int                     id; // id command on or off
//      int                     ir; // ir on or off (only matters if player has ir device, currently bandolier)

//NORADIO        qboolean                radio_partner_mode; // 'radio' command using team or partner
//NORADIO        qboolean                radio_gender;       // radiogender
//NORADIO        qboolean                radio_power_off;    // radio_power
//---
} client_respawn_t;

// this structure is cleared on each PutClientInServer(),
// except for 'client->pers'
struct gclient_s
{
        // known to server
        player_state_t			ps;								// communicated by server to clients
        int						ping;

        // private to game
        client_persistant_t     pers;
        client_respawn_t        resp;
        pmove_state_t           old_pmove;						// for detecting out-of-pmove changes
        qboolean				showscores;						// set layout stat

//FIREBLADE
        int             rate;									// their "rate" setting
        int             scoreboardnum;
		int				scoreboard;

//FIREBLADE
        qboolean        showinventory;							// set layout stat
        qboolean        showhelp;

//NO AMMO_INDEX        int					ammo_index;

        int             buttons;
        int             oldbuttons;
        int             latched_buttons;

        qboolean        weapon_thunk;

        gitem_t         *newweapon;
		edict_t			*puppet;

        // sum up damage over an entire frame, so
        // shotgun blasts give a single big kick
        int             damage_armor;							// damage absorbed by armor
        int             damage_parmor;							// damage absorbed by power armor
        int             damage_blood;							// damage taken out of health
        int             damage_knockback;						// impact damage
        vec3_t          damage_from;							// origin for vector calculation

        float           killer_yaw;								// when dead, look at killer

        weaponstate_t   weaponstate;
        vec3_t          kick_angles;							// weapon kicks
        vec3_t          kick_origin;
        float           v_dmg_roll, v_dmg_pitch, v_dmg_time;    // damage kicks
        float           fall_time, fall_value;					// for view drop on fall
        float           damage_alpha;
        float           bonus_alpha;
        vec3_t          damage_blend;
        vec3_t          v_angle;								// aiming direction
        float           bobtime;								// so off-ground doesn't change it
        vec3_t          oldviewangles;
        vec3_t          oldvelocity;

        float           next_drown_time;
        int             old_waterlevel;
        int             breather_sound;

        int             machinegun_shots;						// for weapon raising

        // animation vars
        int             anim_end;
        int             anim_priority;
        qboolean        anim_duck;
        qboolean        anim_run;

        // powerup timers
        float           quad_framenum;
        float           invincible_framenum;
        float           breather_framenum;
        float           enviro_framenum;

        qboolean        grenade_blew_up;
        float           grenade_time;
        int             silencer_shots;
        int             weapon_sound;

        float           pickup_msg_time;

        float           flood_locktill;					// locked from talking
        float           flood_when[10];					// when messages were said
        int             flood_whenhead;					// head pointer for when said

        float           respawn_time;					// can respawn when time > this
/*
		//PaTMaN - teleport storage variables
		qboolean	teleport_stored;
		vec3_t		teleport_angles;
		vec3_t		teleport_origin;
		vec3_t		teleport_goto;
*/
// zucc
// weapon ammo information
//No MK23 AMMO			int						mk23_max;         
//No MK23 AMMO			int						mk23_rds;
        
//No DUAL AMMO			int                     dual_max;
//No DUAL AMMO			int                     dual_rds;
//No SHOT AMMO			int                     shot_max;
//No SHOT AMMO			int                     shot_rds;
//No SNIPER AMMO        int                     sniper_max;
//No SNIPER AMMO        int                     sniper_rds;

//No MP5 AMMO			int                     mp5_max;
//No MP5 AMMO			int                     mp5_rds;

//No M4 AMMO			int                     m4_max;
//No M4 AMMO			int                     m4_rds;
        
//No CANNON AMMO        int                     cannon_max;
//No CANNON AMMO        int                     cannon_rds;
//No KNIFE AMMO			int                     knife_max;
//No MARKER AMMO		int						marker_max;
        
//No GRENADE AMMO       int                     grenade_max;

        int                     curr_weap; // uses NAME_NUM values

// other
        int                     fired; // keep track of semi auto
        int                     burst; // remember if player is bursting or not
  //      int                     fast_reload; // for shotgun/sniper rifle
        int                     idle_weapon; // how many frames to keep our weapon idle
        int                     desired_fov; // what fov does the player want? (via zooming)
//        int                     unique_weapon_total;
		int						same_ping_count;
		int						last_ping;
//        int                     unique_item_total;

        int                     knife_sound; // we attack several times when slashing but only want 1 sound
        int                     no_sniper_display;
//		int                     bandaging;

//PaTMaN-NOLEGDAMAGE			int                     leg_damage;
//PaTMaN-NOLEGDAMAGE			int                     leg_dam_count;
//PaTMaN-NOLEGDAMAGE			int                     leg_noise;
//PaTMaN-NOLEGDAMAGE			int                     leghits;
//PaTMaN-REMOVE-BLEED			int                     bleeding;			//remaining points to bleed away
//PaTMaN-REMOVE-BLEED			int                     bleed_remain;
//PaTMaN-REMOVE-BLEED			int                     bleedloc;
//PaTMaN-REMOVE-BLEED			vec3_t                  bleedloc_offset;	// location of bleeding (from origin)
//PaTMaN-REMOVE-BLEED			vec3_t                  bleednorm;        
//PaTMaN-REMOVE-BLEED			float                   bleeddelay;			// how long until we bleed again
//PaTMaN-NOBANDAGE				int                     bandage_stopped;

//      int                     have_laser;

        int                     doortoggle;			// set by player with opendoor command
                
        edict_t*                attacker;			// keep track of the last person to hit us
        int                     attacker_mod;		// and how they hit us
        int                     attacker_loc;		// location of the hit
        int                     push_timeout;		// timeout for how long an attacker will get fall death credit
                
        int                     jumping;
        
//      int                     reload_attempts;
        int                     weapon_attempts;


//FIREBLADE
        qboolean				inmenu;				// in menu
		qboolean				inbrowsermenu;		// true if in browser menu
        pmenuhnd_t				*menu;				// current menu
        edict_t					*chase_target;
        qboolean				update_chase;
        int						chase_mode;
		int						fb_menu_page;
		int						fb_menu_max;
		int						fb_con_max;
		int						fb_menu_type;
//FIREBLADE     

//AZEROV
        // Number of team kills this game
//NOTEAM_KILLS       int		team_kills;
//AZEROV

//EEK
		// Number of teammate woundings this game and a "before attack" tracker
	//NOTEAM_WOUNDS	int		team_wounds;
	//NOTEAM_WOUNDS	int		team_wounds_before;
	//NOTEAM_WOUNDS	int		ff_warning;

		// IP address of this host to be collected at Connection time.
		// (getting at it later seems to be unreliable)
		char	ipaddr[100];  // changed to 100  -FB
//EEK
};


struct edict_s
{
        entity_state_t  s;
        struct gclient_s        *client;        // NULL if not a player
                                                                        // the server expects the first part
                                                                        // of gclient_s to be a player_state_t
                                                                        // but the rest of it is opaque

        qboolean        inuse;
        int                     linkcount;

        // FIXME: move these fields to a server private sv_entity_t
        link_t          area;                           // linked to a division node or leaf
        
        int                     num_clusters;           // if -1, use headnode instead
        int                     clusternums[MAX_ENT_CLUSTERS];
        int                     headnode;                       // unused if num_clusters != -1
        int                     areanum, areanum2;

        //================================

        int                     svflags;
        vec3_t          mins, maxs;
        vec3_t          absmin, absmax, size;
        solid_t         solid;
        int                     clipmask;
        edict_t         *owner;


        // DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
        // EXPECTS THE FIELDS IN THAT ORDER!

        //================================

		edict_t         *lastowner;
		char			mmsg[256];

        int                     movetype;
        int                     flags;

        char            *model;
        float           freetime;                       // sv.time when the object was freed
        
        //
        // only used locally in game, not by server
        //
        char            *message;
        char            *classname;
        int                     spawnflags;

        float           timestamp;

        float           angle;                  // set in qe3, -1 = up, -2 = down
        char            *target;
        char            *targetname;
        char            *killtarget;
        char            *team;
        char            *pathtarget;
        char            *deathtarget;
        char            *combattarget;
        edict_t         *target_ent;

        float           speed, accel, decel;
        vec3_t          movedir;
        vec3_t          pos1, pos2;

        vec3_t          velocity;
        vec3_t          avelocity;
        int                     mass;
        float           air_finished;
        float           gravity;                // per entity gravity multiplier (1.0 is normal)
                                                                // use for lowgrav artifact, flares

        edict_t         *goalentity;
        edict_t         *movetarget;
        float           yaw_speed;
        float           ideal_yaw;

        float           nextthink;
        void            (*prethink) (edict_t *ent);
        void            (*think)(edict_t *self);
        void            (*blocked)(edict_t *self, edict_t *other);      //move to moveinfo?
        void            (*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
        void            (*use)(edict_t *self, edict_t *other, edict_t *activator);
        void            (*pain)(edict_t *self, edict_t *other, float kick, int damage);
        void            (*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

        float           touch_debounce_time;            // are all these legit?  do we need more/less of them?
        float           pain_debounce_time;
        float           damage_debounce_time;
        float           fly_sound_debounce_time;        //move to clientinfo
        float           last_move_time;

        int                     health;
        int                     max_health;
        int                     gib_health;
        int                     deadflag;
        qboolean        show_hostile;

        float           powerarmor_time;

        char            *map;                   // target_changelevel
		float			mapvotetimedown;

        int                     viewheight;             // height above origin where eyesight is determined
        int                     takedamage;
        int                     dmg;
        int                     radius_dmg;
        float           dmg_radius;
        int                     sounds;                 //make this a spawntemp var?
        int                     count;

        edict_t         *chain;
        edict_t         *enemy;
        edict_t         *oldenemy;
        edict_t         *activator;
        edict_t         *groundentity;
        int                     groundentity_linkcount;
        edict_t         *teamchain;
        edict_t         *teammaster;

        edict_t         *mynoise;               // can go in client only
        edict_t         *mynoise2;

        int                     noise_index;
        int                     noise_index2;
        float           volume;
        float           attenuation;

        // timing variables
        float           wait;
        float           delay;                  // before firing targets
        float           random;

        float           teleport_time;

        int                     watertype;
        int                     waterlevel;

        vec3_t          move_origin;	//doubles for where puppet will be moving in puppet demo
        vec3_t          move_angles;	//doubles for where puppet will be looking in puppet demo
		vec3_t			from_origin;
		vec3_t			from_angles;

        // move this to clientinfo?
        int                     light_level;

        int                     style; // also used as areaportal number and puppet demo decimate counter

        gitem_t         *item;                  // for bonus items

        // common data blocks
        moveinfo_t              moveinfo;
        monsterinfo_t   monsterinfo;
        
        // laser
        edict_t *lasersight;
      
                // action
                qboolean        splatted;
                int                     classnum;

};


//zucc
void LaserSightThink (edict_t *self);
void SP_LaserSight(edict_t *self, gitem_t *item );
//PaTMaN-NORELOAD void Cmd_Reload_f (edict_t *ent);
//PaTMaN-NORELOAD void Cmd_New_Reload_f (edict_t *ent);
void Cmd_Lens (edict_t *ent, char *cmd);
void Cmd_New_Weapon_f (edict_t *ent);
void Cmd_Weapon_f ( edict_t *ent );
void Cmd_OpenDoor_f (edict_t *ent );
//PaTMaN-NOBANDAGE void Cmd_Bandage_f ( edict_t *ent );
//void Cmd_ID_f (edict_t *ent );
void Cmd_IR_f (edict_t *ent );
//void Cmd_Choose_f (edict_t *ent);
void Cmd_PMLCA_f (edict_t *ent);
int MarkersInt();
int TrailsInt();
void PTrail_Add(edict_t *ent);
void Cmd_Toggle (edict_t *ent);
void Cmd_DemoMode_f (edict_t *ent);

//PaTMaN-NODROP void DropSpecialWeapon (edict_t *ent );
void ReadySpecialWeapon(edict_t *ent );
//PaTMaN-NODROP void DropSpecialItem ( edict_t* ent );
//PaTMaN-NOBANDAGE void Bandage( edict_t* ent );
// hentai's vwep function added by zucc
void ShowGun(edict_t *ent);


// spec functions
void SetupSpecSpawn(void);
void RespawnSpec(edict_t *ent);
void Drop_Spec(edict_t *ent, gitem_t *item);
void SpecThink(edict_t *spec);
void DeadDropSpec(edict_t *ent);

void temp_think_specweap( edict_t* ent ); // p_weapons.c
void ThinkSpecWeap( edict_t* ent );
void DropExtraSpecial( edict_t* ent );
void TransparentListSet(solid_t solid_type);

//local to g_combat but needed in p_view
void SpawnDamage (int type, vec3_t origin, vec3_t normal, int damage);
void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);


//PaTMaN-REMOVE void Add_Frag( edict_t *ent );
//PaTMaN-REMOVE void Subtract_Frag( edict_t *ent );

void kick_attack( edict_t *ent );

void SetIDView(edict_t *ent);
edict_t *GetIDView(edict_t *ent);

void EndDMLevel (void);
//PaTMaN-NOPICKUPqboolean Pickup_Special ( edict_t *ent, edict_t *other );


// action function
edict_t *FindEdictByClassnum (char *classname, int classnum);

void EjectBlooder (edict_t *self, vec3_t start, vec3_t veloc );
void EjectShell (edict_t *self, vec3_t start, int toggle);
void AddDecal (edict_t *self, trace_t* tr);
void AddSplat (edict_t *self, vec3_t point, trace_t* tr);
// weapon names
/*
bind 2 "use M3 Super 90 Assault Shotgun;"
bind 3 "use MP5/10 Submachinegun"
bind 4 "use Handcannon"
bind 5 "use M4 Assault Rifle"
bind 6 "use Sniper Rifle"
*/
#define MK23_NAME "MK23 Pistol"
#define MP5_NAME "MP5/10 Submachinegun"
#define M4_NAME "M4 Assault Rifle"
#define M3_NAME "M3 Super 90 Assault Shotgun"
#define HC_NAME "Handcannon"
#define SNIPER_NAME "Sniper Rifle"
#define DUAL_NAME "Dual MK23 Pistols"
#define KNIFE_NAME "Combat Knife"
#define GRENADE_NAME "M26 Fragmentation Grenade"
#define MARKER_NAME "Jump Marker"

#define SIL_NAME "Silencer"
#define SLIP_NAME "Stealth Slippers"
#define BAND_NAME "Bandolier"
#define KEV_NAME "Kevlar Vest"
#define LASER_NAME "Lasersight"

#define MK23_NUM                0
#define MP5_NUM                 1
#define M4_NUM                  2
#define M3_NUM                  3
#define HC_NUM                  4
#define SNIPER_NUM              5
#define DUAL_NUM                6
#define KNIFE_NUM               7
#define GRENADE_NUM             8
#define MARKER_NUM				9

// types of locations that can be hit
#define LOC_HDAM 1 // head
#define LOC_CDAM 2 // chest
#define LOC_SDAM 3 // stomach
#define LOC_LDAM 4 // legs
#define	LOC_RAND 5 // random //PaTMaN



// sniper modes
#define SNIPER_1X 0
#define SNIPER_2X 1
#define SNIPER_4X 2
#define SNIPER_6X 3



#define GRENADE_IDLE_FIRST 40
#define GRENADE_IDLE_LAST 69
#define GRENADE_THROW_FIRST 4
#define GRENADE_THROW_LAST 9 // throw it on frame 8?


// these should be server variables, when I get around to it
//#define UNIQUE_WEAPONS_ALLOWED 2
//#define UNIQUE_ITEMS_ALLOWED 1
#define SPEC_WEAPON_RESPAWN 1
//PaTMaN-NOBANDAGE #define BANDAGE_TIME    27 // 10 = 1 second
//PaTMaN-NOBANDAGE #define BLEED_TIME              10 // 10 = 1 second is time for losing 1 health at slowest bleed rate
#define GRENADE_DAMRAD  170

void Puppet_Spawn (edict_t *self,qboolean skip,qboolean nomsg);
void Pup_Think (edict_t *self);
void Pup_Spawn (edict_t *self,vec3_t origin,vec3_t angles,float max);
void DisplayPuppetDemoFileInfo (edict_t *ent);
void DisplayFileInfoFinfo (edict_t *ent);

void Cmd_Goto (edict_t *ent);
void Cmd_GotoP (edict_t *ent);
void Cmd_GotoPC (edict_t *ent);
void Cmd_Teleporter (edict_t *ent);
void RemoveMarkersInView(edict_t *ent);
void RemoveAllMarkers(edict_t *ent);
void Cmd_DemoMode_f(edict_t *ent);

void marker_spawn (vec3_t start,vec3_t angles,int color,char message[256]);
void GetMarkerColor(edict_t *self, char *buf);
void DeleteJLOC(edict_t *ent, char filename[MAX_STR_LEN], char *ren);
void LoadJLOC(edict_t *ent, char filename[MAX_STR_LEN]);
void SaveJLOC(edict_t *ent,int cmd);
void PlaceLaserNode(edict_t *ent);
void DeleteJTRAIL(edict_t *ent,char filename[MAX_STR_LEN], char *ren);
void LoadJTRAIL(edict_t *ent,char filename[MAX_STR_LEN],qboolean pdm,int clean,int colorp,int sizep);
void SaveJTRAIL(edict_t *ent,int cmd);
void Cmd_NameMarker (edict_t *ent);
int TCOLORWORDtoNUM2(char *color);
void TCOLORWORDtoNUM(edict_t *ent);
void MCOLORWORDtoNUM (edict_t *ent,int type);
void LOCWORDtoNUM (edict_t *ent);

void CleanLevel();

int  GetBestTrailMassByView (edict_t *ent);

void DeleteLaserTrail_By_Mass (edict_t *ent,int mass,char *message);
void DeleteLaserTrail_All(edict_t *ent);
void DeleteLaserTrail_Last(edict_t *ent);
void DeleteLaserTrail_Viewed(edict_t *ent);
void DeleteLaserTrail_LinksInView(edict_t *ent);

void CleanLaserTrail_By_Mass (edict_t *ent,int mass,char *message);
void CleanLaserTrail_All(edict_t *ent);
void CleanLaserTrail_Last (edict_t *ent);
void CleanLaserTrail_Viewed(edict_t *ent);
void CleanLaserTrail_LinksInView(edict_t *ent);

void Goto_LaserTrail_Start(edict_t *ent);
void Goto_LaserTrail_End(edict_t *ent);

void AddTrailCount(int i);
int LaserTrailsInt();

void ChangeTrailColor(edict_t *ent,int color);

qboolean CheckPuppetShoot(edict_t *ent);

void AddPuppetDemoNode(edict_t *ent);
void DeletePuppetDemoFile (edict_t *ent);
void CreatePuppetDemoFile (edict_t *ent);
void SavePuppetDemoFile (edict_t *ent);
//void LoadPuppetDemoFile (edict_t *ent);

void PuppetDemo_precord(edict_t *ent);
void PuppetDemo_pstop(edict_t *ent);
void PuppetDemo_pplay(edict_t *ent);
void PuppetDemo_pclear(edict_t *ent);
void PuppetDemo_psave(edict_t *ent);

qboolean BadFileName(char *file);
void TeleFX(edict_t *ent);

void InitMapvoteList();
void CallMapVote (edict_t *ent);
void ListMapVote (edict_t *ent,qboolean from_viewvotes);
void SubtractMapVote (edict_t *ent);
void AddMapVote (edict_t *ent);
void ChangeLevelMapVote();
void UnsetAllMapVote();
void AddMap(char *mapname);
void RemoveMap(char *mapname);


char *GetPuppetDemoFileInfo(char *file,qboolean listformat,qboolean isplayernum,qboolean fullpathgiven);
char *GetTrailFileInfo(char *file,qboolean listformat); //ONLY GIVE FILE NAME
char *GetMarkerFileInfo(char *file,qboolean listformat); //ONLY GIVE FILE NAME
qboolean FollowPuppetDemoFile (edict_t *ent,qboolean stempminus_hack);
void PuppetDemo_psminus(edict_t *ent);
void PuppetDemo_psplus(edict_t *ent);
void PuppetDemo_pspause(edict_t *ent);
void PuppetDemo_psstepminus(edict_t *ent);
void PuppetDemo_psstepplus(edict_t *ent);
void LoadPuppetDemoFile_ByName (edict_t *ent, char *filename);
void DeletePuppetDemoFile_ByName (edict_t *ent, char *filename, char *ren);
qboolean PuppetDemo_HasPup(char *file,qboolean isplayernum);

void FileLoad(edict_t *ent);
void FileDelete(edict_t *ent);
void FileRename(edict_t *ent);
void ListAllPuppetDemosForMap_PORT (edict_t *ent, int perpage, int pagenum);
void ListAllTrailsForMap_PORT (edict_t *ent, int perpage, int pagenum);
void ListAllMarkersForMap_PORT (edict_t *ent, int perpage, int pagenum);
char *GetFileFromFList_PORT (int Number, int type);
int GetFileCountForMap_PORT (int type);
#ifdef _WIN32
void ListAllPuppetDemosForMap_PORT_WIN (edict_t *ent,int perpage, int pagenum);
void ListAllTrailsForMap_PORT_WIN (edict_t *ent, int perpage, int pagenum);
void ListAllMarkersForMap_PORT_WIN (edict_t *ent, int perpage, int pagenum);
char *GetFileFromFList_PORT_WIN (int Number, int type);
int GetFileCountForMap_PORT_WIN (int type);
#else
void ListAllPuppetDemosForMap_PORT_LINUX (edict_t *ent,int perpage, int pagenum);
void ListAllTrailsForMap_PORT_LINUX (edict_t *ent, int perpage, int pagenum);
void ListAllMarkersForMap_PORT_LINUX (edict_t *ent, int perpage, int pagenum);
char *GetFileFromFList_PORT_LINUX (int Number, int type);
int GetFileCountForMap_PORT_LINUX (int type);
#endif

void ListAllFilesForMap (edict_t *ent);

float PlayersRangeFromSpot (edict_t *spot);
void Decoy_Think (edict_t *ent);
void DecimatePuppetPath (edict_t *ent);
char *AltString (char *s);

#define	PUPPET_NEW_NOTHING		0
#define	PUPPET_NEW_RECORDING	1
#define	PUPPET_NEW_PLAYING		2
#define	PUPPET_NEW_SPEED_MIN	0		//Min playback speed 0x (Letting it go to 0x instead of 0.1x cause it bugged me when I wanted to pause without a pspause while stepping at 0.1x)
#define	PUPPET_NEW_SPEED_MAX	10		//Max playback speed 1x
#define	PUPPET_NEW_SPEED_STEP	1		//Step of playback increase/decrease

#define	FTYPE_NONE			0
#define	FTYPE_PDEMO			1
#define	FTYPE_JTRAIL		2
#define	FTYPE_JLOC			3

void debug(char *format,...);
void warn(char *format,...);

int strsubcmp(int start, int len, char *a, char *b);
int p_mkdir(const char *path);

void T_HighlightJumpMarkerRadius (edict_t *ent, float radius, qboolean remove);

#ifdef _WIN32
#define nl '\n'
#else
#define nl '\x0D'
#endif
