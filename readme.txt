


				^J  Mod  V1.23.03  -  By  PaTMaN^J
				     http://patman.has.it

				Mod Release Date: June 18, 2006
				Help file Date: June 18, 2006



Table of Contents
------------------------------------
1. Installation
2. Setting Up a Server
3. Game play
4. Heads Up Display (HUD)
5. Players
6. Weapons
7. Commands
8. Jump Trail Use
9. Puppet Demo Recording
10. Spectator Mode
11. History
12. Questions/Comments/Bug Reporting
13. Thanks
------------------------------------

----------------------------------------------------------------------------------------
1. Installation
----------------------------------------------------------------------------------------
Go to your "action" directory.
Rename the file "gamex86.dll" to "gamex86.dll_reg"
Place the ^J Mod "gamex86.dll" file in your action directory.

NOTE: Doing this DOES NOT AFFECT game play on other servers.
      All this does is affect the game play on your server.
NOTE: You may need to uncheck the "Hide protected operating system files" option
      in windows explorer:

        --------------------------------------------------------------------------------
	Windows 2000/XP
	--------------------------------------------------------------------------------
	 1. Open windows explorer
	 2. Select the "Tools" menu
	 3. Select "Folder Options..."
	 4. Select the "View" tab
	 5. Find "Hide protected operating system files (Recommended)" and uncheck it.

----------------------------------------------------------------------------------------
2. Setting Up a Server
----------------------------------------------------------------------------------------
Just run quake2 and run a map by typing "map name" in the console.
This is called a "listen" server.
NOTE: Server settings should be as followed:

	   deathmatch 1
	   cheats 0
	   dmflags 0
           weaponban # (read about weapon banning in the commands section)
           allow_downloading 1 (if you want to allow people to download jmod files)
           (also read about the map voting commands)

To make a dedicated server:
Make a shortcut to your quake2.exe like this:
your path\quake2.exe +set dedicated 1 +set deathmatch 1 +set cheats 0 +set dmflags 0 +set weaponban # ....
You could also make a config file dedicated to the server for setting server variables, then exec it.

Also, since map voting only searches for maps in your action/maps directory, you can make
a file in your action directory called jmodmaplist.txt. In this file, list additional maps
that you know you have but are in .pak files. Maps should be listed 1 per line and must
reflect the map's filename, such as urban, urban2, teamjungle, and so on. Manipulating this
list can also be done through the sv commands addmap and removemap.

If you would like users to be able to use the "man" command, you need a file named
jmod.man in your action directory. This file has a simple format to it and the game does
not need to be reloaded if it is modified. Be careful not to overflow players with too much
text for a command. The jmod.man file format is as follows:

//This is a ^J Mod layout example file
//Lines beginning with // are comments and will not be parsed
[Version]
^J Mod Manual V1.00
[keyword_1]
Line of text that will be displayed in the game
Multiple lines can be used

And also blank lines for spacing
//and here is a comment line that won't be displayed
[keyword_2]
As you can see, keywords to search for start in brackets
like this one, [keyword_2]
Keywords are not case sensitive
[keyword_3]
Keywords must be a single word
If you wish to extend info on a keyword, then simply use an underscore
like [teleporter_set and state in the [teleporter] keyword text that you may
also look up the man for teleporter_set for even more info
//
//Also note that text lines MUST NOT begin with a [ or the parser will think
//it has come upon a new section
//
//So here is a real command:
[teleporter]
Syntax: teleporter <"set"/"recall">
teleporter set - Sets your teleporter destination to your current location
teleporter recall - Teleports you to the destination set by "teleporter set"
//And thats all there is to it
//In the game, someone would type "man teleporter" and get the above block of text

----------------------------------------------------------------------------------------
3. Gameplay
----------------------------------------------------------------------------------------
You can not drop weapons.
You can not drop items.
You can not die.
Falling damage is displayed instead of taken.
There is no teamplay, this is purely deathmatch.
Noclip is no longer a cheat.
Pings over 999 are no longer masked to 999.
Laser is used more as a pointer.
You can spawn a "puppet" (fake player) to aid you in team jumping practice.
You can set your own movement gravity.
You can set jump markers to aid in practice.
You can create jump trails to aid in practice.
You can record your movement for later playback by a puppet.

----------------------------------------------------------------------------------------
4. Heads Up Display (HUD)
----------------------------------------------------------------------------------------
The HUD has been modified to only display information useful to the ^J mod.
At the top right corner of the screen, you will see your:
	-Current Speed
	-High Speed
	-Last Falling Damage
	-Your Gravity
You can modify your HUD with toggle commands listed below

----------------------------------------------------------------------------------------
5. Players
----------------------------------------------------------------------------------------
Can walk through each other (if team jumping isn't enabled. (see Commands))
Can only be hit by weapons if shootable is enabled. (do not take damage though)
Can only be kicked by others if they have kicking enabled. (see Commands)
Are not affected by grenade blasts if grenade jumping isn't turned on. (see Commands)
Can spawn a "Puppet" to aid them in team jumping. (see commands)
Turn red if ping is >999.
Turn green if noclip is on.

----------------------------------------------------------------------------------------
6. Weapons
----------------------------------------------------------------------------------------
Have infinite ammo.
Do not need to be reloaded.
There is a new jump marker weapon for placing and deleting jump markers.

----------------------------------------------------------------------------------------
7. Commands (These should be typed in the console or binded to keys)
----------------------------------------------------------------------------------------
---------------------------------
Normal Quake2 Commands
---------------------------------

inven
	-This displays the ^J menu system, where most of the below commands are
	 available
	-This is the same command used in AQ2 to display the team selection menu
	 and weapon selection menu

invnext
	-This moves the cursor in a menu to the next item

invprev
	-This moves the cursor in a menu to the previous item

invuse
	-This selects the item in a menu that the cursor is currently on

fov <#>
	-Change your field of view
	-Default is 90, Lowest is 1, Highest is 179

noclip
	-This allows you to float around and all collision detection is ignored (you can
	 move through walls)

spectator <#>
 	-When on, you can spectate people
	-Press your shoot button to cycle through spectating modes
	-Press your jump button to cycle through players
	- 1 is on, 0 is off

---------------------------------
Toggle Commands
---------------------------------

tog -or- toggle
	-Displays the available options to toggle (listed below)

  --------------------
  Item Toggles
  --------------------

tog laser 
	-Turns the laser on/off
	-When on, it will be on for any weapon
	-The laser doesn't disappear when it hits the sky, so you can check sky height

tog silencer
	-Turns the silencer on/off

tog slippers
	-Turns slippers on/off

tog vest
	-Turns the vest on/off

tog ir
	-Toggles IR Vision mode

tog id
	-Turns the player identification on/off
	-This is the name displayed below your crosshair when someone crosses it
	-This is always on in spectator mode

tog range
	-Turns the range finder on/off
	-This does measure even when it hits the sky, so you can check sky height
	-If the range is a negative number, it means the current slope you are pointing
	 at is too steep to walk up

  --------------------
  Player Mode Toggles
  --------------------

tog teamjump
	-Allows you to interact with other players by becoming solid
	-This SHOULD NOT be turned on if you are not doing team jumps because it can
	 interfere with other's movement if you touch them

tog grenadejump
	-Allows you to interact with your own grenades
	-Others are never affected by your grenades

tog grenadejumps
	-Same as tog grenadejump but allows you to interact with others' grenades

tog kickable
	-Allows you to be kicked by others
	-Teamjumping must be enabled for you and the kicker

tog shootable
	-When enabled, you can be shot

tog spectator
	-Toggles spectator mode

  --------------------
  Jump Marker Toggles
  --------------------

tog markercolor
	-Switches the color of the markers thrown by the jump marker weapon
	-Colors in order are: red, green, blue, yellow, white, purple, and aqua

tog jumpmarkercolor
	-Switches the color of the markers set when jumping
	-Colors listed above

tog fallmarkercolor
	-Switches the color of the markers set when falling
	-Colors listed above

tog markerflashes
	-Turns marker flashes on/off
	-Touching a marker will make your screen flash to that color

tog markertext
	-Turns marker text on/off
	-When on, if a marker has a message attached to it, it will be displayed on your screen

tog markjumps
	-Turns jump marking on/off
	-When on, every time you start, continue, and end a jump, a marker is set at your feet

tog markfalls
	-Turns fall marking on/off
	-When on, every time you fall and hit the ground, a marker is set at your feet

  --------------------
  Jump Trail Toggles
  --------------------

tog trailing
	-A trail is created respective to your movement

tog trailcolor
	-Cycle through the different trail colors
	-Colors in order are: red, green, blue, yellow, white, purple, and aqua

tog trailflashes
	-When you touch a trail, your screen turns that color

  --------------------
  Puppet Toggles
  --------------------

tog puppettrans
	-Toggle your puppet's transparency

tog puppetshoot
	-Toggle whether or not the puppet will shoot when you fire

tog puppetsight
	-Toggle the sight line of your puppet

tog puppetshootarea
	-Toggles through the areas where your puppet aims at you
	-Head, Chest, Stomach, Legs, and Random

tog puppetsolid
	-Toggles the puppet's solid state

  --------------------
  HUD Toggles
  --------------------

tog hud
	-Toggles the display of the entire HUD

tog hud_speed
	-Toggles the display of the HUD Speed

tog hud_highspeed
	-Toggles the display of the HUD High Speed

tog hud_lastfalldamage (tog hud_lastfd)
	-Toggles the display of the HUD Last Falling Damage

tog hud_gravity
	-Toggles the display of the HUD Gravity

---------------------------------
Teleporter Commands
---------------------------------

teleporter set
	-Sets a teleportation point for recalling
	-This MUST be set before using "teleporter recall"

teleporter recall
	-Sends you to the teleportation point stored by "teleporter set"

goto <#> <#> <#>
        -Teleports you to the coordinates you specify (ex: goto -135 544 64)

spawnp [<#>]
	-Sends you to a random spawn point or specific spawn point with #

spawnc
	-Sends you to the closest spawn point

---------------------------------
HUD Commands
---------------------------------

rhs
	-Resets the high speed display on the HUD

demomode
	-Clears all of the HUD stats for cleaner demo recording
	
hud_speed_source <axes(xyz)>
	-Sets which world axes to use in speed calculation
	-Any combination of x y and z is valid

---------------------------------
Jump Marker Commands (JLOC)
---------------------------------

use jump marker
	-Changes your weapon to the jump marker
	-With the jump marker, you can place colored objects around for aid
	 in teaching jumps to people
	-Use the "weapon" command to cycle through marker placement/deletion mode
	-When in deletion mode, markers that are highlighted will be removed upon firing

markercount
	-Displays the total number of markers set in the level

markercolor <color>
	-Change the color of your thrown markers
	-Colors are: red, green, blue, yellow, white, purple, and aqua

jumpmarkercolor <color>
	-Change the color of your jump markers
	-Colors listed above

fallmarkercolor <color>
	-Change the color of your fall markers
	-Colors listed above

remallmarkers
	-Removes all the markers from the map

remviewmarkers
	-Removes all markers in your view

namemarker <text>
	-Attaches a text string to a marker
	-40 Character max per marker
	-You must be pointing at a marker when using this command
	-"namemarker" alone will erase the message attached to the marker

  --------------------
  Other Marker Commands
  --------------------

jlocsave all <filename>
	-Saves all the markers in the level to a jloc file of <filename>
	-These files are saved to action/jloc
	-These are only saved on the server's machine

jlocsave view <filename>
	-Saves the markers in your current view to a jloc file of <filename>
	-These files are saved to action/jloc
	-These are only saved on the server's machine

msave <all/view> <filename>
	-Shortcut to the above marker saving commands

---------------------------------
Jump Trail Commands (JTrail)
---------------------------------

trailcount
	-Displays the total number of trail links set in the level

trailcolor <color>
	-Change the color of your trail links
	-Colors are: green, blue, yellow, white, orange, black, and red

trailcolor change <color>
	-Point at a trail and use this to change the trail's color to <color>

trailwidth <#>
	-Sets the width of your jump trail
	-Default is 4

goto trail start
	-Teleports you to the start of the trail you are pointing at

goto trail end
	-Teleports you to the end of the trail you are pointing at

  --------------------
  JTRAIL Removing
  --------------------

remalltrails
	-Removes all the jump trails from the level

remviewtrails
	-Removes all the jump trails in your view (the full trail)

remviewlinks
	-Removes all the jump trail links in your view (trail is then relinked)

remlasttrail
	-Removes the last jump trail that you created

remtouchtrail
	-Removes the jump trail that you are currently touching

rempointtrail
	-Removes the jump trail that you are currently pointing at (based on best pointing at)

  --------------------
  JTRAIL Cleaning
  --------------------

  Jump trail cleaning is taking a set of links and deleting every other one to create room
  for more trails or cut down on unnecessary links

cleanalltrails
	-Cleans all the jump trails in the level

cleanviewtrails
	-Cleans all the jump trails in your view (the full trail)

cleanviewlinks
	-Cleans all the jump trail links in your view

cleanlasttrail
	-Cleans the last jump trail that you created

cleantouchtrail
	-Cleans the jump trail that you are currently touching

cleanpointtrail
	-Cleans the jump trail that you are currently pointing at (based on best pointing at)

  --------------------
  Other Trail Commands
  --------------------

jtrailsave all <filename>
	-Saves all the trails in the level to a jtrail file of <filename>
	-These files are saved to action/jtrail
	-These are only saved on the server's machine

jtrailsave view <filename>
	-Saves the trails in your current view to a jtrail file of <filename>
	-These files are saved to action/jtrail
	-These are only saved on the server's machine

jtrailsave last <filename>
	-Saves the last trail you created to a jtrail file of <filename>
	-These files are saved to action/jtrail
	-These are only saved on the server's machine

tsave <all/view/last> <filename>
	-Shortcut to the above jtrail saving commands

---------------------------------
Puppet Commands
---------------------------------

puppet
	-Create/Remove your puppet

puppetshootarea <area>
	-Set your puppet to aim at a specific area of you
	-Head, Chest, Stomach, Legs, and Random are valid areas

pswitch
	-Switch positions with your puppet
	-This can be used to quickly modify the puppet's position for setting up

  --------------------
  Puppet Demo Commands
  --------------------

  Read section 9 for more info on puppet demo recording


precord
	-Starts recording your puppet demo

pstop
	-Stops recording your puppet demo

pplay
	-Plays your puppet demo

ppause
	-Pauses your running puppet demo

ps+
	-Increases the speed of your running puppet demo
	-Speed range is from 0.1x to 1x

ps-
	-Decreases the speed of your running puppet demo
	-Speed range is from 0.1x to 1x

pstep+
	-Steps 1 frame forward in your running puppet demo
	-The step amount reflects the puppet speed

pstep-
	-Steps 1 frame backward in your running puppet demo
	-The step amount reflects the puppet speed

pclear
	-Clears your puppet demo

psave <filename>
	-Saves your puppet demo
	-These files are saved in action/puppetdemo
	-These are only saved on the server's machine

---------------------------------
Map Voting Commands
---------------------------------

votemap <name>
	-Votes for a map if it exists
	-You can do this multiple times to change your vote

viewvotes
	-Displays the current stats of a map vote if active


---------------------------------
Misc Commands
---------------------------------

lca
	-Initiates a "LIGHTS, CAMERA, ACTION!" teamplay round start sequence so you
	 can practice start jumps, such as in mesto

gravity
	-Set your movement gravity

playerlist
	-Displays the info of players on the server:
		-Time connected
		-Ping
		-Score
		-Name (Name (spectator) if they are spectating)

cleanlevel
	-Removes all markers/trails/knives on the map
	-This should be used if an out of control overflow ever occurs

jrecord <filename>
	-This will make you record a demo and a puppet demo at the same time
	-The puppet demo messages of starting/stopping will not be displayed
	 but it will be recording one correctly

jstop
	-This will stop recording a demo and a puppet demo if any is being recorded

lens [<0/2/4/6/in/out>]
	-Lens command from TNG, for sniper rifle zooming

cvar <inc / dec> [<step (1)>] [<start #>]
	-Used to increment or decrement a numeric cvar

grenadejumplist <# # # ... / clear>
	-Used to specify which client IDs can hit you with grenades

man <keyword>
	-Used to look up help or info on a command or keyword

---------------------------------
File Browser Commands and Info
---------------------------------

flist <type> [<page #>]
	-Console style file browser
	-Displays more info on files than the menu style file browser

fload <type> <filename>
fload flist <type> <file #>
	-Loads files
	-Trails load 10 nodes per second to avoid overflows
	-Markers load 10 per second to avoid overflows
	-Special syntax for loading trails:
	   fload trail <filename> [<clean_level>]
	   fload flist trail <file #> [<clean_level>]
        -Special syntax for loading trails from puppet demos:
           fload puppet <filename> <"trail"> [<clean_level>] [<color>] [<size>]
           fload flist puppet <file #> <"trail"> [<clean_level>] [<color>] [<size>]

finfo <type> <filename>
finfo flist <type> <file #>
	-Displays file info
	-Extent of info for puppetdemo files may differ due to changes in the
	 file format over versions

fdownload <type> <filename>
fdownload flist <type> <file #>
	-Downloads files
	-Server downloading must be enabled (set allow_downloading 1)

fb_menu_max <1-15>
	-Max items to display per page for the menu file browser
	-Set this lower if characters are missing from the menu

fb_con_max <1-15>
	-Max items to display per page for the console file browser
	-Set this lower if overflows occur with flist

Other Notes:
	-Valid types are puppet, trail, and marker
	-The files displayed are on the server's machine
	-When saving files, they are saved to the server's machine

About file saving security:
	-No information is sent from the client to the server when saves occur
	-All saved information is from the server's memory or temporary files

---------------------------------
Spectator Commands
---------------------------------

spectarg
	-Spectates the targeted player, puppet, or pup
	-You need to be in spectator mode for this to function

---------------------------------
Server Commands (sv)
---------------------------------

sv killvote
	-Kills a mapvote if one is in progress

sv addmap <mapname>
        -Adds a map to the jmodmaplist.txt file for voting use

sv removemap <mapname>
        -Removes a map from the jmodmaplist.txt file

sv fdelete
	-Deletes a puppet demo, jump trail, or marker file

sv frename
	-Renames a puppet demo, jump trail, or marker file

---------------------------------
Server Variables
---------------------------------

mapvoting <1/0>
	-Set to 1 to allow the votemap command
	-Default 1

mapvotingtime <#>
	-Number of seconds to wait before changing to the voted map
	-Default 15
	-If a map vote of 100% is achieved, then this variable is bypassed

mapvotingpercent <#>
	-The votemap percentage has to be more than this setting in order for a vote to pass
	-Default 50

maxpuppetdemolength <#>
	-Maximum length of a puppet demo in seconds
	-Default 30

puppetdemosmoothing <1/0>
	-Controls the smoothing of puppet demo playback
	-Default 1

markerlimit <#>
	-New markers can not be placed while this limit is hit
	-Default 100

traillimit <#>
	-New trails can not be placed while this limit is hit
	-Default 100

weaponban <#>
	-Bans FIRING/USAGE of certain weapons/items
	-This does not prevent weapon selection, since some people, like myself, feel
	 more comfortable jumping with something other than a pistol. Maybe even 2x.
	-To use weaponban:
		Take a look at the list below of weapons and their values.
		To ban weapons, take their values and add them together.
		EX: To ban the m4, mp5, and shotgun: 16+1+2=19: weaponban 19

		MP5/10 Submachinegun		1
		M3 Super90 Assault Shotgun	2
		Handcannon			4
		SSG 3000 Sniper Rifle		8
		M4 Assault Rifle		16
		Combat Knives			32
		Akimbo Pistols			64
		MK23 Pistol			128
		Jump Marker			256
		Jump Trail			512
		Grenades			1024

	---------------------
  	  Quick Ban Numbers
	---------------------
	 -    1279  - ban all weapons except markers and trails
	 -    2047  - ban everything

-------------------------
  Text Message Commands
-------------------------
  TEXT  -  Announces
-------------------------
   %C   -  Your fall Count
   %D   -  Your falling damage
   %G   -  Your gravity
   %H   -  Your high-speed
   %I   -  Your active items
   %J   -  Your jump modes
   %L   -  Your last falling damage
   %M   -  Your marker color
   %N   -  The player's name you are looking at
   %P   -  Your current position (feet) on the map (relative to the map origin)
   %S   -  Your current speed
   %T   -  The names of people near you
   %W   -  Your current weapon


----------------------------------------------------------------------------------------
8. Jump Trail Usage
----------------------------------------------------------------------------------------
You should always keep in mind that jump trails can cause overflows so use them carefully.
Clean up any trails that you aren't using, instead of just increasing the traillimit var.
Use the "remalltrails" or "cleanlevel" command if you start overflowing
If you get the message "Netchan_Transmit: dumped unreliable", there are probably too many
 trails on the map. Remove some trails or don't worry about it.

----------------------------------------------------------------------------------------
9. Puppet Demo Recording
----------------------------------------------------------------------------------------
Puppet demo recording is just like recording a regular demo. Start recording via the
 "precord" command. Do whatever u want to record then stop recording via the "pstop"
 command. Then to play the demo back, type "pplay".
When saving a puppet demo, it is only saved on the server's machine. Therefore, you can
 not "fload" or "flist" puppet demos stored on your machine if you are not the server.

----------------------------------------------------------------------------------------
10. Spectator Mode
----------------------------------------------------------------------------------------

The spectator mode has been completely rewritten. Here are its functions:

+attack 	- cycle view modes (3rd person, eyes, free)
+forward 	- move towards target in 3rd person
+back 		- move away from target in 3rd person
+moveleft 	- cycle from eyes to 3rd person on target
+moveright 	- cycle from 3rd person to eyes on target
+moveup 	- cycle through targets forward (player_1, puppet_1, pup_1, player_2, ...)
+movedown 	- cycle through targets backward (pup_2, puppet_2, player_2, pup_1, ...)

----------------------------------------------------------------------------------------
11. History
----------------------------------------------------------------------------------------
V1.23 incremental updates to V1.24
  -Updated puppets so they can now be shot during a puppet demo, while puppetsolidpplay is not on
  -Updated spectating mode to function differently when sole person on server
  -Updated manual syntax to include additional parsing options (find and for)
  -Added tog puppetsolidpplay
  -Updated puppet demos to now replay multiple Pup placements
  -Fixed Pup eyes spectating giving limited viewing
  -Fixed pclear command
  -Fixed last line of last section of the manual printing to the client if it should not
  -Fixed HUD not reflecting client's HUD when chasing
  -Fixed loading a puppet demo while one is playing not resetting puppet positio
  -Fixed player spawning at worldspawn when coming out of spectator mode from spectating a removed puppet
  -Fixed entering spectator mode not stopping puppet demo recording
  -Fixed puppet spinning at certain angles during puppet demo
  -Fixed puppet disappearing momentarily when it spawns a pup in a puppet demo
  -Fixed being hit by an explosion from the world resulting in a crash
  -Fixed laser sight not on target
  -Fixed server var jmodversion not showing in server info
V1.23	(June 18, 2006)
  -Added lens command
  -Added tog spectator command
  -Added HUD tog commands
  -Added man command
  -Added spectarg command
  -Added toggles to be affected by others' grenades
  -Added grenadejumplist command
  -Added cvar command
  -Added server commands for deleting/renaming files for markers, trails, and puppet demos
  -Added the HUD menu section
  -Removed playerlistinfo command
  -Updated trail loading commands to include a clean level
  -Fixed turning off ID display turning off marker text (if it was on)
  -Fixed down knife throwing rate
  -Fixed Puppet/Pup model angles when looking up or down
  -Fixed teleporter recall view angle bugs 
  -Fixed spawnp and spawnc not setting view angle to spawn point's angles
  -Fixed coming out of spectator mode creating player state problems
  -Fixed goto command teleporting you higher than specified
  -Fixed up bugs when slowing down puppet demos
  -Fixed psstep+/- always stepping a full frame no matter what speed the demo is set at
  -Fixed trail loading from a puppetdemo
  -Reworked puppet commands for cleaner usage and coding
  -Disabled shooting on LCA
  -Disabled telefrags
  -Removed Ppt and Pup from score board and added ID and Grav
  -Removed the flashlight effect on the laser
  -Removed map rotation support, making all map changes done through voting
  -A map vote of 100% ignores the change countdown timer
  -Added the new spectator mode with extra controls including zoom and reverse seeking 
  -Puppet no longer freezes up while recording a puppet demo
  -Teleporting into a puppet no longer gets the player stuck
  -Teleporting into a player no longer gets the players stuck
  -Cleaned up view problems with pswitch command
  -Loaded puppet demo is no longer unloaded when failing to load a new demo with fload
  -You can now see what the jump marker removal weapon will remove before firing
V1.22   (January 31, 2006)
  -(Linux) Fixed flist not working
  -(Linux) Fixed crash with addmap
  -Fixed bad filenames when saving marker and trail files
  -Fixed loading trails from puppet demos with a pup resulting in a bad first link
  -Fixed markers being thrown through walls
  -Multiple jrecord commands now keep recording the puppet demo until jstop
  -Added commands: msave, tsave
V1.21   (January 20, 2006)
  -Fixed range finder display
  -Fixed crashes/bugs when additional jmod directories were not created
  -Fixed loaded trails not responding to certain trail commands
  -Fixed Spectator mode not staying through map change correctly
  -Fixed gravity change crash
  -Fixed puppet spectating crashes
  -Fixed goto command crash
  -Fixed spawnp crash
  -Fixed spawnp not finding all spawn points
  -Fixed some small memory leaks
  -Fixed jrecord crash
  -Fixed map voting
  -Knives now damage puppets instead of bouncing off
  -Turn puppet demos into trails through flist and fload
  -2 file browser systems: includes puppet demos, trails, and markers.
  -Added commands: flist, fload, finfo, fdownload, spawnc, pswitch,
   tog jumpmarkercolor, tog fallmarkercolor
  -Added vars: fb_menu_max, fb_con_max
  -Removed commands: jlocload (fload), jtrailload (fload), pload (fload),
   plist (flist), pinfo (finfo), pdownload (fdownload)
V1.20	(March 28, 2004)
  -Added Puppet's puppet for puppet demos
  -Puppet's puppet becomes your puppet on puppet remove
  -Tog puppetsolid
  -Puppet demo files can now be up to about 54% smaller in file size
  -after loading a puppet demo, you can't save unless you start a new puppet demo
  -puppet demo information is now displayed on play
  -Player lagging identification code better
  -Fixed puppets falling through stuff when it is spawned on a player other than its owner
  -Spawnp <spawn point number>
  -plist <page number>
  -pload plist <puppet demo list number>
  -pdownload <puppet demo name>
  -pdownload plist <puppet demo list number>
  -updated find_overlap to include maxs[2] for ducked checking
  -fixed a bug when entering spectator mode after a map change
  -fixed various map voting bugs
  -removed spectator status from playerlist command
  -added "spectators" command
  -added "Player", "Puppet", and "Pup" to id display
  -Can now spectate puppets and pups
  -Can't save a puppet demo with non default level gravity used at somepoint in the demo
  -Added pup (possible teamjump) identification to plist
  -Puppets and Pups can now be shot and hit message displayed
  -Added a new single scoreboard
  -Console based puppet demo lister (plist)
  -Menu based puppet demo lister
  -Map voting commands disabled so I could get this version out. Needs complete rewrite.
V1.19	(June 13, 2003)
  -Fixed crash with grenades hitting markers... again!
  -Added grenades to weaponban server variable
  -Added jmodmaplist.txt for listing maps in .pak files for map voting
  -Added jrecord command
  -Added jstop command
V1.18	(June 6, 2003)
  -Fixed crash when clearing puppet demo while recording
  -Fixed crash when attempting to record a puppet demo while spectator
  -Coming out of spectator mode doesn't result in a respawn anymore
  -Removed various unneeded game variables
  -Map voting
  -votemap command
  -mapvoting cvar
  -mapvotingtime cvar
  -mapvotingpercent cvar
  -killvote cvar
  -viewvotes command
  -Updated playerlistinfo message "(spectating somebody)" to "(spectating <name>)"
  -Removed debug message in spec mode
  -Players with ping of >999 now turn red
  -maxpuppetdemolength cvar
  -pdinfo command
  -ps+ command
  -ps- command
  -pspause command
  -psstep+ command
  -psstep- command
V1.17	(June 2, 2003)
  -Fixed crash when just typing in "tog" or "toggle"
  -Fixed crash when being hurt by target_splash entities
  -Puppet Demos
  -precord - start recording a puppet demo
  -pstop - stop recording a puppet demo
  -pclear - clear your puppet demo
  -pplay - play your puppet demo
  -psave <filename> - saves your puppet demo to <filename>
  -pload <filename> - loads a puppet demo file <filename> if exists
  -Updated puppet menu with puppet demo submenu
  -Bad characters in save filenames now checked for and stopped
V1.16	(March 29, 2003)
  -Fixed Trail Cleaning menu back item not going to correct item of previous menu
  -Fixed Trail Deleting menu back item not going to correct item of previous menu
  -Resized front menu bar for only 2 extra minus signs after my name
  -Fixed creating multiple puppets via spectator mode
  -Fixed puppet's going through shooting animation at unnecessary times
  -Cleaned up some puppet code
  -Fixed bug with multitrailing code
  -Sped up some trail functions
  -Added tog puppetsight command
  -Added tog puppetshootarea command
  -Ducking is now shown in trails
  -Changed the trail delete menu binds to what their real binds are
  -Fixed crash with trailcolor command
  -Added puppetshootarea <area> command
  -Added tog vest command
V1.15	(March 25, 2003)
  -Fixed crash when saving to a jloc or jtrail file when the jloc or jtrail dir doesn't exist again
  -Fixed crash when shooting shootable objects again (such as secret doors)
  -Fixed being shot by sniper rifle when shootable is off
  -Fixed being slashed by knife when shootable is off
  -Fixed puppet from disappearing when squashed by stuff
  -Goto pointing trail start (goto trail start)
  -Goto pointing trail end   (goto trail end)
  -Can toggle puppet shooting (tog puppetshoot)
  -Fixed player's puppet being left behind when they leave the server
  -Change a trail's color via "trailcolor change <color>"
  -Fixed major system lag (had to do with player interaction code (solid/non-solid))
V1.14	(February 8, 2003)
  -Updated toggle option listing for "tog" command with no parameters
  -Added "IR" to "%I" item listing
  -Marker text is now off by default
  -Added "last" to "jtrailsave" options - Saves only the last trail YOU created
  -direct color change of markers ("blah" is not a valid color (list colors))
  -direct color change of trails ("blah" is not a valid color (list colors))
  -Fixed jtrailload loading past the jtraillimit
  -Mulittrailing!
  -removed a debug print in spectator mode
  -redid or added the following commands: cleanalltrails, cleanlasttrail, cleanpointtrail,
   cleanviewtrails, cleantouchtrail, cleanviewlinks, rempointtrail, remviewtrails,
   remtouchtrail, remalltrails, remlasttrail, remviewlinks
  -fixed jtrail saving not saving unique trail numbers as I wanted
  -updated trail menu for new commands
  -added puppet transparency toggle
V1.13	(January 26, 2003)
  -Added trailing
  -Fixed puppet shooting slower than should for dual pistols
  -Added "tog" command
  -Cleaned up some more of the AQ2 code
  -Redid menus
  -Changed "remall" command to "remviewmarkers"
  -Changed "removeallmarkers" command to "remallmarkers"
  -Added "trailcount" command (total number of vertices of all trails)
  -Added "remalltrails" command
  -Fixed "rhs" command
  -Added "cleanlevel" command to remove all markers/knives/trails on the map
  -Trail colors (red,green,blue,yellow,white,orange,black)
  -Added "trailwidth" variable
  -Fixed "Flashes" misspelling
  -Added Trail Flashes
  -Added trailflashes toggle command
  -Added "remviewtrails" command
  -Added "remalltrails" command
  -Fixed jloc save bug preventing saves
  -jtrail files
  -jtrailsave all/view
  -jtrailload
  -Shootable is now a toggle instead of being on only when teamjumping
  -Added trailing to the weaponban (value 512)
  -Prevented all marker use when marker weaponban is in effect
  -Cleaned up this help file a little
V1.12	(January 18, 2003)
  -Fixed the 2002 dates of the past 3 updates below
  -Fixed markers disappearing while flying through the air
  -Added removeallmarkers command
  -Added loading jump markers from jloc files
  -Added jloc saving (markers in view or all markers)
  -Added marker text
  -Added toggle markertext
  -Made the 14 player toggle vars into 1
  -Redid the output of playerlistinfo and added new toggles
  -Marker naming via "namemarker" command
V1.11	(January 15, 2003)
  -Fixed spectator mode once again!
  -When spectating someone, you see their top right hud stats
V1.10	(January 14, 2003)
  -Fixed weaponban example in this readme
  -Took out that annoying "SOME_ROTATING_DOOR at X Y Z with no distance set" message
  -Put shootable doors/buttons back in since I got around the bugs
  -Fixed the ID Display showing your own name when directly on someone
  -Added IR toggle to the toggle command and toggle menu (it was already done, but forgot about it)
  -Added "flashes:#" to playerlistinfo display (1 on, 0 off)
  -People with teamjump off don't get stuck in other people who have teamjump on (so annoying!)
  -Mark Jumps toggle mode
  -Added "markjumps:#" to playerlistinfo display (1 on, 0 off)
V1.09	(January 5, 2003)
  -Fixed a client side gravity bug when a level's gravity isnt 800 (in worldspawn)
  -Added the "gravity default" command
  -Removed a ton of unused code and cleaned up a lot of my own
  -Spawnp now uses the goto command instead of a respawn
  -Fixed weapon going to pistol when coming out of spectator mode
  -Fixed teleport location getting deleted when coming out of spectator mode
  -Fixed yet another AQ2 bug, the knife hitting air but sounding like hit wall, bug
  -Removed puppetc, puppetf, and puppetr commands
  -Added puppet command (cycles create/remove)
  -Puppets now finalize by themselves when noone is in them
  -Puppets now show up on ID with the owner's name and skin name
  -Added puppet shooting
  -Added walkable slope identifier to range finder
  -Added markerflashes to the toggle commands
  -Fixed crash when a grenade hits a marker
  -Fixed crash with trigger_hurt entities with spawnflag including 8
  -Puppets have better vwep support
  -Added remall command
  -Added markerlimit server setting
  -Added toggle command displaying toggle list if nothing is specified
  -Menus have changed a bit
V1.08   (December 31, 2002)
  -Added the demomode command
  -Added the goto command
  -Added the gravity command
  -Fixed jumping on teleport recall - TRULY!!!
  -Added %G
  -Playerlistinfo update
V1.07   (September 2, 2002)
  -Removed a lot of unused/unneeded code and player flags
  -Noclip players now turn green
  -Fixed scoreboard, player names, skins, id, spectator
  -Puppets are now not solid to other players until it is finalized
  -Puppets dont hold a quak2 shotgun anymore
  -Added puppet state to playerlistinfo
  -LCA sound isnt broadcasted to all players, but you can still hear a LCA from a
     person if they are close by.
  -Fixed players always spawning sparks on non vest hits
  -Spectators cant spawn puppets
v1.06   (August 31, 2002)
  -Fixed player jumping on teleport if a jump was performed right before the teleport
  -Add the puppet bind commands to the puppet menu
  -Fixed reset highspeed command. It is now "rhs"
V1.05   (August 30, 2002)
  -Added puppets (commands: puppetc, puppetf, puppetr)
  -Changed menu system layout (Now 3 menus instead of 1)
  -Fixed auto teamjump mode when coming out of spectator mode
  -Modified the MOTD
  -Removed foot steps on a Lights Camera Action! run
  -Can't enter teamplay mode anymore
V1.04   (August 22, 2002)
  -Added Random Spawn Command
  -Removed doors that you had to shoot to open (fixed the bugs)
V1.03   (August 21, 2002)
  -Added "LIGHTS, CAMERA, ACTION!" ability
V1.02   (June 13, 2002)
  -Fixed trigger_hurt brushes crashing the server upon player touch.
V1.01   (June 12, 2002)
  -Fixed laser bug while changing weapons (would spawn at worldspawn (map origin) and
	move to where the player is aiming) (This is an unfixed AQ2 bug!)
  -Menu item 1 is now selected as default (was 2nd)
  -Added weapon banning
  -Added %C,%D,%H,%I,%J,%L,%M,%N,%P,%S,%T,%W
  -Made the jump marker weapon view use the knife view model
V1.00   (June 11, 2002)
  -First release to the public

----------------------------------------------------------------------------------------
12. Questions/Comments/Bug Reporting
----------------------------------------------------------------------------------------
Direct any of these to PaTMaN at:
	patman_q@hotmail.com (include "JMod" in the subject!!! (without the quotes))
	Channel #=SC= on irc.enterthegame.com

----------------------------------------------------------------------------------------
13. Thanks
----------------------------------------------------------------------------------------

SoD - Bug reports, ideas, server
Kickboy - Bug reports
KnightWolf - Bug reports
Haudrauf - Bug reports, ideas
Janner - Ideas
AssKick3r - Ideas
Caracol - Bringing me back to make V1.21, ideas
Uncle- - Bug reports, ideas
Yukmouth - Testing server, bug reports, ideas
Skie - Testing server, bug reports
Teferi - Testing server, bug reports
Option - Testing
Jimmy2003 - Public Server
Uvi - Public Server
Anyone who has come in a server and just said "PaTMaN, your ^J mod rocks!" :)
Support from the AQ2 community & www.AQ2World.com
