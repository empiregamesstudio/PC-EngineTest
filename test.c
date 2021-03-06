#include "huc.h"
/*I am follwing code tutorial from http://obeybrew.com */

/* defines make life easier and code more readable... */
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define SCR_W 33
#define SCR_H 32
#define CHAR_LEFT_THRESHOLD 96
#define CHAR_RIGHT_THRESHOLD 128

/* our new bane sprites! */
#incspr(bane,"banesprites.pcx",0,0,2,18);
#incpal(banepal,"banesprites.pcx");

/* Welcome to bane's world! */
#incbin(levelmap,"tut7map.fmp");
#incchr_ex(leveltiles,"tut7pal0.pcx",0,0,20,7,0,\
                      "tut7pal1.pcx",0,0,20,2,1,\
                      "tut7pal1.pcx",0,16,11,1,1);
#incpal(levelpal0,"tut7pal0.pcx");
#incpal(levelpal1,"tut7pal1.pcx");

/* Globals ftw */
int banex, baney, j1, j2, mapx, mapy, vmapx, vmapy, lastmapx, lastmapy, t3, jumpspeed, jumpdelta;
int MAP_X_THRESHOLD,MAP_X_LOW_THRESHOLD;
char tics, frame, state, direction;

main()
{
  /* set up bane! */
  init_satb();
  tics = 0;
  frame = 0;
  banex = 104;
  baney = 153;
  spr_make(0,banex,baney,0x5100,FLIP_MAS|SIZE_MAS,NO_FLIP|SZ_32x32,0,1);
  load_palette(16,banepal,1);
  set_color(256,0);
  load_vram(0x5000,bane,0x900);
  satb_update();
  /* set up bane's world! */
  MAP_X_THRESHOLD = 1055;
  MAP_X_LOW_THRESHOLD = 0;
  set_map_data(levelmap,164,28);
  set_tile_data(leveltiles);
  load_tile(0x1000);
  load_map(0,0,0,0,34,28);
  load_palette(0,levelpal0,1);
  load_palette(1,levelpal1,1);
  /* stuff for handling the map scrollie thingo */
  mapx = 0;
  lastmapx = 0;
  mapy = 0;
  lastmapy = 0;
  vmapx = 0;
  vmapy = 0;
  /* main game loop! */
  for(;;)
  {
    j1 = joy(0);
	j2 = joytrg(0);
	if (j2 & JOY_STRT)
	{
	  pause();
	} else {
	  bane_state_machine();
	}
	satb_update();
	vsync();
  }
}

spr_make(spriteno,spritex,spritey,spritepattern,ctrl1,ctrl2,sprpal,sprpri)
int spriteno,spritex,spritey,spritepattern,ctrl1,ctrl2,sprpal,sprpri;
{
  spr_set(spriteno);
  spr_x(spritex);
  spr_y(spritey);
  spr_pattern(spritepattern);
  spr_ctrl(ctrl1,ctrl2);
  spr_pal(sprpal);
  spr_pri(sprpri);
}

pause()
{
  for(;;)
  {
	vsync();
    if (joytrg(0) & JOY_STRT) return;
  }
}

bane_state_machine()
{
/*
  bane's State Machine:
  0 - Idle (all buttons available)
  1 - Walking (all buttons available)
  2 - About to jump (all buttons disabled)
  3 - Jumping (directions available only)
  4 - Jumping down (directions available only)
  5 - Landing (all buttons disabled)
  6 - attacking (all buttons disabled)
*/
  if (state == 0) bane_state_0();
  if (state == 1) bane_state_1();
  if (state == 2) bane_state_2();
  if (state == 3) bane_state_3();
  if (state == 4) bane_state_4();
  if (state == 5) bane_state_5();
  if (state == 6) bane_state_6();
}

bane_state_0()
{
  /* this is bane's idle stance... if the tics are equal to 0, set his frame to default and his tic counter to 1 */
  if (tics == 0) spr_pattern(0x5100);
  tics = 1;
  /* most other states can be set in this state */
  if (j1 & JOY_LEFT)
  {
    /* start bane moving to the left! */
	spr_ctrl(FLIP_MAS,FLIP_X);
	tics = 0;
	frame = 0;
	state = 1;
	spr_pattern(0x5000);
	direction = DIR_LEFT;
  }
  if (j1 & JOY_RGHT)
  {
    /* start bane moving to the right! */
	spr_ctrl(FLIP_MAS,NO_FLIP);
	tics = 0;
	frame = 0;
	state = 1;
	spr_pattern(0x5000);
	direction = DIR_RIGHT;
  }
  if(j2 & JOY_B)
  {
    /* set bane in attacking state! */
    tics = 0;
	frame = 0;
	state = 6;
	spr_pattern(0x5700);
  }
  if(j2 & JOY_A)
  {
    /* make bane jump! */
    tics = 0;
	frame = 0;
	state = 2;
	spr_pattern(0x5400);
  }
}

bane_state_1()
{
  /* bane's walking state! */
  char stillwalking;
  stillwalking = 0;
  tics++;
  if (tics > 4)
  {
	tics = 0;
	frame++;
	if (frame > 5) frame = 0;
	if (frame == 0) spr_pattern(0x5100);
	if (frame == 1) spr_pattern(0x5200);
	if (frame == 2) spr_pattern(0x5300);
	if (frame == 3) spr_pattern(0x5200);
	if (frame == 4) spr_pattern(0x5100);
	if (frame == 5) spr_pattern(0x5000);
  }
  if (j1 & JOY_LEFT)
  {
    stillwalking = 1;
  }
  if(j1 & JOY_RGHT)
  {
    stillwalking = 1;
  }
  if(j2 & JOY_B)
  {
    /* set bane in baneing state! */
    tics = 0;
	frame = 0;
	state = 6;
	spr_pattern(0x5700);
  }
  if(j2 & JOY_A)
  {
    /* make bane jump! */
    tics = 0;
	frame = 0;
	state = 2;
	spr_pattern(0x5400);
  }

  if (stillwalking == 0)
  {
    state = 0;
	tics = 0;
	frame = 0;
  } else {
    move_in_world();
  }
}

bane_state_2()
{
  /* bane's pre-jump prep state! all controls are disabled in this one */
  tics++;
  if (tics > 4)
  {
    jumpspeed = 4;
	jumpdelta = 70;
	tics = 0; /* tics is going to be used in a different way in state 3... you'll see! */
	spr_pattern(0x5500);
	state = 3;	
  }
}

bane_state_3()
{
  /* bane's jumping up state! */
  baney-=jumpspeed;
  /* here, we're going to use tics to determine how high bane has jumped, and compare it to the jump delta */
  tics+=jumpspeed;
  if (tics > jumpdelta)
  {
    /* if the total jump distance exceeds the jump delta, reduce the upward motion until it reaches zero */
	/* the reduction in upward motion makes the jump appear a little more natural than just immediately changing directions */
    spr_pattern(0x5600);
	jumpspeed--;
	if (jumpspeed<0)
	{
	  /* when the jump speed decreases past 0, we will need to set bane in state 4 */
	  tics = 0;
	  state = 4;
	  jumpspeed = 1;
	}
  }
  if (j1 & JOY_LEFT)
  {
	spr_ctrl(FLIP_MAS,FLIP_X);
	direction = DIR_LEFT;
	move_in_world();
  }
  if (j1 & JOY_RGHT)
  {
	spr_ctrl(FLIP_MAS,NO_FLIP);
	direction = DIR_RIGHT;
	move_in_world();
  }
  spr_y(baney);
}

bane_state_4()
{
  /* bane's falling down state! */
  baney+=jumpspeed;
  if (jumpspeed < 4) jumpspeed++;
  if (baney > 152)
  {
    baney = 153;
	frame = 0;
	tics = 0;
	state = 5;
	spr_pattern(0x5400);
  }
  if (j1 & JOY_LEFT)
  {
	spr_ctrl(FLIP_MAS,FLIP_X);
	direction = DIR_LEFT;
	move_in_world();
  }
  if (j1 & JOY_RGHT)
  {
	spr_ctrl(FLIP_MAS,NO_FLIP);
	direction = DIR_RIGHT;
	move_in_world();
  }
  spr_y(baney);
}

bane_state_5()
{
  /* bane's landing state; same frame as state 2, but reverts to state 0 when finished rather than state 3 */
  tics++;
  if (tics > 4)
  {
    tics = 0;
	state = 0;
	frame = 0;
  }
}

bane_state_6()
{
  /* bane's attacking state! all controls are disabled in this one; this is just an animation handler/state controller */
  tics++;
  if (tics == 5)
  {
    spr_pattern(0x5800);
	/* we need to adjust bane's position, as this sprite is a few pixels to the left normally... this is why we need 'direction'! */
	if (direction == DIR_LEFT)
	{
	  spr_x(banex-8);
	} else {
	  spr_x(banex+8);
	}
  }
  if (tics > 10)
  {
    /* as this animation ends, restore the sprite to its normal position and set the state back to 0 */
    spr_x(banex);
	tics = 0;
	state = 0;
  }
}

move_in_world()
{
	char map_scroll_dir;
	map_scroll_dir = 0;
	/*
		how this should work:
			-if character is at a direction threshold but not at the map threshold, scroll the map
			-move character's sprite otherwise
	*/
	/* are we moving left? */
	if (direction == DIR_LEFT)
	{
		/* check for map threshold */
		if (vmapx > MAP_X_LOW_THRESHOLD)
		{
			/* map threshold has not been reached, so check for screen threshold */
			if (banex < CHAR_LEFT_THRESHOLD)
			{
				/* we're at the screen threshold, so we'll be scrolling the map now! */
				map_scroll_dir = DIR_LEFT;
			} else {
				/* just move */
				banex--;
			}
		} else {
			/* we're at the map threshold...stop scrolling */
			banex--;
		}
	}
	/* are we moving right? */
	if (direction == DIR_RIGHT)
	{
		/* check for map threshold */
		if (vmapx < MAP_X_THRESHOLD)
		{
			/* map threshold has not been reached, so check for screen threshold */
			if (banex > CHAR_RIGHT_THRESHOLD)
			{
				/* we're at the screen threshold, so we'll be scrolling the map now! */
				map_scroll_dir = DIR_RIGHT;
			} else {
				/* just move */
				banex++;
			}
		} else {
			/* we're at the map threshold...stop scrolling */
			banex++;
		}
	}
	spr_x(banex);
	move_map(map_scroll_dir);
}

move_map(whichway)
char whichway;
{
  if (whichway == 0) return;
  if (whichway == DIR_RIGHT) vmapx++;
  if (whichway == DIR_LEFT) vmapx--;

  mapx = vmapx >> 3;
  mapy = vmapy >> 3;
  
  if (whichway == DIR_RIGHT)
  {
      if ((lastmapx != mapx) || (lastmapy != mapy))
      {
        t3 = (mapx+32) & 0xFF;
        load_map(t3,mapy,t3,mapy,1,SCR_H);
      }
  }
  if (whichway == DIR_LEFT)
  {
      if ((lastmapx != mapx) || (lastmapy != mapy))
        load_map(mapx,mapy,mapx,mapy,1,SCR_H);
  }
  scroll(0,vmapx,vmapy,0,223,0xC0);
  lastmapx = mapx;
  lastmapy = mapy;
}
