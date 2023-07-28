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


qboolean        Pickup_Weapon (edict_t *ent, edict_t *other);
void            Use_Weapon (edict_t *ent, gitem_t *inv);
void    Drop_Weapon (edict_t *ent, gitem_t *inv);

void Weapon_Blaster (edict_t *ent);
void Weapon_Shotgun (edict_t *ent);
void Weapon_SuperShotgun (edict_t *ent);
void Weapon_Machinegun (edict_t *ent);
//void Weapon_Chaingun (edict_t *ent);
void Weapon_HyperBlaster (edict_t *ent);
void Weapon_RocketLauncher (edict_t *ent);
void Weapon_Grenade (edict_t *ent);
void Weapon_GrenadeLauncher (edict_t *ent);
void Weapon_Railgun (edict_t *ent);
void Weapon_BFG (edict_t *ent);

// zucc
void Weapon_MK23 (edict_t *ent);
void Weapon_MP5 (edict_t *ent);
void Weapon_M4 (edict_t *ent);
void Weapon_M3 (edict_t *ent);
void Weapon_HC (edict_t *ent);
void Weapon_Sniper (edict_t *ent);
void Weapon_Dual (edict_t *ent);
void Weapon_Knife (edict_t *ent);
void Weapon_Gas (edict_t *ent);
void Weapon_Marker (edict_t *ent);

gitem_armor_t jacketarmor_info  = { 25,  50, .30, .00, ARMOR_JACKET};
gitem_armor_t combatarmor_info  = { 50, 100, .60, .30, ARMOR_COMBAT};
gitem_armor_t bodyarmor_info    = {100, 200, .80, .60, ARMOR_BODY};

int      jacket_armor_index;
int      combat_armor_index;
int      body_armor_index;
int      power_screen_index;
int      power_shield_index;

#define HEALTH_IGNORE_MAX       1
#define HEALTH_TIMED            2

//PaTMaN-NOUSE void Use_Quad (edict_t *ent, gitem_t *item);
static int      quad_drop_timeout_hack;

//======================================================================

/*
===============
GetItemByIndex
===============
*/
gitem_t *GetItemByIndex (int index)
{
        if (index == 0 || index >= game.num_items)
                return NULL;

        return &itemlist[index];
}


/*
===============
FindItemByClassname

===============
*/
gitem_t *FindItemByClassname (char *classname)
{
        int             i;
        gitem_t *it;

        it = itemlist;
        for (i=0 ; i<game.num_items ; i++, it++)
        {
                if (!it->classname)
                        continue;
                if (!Q_stricmp(it->classname, classname))
                        return it;
        }

        return NULL;
}

/*
===============
FindItem

===============
*/
gitem_t *FindItem (char *pickup_name)
{
        int             i;
        gitem_t *it;

        it = itemlist;
        for (i=0 ; i<game.num_items ; i++, it++)
        {
                if (!it->pickup_name)
                        continue;
                if (!Q_stricmp(it->pickup_name, pickup_name))
                        return it;
        }

        return NULL;
}

//======================================================================

void DoRespawn (edict_t *ent)
{
        if (ent->team)
        {
                edict_t *master;
                int     count;
                int choice;

                master = ent->teammaster;

                for (count = 0, ent = master; ent; ent = ent->chain, count++)
                        ;

                choice = rand() % count;

                for (count = 0, ent = master; count < choice; ent = ent->chain, count++)
                        ;
        }

        ent->svflags &= ~SVF_NOCLIENT;
        ent->solid = SOLID_TRIGGER;
        gi.linkentity (ent);

        // send an effect
        ent->s.event = EV_ITEM_RESPAWN;
}

void SetRespawn (edict_t *ent, float delay)
{
        ent->flags |= FL_RESPAWN;
        ent->svflags |= SVF_NOCLIENT;
        ent->solid = SOLID_NOT;
        ent->nextthink = level.time + delay;
        ent->think = DoRespawn;
        gi.linkentity (ent);
}


//======================================================================
/*//PaTMaN-NOPICKUP
qboolean Pickup_Powerup (edict_t *ent, edict_t *other)
{
        int             quantity;

        quantity = other->client->pers.inventory[ITEM_INDEX(ent->item)];
        if ((skill->value == 1 && quantity >= 2) || (skill->value >= 2 && quantity >= 1))
                return false;

        if ((coop->value) && (ent->item->flags & IT_STAY_COOP) && (quantity > 0))
                return false;

        other->client->pers.inventory[ITEM_INDEX(ent->item)]++;

        if (deathmatch->value)
        {
                if (!(ent->spawnflags & DROPPED_ITEM) )
                        SetRespawn (ent, ent->item->quantity);
                if (((int)dmflags->value & DF_INSTANT_ITEMS) || ((ent->item->use == Use_Quad) && (ent->spawnflags & DROPPED_PLAYER_ITEM)))
                {
                        if ((ent->item->use == Use_Quad) && (ent->spawnflags & DROPPED_PLAYER_ITEM))
                                quad_drop_timeout_hack = (ent->nextthink - level.time) / FRAMETIME;
                        ent->item->use (other, ent->item);
                }
        }

        return true;
}
*///PaTMaN-NOPICKUP
//zucc pickup function for special items
/*//PaTMaN-NOPICKUP
qboolean Pickup_Special ( edict_t *ent, edict_t *other )
{
        if ( other->client->unique_item_total >= unique_items->value )
                return false;
        else
        {
                other->client->pers.inventory[ITEM_INDEX(ent->item)]++;
                other->client->unique_item_total++;
                if ( stricmp( ent->item->pickup_name, LASER_NAME ) == 0 )
                {
                        other->client->have_laser = 1;
                        SP_LaserSight(other, ent->item);//ent->item->use(other, ent->item);
                }
                if ( stricmp( ent->item->pickup_name, BAND_NAME ) == 0 )
                {
				*///PaTMaN-NOPICKUP
                        /*//PaTMaN-NOAMMO
                        if (other->client->pers.max_bullets < 4)
                                other->client->pers.max_bullets = 4;
                        if (other->client->pers.max_shells < 28)
                                other->client->pers.max_shells = 28;
                        if (other->client->pers.max_cells < 2)
                                other->client->pers.max_cells = 2;
                        if (other->client->pers.max_slugs < 40)
                                other->client->pers.max_slugs = 40;
                        if (other->client->pers.max_grenades < 6)
                                other->client->pers.max_grenades = 6;
                        if (other->client->pers.max_rockets < 4)
                                other->client->pers.max_rockets = 4;
                        if (other->client->knife_max < 20 )
                                other->client->knife_max = 20;
                        if (other->client->grenade_max < 4 )
                                other->client->grenade_max = 4;
						*///PaTMaN-NOAMMO
                                                // zucc for ir
/*                                              if ( ir->value && other->client->resp.ir == 0 )
                                                {
                                                        other->client->ps.rdflags |= RDF_IRGOGGLES;
                                                }
*/
/*//PaTMaN-NOPICKUP
                }                       

        
        }
        return true;
}
*///PaTMaN-NOPICKUP
/*//PaTMaN-NODROP
void Drop_Special( edict_t *ent, gitem_t *item)
{
        ent->client->unique_item_total--;
/*      if ( stricmp( item->pickup_name, LASER_NAME ) == 0 
                && ent->client->pers.inventory[ITEM_INDEX(item)] <= 1 )
        {
                ent->client->have_laser = 0;
                item->use(ent, item);
        }
        */
/*//PaTMaN-NODROP
        if ( stricmp( item->pickup_name, BAND_NAME ) == 0 
                        && ent->client->pers.inventory[ITEM_INDEX(item)] <= 1 )
        {
                        ent->client->pers.max_bullets = 2;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("Pistol Clip"))] > 2 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem("Pistol Clip"))] = 2;
                        ent->client->pers.max_shells = 14;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("12 Gauge Shells"))] > 14 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem("12 Gauge Shells"))] = 14;
                        ent->client->pers.max_cells = 1;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("M4 Clip"))] > 1 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem("M4 Clip"))] = 1;
                        
                        ent->client->grenade_max = 2;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(GRENADE_NAME))] > 2 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem(GRENADE_NAME))] = 2;
                        
                        ent->client->pers.max_rockets = 2;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("Machinegun Magazine"))] > 2 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem("Machinegun Magazine"))] = 2;
                        
                        ent->client->knife_max = 10;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(KNIFE_NAME))] > 10 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem(KNIFE_NAME))] = 10;
                        
                        ent->client->pers.max_slugs = 20;
                        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem("AP Sniper Ammo"))] > 20 )
                                ent->client->pers.inventory[ITEM_INDEX(FindItem("AP Sniper Ammo"))] = 20;
                                                
                        if ( ent->client->unique_weapon_total > unique_weapons->value && !allweapon->value ) 
                        {
                                DropExtraSpecial(ent);
                                gi.cprintf(ent, PRINT_HIGH, "One of your guns is dropped with the bandolier.\n");
                        }
                }
        Drop_Spec (ent, item);
        ValidateSelectedItem (ent);
        SP_LaserSight(ent, item);

}

// called by the "drop item" command

void DropSpecialItem ( edict_t* ent )
{

// this is the order I'd probably want to drop them in...       
        if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(BAND_NAME))] )
                Drop_Special (ent, FindItem(BAND_NAME));
        else if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(SLIP_NAME))] )
                Drop_Special (ent, FindItem(SLIP_NAME));
        else if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(SIL_NAME))] )
                Drop_Special (ent, FindItem(SIL_NAME));
        else if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(LASER_NAME))] )
                Drop_Special (ent, FindItem(LASER_NAME));
        else if ( ent->client->pers.inventory[ITEM_INDEX(FindItem(KEV_NAME))] )
                Drop_Special (ent, FindItem(KEV_NAME));
        
}
*///PaTMaN-NODROP
/*//PaTMaN-NODROP
void Drop_General (edict_t *ent, gitem_t *item)
{
        Drop_Item (ent, item);
        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);
}

*///PaTMaN-NODROP
//======================================================================
/*//PaTMaN-NOITEMS
qboolean Pickup_Adrenaline (edict_t *ent, edict_t *other)
{
        if (!deathmatch->value)
                other->max_health += 1;

        if (other->health < other->max_health)
                other->health = other->max_health;

        if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (ent, ent->item->quantity);

        return true;
}

qboolean Pickup_AncientHead (edict_t *ent, edict_t *other)
{
        other->max_health += 2;

        if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (ent, ent->item->quantity);

        return true;
}
*///PaTMaN-NOITEMS
/*//PaTMaN-NOITEMS
qboolean Pickup_Bandolier (edict_t *ent, edict_t *other)
{
*///PaTMaN-NOITEMS
//PaTMaN-NOAMMO        gitem_t *item;
//PaTMaN-NOAMMO        int             index;
/*//PaTMaN-NOAMMO
        if (other->client->pers.max_bullets < 250)
                other->client->pers.max_bullets = 250;
        if (other->client->pers.max_shells < 150)
                other->client->pers.max_shells = 150;
        if (other->client->pers.max_cells < 250)
                other->client->pers.max_cells = 250;
        if (other->client->pers.max_slugs < 75)
                other->client->pers.max_slugs = 75;

        item = FindItem("Bullets");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_bullets)
                        other->client->pers.inventory[index] = other->client->pers.max_bullets;
        }

        item = FindItem("Shells");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_shells)
                        other->client->pers.inventory[index] = other->client->pers.max_shells;
        }
*///PaTMaN-NOAMMO
/*//PaTMaN-NOITEMS
        if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (ent, ent->item->quantity);

        return true;
}
*///PaTMaN-NOITEMS
/*//PaTMaN-NOITEMS
qboolean Pickup_Pack (edict_t *ent, edict_t *other)
{
//PaTMaN-NOAMMO        gitem_t *item;
//PaTMaN-NOAMMO        int             index;
*///PaTMaN-NOITEMS
/*//PaTMaN-NOAMMO
        if (other->client->pers.max_bullets < 300)
                other->client->pers.max_bullets = 300;
        if (other->client->pers.max_shells < 200)
                other->client->pers.max_shells = 200;
        if (other->client->pers.max_rockets < 100)
                other->client->pers.max_rockets = 100;
        if (other->client->pers.max_grenades < 100)
                other->client->pers.max_grenades = 100;
        if (other->client->pers.max_cells < 300)
                other->client->pers.max_cells = 300;
        if (other->client->pers.max_slugs < 100)
                other->client->pers.max_slugs = 100;

        item = FindItem("Bullets");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_bullets)
                        other->client->pers.inventory[index] = other->client->pers.max_bullets;
        }

        item = FindItem("Shells");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_shells)
                        other->client->pers.inventory[index] = other->client->pers.max_shells;
        }

        item = FindItem("Cells");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_cells)
                        other->client->pers.inventory[index] = other->client->pers.max_cells;
        }

        item = FindItem("Grenades");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_grenades)
                        other->client->pers.inventory[index] = other->client->pers.max_grenades;
        }

        item = FindItem("Rockets");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_rockets)
                        other->client->pers.inventory[index] = other->client->pers.max_rockets;
        }

        item = FindItem("Slugs");
        if (item)
        {
                index = ITEM_INDEX(item);
                other->client->pers.inventory[index] += item->quantity;
                if (other->client->pers.inventory[index] > other->client->pers.max_slugs)
                        other->client->pers.inventory[index] = other->client->pers.max_slugs;
        }
*///PaTMaN-NOAMMO
/*//PaTMaN-NOITEMS
        if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (ent, ent->item->quantity);

        return true;
}
*///PaTMaN-NOITEMS
//======================================================================
/*//PaTMaN-NOUSE
void Use_Quad (edict_t *ent, gitem_t *item)
{
        int             timeout;

        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);

        if (quad_drop_timeout_hack)
        {
                timeout = quad_drop_timeout_hack;
                quad_drop_timeout_hack = 0;
        }
        else
        {
                timeout = 300;
        }

        if (ent->client->quad_framenum > level.framenum)
                ent->client->quad_framenum += timeout;
        else
                ent->client->quad_framenum = level.framenum + timeout;

        gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage.wav"), 1, ATTN_NORM, 0);
}

//======================================================================

void Use_Breather (edict_t *ent, gitem_t *item)
{
        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);

        if (ent->client->breather_framenum > level.framenum)
                ent->client->breather_framenum += 300;
        else
                ent->client->breather_framenum = level.framenum + 300;

//      gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage.wav"), 1, ATTN_NORM, 0);
}

//======================================================================

void Use_Envirosuit (edict_t *ent, gitem_t *item)
{
        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);

        if (ent->client->enviro_framenum > level.framenum)
                ent->client->enviro_framenum += 300;
        else
                ent->client->enviro_framenum = level.framenum + 300;

//      gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage.wav"), 1, ATTN_NORM, 0);
}

//======================================================================

void    Use_Invulnerability (edict_t *ent, gitem_t *item)
{
        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);

        if (ent->client->invincible_framenum > level.framenum)
                ent->client->invincible_framenum += 300;
        else
                ent->client->invincible_framenum = level.framenum + 300;

        gi.sound(ent, CHAN_ITEM, gi.soundindex("items/protect.wav"), 1, ATTN_NORM, 0);
}

//======================================================================

void    Use_Silencer (edict_t *ent, gitem_t *item)
{
        ent->client->pers.inventory[ITEM_INDEX(item)]--;
        ValidateSelectedItem (ent);
        ent->client->silencer_shots += 30;

//      gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage.wav"), 1, ATTN_NORM, 0);
}
*///PaTMaN-NOUSE
//======================================================================
/*//PaTMaN-NOPICKUP
qboolean Pickup_Key (edict_t *ent, edict_t *other)
{
        if (coop->value)
        {
                if (strcmp(ent->classname, "key_power_cube") == 0)
                {
                        if (other->client->pers.power_cubes & ((ent->spawnflags & 0x0000ff00)>> 8))
                                return false;
                        other->client->pers.inventory[ITEM_INDEX(ent->item)]++;
                        other->client->pers.power_cubes |= ((ent->spawnflags & 0x0000ff00) >> 8);
                }
                else
                {
                        if (other->client->pers.inventory[ITEM_INDEX(ent->item)])
                                return false;
                        other->client->pers.inventory[ITEM_INDEX(ent->item)] = 1;
                }
                return true;
        }
        other->client->pers.inventory[ITEM_INDEX(ent->item)]++;
        return true;
}
*///PaTMaN-NOPICKUP
//======================================================================
//PaTMaN - NO AMMO
/*
qboolean Add_Ammo (edict_t *ent, gitem_t *item, int count)
{
        int                     index;
        int                     max;

        if (!ent->client)
                return false;

        if (item->tag == AMMO_BULLETS)
                max = ent->client->pers.max_bullets;
        else if (item->tag == AMMO_SHELLS)
                max = ent->client->pers.max_shells;
        else if (item->tag == AMMO_ROCKETS)
                max = ent->client->pers.max_rockets;
        else if (item->tag == AMMO_GRENADES)
                max = ent->client->pers.max_grenades;
        else if (item->tag == AMMO_CELLS)
                max = ent->client->pers.max_cells;
        else if (item->tag == AMMO_SLUGS)
                max = ent->client->pers.max_slugs;
        else

                return false;

        index = ITEM_INDEX(item);

        if (ent->client->pers.inventory[index] == max)
                return false;

        ent->client->pers.inventory[index] += count;

        if (ent->client->pers.inventory[index] > max)
                ent->client->pers.inventory[index] = max;

        return true;
}

qboolean Pickup_Ammo (edict_t *ent, edict_t *other)
{
        int                     oldcount;
        int                     count;
        qboolean        weapon;

        weapon = (ent->item->flags & IT_WEAPON);
        if ( (weapon) && ( (int)dmflags->value & DF_INFINITE_AMMO ) )
                count = 1000;
        else if (ent->count)
                count = ent->count;
        else
                count = ent->item->quantity;

        oldcount = other->client->pers.inventory[ITEM_INDEX(ent->item)];

        if (!Add_Ammo (other, ent->item, count))
                return false;

        if (weapon && !oldcount)
        {
                if (other->client->pers.weapon != ent->item && ( !deathmatch->value || other->client->pers.weapon == FindItem("blaster") ) )
                        other->client->newweapon = ent->item;
        }

        if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (deathmatch->value))
                SetRespawn (ent, 30);
        return true;
}

void Drop_Ammo (edict_t *ent, gitem_t *item)
{
        edict_t *dropped;
        int             index;

        index = ITEM_INDEX(item);
        dropped = Drop_Item (ent, item);
        if (ent->client->pers.inventory[index] >= item->quantity)
                dropped->count = item->quantity;
        else
                dropped->count = ent->client->pers.inventory[index];
        ent->client->pers.inventory[index] -= dropped->count;
        ValidateSelectedItem (ent);
}

*/
//======================================================================

void MegaHealth_think (edict_t *self)
{
        if (self->owner->health > self->owner->max_health)
        {
                self->nextthink = level.time + 1;
                self->owner->health -= 1;
                return;
        }

        if (!(self->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (self, 20);
        else
                G_FreeEdict (self);
}

qboolean Pickup_Health (edict_t *ent, edict_t *other)
{
        if (!(ent->style & HEALTH_IGNORE_MAX))
                if (other->health >= other->max_health)
                        return false;

        other->health += ent->count;

        if (ent->count == 2)
                ent->item->pickup_sound = "items/s_health.wav";
        else if (ent->count == 10)
                ent->item->pickup_sound = "items/n_health.wav";
        else if (ent->count == 25)
                ent->item->pickup_sound = "items/l_health.wav";
        else // (ent->count == 100)
                ent->item->pickup_sound = "items/m_health.wav";

        if (!(ent->style & HEALTH_IGNORE_MAX))
        {
                if (other->health > other->max_health)
                        other->health = other->max_health;
        }

        if (ent->style & HEALTH_TIMED)
        {
                ent->think = MegaHealth_think;
                ent->nextthink = level.time + 5;
                ent->owner = other;
                ent->flags |= FL_RESPAWN;
                ent->svflags |= SVF_NOCLIENT;
                ent->solid = SOLID_NOT;
        }
        else
        {
                if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                        SetRespawn (ent, 30);
        }

        return true;
}

//======================================================================

int ArmorIndex (edict_t *ent)
{
        if (!ent->client)
                return 0;

        if (ent->client->pers.inventory[jacket_armor_index] > 0)
                return jacket_armor_index;

        if (ent->client->pers.inventory[combat_armor_index] > 0)
                return combat_armor_index;

        if (ent->client->pers.inventory[body_armor_index] > 0)
                return body_armor_index;

        return 0;
}

qboolean Pickup_Armor (edict_t *ent, edict_t *other)
{
        int                             old_armor_index;
        gitem_armor_t   *oldinfo;
        gitem_armor_t   *newinfo;
        int                             newcount;
        float                   salvage;
        int                             salvagecount;

        // get info on new armor
        newinfo = (gitem_armor_t *)ent->item->info;

        old_armor_index = ArmorIndex (other);

        // handle armor shards specially
        if (ent->item->tag == ARMOR_SHARD)
        {
                if (!old_armor_index)
                        other->client->pers.inventory[jacket_armor_index] = 2;
                else
                        other->client->pers.inventory[old_armor_index] += 2;
        }

        // if player has no armor, just use it
        else if (!old_armor_index)
        {
                other->client->pers.inventory[ITEM_INDEX(ent->item)] = newinfo->base_count;
        }

        // use the better armor
        else
        {
                // get info on old armor
                if (old_armor_index == jacket_armor_index)
                        oldinfo = &jacketarmor_info;
                else if (old_armor_index == combat_armor_index)
                        oldinfo = &combatarmor_info;
                else // (old_armor_index == body_armor_index)
                        oldinfo = &bodyarmor_info;

                if (newinfo->normal_protection > oldinfo->normal_protection)
                {
                        // calc new armor values
                        salvage = oldinfo->normal_protection / newinfo->normal_protection;
                        salvagecount = salvage * other->client->pers.inventory[old_armor_index];
                        newcount = newinfo->base_count + salvagecount;
                        if (newcount > newinfo->max_count)
                                newcount = newinfo->max_count;

                        // zero count of old armor so it goes away
                        other->client->pers.inventory[old_armor_index] = 0;

                        // change armor to new item with computed value
                        other->client->pers.inventory[ITEM_INDEX(ent->item)] = newcount;
                }
                else
                {
                        // calc new armor values
                        salvage = newinfo->normal_protection / oldinfo->normal_protection;
                        salvagecount = salvage * newinfo->base_count;
                        newcount = other->client->pers.inventory[old_armor_index] + salvagecount;
                        if (newcount > oldinfo->max_count)
                                newcount = oldinfo->max_count;

                        // if we're already maxed out then we don't need the new armor
                        if (other->client->pers.inventory[old_armor_index] >= newcount)
                                return false;

                        // update current armor value
                        other->client->pers.inventory[old_armor_index] = newcount;
                }
        }

        if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->value))
                SetRespawn (ent, 20);

        return true;
}

//======================================================================

int PowerArmorType (edict_t *ent)
{
        if (!ent->client)
                return POWER_ARMOR_NONE;

        if (!(ent->flags & FL_POWER_ARMOR))
                return POWER_ARMOR_NONE;

        if (ent->client->pers.inventory[power_shield_index] > 0)
                return POWER_ARMOR_SHIELD;

        if (ent->client->pers.inventory[power_screen_index] > 0)
                return POWER_ARMOR_SCREEN;

        return POWER_ARMOR_NONE;
}

void Use_PowerArmor (edict_t *ent, gitem_t *item)
{
        int             index;

        if (ent->flags & FL_POWER_ARMOR)
        {
                ent->flags &= ~FL_POWER_ARMOR;
                gi.sound(ent, CHAN_AUTO, gi.soundindex("misc/power2.wav"), 1, ATTN_NORM, 0);
        }
        else
        {
                index = ITEM_INDEX(FindItem("cells"));
                if (!ent->client->pers.inventory[index])
                {
                        gi.cprintf (ent, PRINT_HIGH, "No cells for power armor.\n");
                        return;
                }
                ent->flags |= FL_POWER_ARMOR;
                gi.sound(ent, CHAN_AUTO, gi.soundindex("misc/power1.wav"), 1, ATTN_NORM, 0);
        }
}

qboolean Pickup_PowerArmor (edict_t *ent, edict_t *other)
{
        int             quantity;

        quantity = other->client->pers.inventory[ITEM_INDEX(ent->item)];

        other->client->pers.inventory[ITEM_INDEX(ent->item)]++;

        if (deathmatch->value)
        {
                if (!(ent->spawnflags & DROPPED_ITEM) )
                        SetRespawn (ent, ent->item->quantity);
                // auto-use for DM only if we didn't already have one
                if (!quantity)
                        ent->item->use (other, ent->item);
        }

        return true;
}

void Drop_PowerArmor (edict_t *ent, gitem_t *item)
{
        if ((ent->flags & FL_POWER_ARMOR) && (ent->client->pers.inventory[ITEM_INDEX(item)] == 1))
                Use_PowerArmor (ent, item);
//PaTMaN-NODROP        Drop_General (ent, item);
}

//======================================================================

/*
===============
Touch_Item
===============
*/
void Touch_Item (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
        qboolean        taken;

        if (!other->client)
                return;
        if (other->health < 1)
                return;         // dead people can't pickup
        if (!ent->item->pickup)
                return;         // not a grabbable item?

        taken = ent->item->pickup(ent, other);

        if (taken)
        {
                // flash the screen
                other->client->bonus_alpha = 0.25;      

                // show icon and name on status bar
//FIREBLADE (debug code)
                if (!ent->item->icon || strlen(ent->item->icon) == 0)
                {
                        if (ent->item->classname)
                                gi.dprintf("Warning: null icon filename (classname = %s)\n",
                                        ent->item->classname);
                                else
                                gi.dprintf("Warning: null icon filename (no classname)\n");

                }
//FIREBLADE
//                other->client->ps.stats[STAT_PICKUP_ICON] = gi.imageindex(ent->item->icon);
//                other->client->ps.stats[STAT_PICKUP_STRING] = CS_ITEMS+ITEM_INDEX(ent->item);
                other->client->pickup_msg_time = level.time + 3.0;

                // change selected item
//                if (ent->item->use)
//                        other->client->pers.selected_item = other->client->ps.stats[STAT_SELECTED_ITEM] = ITEM_INDEX(ent->item);

                gi.sound(other, CHAN_ITEM, gi.soundindex(ent->item->pickup_sound), 1, ATTN_NORM, 0);
        }

        if (!(ent->spawnflags & ITEM_TARGETS_USED))
        {
                G_UseTargets (ent, other);
                ent->spawnflags |= ITEM_TARGETS_USED;
        }

        if (!taken)
                return;

        if (!((coop->value) &&  (ent->item->flags & IT_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
        {
                if (ent->flags & FL_RESPAWN)
                        ent->flags &= ~FL_RESPAWN;
                else
                        G_FreeEdict (ent);
        }
}

//======================================================================

static void drop_temp_touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
        if (other == ent->owner)
                return;

        Touch_Item (ent, other, plane, surf);
}

static void drop_make_touchable (edict_t *ent)
{
        ent->touch = Touch_Item;
        if (deathmatch->value)
        {
                ent->nextthink = level.time + 119;
                ent->think = G_FreeEdict;
        }
}

edict_t *Drop_Item (edict_t *ent, gitem_t *item)
{

        edict_t *dropped;
        vec3_t  forward, right;
        vec3_t  offset;



        dropped = G_Spawn();

        dropped->classname = item->classname;
        dropped->item = item;
        dropped->spawnflags = DROPPED_ITEM;
        dropped->s.effects = item->world_model_flags;
        dropped->s.renderfx = RF_GLOW;
        VectorSet (dropped->mins, -15, -15, -15);
        VectorSet (dropped->maxs, 15, 15, 15);
        // zucc dumb hack to make knife look like it is on the ground
        if ( (stricmp(item->pickup_name, KNIFE_NAME) == 0)
                || (stricmp(item->pickup_name, LASER_NAME) == 0) 
                || (stricmp(item->pickup_name, GRENADE_NAME) == 0 ) )
        {
                VectorSet (dropped->mins, -15, -15, -1);
                VectorSet (dropped->maxs, 15, 15, 1);
        }
        // spin?
        VectorSet (dropped->avelocity, 0, 600, 0);
        gi.setmodel (dropped, dropped->item->world_model);
        dropped->solid = SOLID_TRIGGER;
        dropped->movetype = MOVETYPE_TOSS;  
        
        dropped->touch = drop_temp_touch;
        dropped->owner = ent;

        if (ent->client)
        {
                trace_t trace;

                AngleVectors (ent->client->v_angle, forward, right, NULL);
                VectorSet(offset, 24, 0, -16);
                G_ProjectSource (ent->s.origin, offset, forward, right, dropped->s.origin);
                PRETRACE();
                trace = gi.trace (ent->s.origin, dropped->mins, dropped->maxs,
                        dropped->s.origin, ent, CONTENTS_SOLID);
                POSTTRACE();
                VectorCopy (trace.endpos, dropped->s.origin);
        }
        else
        {
                AngleVectors (ent->s.angles, forward, right, NULL);
                VectorCopy (ent->s.origin, dropped->s.origin);
        }

        VectorScale (forward, 100, dropped->velocity);
        dropped->velocity[2] = 300;

        dropped->think = drop_make_touchable;
        dropped->nextthink = level.time + 1;

        gi.linkentity (dropped);

        return dropped;
}

void Use_Item (edict_t *ent, edict_t *other, edict_t *activator)
{
        ent->svflags &= ~SVF_NOCLIENT;
        ent->use = NULL;

        if (ent->spawnflags & ITEM_NO_TOUCH)
        {
                ent->solid = SOLID_BBOX;
                ent->touch = NULL;
        }
        else
        {
                ent->solid = SOLID_TRIGGER;
                ent->touch = Touch_Item;
        }

        gi.linkentity (ent);
}

//======================================================================

/*
================
droptofloor
================
*/
void droptofloor (edict_t *ent)
{
        trace_t         tr;
        vec3_t          dest;
        float           *v;

        v = tv(-15,-15,-15);
        VectorCopy (v, ent->mins);
        v = tv(15,15,15);
        VectorCopy (v, ent->maxs);

        if ( ent->item )
        {
                if ( (stricmp(ent->item->pickup_name, KNIFE_NAME) == 0)
                        || (stricmp(ent->item->pickup_name, LASER_NAME) == 0) 
                        || (stricmp(ent->item->pickup_name, GRENADE_NAME) == 0 ) )
                {
                        VectorSet (ent->mins, -15, -15, -1);
                        VectorSet (ent->maxs, 15, 15, 1);
                }
        }

        if (ent->model)
                gi.setmodel (ent, ent->model);
        else
                gi.setmodel (ent, ent->item->world_model);
        ent->solid = SOLID_TRIGGER;
        ent->movetype = MOVETYPE_TOSS;  
        ent->touch = Touch_Item;

        v = tv(0,0,-128);
        VectorAdd (ent->s.origin, v, dest);

        PRETRACE();
        tr = gi.trace (ent->s.origin, ent->mins, ent->maxs, dest, ent, MASK_SOLID);
        POSTTRACE();
        if (tr.startsolid)
        {
                gi.dprintf ("droptofloor: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin));
                G_FreeEdict (ent);
                return;
        }

        VectorCopy (tr.endpos, ent->s.origin);

        if (ent->team)
        {
                ent->flags &= ~FL_TEAMSLAVE;
                ent->chain = ent->teamchain;
                ent->teamchain = NULL;

                ent->svflags |= SVF_NOCLIENT;
                ent->solid = SOLID_NOT;
                if (ent == ent->teammaster)
                {
                        ent->nextthink = level.time + FRAMETIME;
                        ent->think = DoRespawn;
                }
        }

        if (ent->spawnflags & ITEM_NO_TOUCH)
        {
                ent->solid = SOLID_BBOX;
                ent->touch = NULL;
                ent->s.effects &= ~EF_ROTATE;
                ent->s.renderfx &= ~RF_GLOW;
        }

        if (ent->spawnflags & ITEM_TRIGGER_SPAWN)
        {
                ent->svflags |= SVF_NOCLIENT;
                ent->solid = SOLID_NOT;
                ent->use = Use_Item;
        }

        gi.linkentity (ent);
}


/*
===============
PrecacheItem

Precaches all data needed for a given item.
This will be called for each item spawned in a level,
and for each item in each client's inventory.
===============
*/
void PrecacheItem (gitem_t *it)
{
        char    *s, *start;
        char    data[MAX_QPATH];
        int             len;
//PaTMaN - NO AMMO        gitem_t *ammo;

        if (!it)
                return;

        if (it->pickup_sound)
                gi.soundindex (it->pickup_sound);
        if (it->world_model)
                gi.modelindex (it->world_model);
        if (it->view_model)
                gi.modelindex (it->view_model);
        if (it->icon)
                gi.imageindex (it->icon);

//PaTMaN - NO AMMO        // parse everything for its ammo
//PaTMaN - NO AMMO        if (it->ammo && it->ammo[0])
//PaTMaN - NO AMMO        {
//PaTMaN - NO AMMO                ammo = FindItem (it->ammo);
//PaTMaN - NO AMMO                if (ammo != it)
//PaTMaN - NO AMMO                        PrecacheItem (ammo);
//PaTMaN - NO AMMO        }

        // parse the space seperated precache string for other items
        s = it->precaches;
        if (!s || !s[0])
                return;

        while (*s)
        {
                start = s;
                while (*s && *s != ' ')
                        s++;

                len = s-start;
                if (len >= MAX_QPATH || len < 5)
                        gi.error ("PrecacheItem: %s has bad precache string", it->classname);
                memcpy (data, start, len);
                data[len] = 0;
                if (*s)
                        s++;

                // determine type based on extension
                if (!strcmp(data+len-3, "md2"))
                        gi.modelindex (data);
                else if (!strcmp(data+len-3, "sp2"))
                        gi.modelindex (data);
                else if (!strcmp(data+len-3, "wav"))
                        gi.soundindex (data);
                if (!strcmp(data+len-3, "pcx"))
                        gi.imageindex (data);
        }
}

/*
============
SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/

void SpawnItem (edict_t *ent, gitem_t *item)
{

	//KBJump - PaTMaN

//gi.dprintf ("DEBUG: 2: %s\n", item->classname);
//if (item->flags != IT_WEAPON || (Q_stricmp(item->classname,"weapon_railgun")))
//	return;

	//PaTMaN-NOITEMS

        PrecacheItem (item);
/*
        if (ent->spawnflags)
        {
                if (strcmp(ent->classname, "key_power_cube") != 0)
                {
                        ent->spawnflags = 0;
                        gi.dprintf("%s at %s has invalid spawnflags set\n", ent->classname, vtos(ent->s.origin));
                }
        }

        // some items will be prevented in deathmatch
        if (deathmatch->value)
        {
                if ( (int)dmflags->value & DF_NO_ARMOR )
                {
                        if (item->pickup == Pickup_Armor || item->pickup == Pickup_PowerArmor)
                        {
                                G_FreeEdict (ent);
                                return;
                        }
                }
                if ( (int)dmflags->value & DF_NO_ITEMS )
                {
                        if (item->pickup == Pickup_Powerup)
                        {
                                G_FreeEdict (ent);
                                return;
                        }
                }
                // zucc remove health from the game
                if ( 1 ) //(int)dmflags->value & DF_NO_HEALTH )
                {
                        if (item->pickup == Pickup_Health || item->pickup == Pickup_Adrenaline || item->pickup == Pickup_AncientHead)
                        {
                                G_FreeEdict (ent);
                               return;
                        }
                }
                if ( (int)dmflags->value & DF_INFINITE_AMMO )
                {
                        if ( (item->flags == IT_AMMO) || (strcmp(ent->classname, "weapon_bfg") == 0) )
                        {
                                G_FreeEdict (ent);
                                return;
                        }
                }
        }
		*/
/*
        if (coop->value && (strcmp(ent->classname, "key_power_cube") == 0))
        {
                ent->spawnflags |= (1 << (8 + level.power_cubes));
                level.power_cubes++;
        }

        // don't let them drop items that stay in a coop game
        if ((coop->value) && (item->flags & IT_STAY_COOP))
        {
                item->drop = NULL;
        }
*/
        ent->item = item;
        ent->nextthink = level.time + 2 * FRAMETIME;    // items start after other solids
        ent->think = droptofloor;
        ent->s.effects = item->world_model_flags;
        ent->s.renderfx = RF_GLOW;
        if (ent->model)
                gi.modelindex (ent->model);
		//PaTMaN-NOITEMS

}


//======================================================================

gitem_t itemlist[] = 
{
        {
                NULL
        },      // leave index 0 alone

// zucc - New Weapons
/*
gitem_t

referenced by 'entity_name->item.attribute'

Name              Type              Notes

ammo              char *            type of ammo to use
classname         char *            name when spawning it
count_width       int               number of digits to display by icon
drop              void              function called when entity dropped
flags             int               type of pickup :
                                    IT_WEAPON, IT_AMMO, IT_ARMOR
icon              char *            filename of icon
info              void *            ? unused
pickup            qboolean          function called when entity picked up
pickup_name       char *            displayed onscreen when item picked up
pickup_sound      char *            filename of sound to play when picked up
precaches         char *            string containing all models, sounds etc. needed by this
                                    item
quantity          int               ammo gained by item/ammo used per shot by item
tag               int               ? unused
use               void              function called when entity used
view_model        char *            filename of model when being held
weaponthink       void              unused function
world_model       char *            filename of model when item is sitting on level
world_model_flags int               copied to 'ent->s.effects' (see s.effects for values)



  */
                {
                        "weapon_Mk23",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_MK23,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_dual/tris.md2",
                                0, 
                                "models/weapons/v_blast/tris.md2",
                                "w_mk23",
                                MK23_NAME,
                                0,
                                1,
                                NULL,//"Pistol Clip",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/mk23fire.wav weapons/mk23in.wav weapons/mk23out.wav weapons/mk23slap.wav weapons/mk23slide.wav misc/click.wav"
                },


{
                        "weapon_MP5",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_MP5,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_machn/tris.md2",
                                0, 
                                "models/weapons/v_machn/tris.md2",
                                "w_mp5",
                                MP5_NAME,
                                0,
                                0,
                                NULL,//"Machinegun Magazine",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/mp5fire1.wav weapons/mp5in.wav weapons/mp5out.wav weapons/mp5slap.wav weapons/mp5slide.wav"
                },

                {
                        "weapon_M4",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_M4,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_m4/tris.md2",
                                0, 
                                "models/weapons/v_m4/tris.md2",
                                "w_m4",
                                M4_NAME,
                                0,
                                0,
                                NULL,//"M4 Clip",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/m4a1fire.wav weapons/m4a1in.wav weapons/m4a1out.wav weapons/m4a1slide.wav"
                },
                {
                        "weapon_M3",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_M3,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_shotg/tris.md2",
                                0, 
                                "models/weapons/v_shotg/tris.md2",
                                "w_super90",
                                M3_NAME,
                                0,
                                0,
                                NULL,//"12 Gauge Shells",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/m3in.wav weapons/shotgf1b.wav"
                },
                {
                        "weapon_HC",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_HC,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_cannon/tris.md2",
                                0, 
                                "models/weapons/v_cannon/tris.md2",
                                "w_cannon",
                                HC_NAME,
                                0,
                                0,
                                NULL,//"12 Gauge Shells",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/cannon_fire.wav weapons/cclose.wav weapons/cin.wav weapons/cout.wav weapons/copen.wav"
                },
                                {
                        "weapon_Sniper",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_Sniper,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_sniper/tris.md2",
                                0, 
                                "models/weapons/v_sniper/tris.md2",
                                "w_sniper",
                                SNIPER_NAME,
                                0,
                                0,
                                NULL,//"AP Sniper Ammo",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/ssgbolt.wav weapons/ssgfire.wav weapons/ssgin.wav misc/lensflik.wav"
                },

				//KBJump - PaTMaN
/*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16)
*/
	{
		"weapon_railgun", 
		Pickup_Weapon,
		Use_Weapon,
		Drop_Weapon,
		Weapon_Railgun,
		"misc/w_pkup.wav",
		"models/weapons/g_rail/tris.md2", EF_ROTATE,
		"models/weapons/v_rail/tris.md2",
/* icon */		"w_railgun",
/* pickup */	"Railgun",
		0,
		1,
		"Slugs",
		IT_WEAPON|IT_STAY_COOP,
//		WEAP_RAILGUN,
		NULL,
		0,
/* precache */ "weapons/rg_hum.wav"
	},
	
/*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16)
*/
	{
		"weapon_bfg",
		Pickup_Weapon,
		Use_Weapon,
		Drop_Weapon,
		Weapon_BFG,
		"misc/w_pkup.wav",
		"models/weapons/g_bfg/tris.md2", EF_ROTATE,
		"models/weapons/v_bfg/tris.md2",
/* icon */		"w_bfg",
/* pickup */	"BFG10K",
		0,
		50,
		"Cells",
		IT_WEAPON|IT_STAY_COOP,
//		WEAP_BFG,
		NULL,
		0,
/* precache */ "sprites/s_bfg1.sp2 sprites/s_bfg2.sp2 sprites/s_bfg3.sp2 weapons/bfg__f1y.wav weapons/bfg__l1a.wav weapons/bfg__x1b.wav weapons/bfg_hum.wav"
	},

                {
                        "weapon_Dual",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_Dual,
                                //"misc/w_pkup.wav",
                                NULL,
                                "models/weapons/g_dual/tris.md2",
                                0, 
                                "models/weapons/v_dual/tris.md2",
                                "w_akimbo",
                                DUAL_NAME,
                                0,
                                0,
                                NULL,//"Pistol Clip",
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/mk23fire.wav weapons/mk23in.wav weapons/mk23out.wav weapons/mk23slap.wav weapons/mk23slide.wav"
                },
                {
                        "weapon_Knife",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_Knife,
                                NULL,
                                "models/objects/knife/tris.md2",
                                0, 
                                "models/weapons/v_knife/tris.md2",
                                "w_knife",
                                KNIFE_NAME,
                                0,
                                0,
                                NULL,
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/throw.wav weapons/stab.wav weapons/swish.wav weapons/clank.wav"
                }, 
                {
                        "weapon_Marker",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_Marker,
                                NULL,
                                "models/objects/knife/tris.md2",
                                0,
								"models/weapons/v_knife/tris.md2",
								"w_knife",
                                MARKER_NAME,
                                0,
                                0,
                                NULL,
                                IT_WEAPON,
                                NULL,
                                0,
                                "weapons/throw.wav weapons/stab.wav weapons/swish.wav weapons/clank.wav"
                },
                {
                        "weapon_Grenade",
                                NULL, //Pickup_Weapon,
                                Use_Weapon,
                                NULL, //Drop_Weapon,
                                Weapon_Gas,
                                NULL,
                                "models/objects/grenade2/tris.md2",
                                0, 
                                "models/weapons/v_handgr/tris.md2",
                                "a_m61frag",
                                GRENADE_NAME,
                                0,
                                0,
                                NULL,
                                IT_WEAPON,
                                NULL,
                                0,
                                "misc/grenade.wav"
                },



        //
        // AMMO ITEMS
        //

// zucc new ammo
        {
                "ammo_clip",
                NULL, //Pickup_Ammo,
                NULL,
                NULL, //Drop_Ammo,
                NULL,
                //"misc/click.wav",
                NULL,
                "models/items/ammo/clip/tris.md2", 0,
                NULL,
/* icon */              "a_clip",
/* pickup */    NULL,//"Pistol Clip",
/* width */             3,
                1,
                NULL,
                IT_AMMO,
                NULL,
                AMMO_BULLETS,
/* precache */ ""
        },

        {
                "ammo_mag",
                NULL, //Pickup_Ammo,
                NULL,
                NULL, //Drop_Ammo,
                NULL,
                //"misc/click.wav",
                NULL,
                "models/items/ammo/mag/tris.md2", 0,
                NULL,
/* icon */              "a_mag",
/* pickup */    NULL,//"Machinegun Magazine",
/* width */             3,
                1,
                NULL,
                IT_AMMO,
                NULL,
                AMMO_ROCKETS,
/* precache */ ""
        },

        {
                "ammo_m4",
                NULL, //Pickup_Ammo,
                NULL,
                NULL, //Drop_Ammo,
                NULL,
                //"misc/click.wav",
                NULL,
                "models/items/ammo/m4/tris.md2", 0,
                NULL,
/* icon */              "a_m4",
/* pickup */    NULL,//"M4 Clip",
/* width */             3,
                1,
                NULL,
                IT_AMMO,
                NULL,
                AMMO_CELLS,
/* precache */ ""
        },
        {
                "ammo_m3",
                NULL, //Pickup_Ammo,
                NULL,
                NULL, //Drop_Ammo,
                NULL,
                //"misc/click.wav",
                NULL,
                "models/items/ammo/shells/medium/tris.md2", 0,
                NULL,
/* icon */              "a_shells",
/* pickup */    NULL,//"12 Gauge Shells",
/* width */             3,
                7,
                NULL,
                IT_AMMO,
                NULL,
                AMMO_SHELLS,
/* precache */ ""
        },
        {
                "ammo_sniper",
                NULL, //Pickup_Ammo,
                NULL,
                NULL, //Drop_Ammo,
                NULL,
                //"misc/click.wav",
                NULL,
                "models/items/ammo/sniper/tris.md2", 0,
                NULL,
/* icon */              "a_bullets",
/* pickup */    NULL,//"AP Sniper Ammo",
/* width */             3,
                10,
                NULL,
                IT_AMMO,
                NULL,
                AMMO_SLUGS,
/* precache */ ""
        },



        //
        // POWERUP ITEMS
        //

        // zucc the main items
        {
                "item_quiet",
                        NULL, //Pickup_Special,
                        NULL,
                        NULL, //Drop_Special,
                        NULL,
                        "misc/screw.wav",
                        "models/items/quiet/tris.md2",
                        0,
                        NULL,
                        /* icon */              "p_silencer",
                        /* pickup */    "Silencer",
                        /* width */             2,
                        60,
                        NULL,
                        IT_ITEM,
                        NULL,
                        0,
                        /* precache */ ""
        },
                
        {
                "item_slippers",
                        NULL, //Pickup_Special,
                        NULL,
                        NULL, //Drop_Special,
                        NULL,
                        "misc/veston.wav", // sound
                        "models/items/slippers/slippers.md2",
                        0,
                        NULL,
/* icon */              "slippers",
/* pickup */    "Stealth Slippers",
/* width */             2,
                        60,
                        NULL,
                        IT_ITEM,
                        NULL,
                        0,
                /* precache */ ""
        },
                        
                {
                        "item_band",
                                NULL, //Pickup_Special,
                                NULL,
                                NULL, //Drop_Special,
                                NULL,
                                "misc/veston.wav", // sound
                                "models/items/band/tris.md2",
                                0,
                                NULL,
                                /* icon */              "p_bandolier",
                                /* pickup */    "Bandolier",
                                /* width */             2,
                                60,
                                NULL,
                                IT_ITEM,
                                NULL,
                                0,
                                /* precache */ ""
                },
                {
                                "item_vest",
                                        NULL, //Pickup_Special,
                                        NULL,
                                        NULL, //Drop_Special,
                                        NULL,
                                        "misc/veston.wav", // sound
                                        "models/items/armor/jacket/tris.md2",
                                        0,
                                        NULL,
                                        /* icon */              "i_jacketarmor",
                                        /* pickup */    "Kevlar Vest",
                                        /* width */             2,
                                        60,
                                        NULL,
                                        IT_ITEM,
                                        NULL,
                                        0,
                                        /* precache */ ""
                },
                {
                        "item_lasersight",
                                NULL, //Pickup_Special,
                                NULL, //SP_LaserSight,
                                NULL, //Drop_Special,
                                NULL,
                                "misc/lasersight.wav", // sound
                                "models/items/laser/tris.md2",
                                0,
                                NULL,
                                /* icon */              "p_laser",
                                /* pickup */    "Lasersight",
                                /* width */             2,
                                60,
                                NULL,
                                IT_ITEM,
                                NULL,
                                0,
                                /* precache */ ""
                },      

        // end of list marker
        {NULL}
};

void InitItems (void)
{
        game.num_items = sizeof(itemlist)/sizeof(itemlist[0]) - 1;
}


/*
===============
SetItemNames

Called by worldspawn
===============
*/
void SetItemNames (void)
{
        int             i;
        gitem_t *it;

        for (i=0 ; i<game.num_items ; i++)
        {
                it = &itemlist[i];
                gi.configstring (CS_ITEMS+i, it->pickup_name);
        }

        jacket_armor_index = ITEM_INDEX(FindItem("Jacket Armor"));
        combat_armor_index = ITEM_INDEX(FindItem("Combat Armor"));
        body_armor_index   = ITEM_INDEX(FindItem("Body Armor"));
        power_screen_index = ITEM_INDEX(FindItem("Power Screen"));
        power_shield_index = ITEM_INDEX(FindItem("Power Shield"));
}

