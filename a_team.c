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
 * Teamplay-related code for Action (formerly Axshun).  
 * Some of this is borrowed from Zoid's CTF (thanks Zoid)
 * -Fireblade
 */

#include "g_local.h"
#include "cgf_sfx_glass.h"

//qboolean team_game_going = 0;   // is a team game going right now? // V1.21 REMOVED
qboolean team_round_going = 0;  // is an actual round of a team game going right now?
int team_round_countdown = 0;   // countdown variable for start of a round
int rulecheckfrequency = 0;     // accumulator variable for checking rules every 1.5 secs
int lights_camera_action = 0;   // countdown variable for "lights...camera...action!" 
int holding_on_tie_check = 0;   // when a team "wins", countdown for a bit and wait...
int current_round_length = 0;   // frames that the current team round has lasted

//int team1_score = 0; // V1.21 REMOVED
//int team2_score = 0; // V1.21 REMOVED
//int team1_total = 0; // V1.21 REMOVED
//int team2_total = 0; // V1.21 REMOVED

#define MAX_SPAWNS 1000 // max DM spawn points supported

edict_t *potential_spawns[MAX_SPAWNS];  
int num_potential_spawns;
edict_t *teamplay_spawns[MAX_TEAMS];
trace_t trace_t_temp;  // used by our trace replace macro in ax_team.h

//int num_teams = 2; // teams in current game, fixed at 2 for now... // V1.21 REMOVED

transparent_list_t *transparent_list = NULL;

void CreditsMenu(edict_t *ent, pmenu_t *p);

void InitTransparentList()
{
        if (transparent_list != NULL)
        {
                transparent_list_t *p, *q;

                p = transparent_list;
                while (p != NULL)
                {
                        q = p->next;
                        gi.TagFree(p);
                        p = q;
                }

                transparent_list = NULL;
        }
}

void AddToTransparentList(edict_t *ent)
{
        transparent_list_t *p, *n;

        n = (transparent_list_t *)gi.TagMalloc(sizeof(transparent_list_t), TAG_GAME);
        if (n == NULL)
        {
                gi.dprintf("Out of memory\n");
                exit(1);
        }
        n->ent = ent;
        n->next = NULL;

        if (transparent_list == NULL)
        {
                transparent_list = n;
        }
                else
        {
                p = transparent_list;
                while (p->next != NULL)
                {
                        p = p->next;
                }
                p->next = n;
        }
}

void RemoveFromTransparentList(edict_t *ent)
{
        transparent_list_t *p, *q, *r;

        if (transparent_list != NULL)
        {
                if (transparent_list->ent == ent)
                {
                        q = transparent_list->next;
                        gi.TagFree(transparent_list);
                        transparent_list = q;
                        return;
                }
                        else
                {
                        p = transparent_list;
                        q = p->next;
                        while (q != NULL)
                        {
                                if (q->ent == ent)
                                {
                                        r = q->next;
                                        gi.TagFree(q);
                                        p->next = r;
                                        return;
                                }
                                p = p->next;
                                q = p->next;
                        }
                }
        }

//possible bug but not bad so don't display this message when and if it occurs
//        gi.dprintf("Warning: attempt to RemoveFromTransparentList when not in it\n");
//		gi.bprintf(PRINT_HIGH,"DEBUG: Warning: attempt to RemoveFromTransparentList when not in it\n");
}               

void TransparentListSet(solid_t solid_type)
{
        transparent_list_t *p = transparent_list;

        while (p != NULL)
        {
			// V2.21 FIX - 1.20.007.01 (following outer IF-ELSE code block added)
			if (p->ent->classname)
			{
				if (!Q_stricmp(p->ent->classname,"player"))
				{
					if (p->ent->client->pers.spectator == 1)
						p = p->next;
					else
					{
						p->ent->solid = solid_type;
						gi.linkentity(p->ent);
						p = p->next;
					}
				}
				else
				{
					p->ent->solid = solid_type;
					gi.linkentity(p->ent);
					p = p->next;
				}
			}
			else
			{
				p->ent->solid = solid_type;
				gi.linkentity(p->ent);
				p = p->next;
			}
        }
}

void ReprintMOTD(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PrintMOTD(ent);
}

void SelectWeaponJM(edict_t *ent, pmenu_t *p)
{
	if (ent->client->pers.spectator)
	{
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
		return;
	}
	ent->client->newweapon = FindItem( MARKER_NAME );
	ChangeWeapon (ent);
}

void ToggleLaser(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode laser");
	Cmd_Toggle(ent);
}

void ToggleSilencer(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode silencer");
	Cmd_Toggle(ent);
}

void ToggleSlippers(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode slippers");
	Cmd_Toggle(ent);
}

void ToggleVest(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode vest");
	Cmd_Toggle(ent);
}

void ToggleIR(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode ir");
	Cmd_Toggle(ent);
}

void ToggleMarkJumps(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode markjumps");
	Cmd_Toggle(ent);
}

void ToggleMarkFalls(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode markfalls");
	Cmd_Toggle(ent);
}

void ToggleKickable(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode kickable");
	Cmd_Toggle(ent);
}

void ToggleSolid(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode teamjump");
	Cmd_Toggle(ent);
}

void ResetHighSpeed(edict_t *ent, pmenu_t *p)
{
	ent->client->resp.highspeed = 0;
}

void DemoMode(edict_t *ent,pmenu_t *p)
{
	Cmd_DemoMode_f(ent);
}

void Cmd_Puppet(edict_t *ent, pmenu_t *p)
{
	Puppet_Spawn(ent,false,false);
}

void MLCA(edict_t *ent, pmenu_t *p)
{
	Cmd_PMLCA_f(ent);
}

void SpawnP(edict_t *ent, pmenu_t *p)
{
	Cmd_GotoP(ent);
}

void SpawnPC(edict_t *ent, pmenu_t *p)
{
	Cmd_GotoPC(ent);
}

void ToggleHUDID(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode id");
	Cmd_Toggle(ent);
}

void ToggleHUDRangefinder(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode range");
	Cmd_Toggle(ent);
}

void ToggleGrenadeJump(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode grenadejump");
	Cmd_Toggle(ent);
}

void ToggleShootableM(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode shootable");
	Cmd_Toggle(ent);
}

void SetTeleporter (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"set");
	Cmd_Teleporter(ent);
}

void RecallTeleporter (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"recall");
	Cmd_Teleporter(ent);
}

void ToggleMarkerFlashes (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode markerflashes");
	Cmd_Toggle(ent);
}

void ToggleTrailFlashes (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode trailflashes");
	Cmd_Toggle(ent);
}

void ToggleMarkerColor (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode markercolor");
	Cmd_Toggle(ent);
}

void ToggleMarkerText(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode markertext");
	Cmd_Toggle(ent);
}

void ToggleTrailing(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode trailing");
	Cmd_Toggle(ent);
}

void TogglePuppetTrans(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode puppettrans");
	Cmd_Toggle(ent);
}

void TogglePuppetShoot(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode puppetshoot");
	Cmd_Toggle(ent);
}

void TogglePuppetSight(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode puppetsight");
	Cmd_Toggle(ent);
}

void TogglePuppetShootArea(edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode puppetshootarea");
	Cmd_Toggle(ent);
}

void Cmd_RemVM (edict_t *ent, pmenu_t *p)
{
	RemoveMarkersInView(ent);
}

void Cmd_RemAM (edict_t *ent, pmenu_t *p)
{
	RemoveAllMarkers(ent);
}

void RemViewTrails (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_LinksInView(ent);
}

void RemAllTrails (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_All(ent);
}

void RemLastTrail (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_Last(ent);
}

void CycleTrailColor (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode trailcolor");
	Cmd_Toggle(ent);
}

void GotoTrailStart (edict_t *ent, pmenu_t *p)
{
	Goto_LaserTrail_Start(ent);
}

void GotoTrailEnd (edict_t *ent, pmenu_t *p)
{
	Goto_LaserTrail_End(ent);
}

//Puppet Demo Menu

void PDMenu_precord(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_precord(ent);
}

void PDMenu_pstop(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_pstop(ent);
}

void PDMenu_pplay(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_pplay(ent);
}

void PDMenu_psminus(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_psminus(ent);
}

void PDMenu_psplus(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_psplus(ent);
}

void PDMenu_pspause(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_pspause(ent);
}

void PDMenu_psstepplus(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_psstepplus(ent);
}

void PDMenu_psstepminus(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_psstepminus(ent);
}

void PDMenu_pclear(edict_t *ent, pmenu_t *p)
{
	PuppetDemo_pclear(ent);
}

//Trail Menu - Removing
void TM_Delete_All (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_All(ent);
}

void TM_Delete_Last (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_Last (ent);
}

void TM_Delete_Point (edict_t *ent, pmenu_t *p)
{
	int mass = GetBestTrailMassByView(ent);
	if (mass != -1)
		DeleteLaserTrail_By_Mass (ent,mass,"View");
}

void TM_Delete_ViewT (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_Viewed(ent);
}

void TM_Delete_Touch (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_By_Mass(ent,ent->client->resp.trailtouch,"Touching");
}

void TM_Delete_ViewL (edict_t *ent, pmenu_t *p)
{
	DeleteLaserTrail_LinksInView(ent);
}

//Trail Menu - Cleaning
void TM_Cleaning_All (edict_t *ent, pmenu_t *p)
{
	CleanLaserTrail_All(ent);
}

void TM_Cleaning_Last (edict_t *ent, pmenu_t *p)
{
	CleanLaserTrail_Last (ent);
}

void TM_Cleaning_Point (edict_t *ent, pmenu_t *p)
{
	int mass = GetBestTrailMassByView(ent);
	if (mass != -1)
		CleanLaserTrail_By_Mass (ent,mass,"View");
}

void TM_Cleaning_ViewT (edict_t *ent, pmenu_t *p)
{
	CleanLaserTrail_Viewed(ent);
}

void TM_Cleaning_Touch (edict_t *ent, pmenu_t *p)
{
	CleanLaserTrail_By_Mass(ent,ent->client->resp.trailtouch,"Touching");
}

void TM_Cleaning_ViewL (edict_t *ent, pmenu_t *p)
{
	CleanLaserTrail_LinksInView(ent);
}


void FBMenuNext (edict_t *ent, pmenu_t *p)
{
	int total_demos=0,total_pages=0;
	int amt=ent->client->fb_menu_max;
	ent->client->fb_menu_page++;

	total_demos = GetFileCountForMap_PORT(ent->client->fb_menu_type);
	total_pages = total_demos / amt;
	if (total_demos % amt != 0)
		total_pages++;

	if (ent->client->fb_menu_page > total_pages)
		ent->client->fb_menu_page = total_pages;
	else
	{
		PMenu_Close(ent);
		OpenFBMenu(ent,100);
	}
}

void FBMenuPrev (edict_t *ent, pmenu_t *p)
{
	ent->client->fb_menu_page--;

	if (ent->client->fb_menu_page < 1)
		ent->client->fb_menu_page = 1;
	else
	{
		PMenu_Close(ent);
		OpenFBMenu(ent,101);
	}
}

void FBMenuLoadDemo (edict_t *ent, pmenu_t *p)
{
	LoadPuppetDemoFile_ByName(ent,p->text);
}

void FBMenuLoadTrail (edict_t *ent, pmenu_t *p)
{
	LoadJTRAIL(ent,p->text,false,0,0,0);
}

void FBMenuLoadMarker (edict_t *ent, pmenu_t *p)
{
	LoadJLOC(ent,p->text);
}

//HUD Menu

void HUD_hud (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode hud");
	Cmd_Toggle(ent);
}
void HUD_speed (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode hud_speed");
	Cmd_Toggle(ent);
}
void HUD_highspeed (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode hud_highspeed");
	Cmd_Toggle(ent);
}
void HUD_lastfd (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode hud_lastfalldamage");
	Cmd_Toggle(ent);
}
void HUD_gravity (edict_t *ent, pmenu_t *p)
{
	strcpy(gi.args(),"togglecode hud_gravity");
	Cmd_Toggle(ent);
}
//------------------------- MENU GOTOS -------------------------

//BACK

void TeleMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,2);
}

void MiscMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,3);
}

void ItemMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,4);
}

void PuppetMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,5);
}

void PuppetDMenuGoToPuppet(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenPuppetMenu(ent,8);
}

void PModeMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,6);
}

void MarkerMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,7);
}

void TrailMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,8);
}

void TrailCMenuGoToTrail(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTrailMenu(ent,9);
}

void TrailDMenuGoToTrail(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTrailMenu(ent,10);
}

void HUDMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,9);
}

void FBSelMenuGoToMain(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMainMenu(ent,10);
}

void FBGoToFBMenuSel(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenFBSelMenu(ent,ent->client->fb_menu_type+1);
}

//FORWARD

void MenuGoToTele(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTeleMenu(ent);
}

void MenuGoToMisc(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMiscMenu(ent);
}

void MenuGoToItem(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenItemMenu(ent);
}

void MenuGoToPuppet(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenPuppetMenu(ent,2);
}

void MenuGoToPuppet_Demo(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenPuppetMenu_Demo(ent);
}

void MenuGoToPMode(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenPModeMenu(ent);
}

void MenuGoToMarker(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenMarkerMenu(ent);
}

void MenuGoToTrail(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTrailMenu(ent,2);
}

void MenuGoToTrail_Cleaning(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTrailMenu_Cleaning(ent);
}

void MenuGoToTrail_Delete(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenTrailMenu_Delete(ent);
}

void MenuGoToHUD(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenHUDMenu(ent,1);
}

void MenuGoToFB(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);
	OpenFBSelMenu(ent,1);
}

void MenuGoToFB_PD(edict_t *ent, pmenu_t *p)
{
	ent->client->fb_menu_type = FTYPE_PDEMO;
	PMenu_Close(ent);
	OpenFBMenu(ent,1);
}

void MenuGoToFB_JT(edict_t *ent, pmenu_t *p)
{
	ent->client->fb_menu_type = FTYPE_JTRAIL;
	PMenu_Close(ent);
	OpenFBMenu(ent,1);
}

void MenuGoToFB_JL(edict_t *ent, pmenu_t *p)
{
	ent->client->fb_menu_type = FTYPE_JLOC;
	PMenu_Close(ent);
	OpenFBMenu(ent,1);
}

//------------------------- MENU GOTOS -------------------------

//PaTMaN - Main Menu
pmenu_t mainmenu[] = {
  //"^J Mod v0.00 - By PaTMaN^J"
  {"*^J Mod v" JMOD_VERSION JMOD_REVISION " - By PaTMaN^J",			PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "----------------------------",						PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*-> Teleporter",									PMENU_ALIGN_LEFT,	NULL, MenuGoToTele			},
  { "*-> Misc",											PMENU_ALIGN_LEFT,	NULL, MenuGoToMisc			},
  { "*-> Item",											PMENU_ALIGN_LEFT,	NULL, MenuGoToItem			},
  { "*-> Puppet",										PMENU_ALIGN_LEFT,	NULL, MenuGoToPuppet		},
  { "*-> Player Mode",									PMENU_ALIGN_LEFT,	NULL, MenuGoToPMode			},
  { "*-> Marker",										PMENU_ALIGN_LEFT,	NULL, MenuGoToMarker		},
  { "*-> Trail",										PMENU_ALIGN_LEFT,	NULL, MenuGoToTrail			},
  { "*-> HUD",											PMENU_ALIGN_LEFT,	NULL, MenuGoToHUD			},
  { "*-> File Browser",									PMENU_ALIGN_LEFT,	NULL, MenuGoToFB			},
};

//PaTMaN - HUD Menu
pmenu_t hudmenu[] = {
  {"*HUD Menu                   (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Toggle HUD             (tog hud)",					PMENU_ALIGN_LEFT,	NULL, HUD_hud				},
  { "Toggle Speed           (tog hud_speed)",			PMENU_ALIGN_LEFT,	NULL, HUD_speed				},
  { "Toggle High Speed      (tog hud_highspeed)",		PMENU_ALIGN_LEFT,	NULL, HUD_highspeed			},
  { "Toggle Fall Damage     (tog hud_lastfd)",			PMENU_ALIGN_LEFT,	NULL, HUD_lastfd			},
  { "Toggle Gravity         (tog hud_gravity)",			PMENU_ALIGN_LEFT,	NULL, HUD_gravity			},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, HUDMenuGoToMain		},
};

//PaTMaN - File Browser Menu
pmenu_t fbmenu[] = {
  {"*File Browser Menu",								PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "----------------------------",						PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*-> Puppet Demo Browser",							PMENU_ALIGN_LEFT,	NULL, MenuGoToFB_PD			},
  { "*-> Trail Browser",								PMENU_ALIGN_LEFT,	NULL, MenuGoToFB_JT			},
  { "*-> Marker Browser",								PMENU_ALIGN_LEFT,	NULL, MenuGoToFB_JL			},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, FBSelMenuGoToMain		},
};

//PaTMaN - Teleporter Menu
pmenu_t telemenu[] = {
  {"*Teleporter Menu            (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Set Teleporter             (teleporter set)",		PMENU_ALIGN_LEFT,	NULL, SetTeleporter			},
  { "Recall Teleporter          (teleporter recall)",	PMENU_ALIGN_LEFT,	NULL, RecallTeleporter		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, TeleMenuGoToMain		},
};

//PaTMaN - Misc Menu
pmenu_t miscmenu[] = {
  {"*Misc Menu                  (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "LIGHTS CAMERA ACTION!      (lca)",					PMENU_ALIGN_LEFT,	NULL, MLCA					},
  { "Random Spawn               (spawnp)",				PMENU_ALIGN_LEFT,	NULL, SpawnP				},
  { "Closest Spawn              (spawnc)",				PMENU_ALIGN_LEFT,	NULL, SpawnPC				},
  { "Reset High Speed           (rhs)",					PMENU_ALIGN_LEFT,	NULL, ResetHighSpeed		},
  { "Reset All Hud Stats        (demomode)",			PMENU_ALIGN_LEFT,	NULL, DemoMode				},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, MiscMenuGoToMain		},
};

//PaTMaN - Puppet Menu
pmenu_t puppetmenu[] = {
  {"*Puppet Control Menu        (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Cycle On/Off               (puppet)",				PMENU_ALIGN_LEFT,	NULL, Cmd_Puppet			},
  { "Toggle Transparency        (tog puppettrans)",		PMENU_ALIGN_LEFT,	NULL, TogglePuppetTrans		},
  { "Toggle Puppet Shooting     (tog puppetshoot)",		PMENU_ALIGN_LEFT,	NULL, TogglePuppetShoot		},
  { "Toggle Puppet Sight Line   (tog puppetsight)",		PMENU_ALIGN_LEFT,	NULL, TogglePuppetSight		},
  { "Toggle Puppet Shoot Area   (tog puppetshootarea)",	PMENU_ALIGN_LEFT,	NULL, TogglePuppetShootArea	},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*-> Puppet Demo",									PMENU_ALIGN_LEFT,	NULL, MenuGoToPuppet_Demo	},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, PuppetMenuGoToMain	},
};

//PaTMaN - Puppet Demo Menu
pmenu_t puppetmenu_demo[] = {
  {"*Puppet Demo Menu           (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Start Recording            (precord)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_precord		},
  { "Stop Recording/Playing     (pstop)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_pstop			},
  { "Play Demo                  (pplay)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_pplay			},
  { "Increase Playback Speed    (ps+)",					PMENU_ALIGN_LEFT,	NULL, PDMenu_psplus			},
  { "Decrease Playback Speed    (ps-)",					PMENU_ALIGN_LEFT,	NULL, PDMenu_psminus		},
  { "Pause Playback             (pspause)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_pspause		},
  { "Step Forward 1 Frame       (psstep+)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_psstepplus		},
  { "Step Backward 1 Frame      (psstep-)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_psstepminus	},
  { "Clear Demo                 (pclear)",				PMENU_ALIGN_LEFT,	NULL, PDMenu_pclear			},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Puppet",										PMENU_ALIGN_LEFT,	NULL, PuppetDMenuGoToPuppet	},
};

//PaTMaN - Item Menu
pmenu_t itemmenu[] = {
  {"*Item Menu                  (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Laser sight                (tog laser)",			PMENU_ALIGN_LEFT,	NULL, ToggleLaser			},
  { "Silencer                   (tog silencer)",		PMENU_ALIGN_LEFT,	NULL, ToggleSilencer		},
  { "Slippers                   (tog slippers)",		PMENU_ALIGN_LEFT,	NULL, ToggleSlippers		},
  { "Vest                       (tog vest)",			PMENU_ALIGN_LEFT,	NULL, ToggleVest			},
  { "IR Vision                  (tog ir)",				PMENU_ALIGN_LEFT,	NULL, ToggleIR				},
  { "Player ID                  (tog id)",				PMENU_ALIGN_LEFT,	NULL, ToggleHUDID			},
  { "Range Finder               (tog range)",			PMENU_ALIGN_LEFT,	NULL, ToggleHUDRangefinder	},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, ItemMenuGoToMain		},
};

//PaTMaN - Player Mode Menu
pmenu_t pmodemenu[] = {
  {"*Player Mode Menu           (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Kickable                   (tog kickable)",		PMENU_ALIGN_LEFT,	NULL, ToggleKickable		},
  { "Team Jumping               (tog teamjump)",		PMENU_ALIGN_LEFT,	NULL, ToggleSolid			}, //V1.21 FIX - 1.20.005.01
  { "Grenade Jumping            (tog grenadejump)",		PMENU_ALIGN_LEFT,	NULL, ToggleGrenadeJump		},
  { "Shootable                  (tog shootable)",		PMENU_ALIGN_LEFT,	NULL, ToggleShootableM		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, PModeMenuGoToMain		},
};

//PaTMaN - Marker Menu
pmenu_t markermenu[] = {
  {"*Jump Marker Menu           (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Use Jump Marker            (use jump marker)",		PMENU_ALIGN_LEFT,	NULL, SelectWeaponJM		},
  { "Remove all markers in view (remviewmarkers)",		PMENU_ALIGN_LEFT,	NULL, Cmd_RemVM				},
  { "Remove all markers in map  (remallmarkers)",		PMENU_ALIGN_LEFT,	NULL, Cmd_RemAM				},
  { "Cycle Marker Color         (tog markercolor)",		PMENU_ALIGN_LEFT,	NULL, ToggleMarkerColor		},
  { "Toggle Marker Flashes      (tog markerflashes)",	PMENU_ALIGN_LEFT,	NULL, ToggleMarkerFlashes	},
  { "Toggle Marker Text         (tog markertext)",		PMENU_ALIGN_LEFT,	NULL, ToggleMarkerText		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Mark Jumps                 (tog markjumps)",		PMENU_ALIGN_LEFT,	NULL, ToggleMarkJumps		},
  { "Mark Falls                 (tog markfalls)",		PMENU_ALIGN_LEFT,	NULL, ToggleMarkFalls		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, MarkerMenuGoToMain	},
};

//PaTMaN - Trail Menu
pmenu_t trailmenu[] = {
  {"*Trail Menu                 (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Toggle Trailing            (tog trailing)",		PMENU_ALIGN_LEFT,	NULL, ToggleTrailing		},
  { "Cycle Trail Color          (tog trailcolor)",		PMENU_ALIGN_LEFT,	NULL, CycleTrailColor		},
  { "Toggle Trail Flashes       (tog trailflashes)",	PMENU_ALIGN_LEFT,	NULL, ToggleTrailFlashes	},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Goto Trail Start           (goto trail start)",	PMENU_ALIGN_LEFT,	NULL, GotoTrailStart		},
  { "Goto Trail End             (goto trail end)",		PMENU_ALIGN_LEFT,	NULL, GotoTrailEnd			},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*-> Trail Cleaning",								PMENU_ALIGN_LEFT,	NULL, MenuGoToTrail_Cleaning},
  { "*-> Trail Deletion",								PMENU_ALIGN_LEFT,	NULL, MenuGoToTrail_Delete	},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Main",											PMENU_ALIGN_LEFT,	NULL, TrailMenuGoToMain		},
};

//PaTMaN - Trail Menu - Cleaning
pmenu_t trailmenu_cleaning[] = {
  {"*Trail Cleaning Menu         (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Clean All Trails            (cleanalltrails)",		PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_All		},
  { "Clean Viewed Trails         (cleanviewtrails)",	PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_ViewT		},
  { "Clean Viewed Links          (cleanviewlinks)",		PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_ViewL		},
  { "Clean Last Trail            (cleanlasttrail)",		PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_Last		},
  { "Clean Last-Touched/Touching (cleantouchtrail)",	PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_Touch		},
  { "Clean Pointing Trail        (cleanpointtrail)",	PMENU_ALIGN_LEFT,	NULL, TM_Cleaning_Point		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Trail",										PMENU_ALIGN_LEFT,	NULL, TrailCMenuGoToTrail	},
};

//PaTMaN - Trail Menu - Deleting
pmenu_t trailmenu_delete[] = {
  {"*Trail Cleaning Menu          (command binds)",		PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "-----------------------------------------------",	PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "Delete All Trails            (remalltrails)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_All			},
  { "Delete Viewed Trails         (remviewtrails)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_ViewT		},
  { "Delete Viewed Links          (remviewlinks)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_ViewL		},
  { "Delete Last Trail            (remlasttrail)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_Last		},
  { "Delete Last-Touched/Touching (remtouchtrail)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_Touch		},
  { "Delete Pointing Trail        (rempointtrail)",		PMENU_ALIGN_LEFT,	NULL, TM_Delete_Point		},
  { NULL,												PMENU_ALIGN_LEFT,	NULL, NULL					},
  { "*<- Trail",										PMENU_ALIGN_LEFT,	NULL, TrailDMenuGoToTrail	},
};

//Puppet Demo Browser Layout (MAX 15 ITEMS [2] - [16])
pmenu_t pdbmenu[] = {
  {"*Puppet Demo Listing (Page 1/1)",					PMENU_ALIGN_LEFT,	NULL, NULL				}, //title
  { "--------------------------------",					PMENU_ALIGN_LEFT,	NULL, NULL				}, //divider
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //first item 1
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //last item 15
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //first item 16
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				},
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //last item 30
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //null
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //next
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //prev
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //null
  { "",													PMENU_ALIGN_LEFT,	NULL, NULL				}, //back
};

void OpenMainMenu(edict_t *ent,int cur)
{
        PMenu_Open(ent, mainmenu, cur, sizeof(mainmenu) / sizeof(pmenu_t));
}

void OpenTeleMenu(edict_t *ent)
{
        PMenu_Open(ent, telemenu, 2, sizeof(telemenu) / sizeof(pmenu_t));
}

void OpenMiscMenu(edict_t *ent)
{
        PMenu_Open(ent, miscmenu, 2, sizeof(miscmenu) / sizeof(pmenu_t));
}

void OpenItemMenu(edict_t *ent)
{
        PMenu_Open(ent, itemmenu, 2, sizeof(itemmenu) / sizeof(pmenu_t));
}

void OpenPuppetMenu(edict_t *ent,int cur)
{
        PMenu_Open(ent, puppetmenu, cur, sizeof(puppetmenu) / sizeof(pmenu_t));
}

void OpenPuppetMenu_Demo(edict_t *ent)
{
        PMenu_Open(ent, puppetmenu_demo, 2, sizeof(puppetmenu_demo) / sizeof(pmenu_t));
}

void OpenPModeMenu(edict_t *ent)
{
        PMenu_Open(ent, pmodemenu, 2, sizeof(pmodemenu) / sizeof(pmenu_t));
}

void OpenMarkerMenu(edict_t *ent)
{
        PMenu_Open(ent, markermenu, 2, sizeof(markermenu) / sizeof(pmenu_t));
}

void OpenTrailMenu(edict_t *ent,int cur)
{
        PMenu_Open(ent, trailmenu, cur, sizeof(trailmenu) / sizeof(pmenu_t));
}

void OpenTrailMenu_Cleaning(edict_t *ent)
{
        PMenu_Open(ent, trailmenu_cleaning, 2, sizeof(trailmenu_cleaning) / sizeof(pmenu_t));
}

void OpenTrailMenu_Delete(edict_t *ent)
{
        PMenu_Open(ent, trailmenu_delete, 2, sizeof(trailmenu_delete) / sizeof(pmenu_t));
}

void OpenHUDMenu(edict_t *ent, int cur)
{
	PMenu_Open(ent, hudmenu, cur, sizeof(hudmenu) / sizeof(pmenu_t));
}

void OpenFBSelMenu(edict_t *ent, int cur)
{
	PMenu_Open(ent, fbmenu, cur, sizeof(fbmenu) / sizeof(pmenu_t));
}

void OpenFBMenu(edict_t *ent, int cur)
{

// [0] = "*Puppet Demo Listing (Page %i/%i)",cur_page,total_pages
// [1] = 
// [2] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [3] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [4] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [5] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [6] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [7] = 
// [8] = cur_page + 1  &&  function to redraw menu
// [9] = cur_page - 1  &&  function to redraw menu
//[10] = 
//[11] = 

	char pdfile[256]="";
	int total_demos=0;
	int total_pages=0;
	int cur_page_start=0;
	int i=0;
	qboolean np;
	qboolean pp;
	int pos_np;
	int pos_pp;
	int pos_back;
	int amt;

	amt = ent->client->fb_menu_max;
	total_demos = GetFileCountForMap_PORT(ent->client->fb_menu_type);

	total_pages = total_demos / amt;
	if (total_demos % amt != 0)
		total_pages++;

	//cur_page_start is actually the demo # to start on, not page #
	cur_page_start = (ent->client->fb_menu_page - 1) * amt;

	if ((cur_page_start / amt) + 1 == total_pages)
		amt = total_demos - cur_page_start;

	if (ent->client->fb_menu_type == FTYPE_PDEMO)
		sprintf(pdfile,"*Puppet Demo Listing (Page %i/%i)",ent->client->fb_menu_page,total_pages);
	else if (ent->client->fb_menu_type == FTYPE_JTRAIL)
		sprintf(pdfile,"*Trail Listing (Page %i/%i)",ent->client->fb_menu_page,total_pages);
	else if (ent->client->fb_menu_type == FTYPE_JLOC)
		sprintf(pdfile,"*Marker Listing (Page %i/%i)",ent->client->fb_menu_page,total_pages);
	pdbmenu[0].text = strdup(pdfile);

	//list the demos
	for (i=2;i<amt+2;i++)
	{
		sprintf(pdfile,"%s",GetFileFromFList_PORT(cur_page_start - 1 + i,ent->client->fb_menu_type));
		if (!Q_stricmp(pdfile,"(null)"))
		{
			pdbmenu[i].text = strdup("");
			pdbmenu[i].SelectFunc = NULL;
		}
		else
		{
			pdbmenu[i].text = strdup(pdfile);
			if (ent->client->fb_menu_type == FTYPE_PDEMO)
				pdbmenu[i].SelectFunc = FBMenuLoadDemo;
			else if (ent->client->fb_menu_type == FTYPE_JTRAIL)
				pdbmenu[i].SelectFunc = FBMenuLoadTrail;
			else if (ent->client->fb_menu_type == FTYPE_JLOC)
				pdbmenu[i].SelectFunc = FBMenuLoadMarker;
		}
	}

	//clear the rest of the structure for writing to
	for (i;i<37;i++)
	{
		pdbmenu[i].text = strdup("");
		pdbmenu[i].SelectFunc = NULL;
	}

//8 "*Next Page",		PMENU_ALIGN_LEFT,	NULL, PDBMenuNext		},
//9 "*Previous Page",	PMENU_ALIGN_LEFT,	NULL, PDBMenuPrev		},

	pos_np = amt+3;
	pos_pp = amt+4;
	if ((ent->client->fb_menu_page < total_pages) && (total_pages > 1))
	{
		pdbmenu[pos_np].text = strdup("*Next Page");
		pdbmenu[pos_np].SelectFunc = FBMenuNext;
		np = true;
	}
	else
		np = false;

	if ((ent->client->fb_menu_page > 1) && (total_pages > 1))
	{
		pdbmenu[pos_pp].text = strdup("*Previous Page");
		pdbmenu[pos_pp].SelectFunc = FBMenuPrev;
		pp = true;
	}
	else
		pp = false;

	if (cur == 100 && np == false) cur = 101;
	if (cur == 101 && pp == false) cur = 100;

	pos_back = amt+6;
	pdbmenu[pos_back].text = strdup("*<- File Browser");
	pdbmenu[pos_back].SelectFunc = FBGoToFBMenuSel;

	if (cur == 100)
		cur = pos_np;
	else if (cur == 101)
		cur = pos_pp;

	ent->client->inbrowsermenu = true;

	PMenu_Open(ent, pdbmenu, cur, sizeof(pdbmenu) / sizeof(pmenu_t));
}

/*
void OpenPDBMenu(edict_t *ent, int cur)
{

// [0] = "*Puppet Demo Listing (Page %i/%i)",cur_page,total_pages
// [1] = 
// [2] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [3] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [4] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [5] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [6] = "%s",pdfile info line  &&  arg set to file path/name  &&  function to load pdfile
// [7] = 
// [8] = cur_page + 1  &&  function to redraw menu
// [9] = cur_page - 1  &&  function to redraw menu
//[10] = 
//[11] = 

	char pdfile[256]="";
	int total_demos=0;
	int total_pages=0;
	int cur_page_start=0;
	int i=0;
	qboolean np = true;
	qboolean pp = true;

	total_demos = GetFileCountForMap_PORT(FTYPE_PDEMO);

	total_pages = total_demos / 5;
	if (total_demos % 5 != 0)
		total_pages++;

	cur_page_start = (ent->client->pdbmenu_page - 1) * 5;

	sprintf(pdfile,"*Puppet Demo Listing (Page %i/%i)",ent->client->pdbmenu_page,total_pages);
	pdbmenu[0].text = strdup(pdfile);

//8 "*Next Page",		PMENU_ALIGN_LEFT,	NULL, PDBMenuNext		},
//9 "*Previous Page",	PMENU_ALIGN_LEFT,	NULL, PDBMenuPrev		},

	if (ent->client->pdbmenu_page < total_pages && total_pages > 1)
	{
		pdbmenu[8].text = strdup("*Next Page");
		pdbmenu[8].SelectFunc = PDBMenuNext;
	}
	else {
		pdbmenu[8].text = strdup("");
		pdbmenu[8].SelectFunc = NULL;
		np = false;
	}

	if (ent->client->pdbmenu_page > 1 && total_pages > 1)
	{
		pdbmenu[9].text = strdup("*Previous Page");
		pdbmenu[9].SelectFunc = PDBMenuPrev;
	}
	else {
		pdbmenu[9].text = strdup("");
		pdbmenu[9].SelectFunc = NULL;
		pp = false;
	}

	if (cur == 8 && np == false) cur = 9;
	if (cur == 9 && pp == false) cur = 8;

	for (i=2;i<7;i++)
	{
		sprintf(pdfile,"%s",GetPuppetDemoFromPList(cur_page_start - 1 + i));
		if (!Q_stricmp(pdfile,"(null)"))
		{
			pdbmenu[i].text = strdup("");
			pdbmenu[i].SelectFunc = NULL;
		}
		else
		{
			pdbmenu[i].text = strdup(pdfile); //GetPuppetDemoFileInfo(pdfile,true,false));
			pdbmenu[i].SelectFunc = PDBMenuLoadDemo;
		}
	}

	PMenu_Open(ent, pdbmenu, cur, sizeof(pdbmenu) / sizeof(pmenu_t));
}
*/

//void OpenTrailMenu_Deleting(edict_t *ent)
//{
//        PMenu_Open(ent, trailmenu_deleting, 2, sizeof(trailmenu_deleting) / sizeof(pmenu_t));
//}

/*//PaTMaN-NOMENUS
pmenu_t weapmenu[] = {
  { "*Action Quake 2",                 PMENU_ALIGN_CENTER, NULL, NULL },
  { "--------------",                   PMENU_ALIGN_CENTER, NULL, NULL },
  { "Select your weapon",               PMENU_ALIGN_CENTER, NULL, NULL },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { "MP5/10 Submachinegun",             PMENU_ALIGN_LEFT, NULL, SelectWeapon2 },
  { "M3 Super90 Assault Shotgun",       PMENU_ALIGN_LEFT, NULL, SelectWeapon3 },
  { "Handcannon",                       PMENU_ALIGN_LEFT, NULL, SelectWeapon4 },
  { "SSG 3000 Sniper Rifle",              PMENU_ALIGN_LEFT, NULL, SelectWeapon5 },
  { "M4 Assault Rifle",                   PMENU_ALIGN_LEFT, NULL, SelectWeapon6 },
  { "Combat Knives",                      PMENU_ALIGN_LEFT, NULL, SelectWeapon0 },
  { "Akimbo Pistols",                     PMENU_ALIGN_LEFT, NULL, SelectWeapon9 },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { "Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL },
  { "ENTER to select",    PMENU_ALIGN_LEFT, NULL, NULL },
  { "TAB to exit menu",    PMENU_ALIGN_LEFT, NULL, NULL },
  { NULL,                                 PMENU_ALIGN_LEFT, NULL, NULL },
  { "v" ACTION_VERSION,       PMENU_ALIGN_RIGHT, NULL, NULL },
};

pmenu_t itemmenu[] = {
  { "*Action Quake 2",                 PMENU_ALIGN_CENTER, NULL, NULL },
  { "--------------",                   PMENU_ALIGN_CENTER, NULL, NULL },
  { "Select your item",               PMENU_ALIGN_CENTER, NULL, NULL },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { "Kevlar Vest",             PMENU_ALIGN_LEFT, NULL, SelectItem1 },
  { "Laser Sight",       PMENU_ALIGN_LEFT, NULL, SelectItem2 },
  { "Stealth Slippers",                       PMENU_ALIGN_LEFT, NULL, SelectItem3 },
  { "Silencer",              PMENU_ALIGN_LEFT, NULL, SelectItem4 },
  { "Bandolier",                   PMENU_ALIGN_LEFT, NULL, SelectItem5 },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { "Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL },
  { "ENTER to select",    PMENU_ALIGN_LEFT, NULL, NULL },
  { "TAB to exit menu",    PMENU_ALIGN_LEFT, NULL, NULL },
  { NULL,                                 PMENU_ALIGN_LEFT, NULL, NULL },
  { "v" ACTION_VERSION,       PMENU_ALIGN_RIGHT, NULL, NULL },
};
*///PaTMaN-NOMENUS
/*//PaTMaN-NOMENUS
pmenu_t joinmenu[] = {
  { "*Action Quake 2",                 PMENU_ALIGN_CENTER, NULL, NULL },
  { NULL /* lvl name */ /*//PaTMaN-NOMENUS,                PMENU_ALIGN_CENTER, NULL, NULL },
  { NULL,                               PMENU_ALIGN_CENTER, NULL, NULL },
  { NULL /* team 1 */ /*//PaTMaN-NOMENUS,                  PMENU_ALIGN_LEFT, NULL, JoinTeam1 },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { NULL /* team 2 */ /*//PaTMaN-NOMENUS,                  PMENU_ALIGN_LEFT, NULL, JoinTeam2 },
  { NULL,                               PMENU_ALIGN_LEFT, NULL, NULL },
  { NULL,                                 PMENU_ALIGN_LEFT, NULL, NULL },
  { "MOTD",                               PMENU_ALIGN_LEFT, NULL, ReprintMOTD },
  { "Credits",                            PMENU_ALIGN_LEFT, NULL, CreditsMenu },
  { NULL,                                 PMENU_ALIGN_LEFT, NULL, NULL },
  { "Use [ and ] to move cursor", PMENU_ALIGN_LEFT, NULL, NULL },
  { "ENTER to select",    PMENU_ALIGN_LEFT, NULL, NULL },
  { "TAB to exit menu",   PMENU_ALIGN_LEFT, NULL, NULL },
  { NULL,                                 PMENU_ALIGN_LEFT, NULL, NULL },
  { "v" ACTION_VERSION,       PMENU_ALIGN_RIGHT, NULL, NULL },
};
*///PaTMaN-NOMENUS
/*//PaTMaN-NOMENUS
void CreditsMenu(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, creditsmenu, 4, sizeof(creditsmenu) / sizeof(pmenu_t));
}

char *TeamName(int team)
{
        if (team == TEAM1)
                return team1_name;
                else if (team == TEAM2)
                return team2_name;
                else
                return "None";
}

void AssignSkin(edict_t *ent, char *s)
{
        int playernum = ent-g_edicts-1;

        switch (ent->client->resp.team)
        {
                case TEAM1:
                        gi.configstring (CS_PLAYERSKINS+playernum, va("%s\\%s", 
                                ent->client->pers.netname, team1_skin) );
                        break;
                case TEAM2:
                        gi.configstring (CS_PLAYERSKINS+playernum,
                                va("%s\\%s", ent->client->pers.netname, team2_skin) );
                        break;
                default:
                        gi.configstring (CS_PLAYERSKINS+playernum, 
                                va("%s\\%s", ent->client->pers.netname, s) );
                        break;
        }
}

void Team_f(edict_t *ent)
{
        char *t;
        int desired_team;

        t = gi.args();
        if (!*t)
        {
                gi.cprintf(ent, PRINT_HIGH, "You are on %s.\n",
                                TeamName(ent->client->resp.team));
                return;
        }

        if (level.framenum < (ent->client->resp.joined_team + 50))
        {
                gi.cprintf(ent, PRINT_HIGH, "You must wait 5 seconds before changing teams again.\n");
                return;
        }

        if (Q_stricmp(t, "none") == 0)
        {
                if (ent->client->resp.team == NOTEAM)
                {
                        gi.cprintf(ent, PRINT_HIGH, "You're not on a team.\n");
                }
                        else
                {
                        LeaveTeam(ent);
                }
                return;
        }

        if (Q_stricmp(t, "1") == 0)
                desired_team = TEAM1;
                else if (Q_stricmp(t, "2") == 0)
                desired_team = TEAM2;
                else if (Q_stricmp(t, team1_name) == 0)
                desired_team = TEAM1;
                else if (Q_stricmp(t, team2_name) == 0)
                desired_team = TEAM2;
                else
        {
                gi.cprintf(ent, PRINT_HIGH, "Unknown team %s.\n", t);
                return;
        }

        if (ent->client->resp.team == desired_team)
        {
                gi.cprintf(ent, PRINT_HIGH, "You are already on %s.\n",
                              TeamName(ent->client->resp.team));
                return;
        }

        JoinTeam(ent, desired_team, 1);
}

void UnevenTeamsMsg(int whichteam, int uneven_amount, char *opponent)
{
        int i;
        edict_t *e;

        for (i = 1; i <= maxclients->value; i++)
        {
                e = g_edicts + i;
                if (e->inuse)
                {
                        if (e->client->resp.team == whichteam)
                        {
                                gi.cprintf(e, PRINT_HIGH, "Your team now has %d more player%s than %s.\n", 
                                              uneven_amount, uneven_amount == 1 ? "" : "s", opponent);
                                stuffcmd(e, "play misc/comp_up.wav");
                        }
                }  
        }
}

void CheckForUnevenTeams()
{
        int i, onteam1 = 0, onteam2 = 0;
        edict_t *e;

        // only use these messages during 2-team games...
        if (num_teams > 2)
                return;

        for (i = 1; i <= maxclients->value; i++)
        {
                e = g_edicts + i;
                if (e->inuse)
                {
                        if (e->client->resp.team == TEAM1)
                                onteam1++;
                                else if (e->client->resp.team == TEAM2)
                                onteam2++;
                }  
        }
        if (onteam1 > onteam2)
                UnevenTeamsMsg(TEAM1, onteam1 - onteam2, team2_name);
                else if (onteam2 > onteam1)
                UnevenTeamsMsg(TEAM2, onteam2 - onteam1, team1_name);
}

void JoinTeam(edict_t *ent, int desired_team, int skip_menuclose)
{
        char *s, *a;
  
        if (!skip_menuclose)
        {
                PMenu_Close(ent);
        }

        if (ent->client->resp.team == desired_team)
                return;

        a = (ent->client->resp.team == NOTEAM) ? "joined" : "changed to";

        ent->client->resp.team = desired_team;
        s = Info_ValueForKey (ent->client->pers.userinfo, "skin");
        AssignSkin(ent, s);

        if (ent->solid != SOLID_NOT)  // alive, in game
        {
                ent->health = 0;
                player_die (ent, ent, ent, 100000, vec3_origin);
                ent->deadflag = DEAD_DEAD;
        }

        ent->client->resp.joined_team = level.framenum;

        CheckForUnevenTeams();

        if (!skip_menuclose)
                OpenWeaponMenu(ent);
}

void LeaveTeam(edict_t *ent)
{
        char *g;
  
        if (ent->client->resp.team == NOTEAM)
                return;

        if (ent->solid != SOLID_NOT)  // alive, in game
        {
                ent->health = 0;
                player_die (ent, ent, ent, 100000, vec3_origin);
                ent->deadflag = DEAD_DEAD;
        }

        if (IsNeutral(ent))
                g = "its";
                else if (IsFemale(ent))
                g = "her";
                else
                g = "his";
        gi.bprintf(PRINT_HIGH, "%s left %s team.\n", ent->client->pers.netname, g);

        ent->client->resp.joined_team = 0;
        ent->client->resp.team = NOTEAM;

        CheckForUnevenTeams();
}

void ReturnToMain(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        OpenJoinMenu(ent);
}

void OpenItemMenu(edict_t *ent)
{
        PMenu_Open(ent, itemmenu, 4, sizeof(itemmenu) / sizeof(pmenu_t));
}

void OpenWeaponMenu(edict_t *ent)
{
        PMenu_Open(ent, weapmenu, 4, sizeof(weapmenu) / sizeof(pmenu_t));
}
*///PaTMaN-NOMENUS
/*
int UpdateToggleMenu(edict_t *ent)
{
        static char levelname[32];
        static char team1players[32];
        static char team2players[32];
     //   int num1, num2, i;

        togglemenu[4].text = ent->client->resp.toggle_laser;
    //    togglemenu[4].SelectFunc = ToggleLaser;
 

        //if (num1 > num2)
                return ent->client->resp.toggle_laser;
           /*     else if (num2 > num1)
                return TEAM1;
                else if (team1_score > team2_score)
                return TEAM2;
                else if (team2_score > team1_score)
                return TEAM1;
                else
                return TEAM1;*/
//}

/*//PaTMaN-NOMENUS
int UpdateJoinMenu(edict_t *ent)
{
        static char levelname[32];
        static char team1players[32];
        static char team2players[32];
        int num1, num2, i;

        joinmenu[3].text = team1_name;
        joinmenu[3].SelectFunc = JoinTeam1;
        joinmenu[5].text = team2_name;
        joinmenu[5].SelectFunc = JoinTeam2;

        levelname[0] = '*';
        if (g_edicts[0].message)
                strncpy(levelname+1, g_edicts[0].message, sizeof(levelname) - 2);
                else
                strncpy(levelname+1, level.mapname, sizeof(levelname) - 2);
        levelname[sizeof(levelname) - 1] = 0;

        num1 = num2 = 0;
        for (i = 0; i < maxclients->value; i++)
        {
                if (!g_edicts[i+1].inuse)
                        continue;
                if (game.clients[i].resp.team == TEAM1)
                        num1++;
                        else if (game.clients[i].resp.team == TEAM2)
                        num2++;
        }

        sprintf(team1players, "  (%d players)", num1);
        sprintf(team2players, "  (%d players)", num2);

        joinmenu[1].text = levelname;
        if (joinmenu[3].text)
                joinmenu[4].text = team1players;
                else
                joinmenu[4].text = NULL;
        if (joinmenu[5].text)
                joinmenu[6].text = team2players;
                else
                joinmenu[6].text = NULL;

        if (num1 > num2)
                return TEAM2;
                else if (num2 > num1)
                return TEAM1;
                else if (team1_score > team2_score)
                return TEAM2;
                else if (team2_score > team1_score)
                return TEAM1;
                else
                return TEAM1;
}



void OpenJoinMenu(edict_t *ent)
{
        int team;

        team = UpdateJoinMenu(ent);
        if (team == TEAM1)
                team = 3;
                else
                team = 5;
        PMenu_Open(ent, joinmenu, team, sizeof(joinmenu) / sizeof(pmenu_t));
}
*/
int member_array(char *str, char *arr[], int num_elems)
{
        int l;
        for (l = 0; l < num_elems; l++)
        {
                if (!strcmp(str, arr[l]))
                        return l;
        }
        return -1;
}

void SetNodeCount(int val);

void CleanLevel()
{
		/*
                "weapon_Mk23",
                "weapon_MP5",
                "weapon_M4",
                "weapon_M3",
                "weapon_HC",
                "weapon_Sniper",
                "weapon_Dual",
				

//PaTMaN - NO AMMO

                "ammo_sniper",
                "ammo_clip",
                "ammo_mag",
                "ammo_m4",
                "ammo_m3",

			
				"item_quiet",
                "item_slippers",
                "item_band",
                "item_lasersight",
                "item_vest",
        };
		*/

        char *remove_classnames[] = 
        {
                "weapon_Knife",
				"weapon_Grenade",
				"thrown_knife",
                "hgrenade",
				"Jump Marker",
				"LaserTrailLink"
		};
        int i;
        int base;
        edict_t *ent;
 
        base = 1 + maxclients->value + BODY_QUEUE_SIZE;
        ent = g_edicts + base;
        for (i = 1 + maxclients->value + BODY_QUEUE_SIZE; 
                        i < globals.num_edicts; 
                        i++, ent++)
        {
                if (!ent->classname)
                        continue;
                if (member_array(ent->classname, remove_classnames, 
                                        sizeof(remove_classnames) / sizeof(char *)) > -1)
                {
                        G_FreeEdict(ent);
                }
        }

		SetNodeCount(1);
		for (i = 0; i < maxclients->value; i++)
		{
			if (!g_edicts[i+1].inuse)
				continue;
			game.clients[i].resp.trailcounter = 1;

		}
        CleanBodies();

        // fix glass
        CGF_SFX_RebuildAllBrokenGlass();
}

qboolean StartClient(edict_t *ent)
{
//PaTMaN-NOTEAM        if (ent->client->resp.team != NOTEAM)
//PaTMaN-NOTEAM                return false;

        // start as 'observer'
        ent->movetype = MOVETYPE_NOCLIP;
        ent->solid = SOLID_NOT;
        ent->svflags |= SVF_NOCLIENT;
//PaTMaN-NOTEAM        ent->client->resp.team = NOTEAM;
        ent->client->ps.gunindex = 0; 
        gi.linkentity (ent);

        return true;
}

void CenterPrintAll(char *msg)
{
        int i;
        edict_t *ent;

        gi.cprintf(NULL, PRINT_HIGH, msg); // so it goes to the server console...

        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
                if (!ent->inuse)
                        continue;
                gi.centerprintf(ent, "%s", msg);
        }               
}

/*//PaTMaN-NOTEAM
qboolean BothTeamsHavePlayers()
{
        int onteam1 = 0, onteam2 = 0, i;
        edict_t *ent;

        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
                if (!ent->inuse)
                        continue;
                if (game.clients[i].resp.team == TEAM1)
                        onteam1++;
                        else if (game.clients[i].resp.team == TEAM2)
                        onteam2++;
        }

        return (onteam1 > 0 && onteam2 > 0);
}

// CheckForWinner: Checks for a winner (or not).
int CheckForWinner()
{
        int onteam1 = 0, onteam2 = 0, i;
        edict_t *ent;

        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
                if (!ent->inuse)
                        continue;
                if (game.clients[i].resp.team == TEAM1 && 
                                ent->solid != SOLID_NOT)
                        onteam1++;
                        else if (game.clients[i].resp.team == TEAM2 && 
                                        ent->solid != SOLID_NOT)
                        onteam2++;
        }
  
        if (onteam1 > 0 && onteam2 > 0)
                return WINNER_NONE;
                else if (onteam1 == 0 && onteam2 == 0)
                return WINNER_TIE;
                else if (onteam1 > 0 && onteam2 == 0)
                return WINNER_TEAM1;
                else
                return WINNER_TEAM2;
}

// CheckForForcedWinner: A winner is being forced, find who it is.
int CheckForForcedWinner()
{
        int onteam1 = 0, onteam2 = 0, i;
        int health1 = 0, health2 = 0;
        edict_t *ent;

        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
                if (!ent->inuse)
                        continue;
                if (game.clients[i].resp.team == TEAM1 && ent->solid != SOLID_NOT)
                {
                        onteam1++;
                        health1 += ent->health;
                }
                        else if (game.clients[i].resp.team == TEAM2 && ent->solid != SOLID_NOT)
                {
                        onteam2++;
                        health2 += ent->health;
                }
        }
  
        if (onteam1 > onteam2)
        {
                return WINNER_TEAM1;
        }
                else if (onteam2 > onteam1)
        {
                return WINNER_TEAM2;
        }
                else
        {
                if (health1 > health2)
                        return WINNER_TEAM1;
                        else if (health2 > health1)
                        return WINNER_TEAM2;
                        else
                        return WINNER_TIE;
        }
}
*///PaTMaN-NOTEAM
/*void SpawnPlayers()
{
        int i;
        edict_t *ent;
        
        GetSpawnPoints();
        SetupTeamSpawnPoints();
        InitTransparentList();
        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
//PaTMaN-NOTEAM                if (ent->inuse && ent->client->resp.team != NOTEAM)
//PaTMaN-NOTEAM                {
//PaTMaN-NOTEAM                        PutClientInServer(ent);
//PaTMaN-NOTEAM                        AddToTransparentList(ent);
//PaTMaN-NOTEAM                }
        }
}
*/
/*
void StartRound()
{
        team_round_going = 1;
        current_round_length = 0;
}
*/
/*void StartLCA()
{
        CleanLevel();
   
        CenterPrintAll("LIGHTS...\n");
        gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD, 
                gi.soundindex("atl/lights.wav"), 1.0, ATTN_NONE, 0.0);
        lights_camera_action = 41;
        SpawnPlayers();
}
*/
// FindOverlap: Find the first (or next) overlapping player for ent.
edict_t *FindOverlap(edict_t *ent, edict_t *last_overlap)
{
        int i;
        edict_t *other;
        vec3_t diff,A,B;
		VectorCopy(ent->s.origin,A);
		
        for (i = last_overlap ? last_overlap - g_edicts : 0; i < game.maxclients; i++)
        {
                other = &g_edicts[i+1];

                if (!other->inuse /*//PaTMaN-NOTEAM|| other->client->resp.team == NOTEAM */
								|| !other->client
                                || other == ent 
                                || other->solid == SOLID_NOT
                                || other->deadflag == DEAD_DEAD)
								continue;
			
				VectorSubtract(ent->s.origin, other->s.origin, diff);
				VectorCopy(other->s.origin,B);
				
				if (diff[0] >= -33 && diff[0] <= 33 &&
					diff[1] >= -33 && diff[1] <= 33)
				{
					
					if (A[2] > B[2])
					{
						//A is ontop, A's viewheight does not matter
						if (other->maxs[2] == 32)
						{
							if ((A[2]-B[2]) < 58)
								return other;
						}
						else
						{
							if ((A[2]-B[2]) < 38)
								return other;
						}
					}
					else if (A[2] < B[2])
					{
						//B is ontop, B's viewheight does not matter
						if (ent->maxs[2] == 32)
						{
							if ((B[2]-A[2]) < 58)
								return other;
						}
						else
						{
							if ((B[2]-A[2]) < 38)
								return other;
						}
					}
					else if (A[2] == B[2])
					{
						//Neither is ontop
						return other;
						
					}
				}
		}
        return NULL;
}

edict_t *FindOverlap_InsidePuppet(edict_t *ent, edict_t *last_overlap)
{
        int i;
        edict_t *other;
        vec3_t diff,A,B;
		VectorCopy(ent->s.origin,A);
		
        for (i = last_overlap ? last_overlap - g_edicts : 0; i < game.maxclients; i++)
        {
                other = &g_edicts[i+1];

                if (!other->inuse /*//PaTMaN-NOTEAM|| other->client->resp.team == NOTEAM */
								|| !other->client
                                || other == ent 
                                || other->solid == SOLID_NOT
                                || other->deadflag == DEAD_DEAD)
								continue;
			
				VectorSubtract(ent->s.origin, other->s.origin, diff);
				VectorCopy(other->s.origin,B);
				
				if (diff[0] >= -31 && diff[0] <= 31 &&
					diff[1] >= -31 && diff[1] <= 31)
				{
					
					if (A[2] > B[2])
					{
						//A is ontop, A's viewheight does not matter
						if (other->maxs[2] == 32)
						{
							if ((A[2]-B[2]) < 55)
								return other;
						}
						else
						{
							if ((A[2]-B[2]) < 35)
								return other;
						}
					}
					else if (A[2] < B[2])
					{
						//B is ontop, B's viewheight does not matter
						if (ent->maxs[2] == 32)
						{
							if ((B[2]-A[2]) < 55)
								return other;
						}
						else
						{
							if ((B[2]-A[2]) < 35)
								return other;
						}
					}
					else if (A[2] == B[2])
					{
						//Neither is ontop
						return other;
						
					}
				}
		}
        return NULL;
}

edict_t *FindOverlap2(edict_t *ent, edict_t *last_overlap)
{
        int i;
        edict_t *other;
        vec3_t diff;
		
        for (i = last_overlap ? last_overlap - g_edicts : 0; i < game.maxclients; i++)
        {
                other = &g_edicts[i+1];

                if (!other->inuse
								|| !other->client
                                || other == ent 
                                || other->solid == SOLID_NOT
                                || other->deadflag == DEAD_DEAD)
                        continue;

                VectorSubtract(ent->s.origin, other->s.origin, diff);
				//this one doesn't need the same maxs[2] treatment as above because
				//  the ranges are so much greater. The above is to almost exact non-overlap.
                if (diff[0] >= -100 && diff[0] <= 100 &&
                                diff[1] >= -100 && diff[1] <= 100 &&
                                diff[2] >= -150 && diff[2] <= 150)
								
                        return other;
        }

        return NULL;
}

/*
edict_t *FindMarkerOverlap(edict_t *ent)
{
	int i;
	//        edict_t *other;
	vec3_t diff;
	edict_t *marker;
	int markers;
	
	markers = MarkersInt();

//	gi.cprintf(ent,PRINT_HIGH,"ent->viewheight == %i\n",ent->viewheight);
//	gi.cprintf (ent,PRINT_HIGH, "markers:%i\n", markers);

	for (i=1;i<markers+1;i++)
	{
		marker = FindEdictByClassnum ("Jump Marker", i);

//		gi.cprintf(ent,PRINT_HIGH,"ent:%i:%f:%f:%f\n",i,ent->s.origin[0],ent->s.origin[1],ent->s.origin[2]);
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
				diff[2] >= -33 && diff[2] <= 25)
			{
		//		gi.cprintf(ent,PRINT_HIGH,"Current ClassNum:%i with a DIFF of %f:%f:%f\n",marker->classnum,diff[0],diff[1],diff[2]);
				return marker;
			}
		}
	}

  return NULL;
}
*/
/*
void ContinueLCA()
{
        if (lights_camera_action == 21)
        {
                CenterPrintAll("CAMERA...\n");
                gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD, 
                gi.soundindex("atl/camera.wav"), 1.0, ATTN_NONE, 0.0);
        }
                else if (lights_camera_action == 1)
        { 
                CenterPrintAll("ACTION!\n");
                gi.sound(&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD, 
                gi.soundindex("atl/action.wav"), 1.0, ATTN_NONE, 0.0);
                StartRound();
        }
        lights_camera_action--;
}
*/
/*//PaTMaN-NOTEAM
void MakeAllLivePlayersObservers()
{
        edict_t *ent;
        int saveteam, i;

        for (i = 0; i < game.maxclients; i++)
        {
                ent = &g_edicts[1+i];
                if (!ent->inuse || ent->solid == SOLID_NOT)
                        continue;
                saveteam = ent->client->resp.team;
                ent->client->resp.team = NOTEAM;
                PutClientInServer(ent);
                ent->client->resp.team = saveteam;
        }
}
*///PaTMaN-NOTEAM
/*//PaTMaN-NOMENUS
// WonGame: returns true if we're exiting the level.
int WonGame(int winner)
{
        gi.bprintf(PRINT_HIGH, "The round is over:\n");

        if (winner == WINNER_TIE)
        {
                gi.bprintf(PRINT_HIGH, "It was a tie, no points awarded!\n");
        }
                else        
        {
                if (winner == WINNER_TEAM1)
                {
                        gi.bprintf(PRINT_HIGH, "%s won!\n", TeamName(TEAM1));
                        team1_score++;
                }
                        else
                {
                        gi.bprintf(PRINT_HIGH, "%s won!\n", TeamName(TEAM2));
                        team2_score++;
                }
        }

        if (timelimit->value)
        {
                if (level.time >= timelimit->value*60)
                {
                        gi.bprintf(PRINT_HIGH, "Timelimit hit.\n");
                        EndDMLevel();
                        team_round_going = team_round_countdown = team_game_going = 0;
                        return 1;
                }
        }

        if (roundlimit->value)
        {
                if (team1_score >= roundlimit->value || team2_score >= roundlimit->value)
                {
                        gi.bprintf(PRINT_HIGH, "Roundlimit hit.\n");
                        EndDMLevel();
                        team_round_going = team_round_countdown = team_game_going = 0;
                        return 1;
                }
        }

        return 0;
}

*///PaTMaN-NOMENUS
/*//PaTMaN-NOMENUS
void CheckTeamRules()
{
        int winner;
        int checked_tie = 0;

        if (lights_camera_action)
        {
                ContinueLCA();
                return;
        }

        if (team_round_going)
                current_round_length++;

        if (holding_on_tie_check)
        {
                holding_on_tie_check--;
                if (holding_on_tie_check > 0)
                        return;
                holding_on_tie_check = 0;
                checked_tie = 1;
        }

        if (team_round_countdown == 1)
        {
                team_round_countdown = 0;
                if (BothTeamsHavePlayers())
                {
                        team_game_going = 1;
                        StartLCA();
                }
                        else
                {
                        CenterPrintAll("Not enough players to play!\n");
                        MakeAllLivePlayersObservers();
                }
        }
                else
        {
                if (team_round_countdown)
                        team_round_countdown--;
        }

        // check these rules every 1.5 seconds...
        rulecheckfrequency++;
        if (rulecheckfrequency % 15 && !checked_tie)  
                return;

        if (!team_round_going)
        {
                if (timelimit->value)
                {
                        if (level.time >= timelimit->value*60)
                        {
                                gi.bprintf (PRINT_HIGH, "Timelimit hit.\n");
                                EndDMLevel();
                                team_round_going = team_round_countdown = team_game_going = 0;
                                return;
                        }
                }

                if (!team_round_countdown)
                {
                        if (BothTeamsHavePlayers())
                        {
                                CenterPrintAll("The round will begin in 20 seconds!\n");
                                team_round_countdown = 201;
                        }
                }
        }
                else /* team_round_going */
/*//PaTMaN-NOMENUS
        {
                if ((winner = CheckForWinner()) != WINNER_NONE)
                {
                        if (!checked_tie)
                        {
                                holding_on_tie_check = 50;
                                return;
                        }
                        if (WonGame(winner))
                                return;
                        team_round_going = 0;
                        lights_camera_action = 0;
                        holding_on_tie_check = 0;
                        team_round_countdown = 71;
                        return;
                }

                if (roundtimelimit->value &&
                        (current_round_length > roundtimelimit->value * 600))
                {
                        gi.bprintf(PRINT_HIGH, "Round timelimit hit.\n");
                        winner = CheckForForcedWinner();
                        if (WonGame(winner))
                                return;
                        team_round_going = 0;
                        lights_camera_action = 0;
                        holding_on_tie_check = 0;
                        team_round_countdown = 71;
                        return;
                }
        }
}
*///PaTMaN-NOMENUS

/* // V1.21 REMOVED
void A_Scoreboard(edict_t *ent)
{
        if (ent->client->showscores && ent->client->scoreboardnum == 1)
        {
                // blink header of the winning team during intermission
                if (level.intermissiontime && (level.framenum & 8)) { // blink 1/8th second
                        if (team1_score > team2_score)
                                ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
                        else if (team2_score > team1_score)
                                ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
                        else if (team1_total > team2_total) // frag tie breaker
                                ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
                        else if (team2_total > team1_total) 
                                ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
                        else { // tie game!
                                ent->client->ps.stats[STAT_TEAM1_PIC] = 0;
                                ent->client->ps.stats[STAT_TEAM2_PIC] = 0;
                        }
                }
                        else
                {
				
                        ent->client->ps.stats[STAT_TEAM1_PIC] = gi.imageindex(team1_skin_index);
                        ent->client->ps.stats[STAT_TEAM2_PIC] = gi.imageindex(team2_skin_index);
                }

                ent->client->ps.stats[STAT_TEAM1_SCORE] = team1_score;
                ent->client->ps.stats[STAT_TEAM2_SCORE] = team2_score;
        }
}
*/ // V1.21 REMOVED

// Maximum number of lines of scores to put under each team's header.
#define MAX_SCORES_PER_TEAM 9

void A_ScoreboardMessage (edict_t *ent, edict_t *killer)
{
	char        string[1400];
	edict_t     *cl_ent;
	int         maxsize = 1000, i, j, k;

	int total, ping, id;
	int sortedid[MAX_CLIENTS], sorted[MAX_CLIENTS];

	total = 0;
	for (i=0 ; i<game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		id = game.clients[i].resp.id;
		for (j=0 ; j<total ; j++)
		{
			if (id < sortedid[j])
				break;
		}
		for (k=total ; k>j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedid[k] = sortedid[k-1];
		}
		sorted[j] = i;
		sortedid[j] = id;
		total++;
	}

	strcpy(string,  "xv 0 yv 32 string2 \"Player          ID Time Ping Grav\" "
		            "xv 0 yv 40 string2 \"--------------- -- ---- ---- -----\" ");

	for (i = 0; i < total; i++)
	{
		ping = game.clients[sorted[i]].ping;
		if (ping > 9999)
			ping = 9999;

		sprintf(string + strlen(string), 
			"xv 0 yv %d string \"%-15s %2d %4d %4d %5d\" ",
			48 + i * 8, 
			game.clients[sorted[i]].pers.netname,game.clients[sorted[i]].resp.id,
			(level.framenum - game.clients[sorted[i]].resp.enterframe)/600,
			ping,game.clients[sorted[i]].resp.cgravity);

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


	if (strlen(string) > 1300)  // for debugging...
		gi.dprintf("Warning: scoreboard string neared or exceeded max length\nDump:\n%s\n---\n", 
		string);

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}

// called when we enter the intermission
/*//PaTMaN-NOTEAM
void TallyEndOfLevelTeamScores(void)
{
        int i;

        team1_total = team2_total = 0;
        for (i = 0; i < maxclients->value; i++)
        {
                if (!g_edicts[i+1].inuse)
                        continue;
                if (game.clients[i].resp.team == TEAM1)
                        team1_total += game.clients[i].resp.score;
                else if (game.clients[i].resp.team == TEAM2)
                        team2_total += game.clients[i].resp.score;
        }
}

*///PaTMaN-NOTEAM
/*
 * Teamplay spawning functions...
 */
/*//PaTMaN-NOTEAM
edict_t *SelectTeamplaySpawnPoint(edict_t *ent)
{
        return teamplay_spawns[ent->client->resp.team - 1];
}
*///PaTMaN-NOTEAM

/* // V1.21 REMOVED
// SpawnPointDistance: 
// Returns the distance between two spawn points (or any entities, actually...)
float SpawnPointDistance(edict_t *spot1, edict_t *spot2)
{
        vec3_t v;
        VectorSubtract (spot1->s.origin, spot2->s.origin, v);
        return VectorLength(v);
}
*/ // V1.21 REMOVED

/* // V1.21 REMOVED
// GetSpawnPoints:
// Put the spawn points into our potential_spawns array so we can work with them easily.
void GetSpawnPoints()
{
        edict_t *spot;

        spot = NULL;
        num_potential_spawns = 0;
        while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
        {
                potential_spawns[num_potential_spawns] = spot;
                num_potential_spawns++;
                if (num_potential_spawns >= MAX_SPAWNS)
                {
                        gi.dprintf("Warning: MAX_SPAWNS exceeded\n");
                        return;
                }
        }
}
*/ // V1.21 REMOVED

/* // V1.21 REMOVED
// newrand returns n, where 0 >= n < top
int newrand(int top)
{
        return (int)(random() * top);
}
*/ // V1.21 REMOVED

/* // V1.21 REMOVED
// compare_spawn_distances is used by the qsort() call
int compare_spawn_distances(const void *sd1, const void *sd2)
{
        if (((spawn_distances_t *)sd1)->distance < 
                ((spawn_distances_t *)sd2)->distance)
                return -1;
                else if (((spawn_distances_t *)sd1)->distance >
                        ((spawn_distances_t *)sd2)->distance)
                return 1;
                else
                return 0;
}
*/ // V1.21 REMOVED

/* // V1.21 REMOVED
void SelectRandomTeamplaySpawnPoint(int team, qboolean teams_assigned[])
{
        int spawn_point;
        spawn_point = newrand(num_potential_spawns);
        teamplay_spawns[team] = potential_spawns[spawn_point];
        teams_assigned[team] = true;
}
*/ // V1.21 REMOVED

/* // V1.21 REMOVED
void SelectFarTeamplaySpawnPoint(int team, qboolean teams_assigned[])
{
        int x, y, spawn_to_use, preferred_spawn_points, num_already_used,
                total_good_spawn_points;
        float closest_spawn_distance, distance;
        spawn_distances_t *spawn_distances;

        spawn_distances = (spawn_distances_t *)gi.TagMalloc(num_potential_spawns * sizeof(spawn_distances_t), TAG_GAME);

        num_already_used = 0;
        for (x = 0; x < num_potential_spawns; x++)
        {
                closest_spawn_distance = 2000000000;

                for (y = 0; y < num_teams; y++)
                {
                        if (teams_assigned[y])
                        {
                                distance = SpawnPointDistance(potential_spawns[x], teamplay_spawns[y]);
                                if (distance < closest_spawn_distance)
                                {
                                        closest_spawn_distance = distance;
                                }
                        }
                }

                if (closest_spawn_distance == 0)
                        num_already_used++;

                spawn_distances[x].s = potential_spawns[x];
                spawn_distances[x].distance = closest_spawn_distance;
        }

        qsort(spawn_distances, num_potential_spawns, 
                sizeof(spawn_distances_t), compare_spawn_distances);

        total_good_spawn_points = num_potential_spawns - num_already_used;

        if (total_good_spawn_points <= 4)
                preferred_spawn_points = 1;
                else
                if (total_good_spawn_points <= 10)
                preferred_spawn_points = 2;
                else
                preferred_spawn_points = 3;

//FB 6/1/99 - make DF_SPAWN_FARTHEST force far spawn points in TP
        if ((int)dmflags->value & DF_SPAWN_FARTHEST)
                preferred_spawn_points = 1;
//FB 6/1/99

        spawn_to_use = newrand(preferred_spawn_points);

	if (team < 0 || team >= MAX_TEAMS)
	{
		gi.dprintf("Out-of-range teams value in SelectFarTeamplaySpawnPoint, skipping...\n");
	}
		else
	{
	        teams_assigned[team] = true;
	        teamplay_spawns[team] = spawn_distances[num_potential_spawns - spawn_to_use - 1].s;
	}

        gi.TagFree(spawn_distances);
}
*/ // V1.21 REMOVED

// SetupTeamSpawnPoints:
//
// Setup the points at which the teams will spawn.
//
/* // V1.21 REMOVED
void SetupTeamSpawnPoints()
{
        qboolean teams_assigned[MAX_TEAMS];
        int started_at, l, firstassignment;

        for (l = 0; l < num_teams; l++)
        {
                teamplay_spawns[l] = NULL;
                teams_assigned[l] = false;
        }

        started_at = l = newrand(num_teams);
        firstassignment = 1;
        do
        {
                if (l < 0 || l >= MAX_TEAMS)
                {
                        gi.dprintf("Warning: attempt to setup spawns for out-of-range team (%d)\n", l);
                }

                if (firstassignment)
                {
                        SelectRandomTeamplaySpawnPoint(l, teams_assigned);
                        firstassignment = 0;
                }
                        else
                {
                        SelectFarTeamplaySpawnPoint(l, teams_assigned);
                }
                l++;
                if (l == num_teams)
                        l = 0;
        } while (l != started_at);
}
*/ // V1.21 REMOVED

