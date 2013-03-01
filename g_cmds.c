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

void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
edict_t *SelectRandomDeathmatchSpawnPoint (void);
edict_t *SelectClosestDeathmatchSpawnPoint (void); // V1.21 ADDED

void TogglePuppetShootArea(edict_t *ent, pmenu_t *p);

char *ClientTeam (edict_t *ent)
{
        char            *p;
        static char     value[512];

        value[0] = 0;

        if (!ent->client)
                return value;

        strcpy(value, Info_ValueForKey (ent->client->pers.userinfo, "skin"));
        p = strchr(value, '/');
        if (!p)
                return value;

        if ((int)(dmflags->value) & DF_MODELTEAMS)
        {
                *p = 0;
                return value;
        }

        // if ((int)(dmflags->value) & DF_SKINTEAMS)
        return ++p;
}

qboolean OnSameTeam (edict_t *ent1, edict_t *ent2)
{
        char    ent1Team [512];
        char    ent2Team [512];

//FIREBLADE
        if (!ent1->client || !ent2->client)
                return false;

/*        if (teamplay->value)
        {
                return ent1->client->resp.team == ent2->client->resp.team;
        }
*/
  //FIREBLADE

        if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
                return false;

        strcpy (ent1Team, ClientTeam (ent1));
        strcpy (ent2Team, ClientTeam (ent2));

        if (strcmp(ent1Team, ent2Team) == 0)
                return true;
        return false;
}


void SelectNextItem (edict_t *ent, int itflags)
{
        gclient_t       *cl;
        int                     i, index;
        gitem_t         *it;

        cl = ent->client;

//FIREBLADE
        if (cl->menu)
        {
                PMenu_Next(ent);
                return;
        }
//FIREBLADE

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        // scan  for the next valid one
        for (i=1 ; i<=MAX_ITEMS ; i++)
        {
                index = (cl->pers.selected_item + i)%MAX_ITEMS;
                if (!cl->pers.inventory[index])
                        continue;
                it = &itemlist[index];
                if (!it->use)
                        continue;
                if (!(it->flags & itflags))
                        continue;

                cl->pers.selected_item = index;
                return;
        }

        cl->pers.selected_item = -1;
}

void SelectPrevItem (edict_t *ent, int itflags)
{
        gclient_t       *cl;
        int                     i, index;
        gitem_t         *it;

        cl = ent->client;

//FIREBLADE
        if (cl->menu)
        {
                PMenu_Prev(ent);
                return;
        } 
//FIREBLADE

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        // scan  for the next valid one
        for (i=1 ; i<=MAX_ITEMS ; i++)
        {
                index = (cl->pers.selected_item + MAX_ITEMS - i)%MAX_ITEMS;
                if (!cl->pers.inventory[index])
                        continue;
                it = &itemlist[index];
                if (!it->use)
                        continue;
                if (!(it->flags & itflags))
                        continue;

                cl->pers.selected_item = index;
                return;
        }

        cl->pers.selected_item = -1;
}

void ValidateSelectedItem (edict_t *ent)
{
        gclient_t       *cl;

        cl = ent->client;

        if (cl->pers.inventory[cl->pers.selected_item])
                return;         // valid

        SelectNextItem (ent, -1);
}


//=================================================================================

/*
==================
Cmd_Give_f

Give items to a client
==================
*/

//give all command
/*//PaTMaN-NOGIVE
void Cmd_Give_f (edict_t *ent)
{
        char            *name;
        gitem_t         *it;
        int                     index;
        int                     i;
        qboolean        give_all;
        edict_t         *it_ent;
                edict_t                 etemp;
return; //PaTMaN - No more give all. Not needed.
        if (deathmatch->value && !sv_cheats->value)
        {
                gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
                return;
        }

        if (ent->solid == SOLID_NOT)
        {
                gi.cprintf(ent, PRINT_HIGH, "This command can't be used by spectators.\n");
                return;
        }

        name = gi.args();

        if (Q_stricmp(name, "all") == 0)
                give_all = true;
        else
                give_all = false;

        if (Q_stricmp(gi.argv(1), "health") == 0)
        {
            /*    if (gi.argc() == 3)
                        ent->health = atoi(gi.argv(2));
                else
                        ent->health = ent->max_health;
                if (!give_all) */
/*//PaTMaN-NOGIVE
                        return;
        }

        if (give_all || Q_stricmp(name, "weapons") == 0)
        {
                for (i=0 ; i<game.num_items ; i++)
                {
                        it = itemlist + i;
                        if (!it->pickup)
                                continue;
                        if (!(it->flags & IT_WEAPON))
                                continue;
                        ent->client->pers.inventory[i] += 1;
                }
                if (!give_all)
                        return;
        }

        if (give_all || Q_stricmp(name, "items") == 0)
        {
                for (i=0 ; i<game.num_items ; i++)
                {
                        it = itemlist + i;
                        if (!it->pickup)
                                continue;
                        if (!(it->flags & IT_ITEM))
                                continue;                
                                                etemp.item = it;
        
                if ( ent->client->unique_item_total >= unique_items->value ) 
                        ent->client->unique_item_total = unique_items->value - 1;

                Pickup_Special ( &etemp, ent );
                }
                if (!give_all)
                        return;
        }


        if (give_all || Q_stricmp(name, "ammo") == 0)
        {
                        ent->client->mk23_rds = ent->client->mk23_max;
                        ent->client->dual_rds = ent->client->dual_max;
                        ent->client->mp5_rds = ent->client->mp5_max;
                        ent->client->m4_rds = ent->client->m4_max;
                        ent->client->shot_rds = ent->client->shot_max;
                        ent->client->sniper_rds = ent->client->sniper_max;
                        ent->client->cannon_rds = ent->client->cannon_max;

                for (i=0 ; i<game.num_items ; i++)
                {
                        it = itemlist + i;
                        if (!it->pickup)
                                continue;
                        if (!(it->flags & IT_AMMO))
                                continue;
                        Add_Ammo (ent, it, 1000);
                }
                if (!give_all)
                        return;
        }

        if (Q_stricmp(name, "armor") == 0)
        {
                        /*
                gitem_armor_t   *info;

                it = FindItem("Jacket Armor");
                ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

                it = FindItem("Combat Armor");
                ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

                it = FindItem("Body Armor");
                info = (gitem_armor_t *)it->info;
                ent->client->pers.inventory[ITEM_INDEX(it)] = info->max_count;

                if (!give_all)
                        */
/*//PaTMaN-NOGIVE
                        return;
        }

        if (Q_stricmp(name, "Power Shield") == 0)
        {
                /*it = FindItem("Power Shield");
                it_ent = G_Spawn();
                it_ent->classname = it->classname;
                SpawnItem (it_ent, it);
                Touch_Item (it_ent, ent, NULL, NULL);
                if (it_ent->inuse)
                        G_FreeEdict(it_ent);

                if (!give_all)
                                */
/*//PaTMaN-NOGIVE
                        return;
        }

        /*if (give_all)
        {
                for (i=0 ; i<game.num_items ; i++)
                {
                        it = itemlist + i;
                        if (!it->pickup)
                                continue;
                        if (it->flags & (IT_ARMOR|IT_WEAPON|IT_AMMO))
                                continue;
                        ent->client->pers.inventory[i] = 1;
                }
                return;
        }*/
/*//PaTMaN-NOGIVE

                if ( give_all )
                {
                        return;
                }


        it = FindItem (name);
        if (!it)
        {
                name = gi.argv(1);
                it = FindItem (name);
                if (!it)
                {
                        gi.dprintf ("unknown item\n");
                        return;
                }
                if ( !(it->flags & IT_AMMO || it->flags & IT_WEAPON || it->flags & IT_ITEM) )
                    return;
        }

                if ( !(it->flags & IT_AMMO || it->flags & IT_WEAPON || it->flags & IT_ITEM) )
                                return;
        

        if (!it->pickup)
        {
                gi.dprintf ("non-pickup item\n");
                return;
        }


        index = ITEM_INDEX(it);

        if (it->flags & IT_AMMO)
        {
              /*  if (gi.argc() == 5)
                        ent->client->pers.inventory[index] = atoi(gi.argv(4));
                else if ( (gi.argc() == 4)  && !(stricmp(it->pickup_name, "12 Gauge Shells")) )
                                                ent->client->pers.inventory[index] = atoi(gi.argv(3));
                                else */
/*//PaTMaN-NOGIVE
                                                ent->client->pers.inventory[index] += it->quantity;
        }
                else if ( it->flags & IT_ITEM)
                {
                        
                        etemp.item = it;
                        if ( ent->client->unique_item_total >= unique_items->value ) 
                                ent->client->unique_item_total = unique_items->value - 1;

                        Pickup_Special ( &etemp, ent );
                }
                
        else
        {
                it_ent = G_Spawn();
                it_ent->classname = it->classname;
                SpawnItem (it_ent, it);
                Touch_Item (it_ent, ent, NULL, NULL);
                if (it_ent->inuse)
                        G_FreeEdict(it_ent);
        }
}

*///PaTMaN-NOGIVE
/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
/*//PaTMaN-NOGOD
void Cmd_God_f (edict_t *ent)
{
        char    *msg;

        if (deathmatch->value && !sv_cheats->value)
        {
                gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
                return;
        }

        ent->flags ^= FL_GODMODE;
        if (!(ent->flags & FL_GODMODE) )
                msg = "godmode OFF\n";
        else
                msg = "godmode ON\n";

        gi.cprintf (ent, PRINT_HIGH, msg);
}

*///PaTMaN-NOGOD
/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
/*//PaTMaN-NONOTARGET
void Cmd_Notarget_f (edict_t *ent)
{
        char    *msg;

        if (deathmatch->value && !sv_cheats->value)
        {
                gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
                return;
        }

        ent->flags ^= FL_NOTARGET;
        if (!(ent->flags & FL_NOTARGET) )
                msg = "notarget OFF\n";
        else
                msg = "notarget ON\n";

        gi.cprintf (ent, PRINT_HIGH, msg);
}
*///PaTMaN-NONOTARGET

/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (edict_t *ent)
{
	char    *msg;
	//PaTMaN
	/*        if (deathmatch->value && !sv_cheats->value)
	{
	gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
	return;
	}
	*/
	if (ent->client->pers.spectator)
	{
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
		return;
	}
	
	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	}
	else
	{
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
		ent->flags ^= FL_POWER_ARMOR;
	}
	
	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void Cmd_Use_f (edict_t *ent)
{
        int                     index;
        gitem_t         *it;
        char            *s;

        s = gi.args();

        //zucc - check for "special"
        if ( stricmp(s, "special") == 0 )
        {
                ReadySpecialWeapon( ent );
                return;
        }

        //zucc - alias names
        if (!stricmp(s, "blaster") || !stricmp(s, "mark 23 pistol"))
                s = MK23_NAME;
        if (!stricmp(s, "A 2nd pistol") || !stricmp(s, "railgun"))
                s = DUAL_NAME;
        if (!stricmp(s, "shotgun"))
                s = M3_NAME;
        if (!stricmp(s, "machinegun"))
                s = HC_NAME;
        if (!stricmp(s, "super shotgun"))
                s = MP5_NAME;
        if (!stricmp(s, "chaingun"))
                s = SNIPER_NAME;
        if (!stricmp(s, "bfg10k"))
                s = KNIFE_NAME;
                 // zucc - let people pull up a knife ready to be thrown
                 if (!stricmp(s, "throwing combat knife"))
                 {
                         if ( ent->client->curr_weap != KNIFE_NUM )
                         {
							 if (!(ent->client->resp.wmodes & WM_KNIFE))
								 ent->client->resp.wmodes += WM_KNIFE;
                                 //ent->client->resp.knife_mode = 1;
                         }
                         // switch to throwing mode if a knife is already out
                         else
                         {
                                 Cmd_New_Weapon_f( ent );
                         }
                         s = KNIFE_NAME;
                 }
                 if (!stricmp(s, "slashing combat knife"))
                 {
                         if ( ent->client->curr_weap != KNIFE_NUM )
                         {
							 if (ent->client->resp.wmodes & WM_KNIFE)
								 ent->client->resp.wmodes -= WM_KNIFE;
                                 //ent->client->resp.knife_mode = 0;
                         }
                         // switch to slashing mode if a knife is already out
                         else
                         {
                                 Cmd_New_Weapon_f( ent );
                         }
                         s = KNIFE_NAME;
                 }
        if (!stricmp(s, "grenade launcher"))
                s = M4_NAME;
        if (!stricmp(s, "grenades"))
                s = GRENADE_NAME;

                it = FindItem (s);
        
//FIREBLADE
        if (!it || (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD))
//FIREBLADE
        {
                gi.cprintf (ent, PRINT_HIGH, "Unknown item: %s\n", s); // fixed capitalization -FB
                return;
        }

        if (!it->use)
        {
                gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n"); 
                return;
        }

        index = ITEM_INDEX(it);
        if (!ent->client->pers.inventory[index])
        {
                gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
                return;
        }

        it->use (ent, it);
}


/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
/*//PaTMaN-NODROP
void Cmd_Drop_f (edict_t *ent)
{
        int                     index;
        gitem_t         *it;
        char            *s;

	return; //PaTMaN - Prevent Dropping of Anything

        s = gi.args();
        
        //zucc check to see if the string is weapon
        if ( stricmp(s, "weapon") == 0 )
        {
                DropSpecialWeapon ( ent );
                return;
        }

        //zucc now for item
        if ( stricmp(s, "item") == 0 )
        {
                DropSpecialItem ( ent );
                return;
        }
        
        it = FindItem (s);
        if (!it)
        {
                gi.cprintf (ent, PRINT_HIGH, "unknown item: %s\n", s);
                return;
        }
        if (!it->drop)
        {
                gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
                return;
        }
        index = ITEM_INDEX(it);
        if (!ent->client->pers.inventory[index])
        {
                gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
                return;
        }

        it->drop (ent, it);
}

*///PaTMaN-NODROP
/*
=================
Cmd_Inven_f
=================
*/
void Cmd_Inven_f (edict_t *ent)
{
//        int                     i;
        gclient_t       *cl;

        cl = ent->client;

        cl->showscores = false;
        cl->showhelp = false;
//FIREBLADE
        if (ent->client->menu)
        {
                PMenu_Close(ent);
                return;
        }
//FIREBLADE

        if (cl->showinventory)
        {
                cl->showinventory = false;
                return;
        }

//if (deathmatch->value)
//{
	OpenMainMenu(ent,2);
return;
//}

//FIREBLADE
/*        if (teamplay->value)
        {
                if (ent->client->resp.team == NOTEAM)
                        OpenJoinMenu(ent);
                        else
                        OpenWeaponMenu(ent);
                return;
        }
*/
  //FIREBLADE
/*
        cl->showinventory = true;

        gi.WriteByte (svc_inventory);
        for (i=0 ; i<MAX_ITEMS ; i++)
        {
                gi.WriteShort (cl->pers.inventory[i]);
        }
        gi.unicast (ent, true);
  */
}

/*
=================
Cmd_InvUse_f
=================
*/
void Cmd_InvUse_f (edict_t *ent)
{
        gitem_t         *it;

//FIREBLADE
        if (ent->client->menu)
        {
                PMenu_Select(ent);
                return;
        }
//FIREBLADE

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        ValidateSelectedItem (ent);

        if (ent->client->pers.selected_item == -1)
        {
                gi.cprintf (ent, PRINT_HIGH, "No item to use.\n");
                return;
        }

        it = &itemlist[ent->client->pers.selected_item];
        if (!it->use)
        {
                gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
                return;
        }
        it->use (ent, it);
}

/*
=================
Cmd_WeapPrev_f
=================
*/
void Cmd_WeapPrev_f (edict_t *ent)
{
        gclient_t       *cl;
        int                     i, index;
        gitem_t         *it;
        int                     selected_weapon;

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        cl = ent->client;

        if (!cl->pers.weapon)
                return;

        selected_weapon = ITEM_INDEX(cl->pers.weapon);

        // scan  for the next valid one
        for (i=1 ; i<=MAX_ITEMS ; i++)
        {
                index = (selected_weapon + i)%MAX_ITEMS;
                if (!cl->pers.inventory[index])
                        continue;
                it = &itemlist[index];
                if (!it->use)
                        continue;
                if (! (it->flags & IT_WEAPON) )
                        continue;
                it->use (ent, it);
                if (cl->pers.weapon == it)
                        return; // successful
        }
}

/*
=================
Cmd_WeapNext_f
=================
*/
void Cmd_WeapNext_f (edict_t *ent)
{
        gclient_t       *cl;
        int                     i, index;
        gitem_t         *it;
        int                     selected_weapon;

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        cl = ent->client;

        if (!cl->pers.weapon)
                return;

        selected_weapon = ITEM_INDEX(cl->pers.weapon);

        // scan  for the next valid one
        for (i=1 ; i<=MAX_ITEMS ; i++)
        {
                index = (selected_weapon + MAX_ITEMS - i)%MAX_ITEMS;
                if (!cl->pers.inventory[index])
                        continue;
                it = &itemlist[index];
                if (!it->use)
                        continue;
                if (! (it->flags & IT_WEAPON) )
                        continue;
                it->use (ent, it);
                if (cl->pers.weapon == it)
                        return; // successful
        }
}

/*
=================
Cmd_WeapLast_f
=================
*/
void Cmd_WeapLast_f (edict_t *ent)
{
        gclient_t       *cl;
        int                     index;
        gitem_t         *it;

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        cl = ent->client;

        if (!cl->pers.weapon || !cl->pers.lastweapon)
                return;

        index = ITEM_INDEX(cl->pers.lastweapon);
        if (!cl->pers.inventory[index])
                return;
        it = &itemlist[index];
        if (!it->use)
                return;
        if (! (it->flags & IT_WEAPON) )
                return;
        it->use (ent, it);
}

/*
=================
Cmd_InvDrop_f
=================
*/
/*
void Cmd_InvDrop_f (edict_t *ent)
{
        gitem_t         *it;

        if (ent->solid == SOLID_NOT && ent->deadflag != DEAD_DEAD)
                return;

        ValidateSelectedItem (ent);

        if (ent->client->pers.selected_item == -1)
        {
                gi.cprintf (ent, PRINT_HIGH, "No item to drop.\n");
                return;
        }

        it = &itemlist[ent->client->pers.selected_item];
        if (!it->drop)
        {
                gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
                return;
        }
        it->drop (ent, it);
}
*/
/*
=================
Cmd_Kill_f
=================
*/
/*
void Cmd_Kill_f (edict_t *ent)
{

	return;
	//FIREBLADE
        if (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD)
                return;
//FIREBLADE

        if((level.time - ent->client->respawn_time) < 5)
                return;
        ent->flags &= ~FL_GODMODE;
        ent->health = 0;
        meansOfDeath = MOD_SUICIDE;
        player_die (ent, ent, ent, 100000, vec3_origin);
// Forget all this... -FB
//        // don't even bother waiting for death frames
//        ent->deadflag = DEAD_DEAD;
////FIREBLADE
//        if (!teamplay->value)
////FIREBLADE
//                respawn (ent);
}
*/
/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (edict_t *ent)
{
        ent->client->showscores = false;
        ent->client->showhelp = false;
        ent->client->showinventory = false;
//FIREBLADE
        if (ent->client->menu)
                PMenu_Close(ent);
//FIREBLADE
}


int PlayerSort (void const *a, void const *b)
{
        int             anum, bnum;

        anum = *(int *)a;
        bnum = *(int *)b;

        anum = game.clients[anum].ps.stats[STAT_HIGHSPEED];
        bnum = game.clients[bnum].ps.stats[STAT_HIGHSPEED];

        if (anum < bnum)
                return -1;
        if (anum > bnum)
                return 1;
        return 0;
}

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (edict_t *ent)
{
        int             i;
        int             count;
        char    small[64];
        char    large[1280];
        int             index[256];

        count = 0;
        for (i = 0 ; i < maxclients->value ; i++)
        {
                if (game.clients[i].pers.connected)
                {
                        index[count] = i;
                        count++;
                }
        }
/* //V1.21 REMOVED
        //if (!teamplay->value || !noscore->value) //PaTMaN-REMOVE_TEAMPLAY
		if (!noscore->value)
        {
                // sort by frags
                qsort (index, count, sizeof(index[0]), PlayerSort);
        }
*/ //V1.21 REMOVED
        // print information
        large[0] = 0;

        for (i = 0 ; i < count ; i++)
        {
                Com_sprintf (small, sizeof(small), "%s\n",
					game.clients[index[i]].pers.netname);
                if (strlen (small) + strlen(large) > sizeof(large) - 100 )
                {       // can't print all of them in one packet
                        strcat (large, "...\n");
                        break;
                }
                strcat (large, small);
        }

        gi.cprintf (ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (edict_t *ent)
{
        int             i;

        i = atoi (gi.argv(1));

        // can't wave when ducked
        if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
                return;

        if (ent->client->anim_priority > ANIM_WAVE)
                return;

        ent->client->anim_priority = ANIM_WAVE;

        switch (i)
        {
        case 0:
                gi.cprintf (ent, PRINT_HIGH, "flipoff\n");
                ent->s.frame = FRAME_flip01-1;
                ent->client->anim_end = FRAME_flip12;
                break;
        case 1:
                gi.cprintf (ent, PRINT_HIGH, "salute\n");
                ent->s.frame = FRAME_salute01-1;
                ent->client->anim_end = FRAME_salute11;
                break;
        case 2:
                gi.cprintf (ent, PRINT_HIGH, "taunt\n");
                ent->s.frame = FRAME_taunt01-1;
                ent->client->anim_end = FRAME_taunt17;
                break;
        case 3:
                gi.cprintf (ent, PRINT_HIGH, "wave\n");
                ent->s.frame = FRAME_wave01-1;
                ent->client->anim_end = FRAME_wave11;
                break;
        case 4:
        default:
                gi.cprintf (ent, PRINT_HIGH, "point\n");
                ent->s.frame = FRAME_point01-1;
                ent->client->anim_end = FRAME_point12;
                break;
        }
}

/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f (edict_t *ent, qboolean team, qboolean arg0, qboolean partner_msg)
{
        int             j, i, offset_of_text;
        edict_t *other;
        char    *p;
        char    text[2048];
        gclient_t *cl;

        if (gi.argc () < 2 && !arg0)
                return;

//FIREBLADE
//PaTMaN-REMOVE_TEAMPLAY        if (!teamplay->value)
//PaTMaN-REMOVE_TEAMPLAY        {
//FIREBLADE
                if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
                        team = false;
//PaTMaN-REMOVE_TEAMPLAY        }

//PaTMaN-NOTEAM        if (team)
//PaTMaN-NOTEAM	{
//PaTMaN-NOTEAM		if (ent->client->resp.team == NOTEAM)
//PaTMaN-NOTEAM		{
//PaTMaN-NOTEAM			gi.cprintf(ent, PRINT_HIGH, "You're not on a team.\n");
//PaTMaN-NOTEAM			return;
//PaTMaN-NOTEAM		}
//PaTMaN-NOTEAM                Com_sprintf (text, sizeof(text), "%s(%s): ", 
//PaTMaN-NOTEAM                        (teamplay->value && (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD)) ? "[DEAD] " : "",
//PaTMaN-NOTEAM                        ent->client->pers.netname);
//PaTMaN-NOTEAM	}
//NORADIO                else if (partner_msg)
//NORADIO	{
//NORADIO		if (ent->client->resp.radio_partner == NULL)
//NORADIO		{
//NORADIO			gi.cprintf(ent, PRINT_HIGH, "You don't have a partner.\n");
//NORADIO			return;
//NORADIO		}
//NORADIO                Com_sprintf (text, sizeof(text), "[%sPARTNER] %s: ", 
//NORADIO                        (teamplay->value && (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD)) ? "DEAD " : "",
//NORADIO                        ent->client->pers.netname);
//NORADIO	}
//PaTMaN-NOTEAM                else
//PaTMaN-NOTEAM	{

//PaTMaN-REMOVE_TEAMPLAY FULL MOD TO CODE BELOW
/*
                Com_sprintf (text, sizeof(text), "%s%s: ", 
                        (teamplay->value && (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD)) ? "[DEAD] " : "",
                        ent->client->pers.netname);
*/

                Com_sprintf (text, sizeof(text), "%s: ",ent->client->pers.netname);

//PaTMaN-NOTEAM	}

        offset_of_text = strlen(text);  //FB 5/31/99
        
        if (arg0)
        {
                strcat (text, gi.argv(0));
                strcat (text, " ");
                strcat (text, gi.args());
        }
        else
        {
                p = gi.args();

                if (*p == '"')
                {
                        p++;
                        p[strlen(p)-1] = 0;
                }
                strcat(text, p);
        }

        // don't let text be too long for malicious reasons
        // ...doubled this limit for Axshun -FB
        if (strlen(text) > 300)
                text[300] = 0;

        if (ent->solid != SOLID_NOT && ent->deadflag != DEAD_DEAD)
                ParseSayText(ent, text + offset_of_text);  //FB 5/31/99 - offset change
                                // this will parse the % variables, 
                                // and again check 300 limit afterwards -FB
                                // (although it checks it without the name in front, oh well)

        strcat(text, "\n");

        if (flood_msgs->value)
        {
                cl = ent->client;

                if (level.time < cl->flood_locktill) 
                {
                        gi.cprintf(ent, PRINT_HIGH, "You can't talk for %d more seconds.\n",
                                        (int)(cl->flood_locktill - level.time));
                        return;
                }
                i = cl->flood_whenhead - flood_msgs->value + 1;
                if (i < 0)
                        i = (sizeof(cl->flood_when)/sizeof(cl->flood_when[0])) + i;
                if (cl->flood_when[i] && 
                        level.time - cl->flood_when[i] < flood_persecond->value) 
                {
                        cl->flood_locktill = level.time + flood_waitdelay->value;
                                gi.cprintf(ent, PRINT_HIGH, "You can't talk for %d seconds.\n",
                                                (int)flood_waitdelay->value);
                        return;
                }
                cl->flood_whenhead = (cl->flood_whenhead + 1) %
                                (sizeof(cl->flood_when)/sizeof(cl->flood_when[0]));
                cl->flood_when[cl->flood_whenhead] = level.time;
        }

        if (dedicated->value)
                gi.cprintf(NULL, PRINT_CHAT, "%s", text);

        for (j = 1; j <= game.maxclients; j++)
        {
                other = &g_edicts[j];
                if (!other->inuse)
                        continue;
                if (!other->client)
                        continue;
                if (team)
                {
                        if (!OnSameTeam(ent, other))
                                continue;
                }
//NORADIO                if (partner_msg)
//NORADIO                {
//NORADIO                        if (other != ent->client->resp.radio_partner && other != ent)
//NORADIO                                continue;
//NORADIO                }
//FIREBLADE
//PaTMaN-REMOVE_TEAMPLAY                if (teamplay->value && team_round_going)
//PaTMaN-REMOVE_TEAMPLAY                {
//PaTMaN-REMOVE_TEAMPLAY                        if ((ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD) && 
//PaTMaN-REMOVE_TEAMPLAY                                (other->solid != SOLID_NOT && other->deadflag != DEAD_DEAD))
//PaTMaN-REMOVE_TEAMPLAY                                continue;
//PaTMaN-REMOVE_TEAMPLAY                }
//FIREBLADE             
                gi.cprintf(other, PRINT_CHAT, "%s", text);
        }
}

void Cmd_PlayerList_f(edict_t *ent)
{
      int i;
      char st[80];
      char text[1280];
      edict_t *e2;

      // connect time, ping, score, name
      *text = 0;
      for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++) {
              if (!e2->inuse)
                      continue;

                Com_sprintf(st, sizeof(st), "%02d:%02d %4d %s\n",
                        (level.framenum - e2->client->resp.enterframe) / 600,
                        ((level.framenum - e2->client->resp.enterframe) % 600)/10,
                        e2->client->ping,
                        e2->client->pers.netname);

              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
                      sprintf(text+strlen(text), "...\n");
                      gi.cprintf(ent, PRINT_HIGH, "%s", text);
                      return;
              }
              strcat(text, st);
      }
      gi.cprintf(ent, PRINT_HIGH, "%s", text);
}

/* //old
void Cmd_PlayerListInfo(edict_t *ent)
{
      int i;
      char st[90];
      char text[1280];
      edict_t *e2;

      *text = 0;
      for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++) {
              if (!e2->inuse)
                      continue;

                Com_sprintf(st, sizeof(st), "%s%s%s%s\n",e2->client->pers.netname,
					//here for spectating <name>
					(e2->client->pers.spectator) ? " (spectating " : "",
					(e2->client->resp.id_name && e2->client->pers.spectator) ? "somebody)" : "",
					(!e2->client->resp.id_name && e2->client->pers.spectator) ? "nobody)" : "");

					              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
                      sprintf(text+strlen(text), "...\n");
                      gi.cprintf(ent, PRINT_HIGH, "%s", text);
                      return;
             }
              strcat(text, st);
			  
			  Com_sprintf(st, sizeof(st), "  fd:%d last fd:%d falls:%d highspeed:%d speed:%d gravity:%d\n",
				  e2->client->ps.stats[STAT_FALLDMG],
				  e2->client->ps.stats[STAT_FALLDMGLAST],
				  e2->client->ps.stats[STAT_FALLCOUNT],
				  e2->client->resp.highspeed,
				  e2->client->ps.stats[STAT_SPEEDX],
				  e2->client->resp.cgravity);
			  
              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
				  sprintf(text+strlen(text), "...\n");
				  gi.cprintf(ent, PRINT_HIGH, "%s", text);
				  return;
			  }
              strcat(text, st);

			  Com_sprintf(st, sizeof(st), "  las:%i slip:%i sil:%i kick:%i id:%i range:%i puppet:%i ir:%i lca:%i\n",
				  e2->client->resp.toggles & TG_LASER ? 1 : 0,
				  e2->client->resp.toggles & TG_SLIPPERS ? 1 : 0,
				  e2->client->resp.toggles & TG_SILENCER ? 1 : 0,
				  e2->client->resp.toggles & TG_KICKABLE ? 1 : 0,
				  e2->client->resp.toggles & TG_HUD_ID ? 1 : 0,
				  e2->client->resp.toggles & TG_HUD_RANGE ? 1 : 0,
				  e2->client->resp.toggles & TG_PUPPET ? 1 : 0,
				  e2->client->resp.toggles & TG_IR ? 1 : 0,
				  e2->client->resp.toggle_lca ? 1 : 0);
			  
              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
				  sprintf(text+strlen(text), "...\n");
				  gi.cprintf(ent, PRINT_HIGH, "%s", text);
				  return;
			  }
			  strcat(text, st);
				  
				  Com_sprintf(st, sizeof(st), "  team:%i gren:%i flash:%i mtext:%i mjumps:%i mfalls:%i\n",
					  e2->client->resp.toggles & TG_TEAMJUMP ? 1 : 0,
					  e2->client->resp.toggles & TG_GRENJUMP ? 1 : 0,
					  e2->client->resp.toggles & TG_MFLASH ? 1 : 0,
					  e2->client->resp.toggles & TG_MTEXT ? 1 : 0,
					  e2->client->resp.toggles & TG_MARKJUMPS ? 1 : 0,
					  e2->client->resp.toggles & TG_MARKFALLS ? 1 : 0);
				  
				  if (strlen(text) + strlen(st) > sizeof(text) - 100) {
                      sprintf(text+strlen(text), "...\n");
                      gi.cprintf(ent, PRINT_HIGH, "%s", text);
                      return;
				  }
				  strcat(text, st);
      }
      gi.cprintf(ent, PRINT_HIGH, "%s", text);
}
*/

void Cmd_Spectators(edict_t *ent)
{
	int i;
	char st[90];
	edict_t *e2;
	qboolean PrintHeader=true;

	for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++) {
		if (!e2->inuse)
			continue;
		
		if (e2->client->pers.spectator)
		{
			if (PrintHeader)
			{
				gi.cprintf(ent,PRINT_HIGH,"Spectator Name    Spectating Name   Class   Spec Mode \n");
				gi.cprintf(ent,PRINT_HIGH,"----------------  ----------------  ------  ----------\n");
				//                        "1234567890123456  1234567890123456  Player  Eyes      \n");
				//                        "1234567890123456  1234567890123456  Puppet  3rd Person\n");
				//                        "1234567890123456  1234567890123456  Pup     3rd Person\n");
				//                        "123456789012345678123456789012345678123456781234567890\n");
				PrintHeader=false;
			}
			sprintf(st,"%-18s",e2->client->pers.netname);
			if (e2->client->chase_target)
			{
				if (!Q_stricmp(e2->client->chase_target->classname,"player"))
					sprintf(st,"%s%-18s%-8s",st,e2->client->chase_target->client->pers.netname,"Player");
				else if (!Q_stricmp(e2->client->chase_target->classname,"puppet"))
					sprintf(st,"%s%-18s%-8s",st,e2->client->chase_target->lastowner->client->pers.netname,"Puppet");
				else if (!Q_stricmp(e2->client->chase_target->classname,"pup"))
					sprintf(st,"%s%-18s%-8s",st,e2->client->chase_target->owner->lastowner->client->pers.netname,"Pup");
				else
					sprintf(st,"%s%-18s%-8s",st,"bad chase target","<error>");
				if (e2->client->chase_mode == 2)
					sprintf(st,"%sEyes",st);
				else if (e2->client->chase_mode == 1)
					sprintf(st,"%s3rd Person",st);
			}
			else
				sprintf(st,"%s%-18s%-8sNone",st,"Nobody","None");
			gi.cprintf(ent,PRINT_HIGH,"%s\n",st);
		}
	}
	if (PrintHeader)
		gi.cprintf(ent,PRINT_HIGH,"Nobody is currently spectating\n");
}

/*
void Cmd_PlayerListInfo(edict_t *ent)
{
      int i;
      char st[90];
      char text[1280];
      edict_t *e2;

      *text = 0;
      for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++) {
              if (!e2->inuse)
                      continue;

			  sprintf(st,"%s",e2->client->pers.netname);
			  if (e2->client->pers.spectator)
			  {
				  
				  sprintf(st,"%s (spectating ",st);
				  if (e2->client->chase_target)
				  {
					  if (!Q_stricmp(e2->client->chase_target->classname,"player"))
						  sprintf(st,"%s%s",st,e2->client->chase_target->client->pers.netname);
					  else if (!Q_stricmp(e2->client->chase_target->classname,"puppet"))
						  sprintf(st,"%s%s's puppet ",st,e2->client->chase_target->lastowner->client->pers.netname);
					  else if (!Q_stricmp(e2->client->chase_target->classname,"pup"))
						  sprintf(st,"%s%s's pup ",st,e2->client->chase_target->owner->lastowner->client->pers.netname);
					  else
						  sprintf(st,"%s%s (bad chase target!) ",st,e2->client->chase_target->classname);
					  if (e2->client->chase_mode == 2)
						  sprintf(st,"%sin eyes",st);
					  else if (e2->client->chase_mode == 1)
						  sprintf(st,"%sin 3rd person",st);
					  sprintf(st,"%s)",st);
				  }
				  else
					  sprintf(st,"%snobody)",st);
			  }
			  sprintf(st,"%s\n",st);

					  if (strlen(text) + strlen(st) > sizeof(text) - 100) {
                      sprintf(text+strlen(text), "...\n");
                      gi.cprintf(ent, PRINT_HIGH, "%s", text);
                      return;
             }
              strcat(text, st);
			  
//			  Com_sprintf(st, sizeof(st), "  fd:%d last fd:%d falls:%d highspeed:%d speed:%d gravity:%d\n",
			  Com_sprintf(st, sizeof(st), "  last fd:%d highspeed:%d speed:%d gravity:%d\n",
//				  e2->client->ps.stats[STAT_FALLDMG],
				  e2->client->ps.stats[STAT_FALLDMGLAST],
//				  e2->client->ps.stats[STAT_FALLCOUNT],
				  e2->client->resp.highspeed,
				  e2->client->ps.stats[STAT_SPEEDX],
				  e2->client->resp.cgravity);
			  
              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
				  sprintf(text+strlen(text), "...\n");
				  gi.cprintf(ent, PRINT_HIGH, "%s", text);
				  return;
			  }
              strcat(text, st);
			  
			  Com_sprintf(st, sizeof(st), "  kick:%i puppet:%i trailing:%i tflash:%i shootable:%i lca:%i\n",
				  e2->client->resp.toggles & TG_KICKABLE ? 1 : 0,
				  e2->client->puppet ? 1 : 0,
				  e2->client->resp.toggles & TG_TRAIL ? 1 : 0,
				  e2->client->resp.toggles & TG_TFLASH ? 1 : 0,
				  e2->client->resp.toggles & TG_SHOOTABLE ? 1 : 0,
				  e2->client->resp.toggle_lca ? 1 : 0);
			  
              if (strlen(text) + strlen(st) > sizeof(text) - 100) {
				  sprintf(text+strlen(text), "...\n");
				  gi.cprintf(ent, PRINT_HIGH, "%s", text);
				  return;
			  }
			  strcat(text, st);
			  strcat(text,"\0");
				  
				  Com_sprintf(st, sizeof(st), "  teamj:%i grenj:%i mflash:%i mtext:%i mjumps:%i mfalls:%i\n",
					  e2->client->resp.toggles & TG_TEAMJUMP ? 1 : 0,
					  e2->client->resp.toggles & TG_GRENJUMP ? 1 : 0,
					  e2->client->resp.toggles & TG_MFLASH ? 1 : 0,
					  e2->client->resp.toggles & TG_MTEXT ? 1 : 0,
					  e2->client->resp.toggles & TG_MARKJUMPS ? 1 : 0,
					  e2->client->resp.toggles & TG_MARKFALLS ? 1 : 0);
				  
				  if (strlen(text) + strlen(st) > sizeof(text) - 100) {
                      sprintf(text+strlen(text), "...\n");
                      gi.cprintf(ent, PRINT_HIGH, "%s", text);
                      return;
				  }
				  strcat(text, st);
				  
      }
      gi.cprintf(ent, PRINT_HIGH, "%s", text);
}
*/

//PaTMaN - Teleporter
void Cmd_Teleporter (edict_t *ent)
{
	char            *s;

	s = gi.args();

	if ( stricmp(s, "set") == 0 )
	{
		if (ent->client->pers.spectator)
		{
			gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
			return;
		}
		
		VectorCopy (ent->s.origin, ent->client->resp.teleport_origin);
		VectorCopy(ent->client->v_angle, ent->client->resp.teleport_v_angle);

		if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			if (!(ent->client->resp.wmodes & OS_DUCKING)) ent->client->resp.wmodes += OS_DUCKING;
			//ent->client->resp.ducking = 1;
		ent->client->resp.teleport_stored = true;
		gi.centerprintf (ent, "Teleport Location Stored\n");
	}
	else if ( stricmp(s, "recall") == 0 )
	{
		if (!ent->deadflag && !ent->client->pers.spectator)
		{
			if (ent->client->resp.teleport_stored)
			{
				int i;

				ent->client->jumping = 0;

				if (!(ent->client->resp.wmodes & OS_TELEPORTING))
					ent->client->resp.wmodes += OS_TELEPORTING;

				ent->movetype = MOVETYPE_NOCLIP;

				TeleFX(ent);

				gi.unlinkentity (ent);

				VectorCopy (ent->client->resp.teleport_origin, ent->s.origin);
				VectorCopy (ent->client->resp.teleport_origin, ent->s.old_origin);

				VectorClear (ent->velocity);

				ent->client->ps.pmove.pm_time = 160>>3;
				//ent->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT; //Causes bad view stuff

				ent->s.event = EV_PLAYER_TELEPORT;

				VectorClear(ent->s.angles);
				VectorClear(ent->client->ps.viewangles);
				VectorClear(ent->client->ps.kick_angles);
				VectorClear(ent->client->v_angle);
				VectorClear(ent->client->ps.pmove.delta_angles);
				VectorClear(ent->client->kick_angles);
				ent->client->fall_time = 0;
				ent->client->fall_value = 0;

				VectorCopy(ent->client->resp.teleport_v_angle, ent->client->v_angle);
				VectorCopy(ent->client->v_angle, ent->client->ps.viewangles);

				for (i=0;i<2;i++)
					ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

				if (ent->client->resp.wmodes & OS_DUCKING)
					ent->client->ps.pmove.pm_flags = PMF_DUCKED;

				// KillBox (ent);

				gi.linkentity (ent);
				ent->movetype = MOVETYPE_WALK;
			}
			else
				gi.centerprintf (ent, "Must set with %s\n",AltString("teleporter set"));
		}
		else
			gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"Teleporter commands are %s",AltString("teleporter set"));
		gi.cprintf(ent,PRINT_HIGH," and %s\n",AltString("teleporter recall"));
	}
}

void Cmd_Goto_Now_PSwitch (edict_t *ent, vec3_t origin, vec3_t angles, float vh, float max)//, float x, float y, float z, float vh, float max)
{
	int i;

	VectorClear(ent->s.angles);
	VectorClear(ent->client->ps.viewangles);
	VectorClear(ent->client->ps.kick_angles);
	VectorClear(ent->client->v_angle);
	VectorClear(ent->client->ps.pmove.delta_angles);
	VectorClear(ent->client->kick_angles);
	VectorClear (ent->velocity);
	ent->client->fall_time = 0;
	ent->client->fall_value = 0;
	ent->client->resp.wmodes |= OS_TELEPORTING;

	ent->movetype = MOVETYPE_NOCLIP;

	gi.unlinkentity (ent);

	ent->client->ps.pmove.pm_time = 160>>3;

	VectorCopy(origin,ent->s.origin);
	VectorCopy(origin,ent->s.old_origin);
	//VectorCopy(angles,ent->s.angles);
	VectorCopy(angles,ent->client->v_angle);
	VectorCopy(angles,ent->client->ps.viewangles);

	for (i=0;i<2;i++)
		ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(angles[i] - ent->client->resp.cmd_angles[i]);



	ent->maxs[2] = max;
	ent->viewheight = vh;
	if (max = 16)
	{
		ent->client->resp.wmodes |= OS_DUCKING;
		ent->client->ps.pmove.pm_flags |= PMF_DUCKED;
	}

	ent->movetype = MOVETYPE_WALK;

	gi.linkentity (ent);
}

void Cmd_Goto (edict_t *ent)
{
	int tokcount=0,i;
	char            *s, *token;
	vec3_t		teleport_goto;
	
	if (!ent->deadflag && !ent->client->pers.spectator)
	{
		if (gi.argc() == 3)
		{
			if (!stricmp(gi.args(),"trail start") || !stricmp(gi.args(),"trail end"))
			{
				if (!Q_stricmp(gi.args(),"trail start"))
					Goto_LaserTrail_Start(ent);
				else if (!Q_stricmp(gi.args(),"trail end"))
					Goto_LaserTrail_End(ent);
				return;
			}
		}
		else if (gi.argc() == 4)
		{
			s = strdup(gi.args());
			
			i=0;
			token = strtok( s, " " );

			while( token != NULL )
			{
				teleport_goto[i] = 0;
				teleport_goto[i] = atoi(token);
				token = strtok( NULL, " " );
				i++;
			}
			teleport_goto[2] -= ent->viewheight;
			
			ent->client->jumping = 0;
			if (!(ent->client->resp.wmodes & OS_TELEPORTING)) ent->client->resp.wmodes += OS_TELEPORTING;
			//ent->client->resp.teleporting = 1;
			ent->movetype = MOVETYPE_NOCLIP;
			
			TeleFX(ent);

			gi.unlinkentity (ent);
			
			VectorCopy (teleport_goto, ent->s.origin);
			VectorCopy (teleport_goto, ent->s.old_origin);
			
			// clear the velocity and hold them in place briefly
			VectorClear (ent->velocity);
			
			ent->client->ps.pmove.pm_time = 160>>3;		// hold time
			//ent->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;
			
			// draw the teleport splash on the player
			ent->s.event = EV_PLAYER_TELEPORT;

			VectorClear (ent->s.angles);
			VectorClear (ent->client->ps.viewangles);
			VectorClear (ent->client->v_angle);

			gi.linkentity (ent);
			
			ent->movetype = MOVETYPE_WALK;
		}
		else
			gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: goto <#> <#> <#> or goto trail start or goto trail end\n");
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
	
}

edict_t *SelectRandDeathmatchSpawnPoint (int number)
{
        edict_t *spot;
        int             count = 0;
        int             selection;

        spot = NULL;
        while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
                count++;

        if (!count)
                return NULL;

		//if random was selected, pick one
		if (number == 0)
	        selection = rand() % count + 1;
		else
		{
			//if person wanted tele 7 but only 5 found, set to 5
			if (number > count)
				number = count;
			//if person input a negative, set to 0
			else if (number < 0)
				number = 0;
			selection = number;
		}

		spot = NULL;
		do
		{
			spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
			selection--;
		} while (selection > 0);
		/*
        do
        {
                spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
                if (spot == NULL)
                        selection++;
        } while(selection--);
		*/

        return spot;
}

void Cmd_GotoP (edict_t *ent)
{
	vec3_t		teleport_goto, angles;
	edict_t *spot = NULL;
	char *buffer="\0";

	if (!ent->deadflag && !ent->client->pers.spectator)
	{
		int i;
		if (gi.argc() > 1)
		{
			buffer = strtok(gi.args()," ");
			spot = SelectRandDeathmatchSpawnPoint(atoi(buffer));
		}
		else
			spot = SelectRandDeathmatchSpawnPoint(0);

        VectorCopy (spot->s.origin, teleport_goto);
        teleport_goto[2] += 9;
        VectorCopy (spot->s.angles, angles);

		ent->client->jumping = 0;
		if (!(ent->client->resp.wmodes & OS_TELEPORTING)) ent->client->resp.wmodes += OS_TELEPORTING;
		ent->movetype = MOVETYPE_NOCLIP;
		
		TeleFX(ent);

		gi.unlinkentity (ent);
		
		VectorCopy (teleport_goto, ent->s.origin);
		VectorCopy (teleport_goto, ent->s.old_origin);
		
		// clear the velocity and hold them in place briefly
		VectorClear (ent->velocity);
		
		ent->client->ps.pmove.pm_time = 160>>3;		// hold time
		//ent->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;
		
		// draw the teleport splash on the player
		ent->s.event = EV_PLAYER_TELEPORT;
		
		VectorClear (ent->s.angles);
		VectorClear (ent->client->ps.viewangles);
		VectorClear (ent->client->v_angle);

		VectorCopy(angles,ent->s.angles);
		VectorCopy(ent->s.angles,ent->client->v_angle);

		for (i=0;i<2;i++)
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);
		
		if (ent->client->resp.spectating)
			ent->solid = SOLID_BBOX;
		else
			ent->solid = SOLID_TRIGGER;
		
		ent->deadflag = DEAD_NO;
		
		gi.linkentity (ent);
		
		ent->movetype = MOVETYPE_WALK;
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
}

void Cmd_GotoPC (edict_t *ent)
{
	vec3_t		teleport_goto, angles;
	edict_t *spot = NULL;
	char *buffer="\0";

	if (!ent->deadflag && !ent->client->pers.spectator)
	{
		int i;
		spot = SelectClosestDeathmatchSpawnPoint();

        VectorCopy (spot->s.origin, teleport_goto);
        teleport_goto[2] += 9;
        VectorCopy (spot->s.angles, angles);

		ent->client->jumping = 0;
		if (!(ent->client->resp.wmodes & OS_TELEPORTING)) ent->client->resp.wmodes += OS_TELEPORTING;
		ent->movetype = MOVETYPE_NOCLIP;
		
		TeleFX(ent);

		gi.unlinkentity (ent);
		
		VectorCopy (teleport_goto, ent->s.origin);
		VectorCopy (teleport_goto, ent->s.old_origin);
		
		// clear the velocity and hold them in place briefly
		VectorClear (ent->velocity);
		
		ent->client->ps.pmove.pm_time = 160>>3;		// hold time
		//ent->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;
		
		// draw the teleport splash on the player
		ent->s.event = EV_PLAYER_TELEPORT;
		
		VectorClear (ent->s.angles);
		VectorClear (ent->client->ps.viewangles);
		VectorClear (ent->client->v_angle);

		VectorCopy(angles,ent->s.angles);
		VectorCopy(ent->s.angles,ent->client->v_angle);

		for (i=0;i<2;i++)
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);
		
		if (ent->client->resp.spectating)
			ent->solid = SOLID_BBOX;
		else
			ent->solid = SOLID_TRIGGER;
		
		ent->deadflag = DEAD_NO;
		
		gi.linkentity (ent);
		
		ent->movetype = MOVETYPE_WALK;
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"This command cannot be used by spectators\n");
}

void Cmd_Gravity (edict_t *ent)
{
	int grav;
	char grav2[16]=""; // V1.21 FIX - 1.20.010.01

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH, "\"gravity\" is \"%i\"\n",ent->client->resp.cgravity);//ps.stats[STAT_GRAVITY]);// grav = 800;
		return;
	}
	
	if (!strcmp(gi.args(),"default"))
		grav = sv_gravity->value;
	else
	{
		grav = atoi(gi.args());
		if (grav < -9999) grav = -9999;
		if (grav > 9999) grav = 9999;
	}
	
	ent->client->resp.cgravity = grav;

	//LINUX PORT ITOA//itoa(grav,grav2,10);
	sprintf(grav2,"%i",grav);

	strcpy(gi.args(),"My gravity: ");
	strcat(gi.args(),grav2);
	if (grav == sv_gravity->value) strcat(gi.args()," (default)");
	Cmd_Say_f(ent,false,false,false);
}

void Cmd_LaserTrailWidth (edict_t *ent)
{
	int width;

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH, "\"trailwidth\" is \"%i\"\n",ent->client->resp.trail_width);
		return;
	}
	
	if (!strcmp(gi.args(),"default"))
		width = 4;
	else
	{
		width = atoi(gi.args());
		if (width < 2) width = 2;
		if (width > 9999) width = 9999;
	}
	
	ent->client->resp.trail_width = width;
}

void Cmd_Lens (edict_t *ent, char *cmd)
{
	if (ent->client->curr_weap == SNIPER_NUM)
	{
		int new_lens=-2, going_to=-2;							//init to a check val
		if (!Q_stricmp(cmd,"in"))	new_lens = ent->client->resp.sniper_mode;	//to next
		else if (!Q_stricmp(cmd,"out"))
		{
			new_lens = ent->client->resp.sniper_mode - 2;		//   to 
			if (new_lens < SNIPER_1X-1)	new_lens = SNIPER_4X;	//   previous
		}
		else
		{
			float num = atof(cmd);
			if		(num < 1.5) {	new_lens = SNIPER_6X; going_to = 0; }	//to 1x
			else if (num < 3)	{	new_lens = SNIPER_1X; going_to = 1; }	//to 2x
			else if (num < 5)	{	new_lens = SNIPER_2X; going_to = 2; }	//to 4x
			else				{	new_lens = SNIPER_4X; going_to = 3; }	//to 6x
		}
		if ((new_lens != -2)&& (ent->client->resp.sniper_mode != going_to)) //if new lens
		{
			ent->client->resp.sniper_mode = new_lens;		//set the lens
			Cmd_New_Weapon_f(ent);							//do the weapon cmd
		}
	}
}

void Cmd_PSwitch(edict_t *ent)
{
	if (ent->client->puppet)		//if puppet exists
	{
		if (ent->client->resp.puppetdemo_state == PUPPET_NEW_NOTHING)	//if not recording or playing a demo
		{
			if (!ent->deadflag && !ent->client->pers.spectator)	//if not dead and not a spectator
			{
				vec3_t puppet_pos, puppet_angles;
				float viewheight, maxs;

				//cycle puppet to new pos. remove pup first if exists
				if (ent->client->puppet->movetarget)
				{
					edict_t *pup;
					pup = ent->client->puppet->movetarget;
					//VectorCopy(pup->s.origin,puppet_pos);
					Puppet_Spawn(ent,false,true);
					Pup_Think(pup);
				}
				VectorCopy(ent->client->puppet->s.origin,puppet_pos);
				VectorCopy(ent->client->puppet->pos1,puppet_angles);
				viewheight = ent->client->puppet->viewheight;
				maxs = ent->client->puppet->maxs[2];
				Puppet_Spawn(ent,false,true);
				Puppet_Spawn(ent,false,true);
				Cmd_Goto_Now_PSwitch(ent,puppet_pos,puppet_angles,viewheight,maxs);//,puppet_pos[0],puppet_pos[1],puppet_pos[2],ent->client->puppet->viewheight, ent->client->puppet->maxs[2]);
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"You can not use this command while in spectator mode\n");
		}
		else
			gi.cprintf(ent,PRINT_HIGH,"You can not use this command while playing a puppet demo\n");
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"You must have a puppet to use this command\n");
}

/*
//Some test thing from far back... have fun
void Cmd_HitMe (edict_t *ent)
{
	vec3_t forward, right, start, offset, normal;
	int i, height;
	if (ent->client->pers.firing_style == ACTION_FIRING_CLASSIC)
		height = 8;
	else
		height = 0;
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 0, 8, ent->viewheight-height);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);
	for (i=0;i<3;i++)
		forward[i] *= -1;
	gi.cprintf(ent,PRINT_HIGH,"%f:%f:%f\n",forward[0],forward[1],forward[2]);
	VectorSet(normal, 0, 0, 0);
	T_Damage (ent, ent, ent, forward, ent->s.origin, normal, 200, 200, DAMAGE_BULLET, MOD_MK23);
}
*/

void Cmd_HitMe(edict_t *ent, int velocity)
{
	vec3_t forward;
	AngleVectors (ent->client->v_angle, forward, NULL, NULL);
	VectorScale (forward, velocity, ent->velocity);
}

qboolean CharIsUInt(char s[128])
{
	char b[128]="";
	int v;
	v = atoi(s);
	sprintf(b,"%i",v);
	if (!stricmp(s,b))
	{
		if (v < 0)
			return false;
		else
			return true;
	}
	else
		return false;
}

/*
=================
ClientCommand
=================
*/
void ClientCommand (edict_t *ent)
{
        char    *cmd;

        if (!ent->client)
                return;         // not fully in game yet

        cmd = gi.argv(0);

		if (Q_stricmp (cmd, "players") == 0)
		{
			Cmd_Players_f (ent);
			return;
		}
		if (Q_stricmp (cmd, "say") == 0)
		{
			Cmd_Say_f (ent, false, false, false);
			return;
		}
		if (Q_stricmp (cmd, "say_team") == 0)
		{
			Cmd_Say_f (ent, true, false, false);
			return;
		}
		if (Q_stricmp (cmd, "score") == 0)
		{
			Cmd_Score_f (ent);
			return;
		}
		if (Q_stricmp (cmd, "help") == 0)
		{
			Cmd_Help_f (ent);
			return;
		}

		if (level.intermissiontime)
			return;

        if (Q_stricmp (cmd, "use") == 0)
                Cmd_Use_f (ent);
        else if (Q_stricmp (cmd, "noclip") == 0)
                Cmd_Noclip_f (ent);
        else if (Q_stricmp (cmd, "inven") == 0)
                Cmd_Inven_f (ent);
        else if (Q_stricmp (cmd, "invnext") == 0)
                SelectNextItem (ent, -1);
        else if (Q_stricmp (cmd, "invprev") == 0)
                SelectPrevItem (ent, -1);
        else if (Q_stricmp (cmd, "invnextw") == 0)
                SelectNextItem (ent, IT_WEAPON);
        else if (Q_stricmp (cmd, "invprevw") == 0)
                SelectPrevItem (ent, IT_WEAPON);
        else if (Q_stricmp (cmd, "invnextp") == 0)
                SelectNextItem (ent, IT_POWERUP);
        else if (Q_stricmp (cmd, "invprevp") == 0)
                SelectPrevItem (ent, IT_POWERUP);
        else if (Q_stricmp (cmd, "invuse") == 0)
                Cmd_InvUse_f (ent);
        else if (Q_stricmp (cmd, "weapprev") == 0)
                Cmd_WeapPrev_f (ent);
        else if (Q_stricmp (cmd, "weapnext") == 0)
                Cmd_WeapNext_f (ent);
        else if (Q_stricmp (cmd, "weaplast") == 0)
                Cmd_WeapLast_f (ent);
        else if (Q_stricmp (cmd, "putaway") == 0)
                Cmd_PutAway_f (ent);
        else if (Q_stricmp (cmd, "wave") == 0)
                Cmd_Wave_f (ent);
        else if (Q_stricmp (cmd, "weapon") == 0)
                Cmd_New_Weapon_f (ent);
        else if (Q_stricmp (cmd, "opendoor") == 0)
                Cmd_OpenDoor_f (ent);
        else if (Q_stricmp (cmd, "lens") == 0)
				Cmd_Lens(ent,gi.args());
        else if (Q_stricmp(cmd, "playerlist") == 0)
                Cmd_PlayerList_f(ent);
//		else if (Q_stricmp(cmd, "playerlistinfo") == 0)
//                Cmd_PlayerListInfo(ent);
		else if (Q_stricmp(cmd, "spectators") == 0)
				Cmd_Spectators(ent);
        else if (Q_stricmp(cmd, "motd") == 0)
                PrintMOTD(ent);
		else if (Q_stricmp(cmd, "jlocsave") == 0)
                SaveJLOC(ent,1);
		else if (Q_stricmp(cmd, "msave") == 0)
                SaveJLOC(ent,2);
		else if (Q_stricmp(cmd, "rhs") == 0)
                ent->client->resp.highspeed = 0;
		else if (Q_stricmp(cmd, "toggle") == 0)
                Cmd_Toggle(ent);
		else if (Q_stricmp(cmd, "tog") == 0)
                Cmd_Toggle(ent);
		else if (Q_stricmp (cmd, "teleporter") == 0)
				Cmd_Teleporter (ent);
		else if (Q_stricmp (cmd, "goto") == 0)
				Cmd_Goto (ent);
		else if (Q_stricmp (cmd, "gravity") == 0)
				Cmd_Gravity (ent);
		else if (Q_stricmp (cmd, "remviewmarkers") == 0)
				RemoveMarkersInView(ent);
		else if (Q_stricmp (cmd, "remallmarkers") == 0)
				RemoveAllMarkers(ent);
		else if (Q_stricmp (cmd, "markercount") == 0)
				gi.cprintf(ent,PRINT_HIGH,"Marker Count = %i\n",MarkersInt());
		else if (Q_stricmp (cmd, "trailcount") == 0)
				gi.cprintf(ent,PRINT_HIGH,"Trail Count = %i\n",TrailsInt()-1);
		else if (Q_stricmp (cmd, "demomode") == 0)
				Cmd_DemoMode_f(ent);
		else if (Q_stricmp (cmd, "lca") == 0)
				Cmd_PMLCA_f(ent);
		else if (Q_stricmp (cmd, "spawnp") == 0)
				Cmd_GotoP(ent);
		else if (Q_stricmp (cmd, "spawnc") == 0)
				Cmd_GotoPC(ent);
		else if (Q_stricmp(cmd, "puppet") == 0)
				Puppet_Spawn(ent,false,false);
		else if (Q_stricmp(cmd, "pswitch") == 0)
			Cmd_PSwitch(ent);
		else if (Q_stricmp(cmd, "namemarker") == 0 )
				Cmd_NameMarker(ent);
		else if (Q_stricmp(cmd, "trailwidth") == 0 )
				Cmd_LaserTrailWidth(ent);
		else if (Q_stricmp(cmd, "trailcolor") == 0)
				TCOLORWORDtoNUM(ent);
		else if (Q_stricmp(cmd, "markercolor") == 0)
				MCOLORWORDtoNUM(ent,1);
		else if (Q_stricmp(cmd, "jumpmarkercolor") == 0)
				MCOLORWORDtoNUM(ent,2);
		else if (Q_stricmp(cmd, "fallmarkercolor") == 0)
				MCOLORWORDtoNUM(ent,3);
		else if (Q_stricmp(cmd, "puppetshootarea") == 0)
				LOCWORDtoNUM(ent);

//LASER TRAIL JTRAIL
		else if (Q_stricmp(cmd, "jtrailsave") == 0)
				SaveJTRAIL(ent,1);
		else if (Q_stricmp(cmd, "tsave") == 0)
				SaveJTRAIL(ent,2);

//LASER TRAIL REMOVING
		else if (Q_stricmp(cmd, "remalltrails") == 0 )
				DeleteLaserTrail_All(ent);
		else if (Q_stricmp(cmd, "remlasttrail") == 0)
				DeleteLaserTrail_Last(ent);
		else if (Q_stricmp(cmd, "rempointtrail") == 0)
		{
			int mass = GetBestTrailMassByView(ent);
			if (mass != -1)
				DeleteLaserTrail_By_Mass (ent,mass,"View");
		}
		else if (Q_stricmp(cmd, "remviewlinks") == 0 )
				DeleteLaserTrail_LinksInView(ent);
		else if (Q_stricmp(cmd, "remviewtrails") == 0 )
				DeleteLaserTrail_Viewed(ent);
		else if (Q_stricmp(cmd, "remtouchtrail") == 0 )
				DeleteLaserTrail_By_Mass(ent,ent->client->resp.trailtouch,"Touching");

//LASER TRAIL CLEANING
		else if (Q_stricmp(cmd, "cleanalltrails") == 0 )
				CleanLaserTrail_All(ent);
		else if (Q_stricmp(cmd, "cleanlasttrail") == 0 )
				CleanLaserTrail_Last(ent);
		else if (Q_stricmp(cmd, "cleanpointtrail") == 0)
		{
			int mass = GetBestTrailMassByView(ent);
			if (mass != -1)
				CleanLaserTrail_By_Mass (ent,mass,"View");
		}
		else if (Q_stricmp(cmd, "cleanviewlinks") == 0 )
				CleanLaserTrail_LinksInView(ent);
		else if (Q_stricmp(cmd, "cleanviewtrails") == 0 )
				CleanLaserTrail_Viewed(ent);
		else if (Q_stricmp(cmd, "cleantouchtrail") == 0 )
				CleanLaserTrail_By_Mass(ent,ent->client->resp.trailtouch,"Touching");

//		else if (Q_stricmp(cmd, "ln") == 0 )
//				PlaceLaserNode(ent);
		else if (Q_stricmp(cmd, "cleanlevel") == 0 )
				CleanLevel();
		else if (Q_stricmp (cmd, "hitme") == 0)
		{
			if (ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
			{
				gi.cprintf(ent,PRINT_HIGH,"This command can not be used while recording a puppet demo\n");
				return;
			}
			else if (ent->client->resp.toggles & TG_TRAIL)
			{
				gi.cprintf(ent,PRINT_HIGH,"This command can not be used while trailing\n");
				return;
			}
			else
			{
				if (gi.argv(1) != "")
					Cmd_HitMe(ent,atoi(gi.argv(1)));
			}				
		}
		else if (Q_stricmp(cmd, "votemap") == 0)
		{
			if (gi.argc() == 1)
				gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: votemap <mapname>\n");
			else
				CallMapVote(ent);
		}
		else if ((Q_stricmp(cmd, "viewvotes") == 0) ||
			(Q_stricmp(cmd, "mapvotes") == 0))
			ListMapVote(ent,true);
		else if (Q_stricmp(cmd, "pclear") == 0 )
			PuppetDemo_pclear(ent);
		else if (Q_stricmp(cmd, "precord") == 0 )
			PuppetDemo_precord(ent);
		else if (Q_stricmp(cmd, "pstop") == 0 )
			PuppetDemo_pstop(ent);
		else if (Q_stricmp(cmd, "pplay") == 0 )
			PuppetDemo_pplay(ent);
		else if (Q_stricmp(cmd, "psave") == 0)
		{
			if (gi.argc() > 1)
				PuppetDemo_psave(ent);
			else
				gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: psave <filename>\n");
		}
		else if (Q_stricmp(cmd, "fload") == 0)
				FileLoad(ent);
		else if (Q_stricmp(cmd, "finfo") == 0)
		{
			if (gi.argc() > 1)
				DisplayFileInfoFinfo(ent);
			else
				gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: finfo <type> <filename> or finfo <type> flist <#>\n");
		}
		else if (Q_stricmp(cmd, "ps-") == 0)
			PuppetDemo_psminus(ent);
		else if (Q_stricmp(cmd, "ps+") == 0)
			PuppetDemo_psplus(ent);
		else if (Q_stricmp(cmd, "pspause") == 0)
			PuppetDemo_pspause(ent);
		else if (Q_stricmp(cmd, "psstep-") == 0)
			PuppetDemo_psstepminus(ent);
		else if (Q_stricmp(cmd, "psstep+") == 0)
			PuppetDemo_psstepplus(ent);
		else if (Q_stricmp(cmd, "fb_menu_max") == 0)
		{
			if (gi.argc() > 1)
			{
				ent->client->fb_menu_max = atoi(gi.args());
				if (ent->client->fb_menu_max < 1)
					ent->client->fb_menu_max = 1;
				if (ent->client->fb_menu_max > 15)
					ent->client->fb_menu_max = 15;
				ent->client->fb_menu_page = 1;
				if (ent->client->inbrowsermenu == true) {
	                PMenu_Close(ent);
					OpenFBMenu(ent, 1);
				}
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"\"fb_menu_max\" is \"%i\"\n",ent->client->fb_menu_max);
		}
		else if (Q_stricmp(cmd, "fb_con_max") == 0)
		{
			if (gi.argc() > 1)
			{
				ent->client->fb_con_max = atoi(gi.args());
				if (ent->client->fb_con_max < 1)
					ent->client->fb_con_max = 1;
				if (ent->client->fb_con_max > 15)
					ent->client->fb_con_max = 15;
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"\"fb_con_max\" is \"%i\"\n",ent->client->fb_con_max);
		}
		else if (Q_stricmp(cmd, "jrecord") == 0)
		{
			if (gi.argc() > 1)
			{
				// V1.21 FIX ADD/MODIFY - 1.20.008.01
				char filename[MAX_STR_LEN]="";
				sprintf(filename,"%s",strtok(gi.args()," "));
				if (strlen(filename) > MAX_STR_LEN)
					gi.cprintf(ent,PRINT_HIGH,"jrecord error: Filename specified is too long.\n");
				else
				{
					char StuffText[MAX_STR_LEN+25] = "";
					sprintf(StuffText,"precord silent nostop;record %s",filename);
					stuffcmd(ent, StuffText);
				}
				// V1.21 FIX ADD/MODIFY - 1.20.008.01
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: jrecord <filename> (Note: You have to manually save your puppetdemo with psave after jstop)\n");
		}
		else if (Q_stricmp(cmd, "jstop") == 0)
		{
			stuffcmd(ent, "stop;pstop silent");
		}
		else if (Q_stricmp(cmd, "flist") == 0)
			ListAllFilesForMap(ent);
		else if (Q_stricmp(cmd, "fdownload") == 0)
		{
			if (gi.argc() > 1)
			{
				char buf[MAX_STR_LEN];
				sprintf(buf,"%s",strtok(gi.args()," "));
				if (!Q_stricmp(buf,"flist"))
				{
					if (gi.argc() > 2)
					{
						
						int type=FTYPE_NONE;
						sprintf(buf,"%s",strtok(NULL," "));
						if (!Q_stricmp(buf,"puppet"))
							type = FTYPE_PDEMO;
						else if (!Q_stricmp(buf,"trail"))
							type = FTYPE_JTRAIL;
						else if (!Q_stricmp(buf,"marker"))
							type = FTYPE_JLOC;
						else
						{
							gi.cprintf(ent,PRINT_HIGH,"<type> can only be \"puppet\", \"trail\", or \"marker\"\n");
							return;
						}

						if (gi.argc() > 3)
						{
							int number=atoi(strtok(NULL," "));
							sprintf(buf,"%s",GetFileFromFList_PORT(number,type));
							if (!Q_stricmp(buf,"(null)"))
							{
								if (type == FTYPE_PDEMO)
									gi.cprintf(ent,PRINT_HIGH,"Puppet demo #%i does not exist!\n",number);
								else if (type == FTYPE_JTRAIL)
									gi.cprintf(ent,PRINT_HIGH,"Trail file #%i does not exist!\n",number);
								else if (type == FTYPE_JLOC)
									gi.cprintf(ent,PRINT_HIGH,"Marker file #%i does not exist!\n",number);
							}
							else
							{
								char StuffText[MAX_STR_LEN+40]="";
								if (type == FTYPE_PDEMO)
									sprintf(StuffText,"download puppetdemo/%s-%s.pdm",level.mapname,buf);
								else if (type == FTYPE_JTRAIL)
									sprintf(StuffText,"download jtrail/%s-%s.jtrail",level.mapname,buf);
								else if (type == FTYPE_JLOC)
									sprintf(StuffText,"download jloc/%s-%s.jloc",level.mapname,buf);
								stuffcmd(ent, StuffText);
							}
						}
						else
							gi.cprintf(ent,PRINT_HIGH,"You must specify the file number to download\n");
					}
					else
						gi.cprintf(ent,PRINT_HIGH,"You must specify the type of file to download\n");
				}
				else
				{
					char filename[MAX_STR_LEN]="";
					int type=FTYPE_NONE;
					
					if (!Q_stricmp(buf,"puppet"))
						type = FTYPE_PDEMO;
					else if (!Q_stricmp(buf,"trail"))
						type = FTYPE_JTRAIL;
					else if (!Q_stricmp(buf,"marker"))
						type = FTYPE_JLOC;
					else
					{
						gi.cprintf(ent,PRINT_HIGH,"<type> can only be \"puppet\", \"trail\", or \"marker\"\n");
						return;
					}
					
					if (gi.argc() < 3)
					{
						gi.cprintf(ent,PRINT_HIGH,"You must specify a filename to download\n");
						return;
					}

					sprintf(buf,"%s",strtok(NULL," "));
					// V1.21 FIX ADD/MODIFY - 1.20.009.01 (Since modified)
					if (type == FTYPE_PDEMO)
                        sprintf(filename,"puppetdemo/%s-%s.pdm",level.mapname,buf);
					else if (type == FTYPE_JTRAIL)
                        sprintf(filename,"jtrail/%s-%s.jtrail",level.mapname,buf);
					else if (type == FTYPE_JLOC)
                        sprintf(filename,"jloc/%s-%s.jloc",level.mapname,buf);

					if (strlen(filename) > MAX_STR_LEN)
						gi.cprintf(ent,PRINT_HIGH,"Filename specified is too long.\n");
					else
					{
						char StuffText[MAX_STR_LEN + 40]="";
						sprintf(StuffText,"download %s",filename);
						stuffcmd(ent, StuffText);
					}
					// V1.21 FIX ADD/MODIFY - 1.20.009.01 (Since modified)
				}
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: fdownload <type> <filename> or fdownload flist <type> <#>\n");
		}
		else if (Q_stricmp(cmd,"spectarg") == 0)
		{
			if (ent->client->pers.spectator == 1)
			{
				if (ent->client->chase_mode == 0)
				{
					edict_t *targ=NULL;
					targ = GetIDView(ent);
					if (targ)
					{
						ent->client->chase_target = targ;
						ent->client->resp.last_chase_target = targ;
						Spec_GetChaseTarget(ent);
						if (ent->client->chase_target)
						{
							ent->client->chase_mode = 1;
							Spec_UpdateChaseCam(ent);
						}
					}
				}
			}
		}
		else if (Q_stricmp(cmd,"grenadejumplist") == 0)
		{
			if (gi.argc() == 1)
			{
				gi.cprintf(ent,PRINT_HIGH,"Grenade Jump List:");
				if (!Q_stricmp(ent->client->resp.grenadejumplist,""))
					gi.cprintf(ent,PRINT_HIGH," empty (enter player IDs separated by spaces to set, or 'clear' to reset)\n");
				else
					gi.cprintf(ent,PRINT_HIGH,"%s\n",ent->client->resp.grenadejumplist);
			}
			else
			{
				char *tok,buf[8]="";
				edict_t *temp;

				sprintf(ent->client->resp.grenadejumplist,"");
				tok = strtok(gi.args()," ");

				while (tok)
				{
					if (strlen(tok) <= 5)
					{
						if (!Q_stricmp(tok,"clear"))
							sprintf(ent->client->resp.grenadejumplist,"");
						else
						{
							if (((atoi(tok) == 0) && (!Q_stricmp(tok,"0"))) ||
								atoi(tok) != 0)
							{
								temp = g_edicts + atoi(tok) + 1;
								if (temp->inuse)
								{
									sprintf(buf," %s ",tok);
									if (!strstr(ent->client->resp.grenadejumplist,buf))
									{
										if (!Q_stricmp(ent->client->resp.grenadejumplist,""))
											sprintf(ent->client->resp.grenadejumplist," %s ",tok);
										else
											sprintf(ent->client->resp.grenadejumplist,"%s%s ",ent->client->resp.grenadejumplist,tok);
									}
								}
							}
						}
					}
					tok = strtok(NULL," ");
				}
				gi.cprintf(ent,PRINT_HIGH,"Grenade Jump List:");
				if (!Q_stricmp(ent->client->resp.grenadejumplist,""))
					gi.cprintf(ent,PRINT_HIGH," empty (enter player IDs separated by spaces to set, or 'clear' to reset)\n");
				else
					gi.cprintf(ent,PRINT_HIGH,"%s\n",ent->client->resp.grenadejumplist);
			}
		}
		else if (Q_stricmp(cmd,"cvar") == 0)
		{
			char var_name[65]="";
			char cval[65]="";
			int ival, step, i;
			char full[160]="";
			qboolean bad=false;

			//just do all the length checks first for less confusion in the coding
			i=1;
			while (stricmp(gi.argv(i),""))
			{
				if (strlen(gi.argv(i)) > 64)
				{
					gi.cprintf(ent,PRINT_HIGH,"Argument %i is too long\n",i);
					bad = true;
					break;
				}
				i++;
			}

			if ((gi.argc() > 1) && (!bad))
			{
				//got a possible sub-command
				if (!Q_stricmp(gi.argv(1),"inc"))
				{
					//good command: inc
					if (gi.argc() > 2)
					{
						//var name given
						strcpy(var_name,gi.argv(2));
						if (gi.argc() > 3)
						{
							//step given
							if (CharIsUInt(gi.argv(3)))
							{
								//verified as an integer
								step = atoi(gi.argv(3));
								if (gi.argc() > 4)
								{
									//value given
									ent->client->resp.cvar_stuff = 0;
									if (CharIsUInt(gi.argv(4)))
									{
										//verified as integer
										ival = atoi(gi.argv(4)) + step;
										sprintf(full,"set %s %i",var_name,ival);
										stuffcmd(ent,full);
									}
									else
										gi.cprintf(ent,PRINT_HIGH,"Variable %s is not a positive integer\n",AltString(var_name));
								}
								else
								{
									//value needed!
									if (ent->client->resp.cvar_stuff != 1)
									{
										//val default to $var_name
										//stuff the new command over for a single retry
										sprintf(full,"cvar %s %s %i $%s",gi.argv(1),var_name,step,var_name);
										ent->client->resp.cvar_stuff = 1;
										stuffcmd(ent,full);
									}
									else
									{
										//variable hasn't been init'd so screw this
										ent->client->resp.cvar_stuff = 0;
										gi.cprintf(ent,PRINT_HIGH,"Variable %s does not exist\n",AltString(var_name));
									}
								}
							}
							else
								gi.cprintf(ent,PRINT_HIGH,"Step must be a positive integer\n");
						}
						else
						{
							//step and value needed!
							if (ent->client->resp.cvar_stuff != 1)
							{
								//step default to 1, val default to $var_name
								//stuff the new command over for a single retry
								sprintf(full,"cvar %s %s 1 $%s",gi.argv(1),var_name,var_name);
								ent->client->resp.cvar_stuff = 1;
								stuffcmd(ent,full);
							}
						}
					}
					else
						gi.cprintf(ent,PRINT_HIGH,"Usage: cvar inc <var_name> [<step (1)>] [<start #>]\n");
				}
				else if (!Q_stricmp(gi.argv(1),"dec"))
				{
					//good command: inc
					if (gi.argc() > 2)
					{
						//var name given
						strcpy(var_name,gi.argv(2));
						if (gi.argc() > 3)
						{
							//step given
							if (CharIsUInt(gi.argv(3)))
							{
								//verified as an integer
								step = atoi(gi.argv(3));
								if (gi.argc() > 4)
								{
									//value given
									ent->client->resp.cvar_stuff = 0;
									if (CharIsUInt(gi.argv(4)))
									{
										//verified as integer
										ival = atoi(gi.argv(4)) - step;
										if (ival >= 0)
										{
											//don't go below 0!
											sprintf(full,"set %s %i",var_name,ival);
											stuffcmd(ent,full);
										}
										else
											gi.cprintf(ent,PRINT_HIGH,"Can not decrease variable %s below zero\n",AltString(var_name));
									}
									else
										gi.cprintf(ent,PRINT_HIGH,"Variable %s is not a positive integer\n",AltString(var_name));
								}
								else
								{
									//value needed!
									if (ent->client->resp.cvar_stuff != 1)
									{
										//val default to $var_name
										//stuff the new command over for a single retry
										sprintf(full,"cvar %s %s %i $%s",gi.argv(1),var_name,step,var_name);
										ent->client->resp.cvar_stuff = 1;
										stuffcmd(ent,full);
									}
									else
									{
										//variable hasn't been init'd so screw this
										ent->client->resp.cvar_stuff = 0;
										gi.cprintf(ent,PRINT_HIGH,"Variable %s does not exist\n",AltString(var_name));
									}
								}
							}
							else
								gi.cprintf(ent,PRINT_HIGH,"Step must be a positive integer\n");
						}
						else
						{
							//step and value needed!
							if (ent->client->resp.cvar_stuff != 1)
							{
								//step default to 1, val default to $var_name
								//stuff the new command over for a single retry
								sprintf(full,"cvar %s %s 1 $%s",gi.argv(1),var_name,var_name);
								ent->client->resp.cvar_stuff = 1;
								stuffcmd(ent,full);
							}
						}
					}
					else
						gi.cprintf(ent,PRINT_HIGH,"Usage: cvar dec <var_name> [<step (1)>] [<start #>]\n");
				}
			}
		}
		else if (Q_stricmp(cmd, "man") == 0)
		{
			FILE *manfile=NULL;

			if (strlen(gi.args()) > 64)
			{
				gi.cprintf(ent,PRINT_HIGH,"Command too long\n");
				return;
			}

			manfile = fopen(GAMEVERSION "/jmod.man","r");
			if (manfile)
			{
				char org_keyword[65]="";
				char cur_keyword[65]="";
				char match_buf[1400]="";
				qboolean reading=false;
				qboolean cmd_for=false;
				qboolean cmd_find=false;
				int relinkcount=0;
				int next_check=0;
				int block_comment=0;
				char readbuf[1400]="";
				char read_keyword[65]="";

				if (gi.argc() == 1)
					strcpy(org_keyword,"man");
				else
					strcpy(org_keyword,gi.args());
				strcpy(cur_keyword,org_keyword);

				if (!Q_stricmp(gi.argv(1),"for"))
				{
					if (gi.argc() > 2)
						cmd_for = true;
					else
					{
						gi.cprintf(ent,PRINT_HIGH,"You must specify a for-keyword to search for\n");
						return;
					}
				}
				else if (!Q_stricmp(gi.argv(1),"find"))
				{
					if (gi.argc() > 2)
						cmd_find = true;
					else
					{
						gi.cprintf(ent,PRINT_HIGH,"You must specify something to search for\n");
						return;
					}
				}

				while (fgets(readbuf, 1399, manfile) != NULL)
				{
					if (readbuf[strlen(readbuf)-1] == nl)		//windows
						readbuf[strlen(readbuf)-1] = '\0';
					else if (readbuf[strlen(readbuf)-2] == nl)	//linux for windows format
						readbuf[strlen(readbuf)-2] = '\0';
					if (readbuf[0] == '/')
					{
						if (readbuf[1] == '*') //enter a block comment //windows and linux
							block_comment = 1; //            block comment section start
						continue; //Skip commented lines    &^
					}
					else if (block_comment == 1)
					{
						if ((readbuf[0] == '*') && (readbuf[1] == '/')) //leave a block comment //windows and linux
							block_comment = 0; //end found
						continue; //Skip block comment sections and end
					}
					else if (reading && (readbuf[0] == '}'))
					{
						char *buf=NULL;
						if ((buf = strtok(readbuf," ")) != NULL)
						{
							if (cmd_for)
							{
								if (!Q_stricmp(buf,"}for"))
								{

									while ((buf = strtok(NULL," ")) != NULL)
									{
										if (!Q_stricmp(buf,gi.argv(2)))
										{
											if (strlen(match_buf) + strlen(buf) + 1 < 1400)
												sprintf(match_buf,"%s%s%s",match_buf,read_keyword,AltString("\n")); //reversing \n back to white below
											break;
										}
									}
								}
							}
							else if (!Q_stricmp(buf,"}link"))
							{
								buf = buf + 6;
								if (buf[0] != '\0')
								{
									gi.cprintf(ent,PRINT_HIGH,"%s is linked to ",AltString(cur_keyword));
									gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(buf));
									if (!Q_stricmp(cur_keyword,buf))
									{
										gi.cprintf(ent,PRINT_HIGH,"Man file error: %s is linked to itself\n",AltString(cur_keyword));
										fclose(manfile);
										return;
									}
									strcpy(cur_keyword,buf);
									relinkcount++;
									if (relinkcount > 4)
									{
										gi.cprintf(ent,PRINT_HIGH,"Man file error: Too many links for %s\n",AltString(org_keyword));
										fclose(manfile);
										return;
									}
									reading = false;
									next_check = 0;
									block_comment = false;
									rewind(manfile);
								}
							}
							else if (!Q_stricmp(buf,"}see"))
							{
								buf = buf + 5;
								gi.cprintf(ent,PRINT_HIGH,"See: %s\n",AltString(buf));
							}
							else if (!Q_stricmp(buf,"}syntax"))
							{
								buf = buf + 8;
								gi.cprintf(ent,PRINT_HIGH,"Syntax: %s\n",AltString(buf));
							}
						}
						continue; //parser commands are taken care of so back to the file reading
					}

					if (readbuf[0] == '\0') //windows and linux
					{
						if (reading)	//have to be reading
						{
							if (next_check > 0)
								next_check--; //skip over checks for blanks we know exist
							else
							{
								char *check=NULL;
								int block_com=0;
								long curpos;
								curpos = ftell(manfile);

								while ((check = fgets(readbuf, 1399, manfile)) != NULL)
								{
									if (readbuf[strlen(readbuf)-1] == nl)		//windows
										readbuf[strlen(readbuf)-1] = '\0';
									else if (readbuf[strlen(readbuf)-2] == nl)	//linux for windows format
										readbuf[strlen(readbuf)-2] = '\0';
									if (readbuf[0] == '\0')	//newline? count it up... //windows and linux
										next_check++;
									else if (readbuf[0] == ']')
									{
										if (!cmd_for)
										{
											//blanks close out the section, so we're outta here
											fclose(manfile);
											return;
										}
									}
									else if ((readbuf[0] == '/') && (readbuf[1] == '*')) //windows and linux
									{
										block_com = 1;
										continue;
									}
									else if (block_com == 1)
									{
										if ((readbuf[0] == '*') && (readbuf[1] == '/')) //windows and linux
											block_com = 0;
										continue;
									}
									else if (readbuf[0] != '/')		//found something else to print so go back
									{
										fseek(manfile, curpos, SEEK_SET);
										strcpy(readbuf,"\n");
										break;
									}
								}
								if ((check == NULL) && (!cmd_for))	//we hit the end of the file
									return;			//so no printing to be done
							}
						}
					}

					if (readbuf[0] == ']')
					{
						if (reading)
						{
							if (cmd_for)
								reading = false;	//just hopping in and out of keywords
							//if (cmd_find)
							//	reading = false;	//note never reading for a cmd_find
							else
							{
								fclose(manfile);	//finished reading the section so we're outta here
								return;				//
							}
						}
						//we have a keyword
						//remove the bracket for a compare test
						memmove(readbuf,readbuf+1,strlen(readbuf));
						strcpy(read_keyword,readbuf);
						if (cmd_for)
							reading = true;			//just hopping in and out of keywords
						if (cmd_find)		//note never reading for a cmd_find
						{
							if (strstr(read_keyword,gi.argv(2)))
							{
								if (strlen(match_buf) + strlen(read_keyword) + 1 < 1400)
									sprintf(match_buf,"%s%s%s",match_buf,read_keyword,AltString("\n")); //reversing \n back to white below
							}
						}
						else
						{
							if (!Q_stricmp(readbuf,cur_keyword))
								reading = true;		//found the keyword wanted
						}
					}
					else if ((reading) && (!cmd_for))	//don't print the whole file if on a for search!
						gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(readbuf));
				}

				fclose(manfile);

				if (cmd_for)
				{
					if (!strcmp(match_buf,""))
						gi.cprintf(ent,PRINT_HIGH,"Could not find any matches for %s\n",AltString(gi.argv(2)));
					else
						gi.cprintf(ent,PRINT_HIGH,"%s",AltString(match_buf));
				}
				else if (cmd_find)
				{
					if (!strcmp(match_buf,""))
						gi.cprintf(ent,PRINT_HIGH,"Could not find any matches for %s\n",AltString(gi.argv(2)));
					else
						gi.cprintf(ent,PRINT_HIGH,"%s",AltString(match_buf));
				}
				else if ((!reading) && (gi.argc() > 1)) //If we never read, then we never found it!
				{
					if (relinkcount > 0)
						gi.cprintf(ent,PRINT_HIGH,"Man file error: Broken link in %s\n",AltString(org_keyword));
					else
						gi.cprintf(ent,PRINT_HIGH,"%s was not found in the manual\n",AltString(cur_keyword));
				}
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"No man file is available currently\n");
		}
		else if (Q_stricmp(cmd, "hud_speed_source") == 0)
		{
			if (gi.argc() > 1)
			{
				char tmp[4]="";
				if (strstr(gi.argv(1),"x"))		strcat(tmp,"x");
				if (strstr(gi.argv(1),"y"))		strcat(tmp,"y");
				if (strstr(gi.argv(1),"z"))		strcat(tmp,"z");
				if (!strcmp(tmp,""))
					gi.cprintf(ent,PRINT_HIGH,"Values can be any combination of x, y, and z. Ex: xy\n");
				else
					strcpy(ent->client->pers.hud_speed_source,tmp);
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"\"hud_speed_source\" is \"%s\"\n",ent->client->pers.hud_speed_source);
		}
		//else if (Q_stricmp(cmd,"eqdebug") == 0)
		//{
		//	if (strcmp(gi.argv(1),""))
		//		ent->client->resp.eqdebug = atoi(gi.argv(1));
		//	else
		//		ent->client->resp.eqdebug = 0;
		//}
		/*else if (Q_stricmp(cmd,"testlayout") == 0)
		{
			char	string[1024];
			char	*sk;
			sk = "hard";

			// send the layout
			Com_sprintf (string, sizeof(string),
			"xv 202 yv 12 string2 \"%s\" "		// skill
			, 
			ent->client->pers.netname);
			//ent->client->inmenu = true;
			//ent->client->menu = hnd;
			gi.WriteByte (svc_layout);
			gi.WriteString (string);
			gi.unicast (ent, true);
			//debug("%s\n",string);
		}*/		
		else if ((Q_stricmp(cmd,"reload") == 0) ||
				 (Q_stricmp(cmd,"bandage") == 0) ||
				 (Q_stricmp(cmd,"radio") == 0) ||
				 (Q_stricmp(cmd,"radioteam") == 0) ||
				 (Q_stricmp(cmd,"radiogender") == 0) ||
				 (Q_stricmp(cmd,"drop") == 0) ||
				 (Q_stricmp(cmd,"unstick") == 0) ||
				 (Q_stricmp(cmd,"bandage") == 0) ||
				 (Q_stricmp(cmd,"kill") == 0) ||
				 (Q_stricmp(cmd,"punch") == 0) ||
				 (Q_stricmp(cmd,"stats") == 0))
		{
			//ignore it
		}
		else
            Cmd_Say_f (ent, false, true, false);

}

