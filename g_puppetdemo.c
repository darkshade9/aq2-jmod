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

#ifdef _WIN32
#include <io.h>
#endif

#define	PDBUFF	200			//max num of characters that can be stored for pplay
#define	PDBUFFP	198			//max num of characters to pull in for pplay

void CreatePuppetDemoFile (edict_t *ent);
char *GetPuppetDemoFile (edict_t *ent);

void FreePDemo(puppetdemo_t *p)
{
	while (p)
	{
		if (p->next)
		{
			p = p->next;
			gi.TagFree(p->prev);
			p->prev = NULL;
		}
		else
		{
			gi.TagFree(p);
			p = NULL;
		}
	}
}

char *GetPlayersPDFile(edict_t *ent)
{
	static char pdfile[MAX_STR_LEN_PATH]="";
	char path[MAX_STR_LEN_PATH]; //V1.21 FIX - 1.20.002.01
	sprintf (path, GAMEVERSION "/puppetdemo"); //V1.21 FIX - 1.20.002.01
	p_mkdir(path); //V1.21 FIX - 1.20.002.01
	sprintf(pdfile,GAMEVERSION "/puppetdemo/%i.pdm",ent-g_edicts-1);
	return pdfile;
}


void PuppetDemo_precord(edict_t *ent)
{
	if ((ent->client->resp.puppetdemo_state == PUPPET_NEW_NOTHING) ||
		(ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING))
	{
		qboolean Silent = false;
		qboolean Nostop = false;
		if (gi.argc() > 1)
		{
			char *opt="";
			opt = strtok(gi.args()," ");
			while (opt != NULL)
			{
				if (!Q_stricmp(opt,"silent"))
					Silent = true;
				else if (!Q_stricmp(opt,"nostop"))
					Nostop = true;
				opt = strtok(NULL," ");
			}
		}
		
		if (ent->client->pers.spectator)
		{
			if (!Silent)
				gi.cprintf(ent,PRINT_HIGH,"You cannot record a puppet demo while spectating.\n");
		}
		else
		{
			FILE *tempfile;
			char PDFile[256];

			if (ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
			{
				if (!Silent)
					if (!Nostop)
						gi.cprintf(ent,PRINT_HIGH,"Last puppet demo discarded. Recording new puppet demo...\n");
			}
			else
			{
				if (!Silent)
					gi.cprintf(ent,PRINT_HIGH,"Recording new puppet demo...\n");
			}

			if ((ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING) && Nostop)	
				return;

			FreePDemo(ent->client->resp.puppetdemo_head);
			ent->client->resp.puppetdemo_state = PUPPET_NEW_RECORDING;
			ent->client->resp.puppetdemo_pause = false;
			ent->client->resp.puppetdemo_frame = 0;
			ent->client->resp.puppetdemo_subframe = 0;
			ent->client->resp.puppetdemo_speed = PUPPET_NEW_SPEED_MAX;
			ent->client->resp.precord_starttime = level.time;
			ent->client->resp.pdfile = 1;
			ent->client->resp.puppetdemo_not_mine = false;
			ent->client->resp.puppetdemo_bad_grav = false;
			ent->client->resp.puppetdemo_pup_recorded = false;

			sprintf(PDFile,"%s",GetPlayersPDFile(ent));
			tempfile = fopen(PDFile,"w");
			if (tempfile)
				fclose(tempfile);

		}
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"Can not record a puppet demo while one is playing\n");
}

void PuppetDemo_pstop(edict_t *ent)
{
	qboolean Silent = false;

	if (gi.argc() > 1)
		if (Q_stricmp(gi.argv(1),"silent") == 0)
			Silent = true;

	if (ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
	{
		FILE *pdfile_file;
		char header[PDBUFF];
		float time=0;
		char PDFile[256];

		ent->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		ent->client->resp.puppetdemo_frame = 0;
		ent->client->resp.puppetdemo_subframe = 0;
		ent->client->resp.puppetdemo_pause = false;

		sprintf(PDFile,"%s",GetPlayersPDFile(ent));
		pdfile_file = fopen(PDFile, "a");
		if (pdfile_file)
		{
			ent->client->resp.precord_starttime = 0;

			sprintf(header,"info Author:%-16s JMod:%-4s",ent->client->pers.netname,JMOD_VERSION);
			fprintf(pdfile_file,"%s",header);

			fclose(pdfile_file);

			if (!Silent)
				gi.cprintf(ent,PRINT_HIGH,"Stopped recording puppet demo.\n");
		}
	}
	else if (ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING)
	{
		ent->client->resp.puppetdemo_frame = 0;
		ent->client->resp.puppetdemo_subframe = 0;
		ent->client->resp.puppetdemo_pause = false;
		FollowPuppetDemoFile(ent->client->puppet,false);	//set puppet to the beginning of the demo
		ent->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		if (!Silent)
			gi.cprintf(ent,PRINT_HIGH,"Stopped playing puppet demo.\n");
	}
	else if (!Silent)
		gi.cprintf(ent,PRINT_HIGH,"Not recording or playing a puppet demo.\n");
}

void PuppetDemo_pplay(edict_t *ent)
{
	//this may be activated during nothing, record, or play.
	//if on record, record simply stops and play starts immediately with whatever was recorded
	if (!ent->client->resp.pdfile)
		gi.cprintf(ent,PRINT_HIGH,"You have no puppet demo to play.\n");
	else
	{
		if (ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
			PuppetDemo_pstop(ent);

		if (!ent->client->puppet)					//if no puppet
			Puppet_Spawn(ent,true,false);				//make one
		else if (ent->client->puppet->movetarget)	//else if puppet and pup
		{
			char buf[12]="";
			sprintf(buf,"%i",ent-g_edicts-1);
			if (!PuppetDemo_HasPup(buf,true))		//remove pup if demo doesn't need one
			{
				G_FreeEdict(ent->client->puppet->movetarget);
				ent->client->puppet->movetarget = NULL;
			}
		}

		if (ent->client->puppet)					//if puppet exists
		{
			DisplayPuppetDemoFileInfo(ent);			//display the demo info
			ent->client->resp.puppetdemo_state = PUPPET_NEW_PLAYING;
			ent->client->resp.puppetdemo_pause = false;
			ent->client->resp.puppetdemo_frame = 0;	//start at frame 0
			ent->client->resp.puppetdemo_subframe = 0; //start at subframe 0
			ent->client->resp.puppetdemo_current = ent->client->resp.puppetdemo_head;
		}
	}
}

void PuppetDemo_pplay_new(edict_t *ent)
{
	//this may be activated during nothing, record, or play.
	//if on record, record simply stops and play starts immediately with whatever was recorded
//ok//	if (!ent->client->resp.pdfile)
	if (ent->client->resp.puppetdemo_head == NULL)
		gi.cprintf(ent,PRINT_HIGH,"You have no puppet demo to play.\n");
	else
	{
		if (ent->client->resp.puppetdemo_state == PUPPET_NEW_RECORDING)
			PuppetDemo_pstop(ent);

		if (!ent->client->puppet)					//if no puppet
			Puppet_Spawn(ent,true,false);				//make one
		else if (ent->client->puppet->movetarget)	//else if puppet and pup
		{
//ok//			char buf[12]="";
//ok//			sprintf(buf,"%i",ent-g_edicts-1);
//ok//			if (!PuppetDemo_HasPup(buf,true))		//remove pup if demo doesn't need one
//ok//			{
//ok//				G_FreeEdict(ent->client->puppet->movetarget);
//ok//				ent->client->puppet->movetarget = NULL;
//ok//			}
		}

		if (ent->client->puppet)					//if puppet exists
		{
//ok//			DisplayPuppetDemoFileInfo(ent);			//display the demo info
			ent->client->resp.puppetdemo_state = PUPPET_NEW_PLAYING;
			ent->client->resp.puppetdemo_pause = false;
			ent->client->resp.puppetdemo_frame = 0;	//start at frame 0
			ent->client->resp.puppetdemo_subframe = 0; //start at subframe 0
			ent->client->resp.puppetdemo_current = ent->client->resp.puppetdemo_head;
		}
	}
}

void PuppetDemo_pclear(edict_t *ent)
{
	if (!ent->client->resp.pdfile)
		gi.cprintf(ent,PRINT_HIGH,"No puppet demo to clear.\n");
	else //nothing, recording, or playing
	{
		ent->client->resp.puppetdemo_frame = 0;
		ent->client->resp.puppetdemo_subframe = 0;
		ent->client->resp.puppetdemo_pause = false;
		if (ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING)
			FollowPuppetDemoFile(ent->client->puppet,false);
		ent->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		ent->client->resp.pdfile = 0;
		gi.cprintf(ent,PRINT_HIGH,"Puppet demo cleared.\n");
	}
}

void PuppetDemo_psave(edict_t *ent)
{
	if (ent->client->resp.puppetdemo_not_mine == false)
	{
		if (ent->client->resp.puppetdemo_bad_grav == false)
			SavePuppetDemoFile(ent);
		else
			gi.cprintf(ent,PRINT_HIGH,"You cannot save a demo with non level default gravity.\n");
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"You cannot save a puppet demo that was loaded from the server.\n");
}

void FileDelete(edict_t *ent)
{
	if (gi.argc() == 2)
		gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: fdelete <type> <filename> or fdelete flist <type> <#>\n");
	else if (gi.argc() > 2)
	{
		int type;
		char buf[MAX_STR_LEN];
		sprintf(buf,strtok(gi.args()," ")); //remove the "fdelete"
		sprintf(buf,strtok(NULL," "));
		if (!Q_stricmp(buf,"flist"))
		{
			if (gi.argc() == 3)
				gi.cprintf(ent,PRINT_HIGH,"No flist type given\n");
			else
			{
				int number=0;
				sprintf(buf,strtok(NULL," "));
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

				if (gi.argc() == 3)
					gi.cprintf(ent,PRINT_HIGH,"No flist number given\n");
				else
				{
					number = atoi(strtok(NULL," "));
					sprintf(buf,"%s",GetFileFromFList_PORT(number,type));
					if (!Q_stricmp(buf,"(null)"))
					{
						if (type == FTYPE_PDEMO)
							gi.cprintf(ent,PRINT_HIGH,"Puppet demo file #%i does not exist!\n",number);
						else if (type == FTYPE_JTRAIL)
							gi.cprintf(ent,PRINT_HIGH,"Trail file #%i does not exist!\n",number);
						else if (type == FTYPE_JLOC)
							gi.cprintf(ent,PRINT_HIGH,"Marker file #%i does not exist!\n",number);
					}
					else
					{
						if (type == FTYPE_PDEMO)
							DeletePuppetDemoFile_ByName(ent,buf,NULL);
						else if (type == FTYPE_JTRAIL)
							DeleteJTRAIL(ent,buf,NULL);
						else if (type == FTYPE_JLOC)
							DeleteJLOC(ent,buf,NULL);
					}
				}
			}
		}
		else
		{
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
				sprintf(buf,strtok(NULL," "));

				if (BadFileName(buf))
				{
					gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in filename. Nothing deleted.\n");
					return;
				}

				if (type == FTYPE_PDEMO)
					DeletePuppetDemoFile_ByName(ent,buf,NULL);
				else if (type == FTYPE_JTRAIL)
					DeleteJTRAIL(ent,buf,NULL);
				else if (type == FTYPE_JLOC)
					DeleteJLOC(ent,buf,NULL);
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"No filename specified for deletion\n");
		}
	}
}

void FileRename(edict_t *ent)
{
	if (gi.argc() == 2)
		gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: frename <type> <filename> <new_filename> or frename flist <type> <#> <new_filename>\n");
	else if (gi.argc() > 2)
	{
		int type;
		char buf[MAX_STR_LEN];
		sprintf(buf,strtok(gi.args()," ")); //remove the "frename"
		sprintf(buf,strtok(NULL," "));
		if (!Q_stricmp(buf,"flist"))
		{
			if (gi.argc() == 3)
				gi.cprintf(ent,PRINT_HIGH,"No flist type given\n");
			else
			{
				int number=0;
				sprintf(buf,strtok(NULL," "));
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

				if (gi.argc() == 3)
					gi.cprintf(ent,PRINT_HIGH,"No flist number given\n");
				else
				{
					number = atoi(strtok(NULL," "));
					sprintf(buf,"%s",GetFileFromFList_PORT(number,type));
					if (!Q_stricmp(buf,"(null)"))
					{
						if (type == FTYPE_PDEMO)
							gi.cprintf(ent,PRINT_HIGH,"Puppet demo file #%i does not exist!\n",number);
						else if (type == FTYPE_JTRAIL)
							gi.cprintf(ent,PRINT_HIGH,"Trail file #%i does not exist!\n",number);
						else if (type == FTYPE_JLOC)
							gi.cprintf(ent,PRINT_HIGH,"Marker file #%i does not exist!\n",number);
					}
					else
					{
						if (gi.argc() == 4)
							gi.cprintf(ent,PRINT_HIGH,"No new filename given\n");
						else
						{
							char ren[MAX_STR_LEN]="";
							sprintf(ren,strtok(NULL," "));
							if (BadFileName(ren))
								gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in new filename. Nothing renamed.\n");
							else
							{
								if (type == FTYPE_PDEMO)
									DeletePuppetDemoFile_ByName(ent,buf,ren);
								else if (type == FTYPE_JTRAIL)
									DeleteJTRAIL(ent,buf,ren);
								else if (type == FTYPE_JLOC)
									DeleteJLOC(ent,buf,ren);
							}
						}
					}
				}
			}
		}
		else
		{
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
				sprintf(buf,strtok(NULL," "));

				if (BadFileName(buf))
				{
					gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in filename. Nothing renamed.\n");
					return;
				}

				if (gi.argc() > 4)
				{
					char ren[MAX_STR_LEN]="";
					sprintf(ren,strtok(NULL," "));
					if (BadFileName(ren))
						gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in new filename. Nothing renamed.\n");
					else
					{
						if (type == FTYPE_PDEMO)
							DeletePuppetDemoFile_ByName(ent,buf,ren);
						else if (type == FTYPE_JTRAIL)
							DeleteJTRAIL(ent,buf,ren);
						else if (type == FTYPE_JLOC)
							DeleteJLOC(ent,buf,ren);
					}
				}
				else
					gi.cprintf(ent,PRINT_HIGH,"No new filename specified\n");
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"No filename specified for rename\n");
		}
	}
}

void FileLoad(edict_t *ent)
{
	if (gi.argc() == 1)
		gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: fload <type> <filename> or fload flist <type> <#>\n");
	else if (gi.argc() > 1)
	{
		int type;
		char buf[MAX_STR_LEN];
		sprintf(buf,strtok(gi.args()," "));
		if (!Q_stricmp(buf,"flist"))
		{
			if (gi.argc() == 2)
				gi.cprintf(ent,PRINT_HIGH,"No flist type given\n");
			else
			{
				int number=0;
				sprintf(buf,strtok(NULL," "));
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

				if (gi.argc() == 3)
					gi.cprintf(ent,PRINT_HIGH,"No flist number given\n");
				else
				{
					number = atoi(strtok(NULL," "));
					sprintf(buf,"%s",GetFileFromFList_PORT(number,type));
					if (!Q_stricmp(buf,"(null)"))
					{
						if (type == FTYPE_PDEMO)
							gi.cprintf(ent,PRINT_HIGH,"Puppet demo file #%i does not exist!\n",number);
						else if (type == FTYPE_JTRAIL)
							gi.cprintf(ent,PRINT_HIGH,"Trail file #%i does not exist!\n",number);
						else if (type == FTYPE_JLOC)
							gi.cprintf(ent,PRINT_HIGH,"Marker file #%i does not exist!\n",number);
					}
					else
					{
						if (type == FTYPE_PDEMO)
						{
							if (gi.argc() > 4)
							{
								if (!Q_stricmp(strtok(NULL," "),"trail"))
								{
									if (gi.argc() > 5)
									{
										char *scolor;
										int clean, color, size;

										scolor = strtok(NULL," ");
										clean = atoi(scolor);
										if (clean > 10)
											clean = 10;
										if (clean < 0)
											clean = 0;
										if (gi.argc() > 6)
										{
											scolor = strtok(NULL," ");
											color = TCOLORWORDtoNUM2(scolor);
											if (color == 99)
												gi.cprintf(ent,PRINT_HIGH,"Unknown color specified: \"%s\"; Trail colors are: green, blue, yellow, white, orange, black, and red\n",scolor);
											else
											{
												if (gi.argc() > 7)
												{
													size = atoi(strtok(NULL," "));
													if (size < 2) size = 2;
													else if (size > 9999) size = 9999;
													LoadJTRAIL(ent,buf,true,clean,color,size);
												}
												else
													LoadJTRAIL(ent,buf,true,clean,color,-1);
											}
										}
										else
											LoadJTRAIL(ent,buf,true,clean,-1,-1);
									}
									else
										LoadJTRAIL(ent,buf,true,0,-1,-1);
								}
								else
									LoadPuppetDemoFile_ByName(ent,buf);
							}
							else
								LoadPuppetDemoFile_ByName(ent,buf);
						}
						else if (type == FTYPE_JTRAIL)
						{
							if (gi.argc() > 4)
							{
								char *sclean;
								int clean;
								sclean = strtok(NULL," ");
								clean = atoi(sclean);
								if (clean > 10)
									clean = 10;
								if (clean < 0)
									clean = 0;
								LoadJTRAIL(ent,buf,false,clean,0,0);
							}
							else
								LoadJTRAIL(ent,buf,false,0,0,0);
						}
						else if (type == FTYPE_JLOC)
							LoadJLOC(ent,buf);
						//no bad filename message since from flist
					}
				}
			}
		}
		else
		{
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

			if (gi.argc() > 2)
			{
				sprintf(buf,strtok(NULL," "));
				if (type == FTYPE_PDEMO)
				{
					if (gi.argc() > 3)
					{
						if (!Q_stricmp(strtok(NULL," "),"trail"))
						{
							if (gi.argc() > 4)
							{
								char *scolor;
								int clean, color, size;

								scolor = strtok(NULL," ");
								clean = atoi(scolor);
								if (clean > 10)
									clean = 10;
								if (clean < 0)
									clean = 0;
								if (gi.argc() > 5)
								{
									scolor = strtok(NULL," ");
									color = TCOLORWORDtoNUM2(scolor);
									if (color == 99)
										gi.cprintf(ent,PRINT_HIGH,"Unknown color specified: \"%s\"; Trail colors are: green, blue, yellow, white, orange, black, and red\n",scolor);
									else
									{
										if (gi.argc() > 6)
										{
											size = atoi(strtok(NULL," "));
											if (size < 2) size = 2;
											else if (size > 9999) size = 9999;
											LoadJTRAIL(ent,buf,true,clean,color,size);
										}
										else
											LoadJTRAIL(ent,buf,true,clean,color,-1);
									}
								}
								else
									LoadJTRAIL(ent,buf,true,clean,-1,-1);
							}
							else
								LoadJTRAIL(ent,buf,true,0,-1,-1);
						}
						else
							LoadPuppetDemoFile_ByName(ent,buf);
					}
					else
						LoadPuppetDemoFile_ByName(ent,buf);
				}
				else if (type == FTYPE_JTRAIL)
				{
					if (gi.argc() > 3)
					{
						char *sclean;
						int clean;
						sclean = strtok(NULL," ");
						clean = atoi(sclean);
						if (clean > 10)
							clean = 10;
						if (clean < 0)
							clean = 0;
						LoadJTRAIL(ent,buf,false,clean,0,0);
					}
					else
						LoadJTRAIL(ent,buf,false,0,0,0);
				}
				else if (type == FTYPE_JLOC)
					LoadJLOC(ent,buf);
			}
			else
				gi.cprintf(ent,PRINT_HIGH,"No filename specified for loading\n");
		}
	}
}

void PuppetDemo_psminus(edict_t *ent)
{
	if (ent->client->resp.puppetdemo_speed > PUPPET_NEW_SPEED_MIN)
	{
		ent->client->resp.puppetdemo_speed -= PUPPET_NEW_SPEED_STEP;
		gi.cprintf(ent,PRINT_HIGH,"Puppet Speed: %.1fx\n",(float)ent->client->resp.puppetdemo_speed / 10);
	}
}

void PuppetDemo_psplus(edict_t *ent)
{
	if (ent->client->resp.puppetdemo_speed < PUPPET_NEW_SPEED_MAX)
	{
		ent->client->resp.puppetdemo_speed += PUPPET_NEW_SPEED_STEP;
		gi.cprintf(ent,PRINT_HIGH,"Puppet Speed: %.1fx\n",(float)ent->client->resp.puppetdemo_speed / 10);
	}
}

void PuppetDemo_pspause(edict_t *ent)
{
	if (ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING)
	{
		ent->client->resp.puppetdemo_pause = !ent->client->resp.puppetdemo_pause;
		gi.cprintf(ent,PRINT_HIGH,"Puppet %s\n",(ent->client->resp.puppetdemo_pause == true) ? "Paused" : "Unpaused");
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"You must be playing a puppet demo to use this command.\n");
}

void PuppetDemo_psstepminus(edict_t *ent)
{
	if ((ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING) &&
		(ent->client->resp.puppetdemo_pause == true))
	{
		ent->client->resp.puppetdemo_subframe -= ent->client->resp.puppetdemo_speed * 2;
		while (ent->client->resp.puppetdemo_subframe < 0)
		{
			ent->client->resp.puppetdemo_frame -= 1;
			ent->client->resp.puppetdemo_subframe += PUPPET_NEW_SPEED_MAX;
		}

		if (ent->client->resp.puppetdemo_frame < 1)
		{
			ent->client->resp.puppetdemo_frame = 1;
			ent->client->resp.puppetdemo_subframe = 0;
		}
		ent->client->resp.puppetdemo_pause = false;
		FollowPuppetDemoFile(ent->client->puppet,true);
		ent->client->resp.puppetdemo_pause = true;
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"You must have a paused puppetdemo to use this command.\n");
}

void PuppetDemo_psstepplus(edict_t *ent)
{
	if ((ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING) &&
		(ent->client->resp.puppetdemo_pause == true))
	{
		ent->client->resp.puppetdemo_pause = false;
		FollowPuppetDemoFile(ent->client->puppet,false);
		ent->client->resp.puppetdemo_pause = true;
	}
}

void AngleSubdivide (edict_t *ent)
{
	//This solves the problem of a divide from ~-170 to ~170 actually taking into
	//  account a range of ~340 rather than ~20... (left to right) -170 -175 -180 175 170
	VectorSubdivide(ent->from_angles,ent->move_angles,10,/*ent->style*/ent->lastowner->client->resp.puppetdemo_subframe,ent->s.angles);
	if ((ent->from_angles[1] > 0) && (ent->move_angles[1] < 0))
	{
		if (ent->from_angles[1] > 90)
		{
			//178.12 -> -179.68
			vec3_t temp, result;
			VectorCopy(ent->from_angles,temp);
			temp[1] = 180 + (ent->move_angles[1] + 180);
			VectorSubdivide(ent->from_angles,temp,10,ent->lastowner->client->resp.puppetdemo_subframe,result);
			if (result[1] >= 180)
				result[1] = (result[1] - 180) - 180;
			ent->s.angles[1] = result[1];
		}
		else
		{
			//0.36 -> -0.51
			vec3_t temp, result;
			VectorCopy(ent->from_angles,temp);
			temp[1] = (ent->move_angles[1] * -1) + ent->from_angles[1]*2;
			VectorSubdivide(ent->from_angles,temp,10,ent->lastowner->client->resp.puppetdemo_subframe,result);
			result[1] = ent->from_angles[1] - (result[1] - ent->from_angles[1]);
			ent->s.angles[1] = result[1];
		}
	}
	else if ((ent->from_angles[1] < 0) && (ent->move_angles[1] > 0))
	{
		if (ent->from_angles[1] < -90)
		{
			//-179.68 -> 178.12
			vec3_t temp, result;
			VectorCopy(ent->from_angles,temp);
			temp[1] = -180 - (180 - ent->move_angles[1]);
			VectorSubdivide(ent->from_angles,temp,10,ent->lastowner->client->resp.puppetdemo_subframe,result);
			if (result[1] < -180)
				result[1] = 180 + (result[1] + 180);
			ent->s.angles[1] = result[1];
		}
		else
		{
			//-0.51 -> 0.36
			vec3_t temp, result;
			VectorCopy(ent->from_angles,temp);
			temp[1] = (ent->move_angles[1] * -1) + ent->from_angles[1]*2;
			VectorSubdivide(ent->from_angles,temp,10,ent->lastowner->client->resp.puppetdemo_subframe,result);
			result[1] = ent->from_angles[1] - (result[1] - ent->from_angles[1]);
			ent->s.angles[1] = result[1];
		}
	}
}

void DecimatePuppetPath (edict_t *ent)
{
	if (puppetdemosmoothing->value)
	{
		//pdemo struct debug("%i %i\n",ent->lastowner->client->resp.puppetdemo_frame,ent->lastowner->client->resp.puppetdemo_subframe);
		VectorSubdivide(ent->from_origin,ent->move_origin,10,ent->lastowner->client->resp.puppetdemo_subframe,ent->s.origin);
		AngleSubdivide(ent); //Some extra angle stuff to do before VectorSubdivide
		ent->pos1[0] = ent->s.angles[0] * 3;
		ent->pos1[1] = ent->s.angles[1];
		ent->pos1[2] = 0;
	}
}

qboolean FollowPuppetDemoFile_new (edict_t *ent,qboolean reverse)
{

	//FILE *pdfile_file;
	char PDFile[MAX_STR_LEN_PATH]="";
	qboolean done=false;
	int linecounter=0;
	client_respawn_t *r;
	puppetdemo_t *f;

	r = &ent->lastowner->client->resp;




/*
if paused, return false
if speed is 0, return false //same as pause


playing is done like this:
there is a puppet demo frame
this puppet demo frame is an actual recorded frame
there are also puppet demo subframes
these are frames calculated by looking where u are coming from, to where u are going

sub frames get worked on before frames
frames only get loaded when a subframe passes a frame boundry, such as >=10
sub frames should be wrapped between 0 and 10 though, so 10 = 0, 11 = 1, 15 = 5

so, puppet demos are really run by subframes then
you increase the subframe by the speed of playback
when the subframe passes a boundry, you increase the frame, pull the data, and wrap the subframe
when wrapping subframe, if the next frame is the last, the subframe should be 0
(NOTE for REVERSE: (subframe_max - (framenum % speed)) is where u start from end)

the function returns false while the puppet demo is not done
the function sets playback to nothing and returns true when it is done

so...
*/
/*
	if (r->puppetdemo_pause == true)
		return false;
	if (r->puppetdemo_speed == PUPPET_NEW_SPEED_MIN)
		return false;

	if (r->puppetdemo_current == NULL)
	{
		r->puppetdemo_state = PUPPET_NEW_NOTHING;
		return true;
	}
	else
        f = r->puppetdemo_current;

	if (r->puppetdemo_subframe <= PUPPET_NEW_SPEED_MAX)
	{
		DecimatePuppetPath(ent);
		gi.linkentity(ent);
		r->puppetdemo_subframe += r->puppetdemo_speed;	//update to the new subframe count
		return false;	//not done with the demo yet
	}
	else
	{
		if (f->next)	//if there is another frame
		{
			f = f->next;	//update the data frame
			r->puppetdemo_current = f;
			r->puppetdemo_subframe = r->puppetdemo_subframe - PUPPET_NEW_SPEED_MAX;	//update the subframe (x = 15 - 10 = 5   correct)
			
			VectorCopy(f->angles,ent->s.angles);
			VectorCopy(f->angles,ent->move_angles);
			ent->s.event = f->event;
			ent->maxs[2] = f->maxs_2;
			if (ent->maxs[2] == CROUCHING_MAXS2)
				ent->viewheight = 22;
			else
				ent->viewheight = CROUCHING_VIEWHEIGHT;
			ent->s.frame = f->modelframe;
			VectorCopy(f->origin,ent->s.origin);
			VectorCopy(f->origin,ent->move_origin);
			ent->s.sound = f->sound;
			VectorCopy(f->v_angle,ent->pos1);
			//VectorCopy(f->velocity,ent->velocity);
			if (f->groundentity == true)
				ent->count = 1;
			else
				ent->count = 0;

			if (ent->lastowner->client->resp.puppetdemo_current->prev)
			{
				VectorCopy(ent->lastowner->client->resp.puppetdemo_current->prev->origin,ent->from_origin);
				VectorCopy(ent->lastowner->client->resp.puppetdemo_current->prev->angles,ent->from_angles);
			}
			else
			{
				VectorClear(ent->from_origin);
				VectorClear(ent->from_angles);
			}
			ent->style = 1;
			
			
			DecimatePuppetPath(ent);
			gi.linkentity(ent);
			r->puppetdemo_frame = f->framenum;
			return false;
		}
		else
		{
			r->puppetdemo_state = PUPPET_NEW_NOTHING;
			r->puppetdemo_subframe = 0;
			return true;
		}
	}
	*/



	if (r->puppetdemo_pause)
		return false;
	if (r->puppetdemo_speed == 0)
		return false; //Same as being paused

	if (r->puppetdemo_frame > 0)
	{
		if (r->puppetdemo_subframe <= PUPPET_NEW_SPEED_MAX)
		{
			//KEEP ALL INFO but update position and angle via subdivides
			DecimatePuppetPath(ent);
			r->puppetdemo_subframe += r->puppetdemo_speed;
			gi.linkentity(ent);
			return false;
		}

		r->puppetdemo_subframe -= PUPPET_NEW_SPEED_MAX;
	}

	f = r->puppetdemo_current;

	if (r->puppetdemo_current != NULL)
	{
		if (reverse)
		{
			if (f->prev == NULL)
			{
				r->puppetdemo_state = PUPPET_NEW_NOTHING;
				return true;
			}
			else
				r->puppetdemo_current = f->prev;
		}
		else
		{
			if (f->next == NULL)
			{
				r->puppetdemo_state = PUPPET_NEW_NOTHING;
				return true;
			}
			else
				r->puppetdemo_current = f->next;
		}
	}
	else
		return true;

	VectorCopy(f->angles,ent->s.angles);
	VectorCopy(f->angles,ent->move_angles);
	
	ent->s.event = f->event;
	ent->maxs[2] = f->maxs_2;
	if (ent->maxs[2] == CROUCHING_MAXS2)
		ent->viewheight = 22;
	else
		ent->viewheight = CROUCHING_VIEWHEIGHT;
	ent->s.frame = f->modelframe;
	VectorCopy(f->origin,ent->s.origin);
	VectorCopy(f->origin,ent->move_origin);
	ent->s.sound = f->sound;
	VectorCopy(f->v_angle,ent->pos1);
	//VectorCopy(f->velocity,ent->velocity);
	//if (f->groundentity == true)
	//	ent->count = 1;
	//else
	//	ent->count = 0;

	if (f->prev)
	{
		VectorCopy(f->prev->origin,ent->from_origin);
		VectorCopy(f->prev->angles,ent->from_angles);
	}
	else
	{
		VectorCopy(f->origin,ent->from_origin);
		VectorCopy(f->angles,ent->from_angles);
	}
	ent->style = 1;
	
	//if (reverse)
	//{
	//	r->puppetdemo_current = r->puppetdemo_current->prev;
	//	if (!r->puppetdemo_current)
	//		r->puppetdemo_current = r->puppetdemo_head;
		//r->puppetdemo_frame = f->framenum-1;
		//if (r->puppetdemo_frame < 0)
		//	r->puppetdemo_frame = 0;
	//}
	//else
		r->puppetdemo_frame = f->framenum;

	DecimatePuppetPath(ent);

	if (!reverse)
		r->puppetdemo_subframe += r->puppetdemo_speed;

	gi.linkentity(ent);

	return false;

/* //file pdemo


		char *token=NULL;
		char buf2[PDBUFF]="";

		int duck,shoot;
		vec3_t origin,angles,v_angles;

		while ((fgets(buf2, PDBUFFP, pdfile_file) != NULL) && (!done))
		{
			if (linecounter == ent->lastowner->client->resp.puppetdemo_frame)
			{
				duck = 0;
				shoot = 0;
				VectorClear(origin);
				VectorClear(angles);

				token = strtok( buf2, "," );
				if ((token[0] == 'P') && (token[1] == 'U') && (token[2] == 'P')) //V1.20
				{
					vec3_t origin,angles;				//(V1.20)
					float max;							//(V1.20)

					//Load Coords
					token = strtok( NULL, "," );		//(V1.20)
					origin[0] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					origin[1] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					origin[2] = atof(token);			//(V1.20)

					//Load Angles
					token = strtok( NULL, "," );		//(V1.20)
					angles[0] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					angles[1] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					angles[2] = atof(token);			//(V1.20)

					//Load Duck
					token = strtok( NULL, "," );		//(V1.20)
					max = atof(token);					//(V1.20)

					Pup_Spawn(ent,origin,angles,max);	//(V1.20)
					ent->lastowner->client->resp.puppetdemo_frame++;	//(V1.20) (Non-destructive update V1.23)
				}
				else if (token[0] != 'i' || token[1] != 'n' ||	//Backwards Compatibility for V1.17 (V1.20 (else),V1.18)
					token[2] != 'f' || token[3] != 'o')			//Backwards Compatibility for V1.17 (V1.18)
				{												//Backwards Compatibility for V1.17 (V1.18)
					
					//Load Coords
					origin[0] = atof(token);
					token = strtok( NULL, "," );		//(V1.17)
					origin[1] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					origin[2] = atof(token);			//(V1.17)
					VectorCopy(origin,ent->s.origin);	//(V1.17)
					VectorCopy(origin,ent->from_origin);

					//Load Angles
					token = strtok( NULL, "," );		//(V1.17)
					angles[0] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					angles[1] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					angles[2] = atof(token);			//(V1.17)
					VectorCopy(angles,ent->s.angles);	//(V1.17)
					VectorCopy(angles,ent->from_angles);//(V1.20)

					//Load V_Angles
					token = strtok( NULL, "," );		//(V1.17)
					v_angles[0] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					v_angles[1] = atof(token);			//(V1.17)
					v_angles[2] = 0;					//(V1.17)
					VectorCopy(v_angles,ent->pos1);		//(V1.17)
					//VectorCopy(v_angles,ent->from_angles); //(V1.20)

					//Load Duck
					token = strtok( NULL, "," );		//(V1.17)
					duck = atof(token);					//(V1.17)
					if (duck == 1)						//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = 16;				//Backwards Compatibility for V1.17 (V1.18)
					else if (duck == 0)					//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = 32;				//Backwards Compatibility for V1.17 (V1.18)
					else								//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = duck;			//(V1.17)

					if (ent->maxs[2] == 16)				//(V1.20)
						ent->viewheight = 8;			//(V1.20)
					else								//(V1.20)
						ent->viewheight = 20;			//(V1.20)

					//Load Shoot
					token = strtok( NULL, "," );		//(V1.17)
					shoot = atoi(token);				//(V1.17)

					//Load Frame
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.frame = atoi(token);			//(V1.17)

					//Load Event
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.event = atoi(token);			//(V1.17)

					//Load Sound
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.sound = atoi(token);			//(V1.17)

					//Load GroundEntity
					token = strtok( NULL, "," );		//(V1.17)
					if (token)							//Backwards Compatibility for V1.17 (V1.18)
						ent->count = atoi(token);
					else								//Backwards Compatibility for V1.17 (V1.18)
						ent->count = 0;					//Backwards Compatibility for V1.17 (V1.18)

					ent->style = 1;						//(V1.20)
					if (fgets(buf2, PDBUFFP, pdfile_file) != NULL)
					{
						token = strtok( buf2, "," );
						//this should really be some kind of while loop
						//this only fixes for 1 PUP line... could there be more? dunno
						if ((token[0] == 'P') && (token[1] == 'U') && (token[2] == 'P'))
						{
							if (fgets(buf2, PDBUFFP, pdfile_file) != NULL)
								token = strtok( buf2, "," );
							else
								break;
						}
						if (token[0] != 'i' || token[1] != 'n' ||	//Backwards Compatibility for V1.17 (V1.18,V1.20)
							token[2] != 'f' || token[3] != 'o')			//Backwards Compatibility for V1.17 (V1.18,V1.20)
						{												//Backwards Compatibility for V1.17 (V1.18,V1.20)
							//Load Coords
							origin[0] = atof(token);
							token = strtok( NULL, "," );			//(V1.20)
							origin[1] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							origin[2] = atof(token);				//(V1.20)
							VectorCopy(origin,ent->move_origin);	//(V1.20)

							//Load Angles
							token = strtok( NULL, "," );			//(V1.20)
							angles[0] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							angles[1] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							angles[2] = atof(token);				//(V1.20)
							VectorCopy(angles,ent->move_angles);	//(V1.20)
						}
					}
				}										//Backwards Compatibility for V1.17 (V1.18)

				if (linecounter == ent->lastowner->client->resp.puppetdemo_frame)
				{
					DecimatePuppetPath(ent);
					ent->lastowner->client->resp.puppetdemo_subframe += ent->lastowner->client->resp.puppetdemo_speed;

					done = true;
				}
			}
			else
				token = strtok( buf2, "," );
			linecounter++;
		}
		if (!done)
		{
			done = true;
			ent->lastowner->client->resp.puppetdemo_frame = linecounter+2;
			ent->lastowner->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		}
		else
		{
			done = false;
			ent->lastowner->client->resp.puppetdemo_frame = linecounter;
		}
		fclose(pdfile_file);
		gi.linkentity(ent);
	}
	else
		return true;
	ent->lastowner->client->resp.puppetdemo_frame = linecounter;

	return done;
	*/
}

qboolean FollowPuppetDemoFile (edict_t *ent,qboolean stepminus_hack)
{

	FILE *pdfile_file;
	char PDFile[MAX_STR_LEN_PATH]="";
	qboolean done=false;
	int linecounter=0;

	if (ent->lastowner->client->resp.puppetdemo_pause)
		return false;
	if (ent->lastowner->client->resp.puppetdemo_speed == 0)
		return false; //Same as being paused

	if (ent->lastowner->client->resp.puppetdemo_frame > 0)
	{
		if ((ent->lastowner->client->resp.puppetdemo_subframe <= PUPPET_NEW_SPEED_MAX) &&
			(!stepminus_hack))
		{
			//KEEP ALL INFO but update position and angle via subdivides
			DecimatePuppetPath(ent);
			ent->lastowner->client->resp.puppetdemo_subframe += ent->lastowner->client->resp.puppetdemo_speed;
			return false;
		}
		else
			ent->lastowner->client->resp.puppetdemo_subframe -= PUPPET_NEW_SPEED_MAX;
	}

	sprintf(PDFile,"%s",GetPlayersPDFile(ent->lastowner));

	pdfile_file = fopen(PDFile, "r");
	if (pdfile_file)
	{
		char *token=NULL;
		char buf2[PDBUFF]="";

		int duck,shoot;
		vec3_t origin,angles,v_angles;

		while ((fgets(buf2, PDBUFFP, pdfile_file) != NULL) && (!done))
		{
			if (linecounter == ent->lastowner->client->resp.puppetdemo_frame)
			{
				duck = 0;
				shoot = 0;
				VectorClear(origin);
				VectorClear(angles);

				token = strtok( buf2, "," );
				if ((token[0] == 'P') && (token[1] == 'U') && (token[2] == 'P')) //V1.20
				{
					vec3_t origin,angles;				//(V1.20)
					float max;							//(V1.20)

					//Load Coords
					token = strtok( NULL, "," );		//(V1.20)
					origin[0] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					origin[1] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					origin[2] = atof(token);			//(V1.20)

					//Load Angles
					token = strtok( NULL, "," );		//(V1.20)
					angles[0] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					angles[1] = atof(token);			//(V1.20)
					token = strtok( NULL, "," );		//(V1.20)
					angles[2] = atof(token);			//(V1.20)

					//Load Duck
					token = strtok( NULL, "," );		//(V1.20)
					max = atof(token);					//(V1.20)

					Pup_Spawn(ent,origin,angles,max);	//(V1.20)
					ent->lastowner->client->resp.puppetdemo_frame++;	//(V1.20) (Non-destructive update V1.23)
				}
				else if (token[0] != 'i' || token[1] != 'n' ||	//Backwards Compatibility for V1.17 (V1.20 (else),V1.18)
					token[2] != 'f' || token[3] != 'o')			//Backwards Compatibility for V1.17 (V1.18)
				{												//Backwards Compatibility for V1.17 (V1.18)
					
					//Load Coords
					origin[0] = atof(token);
					token = strtok( NULL, "," );		//(V1.17)
					origin[1] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					origin[2] = atof(token);			//(V1.17)
					VectorCopy(origin,ent->s.origin);	//(V1.17)
					VectorCopy(origin,ent->from_origin);

					//Load Angles
					token = strtok( NULL, "," );		//(V1.17)
					angles[0] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					angles[1] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					angles[2] = atof(token);			//(V1.17)
					VectorCopy(angles,ent->s.angles);	//(V1.17)
					VectorCopy(angles,ent->from_angles);//(V1.20)

					//Load V_Angles
					token = strtok( NULL, "," );		//(V1.17)
					v_angles[0] = atof(token);			//(V1.17)
					token = strtok( NULL, "," );		//(V1.17)
					v_angles[1] = atof(token);			//(V1.17)
					v_angles[2] = 0;					//(V1.17)
					VectorCopy(v_angles,ent->pos1);		//(V1.17)
					//VectorCopy(v_angles,ent->from_angles); //(V1.20)

					//Load Duck
					token = strtok( NULL, "," );		//(V1.17)
					duck = atof(token);					//(V1.17)
					if (duck == 1)						//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = 16;				//Backwards Compatibility for V1.17 (V1.18)
					else if (duck == 0)					//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = 32;				//Backwards Compatibility for V1.17 (V1.18)
					else								//Backwards Compatibility for V1.17 (V1.18)
						ent->maxs[2] = duck;			//(V1.17)

					if (ent->maxs[2] == 16)				//(V1.20)
						ent->viewheight = 8;			//(V1.20)
					else								//(V1.20)
						ent->viewheight = 20;			//(V1.20)

					//Load Shoot
					token = strtok( NULL, "," );		//(V1.17)
					shoot = atoi(token);				//(V1.17)

					//Load Frame
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.frame = atoi(token);			//(V1.17)

					//Load Event
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.event = atoi(token);			//(V1.17)

					//Load Sound
					token = strtok( NULL, "," );		//(V1.17)
					ent->s.sound = atoi(token);			//(V1.17)

					//Load GroundEntity
					token = strtok( NULL, "," );		//(V1.17)
					if (token)							//Backwards Compatibility for V1.17 (V1.18)
						ent->count = atoi(token);
					else								//Backwards Compatibility for V1.17 (V1.18)
						ent->count = 0;					//Backwards Compatibility for V1.17 (V1.18)

					ent->style = 1;						//(V1.20)
					if (fgets(buf2, PDBUFFP, pdfile_file) != NULL)
					{
						token = strtok( buf2, "," );
						//this should really be some kind of while loop
						//this only fixes for 1 PUP line... could there be more? dunno
						if ((token[0] == 'P') && (token[1] == 'U') && (token[2] == 'P'))
						{
							if (fgets(buf2, PDBUFFP, pdfile_file) != NULL)
								token = strtok( buf2, "," );
							else
								break;
						}
						if (token[0] != 'i' || token[1] != 'n' ||	//Backwards Compatibility for V1.17 (V1.18,V1.20)
							token[2] != 'f' || token[3] != 'o')			//Backwards Compatibility for V1.17 (V1.18,V1.20)
						{												//Backwards Compatibility for V1.17 (V1.18,V1.20)
							//Load Coords
							origin[0] = atof(token);
							token = strtok( NULL, "," );			//(V1.20)
							origin[1] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							origin[2] = atof(token);				//(V1.20)
							VectorCopy(origin,ent->move_origin);	//(V1.20)

							//Load Angles
							token = strtok( NULL, "," );			//(V1.20)
							angles[0] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							angles[1] = atof(token);				//(V1.20)
							token = strtok( NULL, "," );			//(V1.20)
							angles[2] = atof(token);				//(V1.20)
							VectorCopy(angles,ent->move_angles);	//(V1.20)

							/*
							//Load V_Angles
							token = strtok( NULL, "," );			//(V1.17)
							v_angles[0] = atof(token);				//(V1.17)
							token = strtok( NULL, "," );			//(V1.17)
							v_angles[1] = atof(token);				//(V1.17)
							v_angles[2] = 0;						//(V1.17)
							//VectorCopy(v_angles,ent->pos1);		//(V1.17)
							VectorCopy(v_angles,ent->move_angles);	//(V1.20)
							*/
						}
					}
				}										//Backwards Compatibility for V1.17 (V1.18)

				if (linecounter == ent->lastowner->client->resp.puppetdemo_frame)
				{
					DecimatePuppetPath(ent);
					ent->lastowner->client->resp.puppetdemo_subframe += ent->lastowner->client->resp.puppetdemo_speed;

					done = true;
				}
			}
			else
				token = strtok( buf2, "," );
			linecounter++;
		}
		if (!done)
		{
			done = true;
			ent->lastowner->client->resp.puppetdemo_frame = linecounter+2;
			ent->lastowner->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
		}
		else
		{
			done = false;
			ent->lastowner->client->resp.puppetdemo_frame = linecounter;
		}
		fclose(pdfile_file);
		gi.linkentity(ent);
	}
	else
		return true;
	ent->lastowner->client->resp.puppetdemo_frame = linecounter;

	return done;
}

void ShiftArray (char array[256],unsigned int start,int number)
{
	int i;
	int start_orig = start;
	for (i=0;i < number;i++)
	{
		start = start_orig;
		for (start;start < strlen(array);start++)
		{
			if (array[start+1])
				array[start] = array[start+1];
		}
	}
	array[strlen(array) - number] = '\0';
}

void CompressNumbers(char *PDFile)
{
	int i;
	qboolean removed = true;
	qboolean safe = false;
	while (removed) {
		removed = false;
		for (i=0;i<strlen(PDFile);i++)
		{
			if (PDFile[i] == ',') safe = false;
			if (PDFile[i] == '.') safe = true;
			
			if (safe)
			{
				if (!((PDFile[i-1] == ',') && (PDFile[i] == '0') && (PDFile[i+1] == ',')))
				{
					if ((PDFile[i] == '0') && (PDFile[i+1] == ','))
					{
						ShiftArray((char *)PDFile,i,1);
						removed = true;
					}
				}
			}
		}
		for (i=0;i<strlen(PDFile);i++)
		{
			if ((PDFile[i] == '.') && (PDFile[i+1] == ','))
				ShiftArray((char *)PDFile,i,1);
		}
	}
}

void AddPuppetDemoNode_new (edict_t *ent)
{
	puppetdemo_t *pnode=NULL;
	pnode = gi.TagMalloc(sizeof(puppetdemo_t),TAG_LEVEL);

	if (ent->client->resp.puppetdemo_head == NULL)
	{
		ent->client->resp.puppetdemo_frame = 0;
		ent->client->resp.puppetdemo_head = pnode;
		pnode->prev = NULL;
		pnode->next = NULL;
		pnode->framenum = 0;
		ent->client->resp.puppetdemo_last = pnode;
	}
	else
	{
		ent->client->resp.puppetdemo_last->next = pnode;
		pnode->prev = ent->client->resp.puppetdemo_last;
		pnode->next = NULL;
		pnode->framenum = ent->client->resp.puppetdemo_frame;
		ent->client->resp.puppetdemo_last = pnode;
	}

	VectorCopy(ent->s.origin,pnode->origin);
	VectorCopy(ent->s.angles,pnode->angles);
	VectorCopy(ent->client->v_angle,pnode->v_angle);
	VectorCopy(ent->velocity,pnode->velocity);
	VectorCopy(ent->client->resp.movekeys,pnode->movekeys);
	pnode->maxs_2 = ent->maxs[2];
	pnode->weaponstate = ent->client->weaponstate;
	pnode->modelframe = ent->s.frame;
	pnode->event = ent->s.event;
	pnode->sound = ent->s.sound;
	pnode->groundentity = (ent->groundentity != NULL) ? true : false;

	pnode->falldamage = ent->client->ps.stats[STAT_FALLDMGLAST];
	pnode->speed = ent->client->ps.stats[STAT_SPEEDX];
	pnode->highspeed = ent->client->ps.stats[STAT_HIGHSPEED];
	
	ent->client->resp.puppetdemo_frame = ent->client->resp.puppetdemo_frame + 1;

	/* //file pdemo
	FILE *pdfile_file;
	char PDFile[256]="";

	if (!ent->client->resp.pdfile)
		ent->client->resp.pdfile = 1;
	
	sprintf (PDFile,"%s",GetPlayersPDFile(ent));//GAMEVERSION "/puppetdemo/%s.pdm",buffer);

	pdfile_file = fopen(PDFile, "a");
	if (pdfile_file != NULL)
	{
		if (ent->client->puppet)
		{
			if (ent->client->resp.puppetdemo_pup_recorded == false)
			{
				sprintf(PDFile,"PUP,%.3f,%.3f,%.3f,%f,%f,%f,%.0f\n",
					ent->client->puppet->s.origin[0],ent->client->puppet->s.origin[1],ent->client->puppet->s.origin[2],
					ent->client->puppet->s.angles[0],ent->client->puppet->s.angles[1],ent->client->puppet->s.angles[2],
					ent->client->puppet->maxs[2]);
				CompressNumbers((char*)PDFile);
				fputs(PDFile,pdfile_file);
				ent->client->resp.puppetdemo_pup_recorded = true;
			}
		}
		sprintf(PDFile,"%.3f,%.3f,%.3f,%f,%f,%f,%f,%f,%.0f,%i,%i,%i,%i,%i\n",
			ent->s.origin[0],ent->s.origin[1],ent->s.origin[2],
			ent->s.angles[0],ent->s.angles[1],ent->s.angles[2],
			ent->client->v_angle[0],ent->client->v_angle[1],
			ent->maxs[2],
			(ent->client->weaponstate == WEAPON_FIRING) ? 1 : 0,
			ent->s.frame,
			ent->s.event,
			ent->s.sound,
			(ent->groundentity != NULL) ? 1 : 0);
		CompressNumbers((char*)PDFile);
		fputs(PDFile,pdfile_file);
		fclose(pdfile_file);
		if (ent->client->resp.cgravity != sv_gravity->value)
			ent->client->resp.puppetdemo_bad_grav = true;
	}
	*/ //file pdemo
}

void AddPuppetDemoNode (edict_t *ent)
{
	FILE *pdfile_file;
	char PDFile[256]="";

	if (!ent->client->resp.pdfile)
		ent->client->resp.pdfile = 1;
	
	sprintf (PDFile,"%s",GetPlayersPDFile(ent));//GAMEVERSION "/puppetdemo/%s.pdm",buffer);

	pdfile_file = fopen(PDFile, "a");
	if (pdfile_file != NULL)
	{
		if (ent->client->puppet)
		{
			if (ent->client->resp.puppetdemo_pup_recorded == false)
			{
				sprintf(PDFile,"PUP,%.3f,%.3f,%.3f,%f,%f,%f,%.0f\n",
					ent->client->puppet->s.origin[0],ent->client->puppet->s.origin[1],ent->client->puppet->s.origin[2],
					ent->client->puppet->s.angles[0],ent->client->puppet->s.angles[1],ent->client->puppet->s.angles[2],
					ent->client->puppet->maxs[2]);
				CompressNumbers((char*)PDFile);
				fputs(PDFile,pdfile_file);
				ent->client->resp.puppetdemo_pup_recorded = true;
			}
		}
		sprintf(PDFile,"%.3f,%.3f,%.3f,%f,%f,%f,%f,%f,%.0f,%i,%i,%i,%i,%i\n",
			ent->s.origin[0],ent->s.origin[1],ent->s.origin[2],
			ent->s.angles[0],ent->s.angles[1],ent->s.angles[2],
			ent->client->v_angle[0],ent->client->v_angle[1],
			ent->maxs[2],
			(ent->client->weaponstate == WEAPON_FIRING) ? 1 : 0,
			ent->s.frame,
			ent->s.event,
			ent->s.sound,
			(ent->groundentity != NULL) ? 1 : 0);
		CompressNumbers((char*)PDFile);
		fputs(PDFile,pdfile_file);
		fclose(pdfile_file);
		if (ent->client->resp.cgravity != sv_gravity->value)
			ent->client->resp.puppetdemo_bad_grav = true;
	}
}

void CreatePuppetDemoFile (edict_t *ent)
{
	FILE *pdfile;

    char PDFile[256],buffer[6];
	int playernum;
	
	playernum = ent-g_edicts-1;
	itoa(playernum,buffer,10);
	strcpy(PDFile,GAMEVERSION "/puppetdemo");
	p_mkdir(PDFile);

	strcpy(PDFile,GAMEVERSION "/puppetdemo/");
	strcat(PDFile,buffer);
	strcat(PDFile,".pdm");
	pdfile = fopen(PDFile,"w");
	fclose(pdfile);
}

void CreatePuppetDemoFile_new (edict_t *ent)
{
//	FILE *pdfile;

/* //file pdemo
    char PDFile[256],buffer[6];
	int playernum;
	
	playernum = ent-g_edicts-1;
	itoa(playernum,buffer,10);
	strcpy(PDFile,GAMEVERSION "/puppetdemo");
	p_mkdir(PDFile);

	strcpy(PDFile,GAMEVERSION "/puppetdemo/");
	strcat(PDFile,buffer);
	strcat(PDFile,".pdm");
//	pdfile = fopen(PDFile,"w");
//	fclose(pdfile);
*/ //file pdemo
	ent->client->resp.pdfile = 1;
}

void DeletePuppetDemoFile (edict_t *ent)
{
	if (ent->client->resp.pdfile)
		ent->client->resp.pdfile = 0;
}

void SavePuppetDemoFile (edict_t *ent)
{

	FILE *target_file;
	FILE *pdfile_file;

	char PDFileT[MAX_STR_LEN_PATH]; //V1.21 FIX - 1.20.002.01
	char JustFileName[256];
	char buf2[PDBUFF];

	if (!ent->client->resp.pdfile)
	{
		gi.cprintf(ent,PRINT_HIGH,"No puppet demo recorded to save!\n");
		return;
	}
	
	if (BadFileName(gi.args()))
	{
		gi.cprintf(ent,PRINT_HIGH,"Illegal characters found in filename. Puppet demo not saved.\n");
		return;
	}

	sprintf (PDFileT, GAMEVERSION "/puppetdemo");
	p_mkdir(PDFileT);
	sprintf (JustFileName, "%s-%s.pdm", level.mapname, strtok(gi.args()," "));
	sprintf (PDFileT, GAMEVERSION "/puppetdemo/%s", JustFileName);

	target_file = fopen(PDFileT, "r");
	if (target_file != NULL)
	{
		fclose(target_file);
		gi.cprintf(ent,PRINT_HIGH,"Cannot save to %s. The file already exists!\n",JustFileName);
		return;
	}

	target_file = fopen(PDFileT, "w");
	if (target_file)
	{
		sprintf(PDFileT,"%s",GetPlayersPDFile(ent));
		pdfile_file = fopen(PDFileT, "r");
		if (pdfile_file)
		{
			while (fgets(buf2, PDBUFFP, pdfile_file) != NULL)
				fprintf(target_file,"%s",buf2);

			gi.bprintf(PRINT_HIGH,"%s: Puppet demo saved to %s\n",ent->client->pers.netname,JustFileName);
			fclose(pdfile_file);
		}
		fclose(target_file);
	}
}

void DisplayFileInfoFinfo (edict_t *ent)
{
	FILE *source_file;

	char PDFile[MAX_STR_LEN_PATH]="";
	char JustFileName[MAX_STR_LEN_PATH]="";
	char buf2[PDBUFF]="";
	int type = FTYPE_NONE;

	sprintf(buf2,"%s",strtok(gi.args()," "));
	if (!Q_stricmp(buf2,"puppet"))
	{
		type = FTYPE_PDEMO;
		sprintf(PDFile,GAMEVERSION "/puppetdemo");
	}
	else if (!Q_stricmp(buf2,"trail"))
	{
		type = FTYPE_JTRAIL;
		sprintf(PDFile,GAMEVERSION "/jtrail");
	}
	else if (!Q_stricmp(buf2,"marker"))
	{
		type = FTYPE_JLOC;
		sprintf(PDFile,GAMEVERSION "/jloc");
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"<type> can only be \"puppet\", \"trail\", or \"marker\"\n");
		return;
	}

	p_mkdir(PDFile);

	if (gi.argc() == 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"No filename specified\n");
		return;
	}
	sprintf(buf2,"%s",strtok(NULL," "));

	if (!Q_stricmp(buf2,"flist"))
	{
		int number;
		if (gi.argc() < 3)
		{
			gi.cprintf(ent,PRINT_HIGH,"No flist number specified for loading\n");
			return;
		}

		number=atoi(strtok(NULL," "));
		sprintf(JustFileName,"%s",GetFileFromFList_PORT(number,type));//ent->client->pdbmenu_type));
		if (!Q_stricmp(JustFileName,"(null)"))
		{
			if (type == FTYPE_PDEMO)
                gi.cprintf(ent,PRINT_HIGH,"Puppet demo file #%i does not exist!\n",number);
			else if (type == FTYPE_JTRAIL)
                gi.cprintf(ent,PRINT_HIGH,"Trail file #%i does not exist!\n",number);
			else if (type == FTYPE_JLOC)
                gi.cprintf(ent,PRINT_HIGH,"Marker file #%i does not exist!\n",number);
			return;
		}

		if (type == FTYPE_PDEMO)
			sprintf (buf2, "%s-%s.pdm", level.mapname, JustFileName);
		else if (type == FTYPE_JTRAIL)
			sprintf (buf2, "%s-%s.jtrail", level.mapname, JustFileName);
		else if (type == FTYPE_JLOC)
			sprintf (buf2, "%s-%s.jloc", level.mapname, JustFileName);
	}
	else
	{
		sprintf(JustFileName,"%s",buf2);
		if (type == FTYPE_PDEMO)
			sprintf(buf2,"%s-%s.pdm",level.mapname,JustFileName);
		else if (type == FTYPE_JTRAIL)
			sprintf(buf2,"%s-%s.jtrail",level.mapname,JustFileName);
		else if (type == FTYPE_JLOC)
			sprintf(buf2,"%s-%s.jloc",level.mapname,JustFileName);
	}


	if (type == FTYPE_PDEMO)
		sprintf(PDFile,GAMEVERSION "/puppetdemo/%s-%s.pdm",level.mapname,JustFileName);
	else if (type == FTYPE_JTRAIL)
		sprintf(PDFile,GAMEVERSION "/jtrail/%s-%s.jtrail",level.mapname,JustFileName);
	else if (type == FTYPE_JLOC)
		sprintf(PDFile,GAMEVERSION "/jloc/%s-%s.jloc",level.mapname,JustFileName);

	source_file = fopen(PDFile,"r");
	if (source_file)
	{
		fclose(source_file);
		if (type == FTYPE_PDEMO)
            gi.cprintf(ent,PRINT_HIGH,"%s\n",GetPuppetDemoFileInfo(PDFile,false,false,true));
		else if (type == FTYPE_JTRAIL)
            gi.cprintf(ent,PRINT_HIGH,"%s\n",GetTrailFileInfo(JustFileName,false));
		else if (type == FTYPE_JLOC)
            gi.cprintf(ent,PRINT_HIGH,"%s\n",GetMarkerFileInfo(JustFileName,false));
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"\"%s\" was not found!\n",buf2);
}

void LoadPuppetDemoFile_ByName (edict_t *ent, char *name)
{
	FILE *source_file;

	char PDFile[MAX_STR_LEN_PATH]="";
	char JustFileName[MAX_STR_LEN_PATH]="";
	char buf2[PDBUFF]="";

	sprintf(PDFile,GAMEVERSION "/puppetdemo");
	p_mkdir(PDFile);

	sprintf(JustFileName,"%s-%s.pdm",level.mapname,name);
	sprintf(PDFile,GAMEVERSION "/puppetdemo/%s",JustFileName);

	source_file = fopen(PDFile,"r");
	if (source_file)
	{
		FILE *pdfile_file = NULL;

		ent->client->resp.pdfile = 0;

		sprintf(PDFile,"%s",GetPlayersPDFile(ent));
		pdfile_file = fopen(PDFile, "w");
		if (pdfile_file)
		{
			while (fgets(buf2, PDBUFFP, source_file) != NULL)
				fprintf(pdfile_file,buf2);
			fclose(pdfile_file);
			ent->client->resp.pdfile = 1;
			gi.cprintf(ent,PRINT_HIGH,"Puppet demo %s loaded successfully.\n",JustFileName);

			if (ent->client->resp.puppetdemo_state == PUPPET_NEW_PLAYING)
				PuppetDemo_pstop(ent);

			ent->client->resp.puppetdemo_frame = 0;
			ent->client->resp.puppetdemo_subframe = 0;
			ent->client->resp.puppetdemo_pause = false;
			if (ent->client->puppet)
				FollowPuppetDemoFile(ent->client->puppet,false);	//set puppet to the beginning of the demo
			ent->client->resp.puppetdemo_not_mine = true;
			ent->client->resp.puppetdemo_state = PUPPET_NEW_NOTHING;
			
			ent->client->resp.puppetdemo_speed = PUPPET_NEW_SPEED_MAX;
		}
		fclose(source_file);
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"\"%s\" was not found!\n",JustFileName);
}

void DeletePuppetDemoFile_ByName (edict_t *ent, char *name, char *ren)
{
	FILE *source_file;

	char PDFile[MAX_STR_LEN_PATH]="";
	char JustFileName[MAX_STR_LEN_PATH]="";

	sprintf(PDFile,GAMEVERSION "/puppetdemo");
	p_mkdir(PDFile);

	sprintf(JustFileName,"%s-%s.pdm",level.mapname,name);
	sprintf(PDFile,GAMEVERSION "/puppetdemo/%s",JustFileName);

	source_file = fopen(PDFile,"r");
	if (source_file)
	{
		fclose(source_file);

		if (ren != NULL)
		{
			char PDF_ren[MAX_STR_LEN_PATH]="";
			char JFN_ren[MAX_STR_LEN_PATH]="";
			sprintf(JFN_ren,"%s-%s.pdm",level.mapname,ren);
			sprintf(PDF_ren,GAMEVERSION "/puppetdemo/%s",JFN_ren);
			source_file = fopen(PDF_ren,"r");
			if (source_file == NULL)
			{
				rename(PDFile,PDF_ren);
				gi.cprintf(ent,PRINT_HIGH,"\"%s\" was renamed to \"%s\"\n",JustFileName,JFN_ren);
			}
			else
			{
				fclose(source_file);
				gi.cprintf(ent,PRINT_HIGH,"Destination file exists. Can not rename!\n");
			}
		}
		else
		{
			remove(PDFile);
			gi.cprintf(ent,PRINT_HIGH,"\"%s\" was deleted\n",JustFileName);
		}
	}
	else
		gi.cprintf(ent,PRINT_HIGH,"\"%s\" was not found!\n",JustFileName);
}

char *AltString (char *s)
{
	static char r[MAX_STR_LEN]="";
	int i;
	sprintf(r,"%s",s);
	for (i=0;i<strlen(r);i++)
		r[i] = r[i] ^ 0x80;
	return r;
}

char *CharFloat (float num)
{
	static char r[16];
	if (num < 0.08)
		sprintf(r,"0.0");
	else
	{
		char buf[16];
		gcvt(num,5,r);
		if (r[strlen(r)-1] == '.')
		{
			sprintf(buf,"%s0",r);
			sprintf(r,"%s",buf);
		}
	}
	return r;
}

qboolean PuppetDemo_HasPup(char *file,qboolean isplayernum)
{
	char PDFile[MAX_STR_LEN_PATH];
	char buf[PDBUFF];
	FILE *pdfile_file;

	if (isplayernum)
		sprintf(PDFile,"%s/puppetdemo/%s.pdm",GAMEVERSION,file);
	else
		sprintf(PDFile,"%s/puppetdemo/%s-%s.pdm",GAMEVERSION,level.mapname,file);

	pdfile_file = fopen(PDFile,"r");
	if (pdfile_file)
	{
		while (fgets(buf, PDBUFFP, pdfile_file) != NULL)
		{
			if ((buf[0] == 'P') && (buf[1] == 'U') && (buf[2] == 'P') && (buf[3] == ',')) //V1.20
				return true;
		}
	}
	return false;
}

char *GetTrailFileInfo(char *file,qboolean listformat)
{
	FILE *pdfile_file;
	static char retval[MAX_STR_LEN];
	char pathfile[MAX_STR_LEN_PATH+MAX_STR_LEN];

	sprintf(pathfile,GAMEVERSION "/jtrail/%s-%s.jtrail",level.mapname,file);

	pdfile_file = fopen(pathfile,"r");
	if (pdfile_file)
	{
		char buf[PDBUFF];
		int lines=0;
		while (fgets(buf, PDBUFFP, pdfile_file) != NULL)
			lines += 1;
		fclose(pdfile_file);
		//LINUX PORT ITOA//itoa(lines,buf,10);
		sprintf(buf,"%i",lines);
		if (listformat)
			sprintf(retval,"%5s %s",AltString(buf),file);
		else
			sprintf(retval,"Nodes: %s",AltString(buf));
	}
	else
		sprintf(retval,"ERROR: %s",file);
	return retval;
}

char *GetMarkerFileInfo(char *file,qboolean listformat)
{
	FILE *pdfile_file;
	static char retval[MAX_STR_LEN];
	char pathfile[MAX_STR_LEN_PATH+MAX_STR_LEN];

	sprintf(pathfile,GAMEVERSION "/jloc/%s-%s.jloc",level.mapname,file);

	pdfile_file = fopen(pathfile,"r");
	if (pdfile_file)
	{
		char buf[PDBUFF];
		int lines=0;
		while (fgets(buf, PDBUFFP, pdfile_file) != NULL)
			lines += 1;
		fclose(pdfile_file);
		//LINUX PORT ITOA//itoa(lines,buf,10);
		sprintf(buf,"%i",lines);
		if (listformat)
			sprintf(retval,"%5s %s",AltString(buf),file);
		else
			sprintf(retval,"Markers: %s",AltString(buf));
	}
	else
		sprintf(retval,"ERROR: %s",file);
	return retval;
}

char *GetPuppetDemoFileInfo(char *file,qboolean listformat,qboolean isplayernum,qboolean fullpathgiven)
{
	char PDFile[MAX_STR_LEN_PATH];
	char buf[PDBUFF];
	static char retval[MAX_STR_LEN];
	FILE *pdfile_file;

	if (fullpathgiven)
		sprintf(PDFile,"%s",file);
	else if (isplayernum)
		sprintf(PDFile,GAMEVERSION "/puppetdemo/%s.pdm",file);
	else
		sprintf(PDFile,GAMEVERSION "/puppetdemo/%s-%s.pdm",level.mapname,file);

	pdfile_file = fopen(PDFile,"r");
	if (pdfile_file)
	{
		float time=0.0;
		qboolean pup=false;

		while (fgets(buf, PDBUFFP, pdfile_file) != NULL)
		{
			time += .1;
			if ((buf[0] == 'P') && (buf[1] == 'U') && (buf[2] == 'P') && (buf[3] == ',')) //V1.20
				pup = true;
		}

		fclose(pdfile_file);

		if (buf != NULL)
		{
			int ver=0;
			
			//SITUATIONS TO CHECK FOR
			//JMod V1.17 = info <name>
			if (!strstr(buf," V1.18") && !strstr(buf," V1.19") && !strstr(buf," Author:"))
				ver=117;

			//JMod V1.18 = info <name> V1.18
			if (strstr(buf," V1.18"))
				ver=118;

			//JMod V1.19 = info <name> V1.19
			if (strstr(buf," V1.19"))
				ver=119;

			//JMod V1.20 = info Author:<name> JMod:1.20
			//if (strstr(buf," JMod:1.20"))
			//	ver=120;
			//if this is in now, then it must be updated for each new version
			//if not in, then all versions not 117, 118, or 119 will
			//  be assumed to have the 1.20 format
			//When this format changes, then this should be updated
			//--I got it... just look over the code and you'll get it again

			if (ver != 117)
			{
				if ((buf[0] == 'i') && (buf[1] == 'n') && (buf[2] == 'f') && (buf[3] == 'o') && (buf[4] == ' '))
				{
					unsigned int i,j;
					char bigbuf[128] = "";
					char player[32] = "";
					char version[12] = "";

					if (time > 0)
						time -= .1;
					else time = 1.1;
					for (i=5,j=0;i<strlen(buf);i++,j++)
						buf[j]=buf[i];
					buf[j]='\0';

					//gi.cprintf(ent,PRINT_HIGH,"%4i %-16s %-4.1f %s\n",counter,player,time,filename);
					//gi.cprintf(ent,PRINT_HIGH," %4i %-16s           %-4.1f %s                  \n");
					//gi.cprintf(ent,PRINT_HIGH," ### Author           Time  Filename            \n");
					//gi.cprintf(ent,PRINT_HIGH,"---- ---------------- ----- --------------------\n");
					//gi.cprintf(ent,PRINT_HIGH,"   1 PaTMaN^J=SC=     1.1   4box-to-bridgetop   \n");

					for (i=0;i<strlen(buf);i++)
					{
						if ((ver != 118) && (ver != 119))
						{
							if ((buf[i]=='A') && (buf[i+1]=='u') && (buf[i+2]=='t') && (buf[i+3]=='h')
								&& (buf[i+4]=='o') && (buf[i+5]=='r') && (buf[i+6]==':'))
							{
								if (!Q_stricmp(player,""))
								{
									i+=7;
									for (i,j=0;j<16;i++,j++)
										bigbuf[j] = buf[i];
									
									bigbuf[16]='\0';
									
									if (!listformat)
									{
										for (i=15;i>0;i--)
										{
											if (bigbuf[i] == ' ')
												bigbuf[i]='\0';
											else
												break;
										}
									}
									
									sprintf(player,"%s",bigbuf);
								}
							}
							
							if ((buf[i]=='J') && (buf[i+1]=='M') && (buf[i+2]=='o') && (buf[i+3]=='d')
								&& (buf[i+4]==':'))
							{
								if (!Q_stricmp(version,""))
								{
									i+=5;
									for (i,j=0;j<4;i++,j++)
										bigbuf[j] = buf[i];
									bigbuf[4]='\0';
									sprintf(version,"%s",bigbuf);
								}
							}
						}
						else if ((ver = 118) || (ver = 119))
						{
							if (!Q_stricmp(player,""))
							{
								int Verpos=0;
								int h=0;
								
								for (h=0;h<strlen(buf);h++)
								{
									if ((buf[h]==' ') && (buf[h+1]=='V') && (buf[h+2]=='1') && (buf[h+3]=='.')
										&& (buf[h+4]=='1') && ((buf[h+5]=='8') || (buf[h+5]=='9')))
									{
										Verpos = h;
										break;
									}
								}
								
								for (i=0,j=0;j<16;i++,j++)
								{
									if (j >= Verpos)
										bigbuf[j] = ' ';
									else
										bigbuf[j] = buf[i];
								}
								
								bigbuf[16]='\0';
								
								if (!listformat)
								{
									for (i=15;i>0;i--)
									{
										if (bigbuf[i] == ' ')
											bigbuf[i]='\0';
										else
											break;
									}
								}

//								strcpy(player,bigbuf);
								sprintf(player,"%s",bigbuf);
							}
						}
					}

					if (listformat)
					{
						//retval=strdup("1234567890123456 123.123 123 123456789012345678901234567890\0");
						//sprintf(buf,"%-16s %5s %-3s %s\0",AltString(player),CharFloat(time),(pup) ? "Yes" : "No",AltString(file));
						sprintf(buf,"%-16s %5s %-3s",AltString(player),CharFloat(time),(pup) ? "Yes" : "No");
						sprintf(buf,"%s %s",buf,AltString(file));
					}
					else
					{
						char lilbuf[PDBUFF];
						sprintf(lilbuf,"%.1f",time);
						//retval=strdup("Author:1234567890123456 Pup:Yes Time:123.123\0");
						sprintf(buf,"Author:%s Pup:%s",player,pup ? AltString("Yes") : AltString("No"));
						sprintf(buf,"%s Time:%s",buf,AltString(lilbuf));
					}
				}
			}
			else if (ver == 117)
			{
				if (listformat)
				{
					//retval=strdup("1234567890123456 123.123 123 123456789012345678901234567890\0");
					//sprintf(buf,"%-16s %5s %-3s %s\0",AltString("Unknown"),CharFloat(time),(pup) ? "Yes" : "No",AltString(file));
					sprintf(buf,"%-16s %5s %-3s",AltString("Unknown"),CharFloat(time),(pup) ? "Yes" : "No");
					sprintf(buf,"%s %s",buf,AltString(file));
				}
				else
				{
					char lilbuf[PDBUFF];
					sprintf(lilbuf,"%.1f",time);
					//retval=strdup("Author:1234567890123456 Pup:Yes Time:123.123\0");
					sprintf(buf,"Author:%s Pup:%s","Unknown",pup ? AltString("Yes") : AltString("No"));
					sprintf(buf,"%s Time:%s",buf,AltString(lilbuf));
				}
			}
		}
	}

//	fclose(deb);
//	return &*buf;
	sprintf(retval,"%s",buf);
	//retval = strdup(buf);
	return retval;
}

void DisplayPuppetDemoFileInfo (edict_t *ent)
{
	if (ent->client->resp.pdfile)
	{
		char buf[12];
		//LINUX PORT ITOA//itoa(ent-g_edicts-1,buf,10);
		sprintf(buf,"%i",ent-g_edicts-1);
		gi.cprintf(ent,PRINT_HIGH,"Playing Puppet demo: %s\n",GetPuppetDemoFileInfo(buf,false,true,false));
	}
}

char *GetFileFromFList_PORT (int Number, int type)
{
#ifdef _WIN32
	return GetFileFromFList_PORT_WIN (Number, type);
#else
	return GetFileFromFList_PORT_LINUX (Number, type);
#endif
}

#ifdef _WIN32

char *GetFileFromFList_PORT_WIN (int Number, int type)
{
	static char retval[MAX_STR_LEN_PATH]="";
	char search[256]="";
	int counter=0;
	char name[256]="";
	unsigned int i;
	int mod;


	//WINDOWS ONLY STUFF//
	struct _finddata_t c_file;
    long hFile;
	//////////////////////


	if (type == FTYPE_PDEMO)
	{
		sprintf(search,GAMEVERSION "/puppetdemo/%s-*.pdm",level.mapname);
		mod = 4;
	}
	else if (type == FTYPE_JTRAIL)
	{
		sprintf(search,GAMEVERSION "/jtrail/%s-*.jtrail",level.mapname);
		mod = 7;
	}
	else if (type == FTYPE_JLOC)
	{
		sprintf(search,GAMEVERSION "/jloc/%s-*.jloc",level.mapname);
		mod = 5;
	}
	
    if( (hFile = _findfirst( search, &c_file )) == -1L )
		return NULL;
	else
	{
		counter++;
		if (counter == Number)
		{
			strcpy(name,c_file.name);
			for (i=strlen(level.mapname)+1;i<strlen(name)-mod;i++)
				name[i-(strlen(level.mapname)+1)] = name[i];
			name[i-(strlen(level.mapname)+1)]='\0';
			sprintf(retval,"%s",name);
			_findclose( hFile );
			return retval;
		}
		
		while( _findnext( hFile, &c_file ) == 0 )
		{
			counter++;
			if (counter == Number)
			{
				
				strcpy(name,c_file.name);

				for (i=strlen(level.mapname)+1;i<strlen(name)-mod;i++)
					name[i-(strlen(level.mapname)+1)] = name[i];

				name[i-(strlen(level.mapname)+1)]='\0';

				sprintf(retval,"%s",name);
				_findclose( hFile );
				return retval;
			}
		}
		_findclose( hFile );

	}
	return NULL;
}

#else

char *GetFileFromFList_PORT_LINUX (int Number, int type)
{
	static char	retval[MAX_STR_LEN_PATH]="";
	char search[256]="";
	int	counter=0;
	char name[256]="";
	unsigned int i;
	int	mod;


	//LINUX ONLY STUFF//
    DIR             *dip;
    struct dirent   *dit;
	char match_lev[128]="";
	char match_ext[8]="";
	int len_lev=0;
	int len_ext=0;
	////////////////////

	
	if (type ==	FTYPE_PDEMO)
	{
		sprintf(search,GAMEVERSION "/puppetdemo/");
		mod = 4;
		strcpy(match_ext,".pdm");
	}
	else if (type == FTYPE_JTRAIL)
	{
		sprintf(search,GAMEVERSION "/jtrail/");
		mod = 7;
		strcpy(match_ext,".jtrail");
	}
	else if (type == FTYPE_JLOC)
	{
		sprintf(search,GAMEVERSION "/jloc/");
		mod = 5;
		strcpy(match_ext,".jloc");
	}

	sprintf(match_lev,"%s-",level.mapname);

	len_lev = strlen(match_lev);
	len_ext = strlen(match_ext);

	if ((dip = opendir(search)) == NULL)
		return NULL;
	else
	{
		while ((dit = readdir(dip)) != NULL)
		{
			strcpy(name,dit->d_name);
			if ((strcmp(name,".") && strcmp(name,"..")) && 
				((!strsubcmp(0,len_lev,match_lev,name)) &&
				 (!strsubcmp(strlen(name)-len_ext,len_ext,match_ext,name))))
				counter++;

			if (counter == Number)
			{
				for (i=strlen(level.mapname)+1;i<strlen(name)-mod;i++)
					name[i-(strlen(level.mapname)+1)] = name[i];
				name[i-(strlen(level.mapname)+1)]='\0';
				sprintf(retval,"%s",name);
				closedir(dip);
				return retval;
			}
		}
		closedir(dip);
	}
	return NULL;
}

#endif

// V1.21 ADD - New File Browser vvv

int GetFileCountForMap_PORT (int type)
{
#ifdef _WIN32
	return GetFileCountForMap_PORT_WIN (type);
#else
	return GetFileCountForMap_PORT_LINUX (type);
#endif
}

#ifdef _WIN32

int GetFileCountForMap_PORT_WIN (int type)
{

	char search[256]="\0";
	int counter=0;


	//WINDOWS ONLY STUFF//
	struct _finddata_t c_file;
	long hFile;
	//////////////////////


	if (type == FTYPE_JTRAIL)
		sprintf(search,GAMEVERSION "/jtrail/%s-*.jtrail",level.mapname);
	else if (type == FTYPE_JLOC)
		sprintf(search,GAMEVERSION "/jloc/%s-*.jloc",level.mapname);
	else if (type == FTYPE_PDEMO)
		sprintf(search,GAMEVERSION "/puppetdemo/%s-*.pdm",level.mapname);

    if( (hFile = _findfirst( search, &c_file )) == -1L )
		return 0;
	else
	{
		counter++;
		while( _findnext( hFile, &c_file ) == 0 )
			counter++;
		_findclose( hFile );
	}
	return counter;
}

#else

int GetFileCountForMap_PORT_LINUX (int type)
{
	char search[256]="\0";
	int counter=0;


	//LINUX ONLY STUFF//
	DIR             *dip;
	struct dirent   *dit;
	char match_lev[128]="";
	char match_ext[8]="";
	int len_lev=0;
	int len_ext=0;
	char name[256]="";
	////////////////////


	if (type == FTYPE_JTRAIL)
	{
		sprintf(search,GAMEVERSION "/jtrail/");
		strcpy(match_ext,".jtrail");
	}
	else if (type == FTYPE_JLOC)
	{
		sprintf(search,GAMEVERSION "/jloc/");
		strcpy(match_ext,".jloc");
	}
	else if (type == FTYPE_PDEMO)
	{
		sprintf(search,GAMEVERSION "/puppetdemo/");
		strcpy(match_ext,".pdm");
	}

	sprintf(match_lev,"%s-",level.mapname);

	len_lev = strlen(match_lev);
	len_ext = strlen(match_ext);

	if ((dip = opendir(search)) == NULL)
		return 0;
	else
	{
		while ((dit = readdir(dip)) != NULL)
		{
			strcpy(name,dit->d_name);
			if ((strcmp(name,".") && strcmp(name,"..")) && 
				((!strsubcmp(0,len_lev,match_lev,name)) &&
				 (!strsubcmp(strlen(name)-len_ext,len_ext,match_ext,name))))
				counter++;
		}
		closedir(dip);
	}
	return counter;
}

#endif

// V1.21 ADD - New File Browser ^^^

void ListAllFilesForMap (edict_t *ent)
{
	int type=FTYPE_NONE;
	int page;
	if (gi.argc() == 1)
		gi.cprintf(ent,PRINT_HIGH,"Wrong syntax: flist <type> [<page#>]\n");
	else if (gi.argc() > 1)
	{
		char buf[16]="";
		sprintf(buf,"%s",strtok(gi.args()," "));
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

		if (gi.argc() == 2)
			page = 1;
		else
		{
			sprintf(buf,"%s",strtok(NULL," "));
			page = atoi(buf);
		}

		if (type == FTYPE_PDEMO)
		{
			ListAllPuppetDemosForMap_PORT(ent,ent->client->fb_con_max,page);
		}
		else if (type == FTYPE_JTRAIL)
		{
			ListAllTrailsForMap_PORT(ent,ent->client->fb_con_max,page);
		}
		else if (type == FTYPE_JLOC)
		{
			ListAllMarkersForMap_PORT(ent,ent->client->fb_con_max,page);
		}
	}
}

void ListAllTrailsForMap_PORT (edict_t *ent, int perpage, int pagenum)
{
#ifdef _WIN32
	ListAllTrailsForMap_PORT_WIN (ent, perpage, pagenum);
#else
	ListAllTrailsForMap_PORT_LINUX (ent, perpage, pagenum);
#endif
}

#ifdef _WIN32

void ListAllTrailsForMap_PORT_WIN (edict_t *ent, int perpage, int pagenum)
{


	//WINDOWS ONLY STUFF//
    struct _finddata_t c_file;
    long hFile;
	//////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_WIN(FTYPE_JTRAIL);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/jtrail/%s-*.jtrail",level.mapname);
	
    if( (hFile = _findfirst( search, &c_file )) == -1L )
		gi.cprintf( ent,PRINT_HIGH,"No trail files found for current map.\n" );
	else
	{
		char Info[MAX_STR_LEN]="";
		
		//gi.cprintf(ent,PRINT_HIGH,"%4i %-5i %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-5i  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Nodes Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 23    4box-to-bridgetop   \n");
		
		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Trail Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i trails total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Nodes Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ----- --------------------"));
		
		counter++;
		if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
		{
			sprintf(name,"%s",c_file.name);
			for (i=strlen(level.mapname)+1;i<strlen(name)-7;i++)
				name[i-(strlen(level.mapname)+1)] = name[i];
			name[i-(strlen(level.mapname)+1)]='\0';
			sprintf (Info, "%s",GetTrailFileInfo(name,true));
			gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
		}
		
		while( _findnext( hFile, &c_file ) == 0 )
		{
			counter++;
			if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
			{
				sprintf(name,"%s",c_file.name);
				for (i=strlen(level.mapname)+1;i<strlen(name)-7;i++)
					name[i-(strlen(level.mapname)+1)] = name[i];
				name[i-(strlen(level.mapname)+1)]='\0';
				sprintf (Info, "%s",GetTrailFileInfo(name,true));
				gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
			}
		}
		_findclose( hFile );
		if (maxpages > 1)
			gi.cprintf(ent,PRINT_HIGH,"Type 'flist trail <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist trail <#>' to load a trail file from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist trail <#>' to download a trail file from the list.\n");
	}
}

#else

void ListAllTrailsForMap_PORT_LINUX (edict_t *ent, int perpage, int pagenum)
{


	//LINUX ONLY STUFF//
	DIR             *dip;
	struct dirent   *dit;
	char match_lev[128]="";
	char match_ext[8]="";
	int len_lev=0;
	int len_ext=0;
	////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_LINUX(FTYPE_JTRAIL);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/jtrail/");
	
	sprintf(match_ext,".jtrail");
	sprintf(match_lev,"%s-",level.mapname);

	len_lev = strlen(match_lev);
	len_ext = strlen(match_ext);

	if(GetFileCountForMap_PORT_LINUX(FTYPE_JTRAIL) < 1)
	{
		gi.cprintf( ent,PRINT_HIGH,"No trail files found for current map.\n" );
		return;
	}

	if ((dip = opendir(search)) == NULL)
		gi.cprintf( ent,PRINT_HIGH,"No trail files found for current map.\n" );
	else
	{
		char Info[MAX_STR_LEN]="";

		//gi.cprintf(ent,PRINT_HIGH,"%4i %-5i %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-5i  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Nodes Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 23    4box-to-bridgetop   \n");

		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Trail Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i trails total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Nodes Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ----- --------------------"));

		while ((dit = readdir(dip)) != NULL)
		{
			strcpy(name,dit->d_name);
			if ((strcmp(name,".") && strcmp(name,"..")) && 
				((!strsubcmp(0,len_lev,match_lev,name)) &&
				(!strsubcmp(strlen(name)-len_ext,len_ext,match_ext,name))))
			{
				counter++;
				if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
				{
					for (i=strlen(level.mapname)+1;i<strlen(name)-7;i++)
						name[i-(strlen(level.mapname)+1)] = name[i];
					name[i-(strlen(level.mapname)+1)]='\0';
					sprintf (Info, "%s",GetTrailFileInfo(name,true));
					gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
				}
			}
		}
		closedir(dip);
			if (maxpages > 1)
				gi.cprintf(ent,PRINT_HIGH,"Type 'flist trail <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist trail <#>' to load a trail file from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist trail <#>' to download a trail file from the list.\n");
	}
}

#endif

void ListAllMarkersForMap_PORT (edict_t *ent,int perpage, int pagenum)
{
#ifdef _WIN32
	ListAllMarkersForMap_PORT_WIN (ent, perpage, pagenum);
#else
	ListAllMarkersForMap_PORT_LINUX (ent, perpage, pagenum);
#endif
}

#ifdef _WIN32

void ListAllMarkersForMap_PORT_WIN (edict_t *ent,int perpage, int pagenum)
{

	//WINDOWS ONLY STUFF//
    struct _finddata_t c_file;
    long hFile;
	//////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_WIN(FTYPE_JLOC);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/jloc/%s-*.jloc",level.mapname);
	
    if( (hFile = _findfirst( search, &c_file )) == -1L )
		gi.cprintf( ent,PRINT_HIGH,"No marker files found for current map.\n" );
	else
	{
		char Info[MAX_STR_LEN]="";
		
		//gi.cprintf(ent,PRINT_HIGH,"%4i %-5i %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-5i  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Marks Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 23    4box-to-bridgetop   \n");
		
		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Trail Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i trails total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Marks Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ----- --------------------"));
		
		counter++;
		if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
		{
			sprintf(name,"%s",c_file.name);
			for (i=strlen(level.mapname)+1;i<strlen(name)-5;i++)
				name[i-(strlen(level.mapname)+1)] = name[i];
			name[i-(strlen(level.mapname)+1)]='\0';
			sprintf (Info, "%s",GetMarkerFileInfo(name,true));
			gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
		}
		
		while( _findnext( hFile, &c_file ) == 0 )
		{
			counter++;
			if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
			{
				sprintf(name,"%s",c_file.name);
				for (i=strlen(level.mapname)+1;i<strlen(name)-5;i++)
					name[i-(strlen(level.mapname)+1)] = name[i];
				name[i-(strlen(level.mapname)+1)]='\0';
				sprintf (Info, "%s",GetMarkerFileInfo(name,true));
				gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
			}
		}
		_findclose( hFile );
		if (maxpages > 1)
			gi.cprintf(ent,PRINT_HIGH,"Type 'flist marker <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist marker <#>' to load a marker file from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist marker <#>' to download a marker file from the list.\n");
	}
}

#else

void ListAllMarkersForMap_PORT_LINUX (edict_t *ent,int perpage, int pagenum)
{

	//LINUX ONLY STUFF//
	DIR             *dip;
	struct dirent   *dit;
	char match_lev[128]="";
	char match_ext[8]="";
	int len_lev=0;
	int len_ext=0;
	////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_LINUX(FTYPE_JLOC);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/jloc/");
	
	sprintf(match_ext,".jloc");
	sprintf(match_lev,"%s-",level.mapname);

	len_lev = strlen(match_lev);
	len_ext = strlen(match_ext);

	if (GetFileCountForMap_PORT_LINUX(FTYPE_JLOC) < 1)
	{
		gi.cprintf( ent,PRINT_HIGH,"No marker files found for current map.\n" );
		return;
	}

	if ((dip = opendir(search)) == NULL)
		gi.cprintf( ent,PRINT_HIGH,"No marker files found for current map.\n" );
	else
	{
		char Info[MAX_STR_LEN]="";

		//gi.cprintf(ent,PRINT_HIGH,"%4i %-5i %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-5i  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Marks Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 23    4box-to-bridgetop   \n");

		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Trail Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i trails total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Marks Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ----- --------------------"));

		while ((dit = readdir(dip)) != NULL)
		{
			sprintf(name,dit->d_name);
			if ((strcmp(name,".") && strcmp(name,"..")) && 
				((!strsubcmp(0,len_lev,match_lev,name)) &&
				(!strsubcmp(strlen(name)-len_ext,len_ext,match_ext,name))))
			{
				counter++;
				if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
				{
					for (i=strlen(level.mapname)+1;i<strlen(name)-5;i++)
						name[i-(strlen(level.mapname)+1)] = name[i];
					name[i-(strlen(level.mapname)+1)]='\0';
					sprintf (Info, "%s",GetMarkerFileInfo(name,true));
					gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
				}
			}
		}
		closedir(dip);
			if (maxpages > 1)
				gi.cprintf(ent,PRINT_HIGH,"Type 'flist marker <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist marker <#>' to load a marker file from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist marker <#>' to download a marker file from the list.\n");
	}
}

#endif

//this is a console browser function

void ListAllPuppetDemosForMap_PORT (edict_t *ent, int perpage, int pagenum)
{
#ifdef _WIN32
	ListAllPuppetDemosForMap_PORT_WIN (ent, perpage, pagenum);
#else
	ListAllPuppetDemosForMap_PORT_LINUX (ent, perpage, pagenum);
#endif
}

#ifdef _WIN32

void ListAllPuppetDemosForMap_PORT_WIN (edict_t *ent,int perpage, int pagenum)
{

	//WINDOWS ONLY STUFF//
    struct _finddata_t c_file;
    long hFile;
	//////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_WIN(FTYPE_PDEMO);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/puppetdemo/%s-*.pdm",level.mapname);
	
    if( (hFile = _findfirst( search, &c_file )) == -1L )
		gi.cprintf( ent,PRINT_HIGH,"No puppet demos found for current map.\n" );
	else
	{
		char Info[256]="";
		
		//gi.cprintf(ent,PRINT_HIGH,"%4i %-16s %5s %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-16s            %5s  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Author           Time  Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ---------------- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 PaTMaN^J=SC=       1.1 4box-to-bridgetop   \n");
		
		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Puppet Demo Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i demos total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Author           Time  Pup Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ---------------- ----- --- --------------------"));
		
		counter++;
		if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
		{
			sprintf(name,"%s",c_file.name);
			for (i=strlen(level.mapname)+1;i<strlen(name)-4;i++)
				name[i-(strlen(level.mapname)+1)] = name[i];
			name[i-(strlen(level.mapname)+1)]='\0';
			sprintf (Info, "%s",GetPuppetDemoFileInfo(name,true,false,false));
			gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
		}
		
		while( _findnext( hFile, &c_file ) == 0 )
		{
			counter++;
			if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
			{
				sprintf(name,"%s",c_file.name);
				for (i=strlen(level.mapname)+1;i<strlen(name)-4;i++)
					name[i-(strlen(level.mapname)+1)] = name[i];
				name[i-(strlen(level.mapname)+1)]='\0';
				sprintf (Info, "%s",GetPuppetDemoFileInfo(name,true,false,false));
				gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
			}
		}
		_findclose( hFile );
		if (maxpages > 1)
			gi.cprintf(ent,PRINT_HIGH,"Type 'flist puppet <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist puppet <#>' to load a puppet demo from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist puppet <#>' to download a puppet demo from the list.\n");
	}
}

#else

void ListAllPuppetDemosForMap_PORT_LINUX (edict_t *ent,int perpage, int pagenum)
{

	//LINUX ONLY STUFF//
	DIR             *dip;
	struct dirent   *dit;
	char match_lev[128]="";
	char match_ext[8]="";
	int len_lev=0;
	int len_ext=0;
	////////////////////


	char search[256]="\0";
	int counter=0;
	char name[256]="\0";
	unsigned int i;
	int totaldemos=GetFileCountForMap_PORT_LINUX(FTYPE_PDEMO);
	int maxpages=totaldemos/perpage;
	if (((float)totaldemos/(float)perpage) > (totaldemos/perpage))
		maxpages = totaldemos/perpage + 1;
	else
		maxpages = totaldemos/perpage;
	if (pagenum > maxpages)
		pagenum = maxpages;
	if (pagenum < 1)
		pagenum = 1;

	sprintf(search,GAMEVERSION "/puppetdemo/");
	
	sprintf(match_ext,".pdm");
	sprintf(match_lev,"%s-",level.mapname);

	len_lev = strlen(match_lev);
	len_ext = strlen(match_ext);

	if (GetFileCountForMap_PORT_LINUX(FTYPE_PDEMO) < 1)
	{
		gi.cprintf( ent,PRINT_HIGH,"No puppet demos found for current map.\n" );
		return;
	}

	if ((dip = opendir(search)) == NULL)
		gi.cprintf( ent,PRINT_HIGH,"No puppet demos found for current map.\n" );
	else
	{
		char Info[256]="";

		//gi.cprintf(ent,PRINT_HIGH,"%4i %-16s %5s %s\n",counter,player,CharFloat(time),filename);
		//gi.cprintf(ent,PRINT_HIGH," %4i %-16s            %5s  %s                  \n");
		//gi.cprintf(ent,PRINT_HIGH," ### Author           Time  Filename            \n");
		//gi.cprintf(ent,PRINT_HIGH,"---- ---------------- ----- --------------------\n");
		//gi.cprintf(ent,PRINT_HIGH,"   1 PaTMaN^J=SC=       1.1 4box-to-bridgetop   \n");

		//LINUX PORT ITOA//itoa(pagenum,Info,10);
		sprintf(Info,"%i",pagenum);
		gi.cprintf(ent,PRINT_HIGH,"\n%s Puppet Demo Page %s ",level.mapname,AltString(Info));
		//LINUX PORT ITOA//itoa(maxpages,Info,10);
		sprintf(Info,"%i",maxpages);
		gi.cprintf(ent,PRINT_HIGH,"of %s (%i demos total)\n",AltString(Info),totaldemos);
		strcpy(Info,"\0");
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString(" ### Author           Time  Pup Filename"));
		gi.cprintf(ent,PRINT_HIGH,"%s\n",AltString("---- ---------------- ----- --- --------------------"));

		while ((dit = readdir(dip)) != NULL)
		{
			sprintf(name,"%s",dit->d_name);
			if ((strcmp(name,".") && strcmp(name,"..")) && 
				((!strsubcmp(0,len_lev,match_lev,name)) &&
				 (!strsubcmp(strlen(name)-len_ext,len_ext,match_ext,name))))
			{
				counter++;
				if ((counter > ((pagenum-1)*perpage)) && (counter <= (pagenum*perpage)))
				{
					for (i=strlen(level.mapname)+1;i<strlen(name)-4;i++)
						name[i-(strlen(level.mapname)+1)] = name[i];
					name[i-(strlen(level.mapname)+1)]='\0';
					sprintf (Info, "%s",GetPuppetDemoFileInfo(name,true,false,false));
					gi.cprintf(ent,PRINT_HIGH,"%4i %s\n",counter,Info);
				}
			}
		}
		closedir(dip);
			if (maxpages > 1)
				gi.cprintf(ent,PRINT_HIGH,"Type 'flist puppet <#>' to jump to another page.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fload flist puppet <#>' to load a puppet demo from the list.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type 'fdownload flist puppet <#>' to download a puppet demo from the list.\n");
	}

}

#endif

/* //For Reference on new puppetdemo command workings
   // + Indicates done

+on pplay:
+  conditions:	puppetdemo is loaded
+	puppetdemo_pause = FALSE;
+	puppetdemo_frame = 0;
+	puppetdemo_speed = N/C
+	puppetdemo_state = PUPPET_NEW_PLAYING
+	** if (puppetdemo_state == PUPPET_NEW_RECORDING) pstop first
+on pclear:
+  conditions:	puppetdemo_state != PUPPET_NEW_RECORDING &&
+				puppetdemo is loaded
+	puppetdemo_pause = FALSE;
+	puppetdemo_frame = 0;
+	puppetdemo_speed = N/C;
+	puppetdemo_state = PUPPET_NEW_NOTHING
+on pspause:
+  conditions:	puppetdemo_state == PUPPET_NEW_PLAYING
+	puppetdemo_pause = toggle TRUE/FALSE
+	puppetdemo_frame = N/C
+	puppetdemo_speed = N/C
+	puppetdemo_state = N/C
+on ps+:
+  conditions:	puppetdemo_speed < PUPPET_NEW_SPEED_MAX
+	puppetdemo_pause = N/C
+	puppetdemo_frame = N/C
+	puppetdemo_speed += 1
+	puppetdemo_state = N/C
+on ps-:
+  conditions:	puppetdemo_speed > PUPPET_NEW_SPEED_MIN
+	puppetdemo_pause = N/C
+	puppetdemo_frame = N/C
+	puppetdemo_speed -= 1
+	puppetdemo_state = N/C
+on pstop:
+  conditions:	puppetdemo_state == PUPPET_NEW_RECORDING ||
+				puppetdemo_state == PUPPET_NEW_PLAYING
+	puppetdemo_pause = FALSE
+	puppetdemo_frame = 0
+	puppetdemo_speed = N/C
+	puppetdemo_state = PUPPET_NEW_NOTHING
+	** if (puppetdemo_state == PUPPET_NEW_PLAYING) make sure puppet sets back to initial
+on precord:
+  conditions:	puppetdemo_state == PUPPET_NEW_NOTHING ||
+		puppetdemo_state == PUPPET_NEW_RECORDING
+	puppetdemo_pause = FALSE
+	puppetdemo_frame = 0
+	puppetdemo_speed = PUPPET_NEW_SPEED_MAX
+	puppetdemo_state = PUPPET_NEW_RECORDING
+on psstep+:
+  conditions:	puppetdemo_state == PUPPET_NEW_PLAYING &&
+				puppetdemo_pause == TRUE &&
+	puppetdemo_pause = N/C (Unset, Run Frame, Set)
+	puppetdemo_frame = N/C
+	puppetdemo_speed = N/C
+	puppetdemo_state = N/C
+on psstep-;
+  conditions:	puppetdemo_state == PUPPET_NEW_PLAYING &&
+				puppetdemo_pause == TRUE &&
+	puppetdmeo_pause = N/C (Unset, Run Frame, Set)
+	puppetdemo_frame -= 2
+	puppetdemo_speed = N/C
+	** Make sure frame is >= 0 after decrease

*/

