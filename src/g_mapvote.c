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

mapvote_t *MapVoteHead = NULL;

qboolean MapListExists ()
{
	FILE *maplist;
	char fullpath[MAX_STR_LEN_PATH]="";
	sprintf(fullpath,GAMEVERSION "/jmodmaplist.txt");

	maplist = fopen(fullpath,"r");
	if (maplist)
		fclose(maplist);
	else
	{
		maplist = fopen(fullpath,"w");
		fclose(maplist);
		maplist = fopen(fullpath,"r");
		if (maplist)
			fclose(maplist);
		else
			return false;
	}
	return true;
}

qboolean MapExists (char map[101])
{
	FILE *bsp;
	char fullpath[MAX_STR_LEN_PATH]="";
	qboolean ans = false;

	sprintf(fullpath,GAMEVERSION "/maps/%s.bsp",map);

	bsp = fopen(fullpath,"r");
	if (bsp)
	{
		fclose(bsp);
		ans = true;
	}
	else
	{
		if (MapListExists())
		{
			FILE *maplist;
			char buf[101]="";
			char map2[101]="";

			sprintf(map2,"%s\n",map);
			sprintf(fullpath,GAMEVERSION "/jmodmaplist.txt");
			maplist = fopen(fullpath,"r");
			while (fgets(buf, 100, maplist) != NULL)
			{
				if (!Q_stricmp(buf,map2))
				{
					ans = true;
					break;
				}
			}
			fclose(maplist);
		}
	}
	
	return ans;
}

void AddMapVote (edict_t *ent)
{
	mapvote_t *p, *n;

	n = (mapvote_t *)gi.TagMalloc(sizeof(mapvote_t), TAG_LEVEL);
	if (n == NULL)
	{
		gi.dprintf("Out of memory\n");
		exit(1);
	}

	sprintf(n->name,"%s",ent->client->resp.votemap);
	n->votes = 1;
	n->next = NULL;

	if (MapVoteHead == NULL)
		MapVoteHead = n;
	else
	{
		p = MapVoteHead;

		if (!Q_stricmp(n->name,p->name))
		{
			p->votes++;
			gi.TagFree(n);
			return;
		}

		while (p->next != NULL)
		{
			p = p->next;
			if (!Q_stricmp(n->name,p->name))
			{
				p->votes++;
				gi.TagFree(n);
				return;
			}
		}
		p->next = n;
	}
}

void SubtractMapVote (edict_t *ent)
{
	mapvote_t *cur,*prev;
	
	if (!MapVoteHead)
		return;
	cur = prev = MapVoteHead;
	while (cur)
	{
		if (!Q_stricmp(cur->name,ent->client->resp.votemap))
		{
			cur->votes--;
			if (cur->votes < 1)
			{
				if (cur == MapVoteHead)
					if (cur->next)
						MapVoteHead = cur->next;
					else
					{
						MapVoteHead = NULL;
						return;
					}

				prev->next = NULL;
				if (cur->next)
					prev->next = cur->next;
				gi.TagFree(cur);
			}
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

void DestroyMapVoteLinkList (mapvote_t *cur)
{
	if (cur->next)
		DestroyMapVoteLinkList(cur->next);
	if (cur == MapVoteHead)
		MapVoteHead = NULL;
	else
		gi.TagFree(cur);
}

void ChangeLevelMapVoteTimeDown(mapvote_t *map, edict_t *ent, qboolean from_viewvotes,qboolean immediate)
{
	Com_sprintf(level.nextmap, sizeof(level.nextmap), "%s", map->name);
	if (immediate)
		level.votemaptimedown = .1;
	else if (level.votemaptimedown < .1)
		level.votemaptimedown = mapvotingtime->value;
	if (from_viewvotes)
		gi.cprintf(ent,PRINT_HIGH,"Vote passed for %s! Map will change in %.0f second%s",level.nextmap,(float)level.votemaptimedown,
			(mapvotingtime->value == 1) ? ".\n" : "s.\n");
	else
		gi.bprintf(PRINT_HIGH,"Vote passed for %s! Map will change in %.0f second%s",level.nextmap,(float)level.votemaptimedown,
			(mapvotingtime->value == 1) ? ".\n" : "s.\n");
}

void ChangeLevelMapVote()
{
		edict_t *ent=NULL;

		ent = G_Spawn();
		ent->classname = "target_changelevel";
		ent->map = level.nextmap;

		gi.bprintf(PRINT_HIGH, "Next map is %s\n",level.nextmap);
		ReadMOTDFile();
		BeginIntermission(ent);
		DestroyMapVoteLinkList(MapVoteHead);
}

void UnsetAllMapVote()
{
	edict_t *ent;
	int i;

	for (i=0 ; i<maxclients->value ; i++)
	{
		ent = g_edicts + 1 + i;
		if (ent->inuse)
			sprintf(ent->client->resp.votemap,"");
	}
	if (MapVoteHead)
		DestroyMapVoteLinkList(MapVoteHead);

	strcpy(level.nextmap,"");
	level.voting = 0;
	level.votemaptimedown = 0;
}

void ListMapVote (edict_t *ent,qboolean from_viewvotes)
{
	if (!MapVoteHead)
	{
		gi.cprintf(ent,PRINT_HIGH,"No votes recorded.\n");
		return;
	}
	else
	{
		mapvote_t *cur = MapVoteHead;
		mapvote_t *bestmap = NULL;
		int numplayers=0,i;
		edict_t *client;
		float percent,bestpercent=0;
		for (i=0 ; i<maxclients->value ; i++)
        {
                client = g_edicts + 1 + i;
                if (client->inuse)
                    numplayers++;
        }

		while (cur)
		{
			percent = (float)cur->votes/(float)numplayers*100;
			if (percent > bestpercent)
			{
				bestpercent = percent;
				bestmap = cur;
			}
			if (from_viewvotes)
				gi.cprintf(ent,PRINT_HIGH,"%s has %i vote%sat %.2f%%\n",cur->name,cur->votes,(cur->votes == 1) ? " " : "s ",percent);
			else
				gi.bprintf(PRINT_HIGH,"%s has %i vote%sat %.2f%%\n",cur->name,cur->votes,(cur->votes == 1) ? " " : "s ",percent);
			cur = cur->next;
		}

		if (mapvotingpercent->value > 100)
			mapvotingpercent->value = 100;

		if (bestpercent >= mapvotingpercent->value)
		{
			if (bestpercent == 100)
				ChangeLevelMapVoteTimeDown(bestmap,ent,from_viewvotes,true);
			else
				ChangeLevelMapVoteTimeDown(bestmap,ent,from_viewvotes,false);
		}
	}
}

void CallMapVote (edict_t *ent)
{
	if (mapvoting->value == 0)
		return;
	else
	{
		if (level.votemaptimedown > 0)
			gi.cprintf(ent,PRINT_HIGH,"%s has been voted as the next map.\n",level.nextmap);
		else {
			char map[101]="";
			sprintf(map,"%s",strtok(gi.args()," "));

			if (Q_stricmp(ent->client->resp.votemap,""))
			{
				if (!Q_stricmp(ent->client->resp.votemap,map))
				{
					gi.cprintf(ent,PRINT_HIGH,"You already voted for %s.\n",map);
					return;
				}
			}
			else
				level.voting++;
			
			if (MapExists(map))
			{
				if (Q_stricmp(ent->client->resp.votemap,""))
				{
					SubtractMapVote(ent);
				}
				sprintf(ent->client->resp.votemap,"%s",map);
				AddMapVote(ent);
				ListMapVote(ent,false);
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"Map not found.\n");
		}
	}
}

void AddMap(char *mapname)
{
	if (strlen(mapname) > 90)
		 gi.cprintf(NULL, PRINT_HIGH, "Add Map Error: max mapname length is 90 characters\n");
	else if (MapListExists())
	{
		FILE *maplist;
		char buf[101]="";
		char map[101]="";
		qboolean add=true;
		char fullpath[101]="";
		
		sprintf(fullpath,"%s/jmodmaplist.txt",GAMEVERSION);
		sprintf(map,"%s\n",mapname);
		
		maplist = fopen(fullpath,"r");
		while (fgets(buf, 100, maplist) != NULL)
		{
			//if ((!Q_stricmp(buf,mapname)) || (!Q_stricmp(buf,map2)))
			if (Q_stricmp(buf,map) == 0)
				add = false; //already exists in map list
		}
		fclose(maplist);
		if (add)
		{

			maplist = fopen(fullpath,"a");
			fputs(map,maplist);
			fclose(maplist);
			gi.dprintf("\"%s\" has been added to the map voting list.\n",mapname);
		}
		else
			gi.dprintf("\"%s\" is already in the map voting list.\n",mapname);
	}
}

void RemoveMap(char *mapname)
{
	if (strlen(mapname) > 90)
		 gi.cprintf(NULL, PRINT_HIGH, "Remove Map Error: max mapname length is 90 characters\n");
	if (MapListExists())
	{
		FILE *maplist, *newmaplist;
		char fullpath[101]="";
		char newfullpath[101]="";
		char buf[101];
		char map[101]="";
		int removes=0;

		sprintf(newfullpath,"%s/jmodmaplist_temp.txt",GAMEVERSION);
		newmaplist = fopen(newfullpath,"w");
		if (!newmaplist)
		{
			gi.cprintf(NULL, PRINT_HIGH, "Remove Map Error: could not create temp copy file\n");
			return;
		}

		sprintf(map,"%s\n",mapname);
		sprintf(fullpath,"%s/jmodmaplist.txt",GAMEVERSION);
		maplist = fopen(fullpath,"r");
		while (fgets(buf, 100, maplist) != NULL)
		{
			if (!Q_stricmp(buf,map))
				removes++;
			else
				fputs(buf,newmaplist);
		}
		fclose(newmaplist);
		fclose(maplist);

		if (removes > 0)
		{
			remove(fullpath);
			rename(newfullpath,fullpath);
			gi.dprintf("\"%s\" has been removed from the map voting list.\n",mapname);
		}
		else
		{
			gi.dprintf("\"%s\" was not found in the map voting list.\n",mapname);
			remove(newfullpath);
		}
	}
}

