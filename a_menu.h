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
 * Action (formerly Axshun) menus
 * From Zoid's CTF.
 */

enum {
        PMENU_ALIGN_LEFT,
        PMENU_ALIGN_CENTER,
        PMENU_ALIGN_RIGHT
};

typedef struct pmenuhnd_s {
        struct pmenu_s *entries;
        int cur;
        int num;
} pmenuhnd_t;

typedef struct pmenu_s {
        char *text;
        int align;
        void *arg;
        void (*SelectFunc)(edict_t *ent, struct pmenu_s *entry);
} pmenu_t;

void PMenu_Open(edict_t *ent, pmenu_t *entries, int cur, int num);
void PMenu_Close(edict_t *ent);
void PMenu_Update(edict_t *ent);
void PMenu_Next(edict_t *ent);
void PMenu_Prev(edict_t *ent);
void PMenu_Select(edict_t *ent);

