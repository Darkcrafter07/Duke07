// PORTALS.C --------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment
Copyright (C) 2026 - Darkcrafter07

This file is part of Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#include "duke3d.h"

extern long tempsectorz[MAXSECTORS];
extern long tempsectorpicnum[MAXSECTORS];
extern short FOFTILE, FOFTILEX, FOFTILEY;

// to be externed in engine.c or eng386.c drawalls
long portalzoomadj;
char rendermodeportal;







// ================ TRIGGER PORTAL DRAWING FUNCTIONS - START  =====================

// based on code from "actors.c"

// Trimmed-down movestandables but dedicated
// for drawing portals like viewscreen actors
void movestandablesportal0(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[120]; // movestandablesportal0 function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL0)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite0 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

// we need to duplicate the portal drawing functions "movestandablesportal#x"
// with different status IDs, otherwise, when we define
// buttons in game.c like below:
// if( PN != SPEAKER && PN != LETTER && PN != DUCK && PN != TARGET
//  && PN != TRIPBOMB && PN != VIEWSCREEN && PN != VIEWSCREEN2 && (CS&48) )
// if( PN != PORTAL0 && PN != PORTAL1) // - 1st PORTAL0 gonna show PORTAL1 contents
void movestandablesportal1(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[121]; // movestandablesportal1 function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL1)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite1 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal2(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[122]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL2)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite2 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal3(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[123]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL3)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite3 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal4(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[124]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL4)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite4 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal5(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[125]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL5)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite5 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal6(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[126]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL6)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite6 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal7(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[127]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL7)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite7 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal8(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[128]; // this function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        // Update bpos for portal sprite itself
        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL8)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }

            // Keep portal sprite global index active for the renderer
            portalsprite8 = i;
            
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal9(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[129]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL9)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite9 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal10(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[130]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL10)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite10 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal11(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[131]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL11)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite11 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal12(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[132]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL12)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite12 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal13(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[133]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL13)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite13 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal14(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[134]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL14)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite14 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal15(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[135]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL15)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite15 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal16(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[136]; // function ID
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL16)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite16 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal17(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[137]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL17)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite17 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal18(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[138]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL18)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite18 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal19(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[139]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL19)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite19 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal20(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[140]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL20)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite20 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal21(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[141]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL21)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite21 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal22(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[142]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL22)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite22 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal23(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[143]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL23)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite23 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal24(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[144]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL24)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite24 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal25(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[145]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL25)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite25 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal26(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[146]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL26)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite26 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal27(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[147]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL27)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite27 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal28(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[148]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL28)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite28 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal29(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[149]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL29)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite29 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal30(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[150]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL30)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite30 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

void movestandablesportal31(void)
{
    short i, nexti;
    spritetype *s;

    i = headspritestat[151]; 
    while(i >= 0)
    {
        nexti = nextspritestat[i];
        s = &sprite[i];

        if( s->sectnum < 0 ) { KILLIT(i); i = nexti; continue; }

        hittype[i].bposx = s->x;
        hittype[i].bposy = s->y;
        hittype[i].bposz = s->z;

        if(s->picnum == PORTAL31)
        {
            if(s->xrepeat == 0) { KILLIT(i); i = nexti; continue; }
            portalsprite31 = i;
            goto BOLT;
        }
        BOLT:
        i = nexti;
    }
}

// ================ TRIGGER PORTAL DRAWING FUNCTIONS - FINISH  =====================







// ================ SETUP VIEW INSIDE PORTALS - START  ============================

void drawroomsportal(long ox, long oy, long p_z, short p_ang, long ohoriz,
                   short osect, spritetype *src_prtl, spritetype *dst_cam)
{
    long dx, dy, ratio, distp, angdiff, fdx, fdy, player_deviation;
    long local_x, local_y, tx, ty, final_x, final_y;
    long maxparaldist, slowed_updown;
    short slowed_turnlr, final_sect;

    // FOV Variables for Zooming logic
    long old_viewingrange = viewingrange;
    long actual_dist, over_dist = 0;

    // ========================================================================
    // DESIGN PRINCIPLES OF UNIVERSAL CAMERA XY MOVEMENT INSIDE PORTALS:
    // DO: Always calculate movement in LOCAL coordinate space of the sprite.
    // DO: Use 'angdiff' for relative view synchronization.
    // DON'T: Use global dx/dy for parallax (it breaks on non-parallel walls).
    // DON'T: Hardcode Picnums for axis inversion (use local rotation instead).
    // DRAWROOMSPORTAL only moves X and Y of the camera, all the rest happen
    //           inside SE40_DrawPortal#x because it's the only way it works.
    // ========================================================================

    // 1. Calculate player vector relative to the entrance portal
    dx = ox - src_prtl->x; dy = oy - src_prtl->y;
    
    // DO: Rotate the vector into the portal's LOCAL system.
    // This "Aligns the Grid to Wall", making the math independent of map rotation.
    // local_x becomes pure strafe, local_y becomes pure depth.
    rotatepoint(0L, 0L, dx, dy, (short)(-(src_prtl->ang)), &local_x, &local_y);
    
    // Absolute distance for the Ratio gradient (must be positive)
    distp = klabs(local_y);
    maxparaldist = 1024;

    // 2. Parallax Scaling (the closer to portal the faster is XY parallax)
    if (distp < 384) ratio = 9000;
    else if (distp < 8092) ratio = 9000 - scale(8000, distp - 384, 8092);
    else ratio = 2000;

    if (ratio > 0)
    {
        // DO: Apply Negative scaling for the "Mirror/Window" effect.
        // DON'T: Use positive local_y, or the image will move AWAY when you approach.
        // Mirroring local_x (-) makes strafing natural (view shifts opposite to move).
        // Mirroring local_y (-) pushes the camera BACK as you approach (creating Zoom).
        fdx = -local_x; fdy = -local_y;
    
        // 3. Translation to Exit Point
        // DO: Maintain a 180-degree flip (+1024) to face OUT of the exit portal.
        angdiff = (dst_cam->ang - src_prtl->ang + 1024) & 2047;
    
        // DO: Project local fdx/fdy onto the DESTINATION portal's angle.
        // This handles the "10-degree wall" compensation automatically.
        rotatepoint(0L, 0L, fdx, fdy, (short)(dst_cam->ang), &tx, &ty);
            
        // 4. Collision Safety & FOV Zoom Emulation
        // DON'T: Let the camera leave the sector (prevents HOM/Black screen glitches).
        actual_dist = klabs(tx) + klabs(ty);
        
        if (actual_dist > maxparaldist)
        {
            // Lock camera position at the limit
            final_x = dst_cam->x + scale(tx, maxparaldist, actual_dist);
            final_y = dst_cam->y + scale(ty, maxparaldist, actual_dist);

            // FOV: Zoom-out as you step away from the portal (beyond maxparaldist).
            over_dist = (actual_dist - maxparaldist) << 2;
            over_dist = over_dist + (over_dist >> 2); // speedup slightly
            
            // Adjusting viewingrange for the walls/floors zoom effect
            viewingrange = old_viewingrange + over_dist; 
            
            if (viewingrange < 65535) viewingrange = 65535; // Safety cap
        }
        else
        {
            // Standard parallax movement when within safe distance
            final_x = dst_cam->x + tx; final_y = dst_cam->y + ty;
            viewingrange = old_viewingrange;
        }
    }
    else
    {
        // Safe fallback for far distances
        final_x = dst_cam->x; final_y = dst_cam->y;
        angdiff = (dst_cam->ang - src_prtl->ang + 1024) & 2047;
    }
    
    // 5. Build Engine Synchronization
    // DO: Call updatesector before drawrooms to prevent rendering the "Void".
    final_sect = dst_cam->sectnum;
    updatesector(final_x, final_y, &final_sect);
    if (final_sect < 0) final_sect = dst_cam->sectnum;

    // 6. Convert angdiff to values from -1024 to 1024
    if (angdiff > 1024) angdiff -= 2048;

    // --- FIX: CALCULATE RELATIVE PLAYER LOOK ---
    // player_deviation is how much you are looking away from the entrance portal angle
    player_deviation = (p_ang - src_prtl->ang);

    // Final angle: Base Exit Angle + 180 flip + look deviation
    slowed_turnlr = (short)(dst_cam->ang + 1024 + (player_deviation)); 
    
    // Slower vertical look
    slowed_updown = 100 + ((ohoriz - 100) >> 3);            
    
    // 7. WIDE FOV INJECTION FOR 256x256 TILES
    // We expand viewingrange BEFORE rendering to capture more of the scene.
    // This provides "overflow" pixels for drawsprite to prevent tiling at edges.
    // Factor: current viewingrange - 12.5% (adjustable)
    viewingrange = viewingrange - (viewingrange >> 4);
    
    // Update sky fix variable to include both: portal zoom and wide-view expansion
    portalzoomadj = viewingrange - old_viewingrange;

    dosetaspect(); // Apply all FOV/Aspect changes to projection tables

    // DO: Apply angdiff to p_ang for proper head rotation synchronization.
    // DON'T: Modify p_ang with hardcoded constants like 512 or 1536 here.
    rendermodeportal = 1; // Enable sky fix before rendering
    drawrooms(final_x, final_y, p_z, (short)(slowed_turnlr & 2047), slowed_updown, final_sect);
    rendermodeportal = 0; // Disable fix after rendering

    // Restore original FOV for the player's main view to prevent screen distortion
    if (viewingrange != old_viewingrange)
    {
        viewingrange = old_viewingrange;
        dosetaspect();
    }
}

// ================ SETUP VIEW INSIDE PORTALS - FINISH  ============================







// *************************************************************************************
// *************************************************************************************
// *************************************************************************************
// =====================================================================================
// =============  INDIVIDUAL PRTL DRAWING FUNCTIONS - START  =========================
// =====================================================================================

// ===========================================================================
// PORTAL ADAPTIVE NORMALIZATION (The Final Prtl Sprite Size Fix):
//
// WHY: In Build Engine, sprite size is linked to Tile Pixels and Screen X/Y.
// When we upscale portals to 384/512px for high-res modes, they expand
// physically, breaking the map's geometry and frame alignment.
//
// HOW: This function standardizes any Portal Sprite back to the original
// 192px "MapEdit Golden Reference". It calculates a complex ratio that
// counteracts both Tile Growth and Engine Resolution Scaling.
//
// DO:
// 1. DO use this only ONCE per sprite spawn or resolution change (T12 flag).
// 2. DO keep 16.16 fixed-point math for precision on 486SX processors.
// 3. DO adjust X/Y multipliers separately; Build scales axis differently.
//
// DON'T:
// 1. DON'T call this every frame; it wastes cycles and causes jitter.
// 2. DON'T let repeats drop to 0, or the portal becomes an invisible void.
// 3. DON'T use integer division without scale, or you'll lose 30% accuracy.

void SE40_NormalizePortalSize(short spnum)
{
    long target_w, target_h, ratio_x, ratio_y;
    long new_xr, new_yr;
    
    // 1. Get current physical tile dimensions (384 or 512)
    target_w = (long)tilesizx[sprite[spnum].picnum];
    target_h = (long)tilesizy[sprite[spnum].picnum];
    
    // 2. TILE SCALE RATIO (Fixed point 16.16)
    ratio_x = divscale16(target_w, 192);
    ratio_y = divscale16(target_h, 192);

    // 3. RESOLUTION COMPENSATION (Duke07 Surgical Precision)
    if (ScreenWidth >= 640 && ScreenWidth < 800) 
    {
        // 640x400: Stable and calibrated
        ratio_x = mulscale16(ratio_x, 70500L); 
        ratio_y = mulscale16(ratio_y, 62650L); 
    }
    else if (ScreenWidth >= 800) 
    {
        // 800x600: Further shrinking
        // Width (X): +3% to ratio (from 57300 to 59019)
        // Height (Y): +7% to ratio (from 55330 to 59203)
        ratio_x = mulscale16(ratio_x, 59019L); 
        ratio_y = mulscale16(ratio_y, 59203L);
    }

    // 4. APPLY INDEPENDENT SCALING
    new_xr = divscale16((long)sprite[spnum].xrepeat << 16, ratio_x) >> 16;
    new_yr = divscale16((long)sprite[spnum].yrepeat << 16, ratio_y) >> 16;
    
    // Safety clamp (1-255)
    if (new_xr < 1) new_xr = 1;
    if (new_yr < 1) new_yr = 1;
    if (new_xr > 255) new_xr = 255;
    if (new_yr > 255) new_yr = 255;

    sprite[spnum].xrepeat = (unsigned char)new_xr;
    sprite[spnum].yrepeat = (unsigned char)new_yr;
}

// --------------------- Draw PORTAL0 - START  -----------------------------------
void SE40_DrawPortal0(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL0)
    {
        target_w = (long)tilesizx[PORTAL0];
        target_h = (long)tilesizy[PORTAL0];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL0, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL0); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL0, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL0); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal0(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[120];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL0)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal0(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal0(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal0(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL0 - FINISH  -----------------------------------



// --------------------- Draw PORTAL1 - START  -----------------------------------
void SE40_DrawPortal1(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL1)
    {
        target_w = (long)tilesizx[PORTAL1];
        target_h = (long)tilesizy[PORTAL1];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL1, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL1); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL1, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL1); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal1(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[121];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL1)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal1(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal1(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal1(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL1 - FINISH  -----------------------------------



// --------------------- Draw PORTAL2 - START  -----------------------------------
void SE40_DrawPortal2(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL2)
    {
        target_w = (long)tilesizx[PORTAL2];
        target_h = (long)tilesizy[PORTAL2];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL2, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL2); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL2, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL2); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal2(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[122];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL2)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal2(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal2(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal2(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL2 - FINISH  -----------------------------------



// --------------------- Draw PORTAL3 - START  -----------------------------------
void SE40_DrawPortal3(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL3)
    {
        target_w = (long)tilesizx[PORTAL3];
        target_h = (long)tilesizy[PORTAL3];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL3, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL3); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL3, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL3); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal3(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[123];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL3)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal3(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal3(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal3(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL3 - FINISH  -----------------------------------



// --------------------- Draw PORTAL4 - START  -----------------------------------
void SE40_DrawPortal4(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL4)
    {
        target_w = (long)tilesizx[PORTAL4];
        target_h = (long)tilesizy[PORTAL4];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL4, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL4); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL4, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL4); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal4(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[124];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL4)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal4(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal4(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal4(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL4 - FINISH  -----------------------------------



// --------------------- Draw PORTAL5 - START  -----------------------------------
void SE40_DrawPortal5(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL5)
    {
        target_w = (long)tilesizx[PORTAL5];
        target_h = (long)tilesizy[PORTAL5];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL5, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL5); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL5, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL5); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal5(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[125];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL5)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal5(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal5(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal5(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL5 - FINISH  -----------------------------------


// --------------------- Draw PORTAL6 - START  -----------------------------------
void SE40_DrawPortal6(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL6)
    {
        target_w = (long)tilesizx[PORTAL6];
        target_h = (long)tilesizy[PORTAL6];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL6, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL6); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL6, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL6); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal6(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[126];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL6)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal6(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal6(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal6(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL6 - FINISH  -----------------------------------


// --------------------- Draw PORTAL7 - START  -----------------------------------
void SE40_DrawPortal7(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL7)
    {
        target_w = (long)tilesizx[PORTAL7];
        target_h = (long)tilesizy[PORTAL7];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL7, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL7); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL7, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL7); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal7(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[127];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL7)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal7(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal7(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal7(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL7 - FINISH  -----------------------------------

// --------------------- Draw PORTAL8 - START  -----------------------------------
void SE40_DrawPortal8(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL8)
    {
        target_w = (long)tilesizx[PORTAL8];
        target_h = (long)tilesizy[PORTAL8];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL8, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL8); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL8, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL8); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal8(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[128];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL8)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal8(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal8(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal8(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL8 - FINISH  -----------------------------------


// --------------------- Draw PORTAL9 - START  -----------------------------------
void SE40_DrawPortal9(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL9)
    {
        target_w = (long)tilesizx[PORTAL9];
        target_h = (long)tilesizy[PORTAL9];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL9, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL9); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL9, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL9); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal9(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[129];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL9)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal9(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal9(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal9(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL9 - FINISH  -----------------------------------


// --------------------- Draw PORTAL10 - START  -----------------------------------
void SE40_DrawPortal10(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL10)
    {
        target_w = (long)tilesizx[PORTAL10];
        target_h = (long)tilesizy[PORTAL10];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL10, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL10); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL10, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL10); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal10(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[130];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL10)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal10(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal10(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal10(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL10 - FINISH  -----------------------------------


// --------------------- Draw PORTAL11 - START  -----------------------------------
void SE40_DrawPortal11(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL11)
    {
        target_w = (long)tilesizx[PORTAL11];
        target_h = (long)tilesizy[PORTAL11];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL11, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL11); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL11, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL11); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal11(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[131];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL11)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal11(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal11(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal11(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL11 - FINISH  -----------------------------------


// --------------------- Draw PORTAL12 - START  -----------------------------------
void SE40_DrawPortal12(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL12)
    {
        target_w = (long)tilesizx[PORTAL12];
        target_h = (long)tilesizy[PORTAL12];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL12, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL12); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL12, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL12); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal12(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[132];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL12)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal12(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal12(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal12(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL12 - FINISH  -----------------------------------


// --------------------- Draw PORTAL13 - START  -----------------------------------
void SE40_DrawPortal13(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL13)
    {
        target_w = (long)tilesizx[PORTAL13];
        target_h = (long)tilesizy[PORTAL13];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL13, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL13); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL13, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL13); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal13(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[133];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL13)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal13(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal13(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal13(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL13 - FINISH  -----------------------------------


// --------------------- Draw PORTAL14 - START  -----------------------------------
void SE40_DrawPortal14(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL14)
    {
        target_w = (long)tilesizx[PORTAL14];
        target_h = (long)tilesizy[PORTAL14];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL14, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL14); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL14, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL14); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal14(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[134];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL14)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal14(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal14(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal14(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL14 - FINISH  -----------------------------------

// --------------------- Draw PORTAL15 - START  -----------------------------------
void SE40_DrawPortal15(int spnum,long x,long y,long z,short a,short h,long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz, relz;
    // STATIC variables to persist between calls
    static long cam_base_z[MAXSPRITES]; 
    static short cam_init_done[MAXSPRITES];
    static short drawing_portal = 0; // RECURSION PROTECTION
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat, *pspr_ptr;
    unsigned char old_xr, old_yr;

    // Prevent portal-inside-portal recursion crash
    if (drawing_portal) return;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    if (sprite[spnum].picnum == PORTAL15)
    {
        target_w = (long)tilesizx[PORTAL15];
        target_h = (long)tilesizy[PORTAL15];

        // 1. Target: tile sizes and repeats calculation
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 2. HARD-BIND to CAMERA1 (Portal Camera) using T11 cache
        k = (short)hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = (long)k;
                    break;
                }
            }
        }

        if (k >= 0 && k < MAXSPRITES)
        {
            drawing_portal = 1; // LOCK rendering pass

            // 3. MEMORIZE original Map Z of the camera to use as base for tracking
            if (cam_init_done[k] == 0)
            {
                cam_base_z[k] = sprite[k].z;
                cam_init_done[k] = 1;
            }

            // Ensure camera sector is correct before parallax math
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // Hide all portal surfaces to avoid visual glitches in the mirror
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat |= 32768;
            }

            // --- RELATIVE Z CALCULATION ---
            // Calculate height offset relative to the entrance portal sprite
            relz = ps[screenpeek].posz - sprite[spnum].z;
            nz = cam_base_z[k] + relz;

            // Clamp nz within target room height boundaries
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            if (nz < (ceilz_at_cam + 512L)) nz = ceilz_at_cam + 512L;
            if (nz > (floorz_at_cam - 512L)) nz = floorz_at_cam - 512L;

            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL15, target_w, target_h);
            
            // --- PORTAL RENDER CALL ---
            // DO: Pass player's world position and angle.
            // DO: Pass the RELATIVE Z (nz) we just calculated.
            // drawroomsportal will handle internal XY parallax and rotation logic.
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, nz, 
                            ps[screenpeek].ang, (long)h, sprite[k].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, ps[screenpeek].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL15); 

            setviewback(); 
            camsprite = backup_camsprite;

            // Restore all portal surfaces visibility
            pspr_ptr = &portalsprite0;
            for(i=0; i<32; i++)
            {
                if (pspr_ptr[i] >= 0)
                    sprite[pspr_ptr[i]].cstat &= ~32768;
            }
            sprite[spnum].cstat = old_cstat; 
            
            drawing_portal = 0; // UNLOCK rendering pass
        }

        // 4. Restore: bring sprite parameters back for the main engine pass
        sprite[spnum].xrepeat = old_xr;
        sprite[spnum].yrepeat = old_yr;
        sprite[spnum].cstat = old_cstat;
        return;
    }

    // FOF handling (Floor Over Floor) sprites
    if(sprite[spnum].ang!=512) return;

    i = FOFTILE;    //Effect TILE
    if (!(gotpic[i>>3]&(1<<(i&7)))) return;
    gotpic[i>>3] &= ~(1<<(i&7));

    floor1=spnum;

    if(sprite[spnum].lotag==42) fofmode=40;
    if(sprite[spnum].lotag==43) fofmode=41;
    if(sprite[spnum].lotag==44) fofmode=40;
    if(sprite[spnum].lotag==45) fofmode=41;

    // fofmode=sprite[spnum].lotag-2;
    // sectnum=sprite[j].sectnum;
    // sectnum=cursectnum;
    ok++;

    // recursive?
    //for(j=0;j<MAXSPRITES;j++)
    //{
    //    if(sprite[j].sectnum==sectnum && sprite[j].picnum==1 && sprite[j].lotag==110)
    //    { DrawFloorOverFloor(j); break;}
    //}

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==fofmode && sprite[j].hitag==sprite[floor1].hitag)
        { floor1=j; fofmode=sprite[j].lotag; ok++; break;}
    }

    if(fofmode==40) k=41; else k=40;

    for(j=0;j<MAXSPRITES;j++)
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k && sprite[j].hitag==sprite[floor1].hitag)
        {floor2=j; ok++; break;}
    }

    for(j=0;j<MAXSPRITES;j++)  // raise ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].floorz;
                sector[sprite[j].sectnum].floorz+=(((z-sector[sprite[j].sectnum].floorz)/32768)+1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].floorpicnum;
                sector[sprite[j].sectnum].floorpicnum=13;
            }
            if(k==41)
            {
                tempsectorz[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingz;
                sector[sprite[j].sectnum].ceilingz+=(((z-sector[sprite[j].sectnum].ceilingz)/32768)-1)*32768;
                tempsectorpicnum[sprite[j].sectnum]=sector[sprite[j].sectnum].ceilingpicnum;
                sector[sprite[j].sectnum].ceilingpicnum=13;
            }
        }
    }

    i=floor1;
    offx=x-sprite[i].x;
    offy=y-sprite[i].y;
    i=floor2;

    // Draw FOF in the portal (if needed)
    setviewtotile(PORTAL15, xdim, ydim);
    //drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
    drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                    sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                    &sprite[spnum], &sprite[k]);
    animatesprites(x,y,a,smoothratio);
    drawmasks();
    squarerotatetile(PORTAL15); // rotate the tile contents 90 degrees right
    setviewback(); // prevents screen from shreding into horizontal scanlines

    for(j=0;j<MAXSPRITES;j++)  // restore ceiling or floor
    {
        if(sprite[j].picnum==1 && sprite[j].lotag==k+2 && sprite[j].hitag==sprite[floor1].hitag)
        {
            if(k==40)
            {
                sector[sprite[j].sectnum].floorz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].floorpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
            if(k==41)
            {
                sector[sprite[j].sectnum].ceilingz=tempsectorz[sprite[j].sectnum];
                sector[sprite[j].sectnum].ceilingpicnum=tempsectorpicnum[sprite[j].sectnum];
            }
        } // end if
    } // end for
} // end SE40

void se40codeportal15(long x, long y, long z, long a, long h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

    i = headspritestat[135];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL15)
        {
            // --- DUKE07 ONCE-PER-RELOAD NORMALIZATION ---
            // T12 (temp_data[11]): activation stats and portal-camera spin counters.
            // Using T12 as an initialization flag to scale repeats once.
            if (hittype[i].temp_data[11] == 0)
            {
                SE40_NormalizePortalSize((short)i);
                hittype[i].temp_data[11] = 1; // Mark as normalized
            }

            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 548 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
                ps[screenpeek].cursectnum, sprite[i].x, sprite[i].y, sprite[i].z, sprite[i].sectnum))
            {
                SE40_DrawPortal15(i, x, y, z, a, h, smoothratio);
            }
            // visibility determination finish
        }
        i = nextspritestat[i];
    }

    i = headspritestat[15];
    while (i >= 0)
    {
        switch (sprite[i].lotag)
        {
            // case 40:
            // case 41:
            //     SE40_DrawPortal15(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                {
                    // Ensure normalization for sector-triggered portals too
                    if (hittype[i].temp_data[11] == 0)
                    {
                        SE40_NormalizePortalSize((short)i);
                        hittype[i].temp_data[11] = 1;
                    }
                    SE40_DrawPortal15(i, x, y, z, a, h, smoothratio);
                }
                break;
        }
        i = nextspritestat[i];
    }
}

// --------------------- Draw PORTAL15 - FINISH  -----------------------------------



// =====================================================================================
// ============= INDIVIDUAL PRTL DRAWING FUNCTIONS - FINISH  =========================
// =====================================================================================
// *************************************************************************************
// *************************************************************************************
// *************************************************************************************







// ============================ PRTL TELEPORTATION CODE START ============================

// this one teleports player in prtls
// 512 is the reference max portal tile size in px
void teleportplayerstuffportal(short snum)
{
    long dxprtl, dyprtl, dzinitprtl, dzprtl, dynrangexyprtl;
    long dynrangezprtl, t512toanyx, t512toanyz, xydisthr512, zdisthr512;
    long xydisthrany, zdisthrany, dynsaferngxyprtl, tile_w, tile_h;
    long dynsaferngzprtl, relplayerz2destz, txprtldest, typrtldest;
    long floorz_src, floorz_dest, ceilz_tmp, height_above_floor;
    long sprite_half_height; // Added for symmetric centering
    short jprtl, spriteindexprtl, camindexprtl, teleportdestprtl;
    short exit_anchor, angdiffprtl2dest;
    struct player_struct *p;

    p = &ps[snum];

    spriteindexprtl = headspritesect[p->cursectnum];
    while(spriteindexprtl >= 0)
    {
        if ( sprite[spriteindexprtl].picnum >= PORTAL0 &&
             sprite[spriteindexprtl].picnum <= PORTAL31 )
        {
            dxprtl = klabs(p->posx - sprite[spriteindexprtl].x);
            dyprtl = klabs(p->posy - sprite[spriteindexprtl].y);
            
            // --- SURGICAL Z-CENTERING ---
            // 1. Half-height of the portal sprite
            sprite_half_height = (long)(sprite[spriteindexprtl].yrepeat * 
                          tilesizy[sprite[spriteindexprtl].picnum]) << 2;

            // 2. Center of portal minus half-height of the player
            // This aligns the player's mid-section with the portal's center
            dzinitprtl = p->posz - (sprite[spriteindexprtl].z - 
                                    sprite_half_height + 10000L);
            dzprtl = klabs(dzinitprtl);

            // 1. Setup portal player teleportation distance thresholds
            tile_w = (long)tilesizx[sprite[spriteindexprtl].picnum];
            tile_h = (long)tilesizy[sprite[spriteindexprtl].picnum];

            t512toanyx = divscale16(512, (tile_w + 1));
            t512toanyz = divscale16(512, (tile_h + 1));

            // Define reference thresholds
            xydisthr512 = 2048; 
            zdisthr512 = 42000; // # * 256 logic, good for raw units

            xydisthrany = divscale16(xydisthr512, t512toanyx);
            zdisthrany  = divscale16(zdisthr512, t512toanyz);

            // Final Dynamic Range
            dynrangexyprtl = (xydisthrany * sprite[spriteindexprtl].xrepeat) >> 5;
            dynrangezprtl  = (zdisthrany * sprite[spriteindexprtl].yrepeat) >> 5;

            // SAFE MARGINS: Anti-ping-pong buffer (1.5x)
            dynsaferngxyprtl = dynrangexyprtl + (dynrangexyprtl >> 1);
            dynsaferngzprtl  = dynrangezprtl + (dynrangezprtl >> 1);

            // --- COMPLETE SAFE ZONE ---
            if (p->lastprtl == spriteindexprtl)
            {
                if ((dxprtl+dyprtl)<dynsaferngxyprtl&&dzprtl<dynsaferngzprtl) 
                {
                    goto NEXTPRTL; 
                }
                else
                {
                    p->lastprtl = -1; 
                }
            }

            // Proximity Check
            if ( ((dxprtl+dyprtl)<dynrangexyprtl) && (dzprtl<dynrangezprtl) )
            {
                // 2. Camera cache (T11 / index 10)
                camindexprtl = (short)hittype[spriteindexprtl].temp_data[10]; 
                if (camindexprtl < 0 || sprite[camindexprtl].picnum != CAMERA1)
                {
                    for(jprtl=0; jprtl<MAXSPRITES; jprtl++)
                    {
                        if(sprite[jprtl].picnum == 
                           CAMERA1 && sprite[jprtl].lotag == 
                               sprite[spriteindexprtl].hitag)
                        {
                            hittype[spriteindexprtl].temp_data[10] = (long)jprtl;
                            camindexprtl = jprtl;
                            break;
                        }
                    }
                }

                // 3. Cache the exit point (T13 / index 12) - for reliability
                teleportdestprtl = (short)hittype[spriteindexprtl].temp_data[12];
                if (teleportdestprtl < 0 || 
                    sprite[teleportdestprtl].picnum != PRTLTELEPDEST)
                {
                    teleportdestprtl = -1;
                    for(jprtl=0; jprtl<MAXSPRITES; jprtl++)
                    {
                        if(sprite[jprtl].picnum == PRTLTELEPDEST && 
                           sprite[jprtl].lotag == sprite[spriteindexprtl].hitag)
                        {
                            hittype[spriteindexprtl].temp_data[12] = (long)jprtl; 
                            teleportdestprtl = jprtl;
                            break;
                        }
                    }
                }

                // 4. Silent teleportation
                if (camindexprtl >= 0)
                {
                    exit_anchor =
                      (teleportdestprtl >= 0) ? teleportdestprtl : camindexprtl;

                    angdiffprtl2dest = (sprite[exit_anchor].ang - 
                                     sprite[spriteindexprtl].ang + 1024) & 2047;

                    dxprtl = p->posx - sprite[spriteindexprtl].x;
                    dyprtl = p->posy - sprite[spriteindexprtl].y;
                    rotatepoint(0L, 0L, dxprtl, dyprtl, angdiffprtl2dest, 
                                               &txprtldest, &typrtldest);

                    // --- VERTICAL OFFSET FIX ---
                    // Calculate target Z center and apply player deviation.
                    // This ensures player exits at the same height relative to 
                    // the center of the destination portal.
                    p->posx = sprite[exit_anchor].x + txprtldest;
                    p->posy = sprite[exit_anchor].y + typrtldest;

                    p->posx += (sintable[(p->ang + 512) & 2047] >> 6);
                    p->posy += (sintable[p->ang & 2047] >> 6);

                    // Maintain relative Z to the portal sprite
                    relplayerz2destz = p->posz - sprite[spriteindexprtl].z;
                    p->posz = sprite[exit_anchor].z + relplayerz2destz;

                    p->cursectnum = sprite[exit_anchor].sectnum;
                    updatesector(p->posx, p->posy, &p->cursectnum);

                    if (p->cursectnum < 0) 
                         p->cursectnum = sprite[exit_anchor].sectnum;

                    p->ang = (p->ang + angdiffprtl2dest) & 2047;
                    p->lastprtl = exit_anchor; 
                
                    return; 
                }
            }
        }
        NEXTPRTL:
        spriteindexprtl = nextspritesect[spriteindexprtl];
    }
}

void teleportspriteportal(short s_idx, short src_prtl, short dst_prtl)
{
    long tx, ty, dx, dy, vx, vy, distp;
    long rel_z, player_horiz_delta;
    short angdiff, smoothed_angdiff;
    char Z_HEIGHT_SMOOTH, Z_VEL_SMOOTH;
    char Z_LOOK_SENSITIVITY, Z_DIST_COMPENSATION;

    // --------------------- VERTICAL REGULATORS ----------------------
    Z_HEIGHT_SMOOTH = 4;    // Shooter to portal height offset (rel_z)
    Z_VEL_SMOOTH = 5;       // Power of vertical impulse (zvel)
    Z_LOOK_SENSITIVITY = 0; // Freelook sensitivity (vertical tilt)
    Z_DIST_COMPENSATION = 11; // Shooter to portal distance compensate

    // 1. Angle & Smoothing
    angdiff = (sprite[dst_prtl].ang - sprite[src_prtl].ang + 1024) & 2047;
    if (angdiff > 1024) angdiff -= 2048;
    smoothed_angdiff = (angdiff >> 2) + (angdiff >> 4);

    // 2. Position & Distance
    dx = sprite[s_idx].x - sprite[src_prtl].x;
    dy = sprite[s_idx].y - sprite[src_prtl].y;
    distp = klabs(dx) + klabs(dy);
    if (distp < 512) distp = 512;

    rotatepoint(0L, 0L, dx, dy, (sprite[dst_prtl].ang - sprite[src_prtl].ang
                                                  + 1024) & 2047, &tx, &ty);
    sprite[s_idx].x = sprite[dst_prtl].x + tx;
    sprite[s_idx].y = sprite[dst_prtl].y + ty;

    // 3. Vertical Sync (Z)
    rel_z = sprite[s_idx].z - sprite[src_prtl].z;
    sprite[s_idx].z = sprite[dst_prtl].z + (rel_z >> Z_HEIGHT_SMOOTH);

    // 4. Trajectory Sync (Z-Velocity)
    player_horiz_delta = (ps[screenpeek].horiz - 100);
    player_horiz_delta = (player_horiz_delta << Z_LOOK_SENSITIVITY);
    player_horiz_delta = player_horiz_delta + (player_horiz_delta >> 4);
    player_horiz_delta = player_horiz_delta - (player_horiz_delta >> 2);
    player_horiz_delta = player_horiz_delta - (player_horiz_delta >> 3);

    player_horiz_delta = scale(player_horiz_delta,
                              1 << Z_DIST_COMPENSATION, distp);
    sprite[s_idx].zvel = -(player_horiz_delta << Z_VEL_SMOOTH);

    // 5. Momentum Transfer (Horiz)
    vx = sprite[s_idx].xvel; vy = sprite[s_idx].yvel;
    rotatepoint(0L, 0L, vx, vy, smoothed_angdiff, &tx, &ty);
    sprite[s_idx].xvel = tx; sprite[s_idx].yvel = ty;

    // 6. Rotation & Push-out
    sprite[s_idx].ang = (sprite[s_idx].ang + smoothed_angdiff) & 2047;
    sprite[s_idx].x += (sintable[(sprite[s_idx].ang + 512) & 2047] >> 4);
    sprite[s_idx].y += (sintable[sprite[s_idx].ang & 2047] >> 4);

    changespritesect(s_idx, sprite[dst_prtl].sectnum);
}

void adjustportalparallax(short s_idx, short dst_prtl, long strength)
{
    long dx, dy, lx, ly, max_h, ratio, expo_ratio;
    long player_deviation, pull_angle, tx, ty, vx, vy;

    // 1. HARD BYPASS: If strength is 0, do absolutely nothing.
    if (strength == 0) return;

    // 2. Anchor to the DESTINATION portal (where the sprite is now)
    dx = sprite[s_idx].x - sprite[dst_prtl].x;
    dy = sprite[s_idx].y - sprite[dst_prtl].y;
    
    // Rotate into local space of the exit portal to find offset from center axis
    rotatepoint(0L, 0L, dx, dy, (short)(-sprite[dst_prtl].ang), &lx, &ly);

    // 3. Exponential Ratio (How far from center of the portal mirror)
    max_h = (long)tilesizx[sprite[dst_prtl].picnum] * sprite[dst_prtl].xrepeat >> 1;
    if (max_h < 1024) max_h = 1024;
    
    ratio = (klabs(lx) << 10) / max_h;
    if (ratio > 1024) ratio = 1024;
    
    // Quadratic lens curve
    expo_ratio = mulscale(ratio, ratio, 10);

    // 4. Calculate Deviation
    // We check how much the projectile's angle differs from its "intended" exit path
    player_deviation = (ps[screenpeek].ang - (sprite[dst_prtl].ang + 1024));
    if (player_deviation > 1024) player_deviation -= 2048;
    if (player_deviation < -1024) player_deviation += 2048;

    // 5. Final Pull Angle
    // This is the extra "nudge" towards the crosshair
    pull_angle = mulscale(player_deviation, mulscale(strength, expo_ratio, 10), 10);

    // 6. Apply Rotation to Velocity
    // We use rotatepoint on the CURRENT velocity to avoid losing magnitude
    vx = sprite[s_idx].xvel; 
    vy = sprite[s_idx].yvel;
    rotatepoint(0L, 0L, vx, vy, (short)pull_angle, &tx, &ty);
    
    sprite[s_idx].xvel = tx;
    sprite[s_idx].yvel = ty;
    
    // Keep sprite's face aligned with new velocity
    sprite[s_idx].ang = (sprite[s_idx].ang + (short)pull_angle) & 2047;
}

// -----------------------------------------------------------------------------
// HITSCAN PORTAL LOGIC - SURGICAL ANCHOR SPLIT VERSION
// -----------------------------------------------------------------------------
// This version ensures that bullets always spawn at the visual window (CAMERA1)
// while maintaining the sector logic of the teleport destination (PRTLDEST).
// -----------------------------------------------------------------------------
void hitscanportal(long xs, long ys, long zs, short sectnum,
                   long vx, long vy, long vz, short *hitsect, 
                   short *hitwall, short *hitsprite, long *hitx, 
                   long *hity, long *hitz, unsigned long cliptype)
{
    long depth = 0;
    long nxs, nys, nzs, nvx, nvy, nvz;
    long lx, ly, lvx, lvy, tx, ty, tvx, tvy, dx, dy, rel_z;
    long dist_h, dist_v, max_h, max_v, ratio_h, ratio_v;
    long lens_h, lens_v;
    short target_exit, cam_anchor, jprtl, nsect, out_ang;
    
    // --------------------- REGULATORS ---------------------
    // STAGE 1: Foundation. Let's make it more standard.
    // H: lower - faster; V: higher - faster
    long BASE_HORIZ = 512; // Standard 1:1 ratio
    long BASE_VERT  = 512;

    // STAGE 2: Secondary Lens (The "Magnet" effect)
    // H: EDGE must be VERY LOW to force convergence at acute angles.
    // V: EDGE must be strong to counteract Build's vertical stretch.
    long H_CENTER = 512; // Perfect in the middle
    long H_MID    = 256; // Already bending halfway
    long H_EDGE   = 8;   // HARSH DROP: Forces bullet into center line
    
    long V_CENTER = 800; long V_MID = 600; long V_EDGE = 400;
    // ---------------------------------------------------------

    // 1. Initial hitscan to find the entrance portal plane
    hitscan(xs, ys, zs, sectnum, vx, vy, vz, hitsect, hitwall, hitsprite, 
                                             hitx, hity, hitz, cliptype);

    // 2. Recursive iteration through PORTAL0..31
    while (depth < 2 && *hitsprite >= 0 && (sprite[*hitsprite].picnum >= 
                       PORTAL0 && sprite[*hitsprite].picnum <= PORTAL31))
    {
        // --- DUKE07 SURGICAL CACHE SPLIT ---
        
        // 1. FORCE CAMERA1 as POSITION ANCHOR (Index 10 / T11)
        // We use the camera because it sits exactly in the portal window plane.
        cam_anchor = (short)hittype[*hitsprite].temp_data[10]; 
        if (cam_anchor < 0 || sprite[cam_anchor].picnum != CAMERA1)
        {
            cam_anchor = -1;
            for(jprtl = 0; jprtl < MAXSPRITES; jprtl++)
            {
                if (sprite[jprtl].picnum == CAMERA1 && 
                    sprite[jprtl].lotag == sprite[*hitsprite].hitag)
                {
                    cam_anchor = jprtl;
                    hittype[*hitsprite].temp_data[10] = (long)jprtl;
                    break;
                }
            }
        }

        // 2. FORCE PRTLTELEPDEST as SECTOR/ANGLE ANCHOR (Index 12 / T13)
        // We use the destination point to know which sector the bullet enters.
        target_exit = (short)hittype[*hitsprite].temp_data[12];
        if (target_exit < 0 || sprite[target_exit].picnum != PRTLTELEPDEST)
        {
            target_exit = -1;
            for(jprtl = 0; jprtl < MAXSPRITES; jprtl++)
            {
                if (sprite[jprtl].picnum == PRTLTELEPDEST && 
                    sprite[jprtl].lotag == sprite[*hitsprite].hitag)
                {
                    target_exit = jprtl;
                    hittype[*hitsprite].temp_data[12] = (long)jprtl;
                    break;
                }
            }
        }

        // Emergency Fallbacks
        if (cam_anchor < 0) break; 
        if (target_exit < 0) target_exit = cam_anchor;

        depth++;

        // --- STEP 1: LOCAL SPACE TRANSFORMATION ---
        dx = *hitx - sprite[*hitsprite].x;
        dy = *hity - sprite[*hitsprite].y;
        rel_z = *hitz - sprite[*hitsprite].z;

        rotatepoint(0L, 0L, dx, dy, (short)(-(sprite[*hitsprite].ang)), &lx, &ly);
        rotatepoint(0L, 0L, vx, vy, (short)(-(sprite[*hitsprite].ang)), &lvx, &lvy);

        // --- STEP 2: PRELIMINARY BASE CORRECTION ---
        lvx = scale(lvx, BASE_HORIZ, 1024);
        nvz = scale(vz,  BASE_VERT,  1024);

        // --- STEP 3: SECONDARY ADAPTIVE LENS (CONVERGENCE) ---
        dist_h = klabs(lx);
        max_h = (long)tilesizx[sprite[*hitsprite].picnum] * sprite[*hitsprite].xrepeat >> 2;
        if (max_h < 1) max_h = 1;
        ratio_h = (dist_h << 10) / max_h;
        if (ratio_h > 1024) ratio_h = 1024;
        if (ratio_h < 512) lens_h = H_CENTER + ((H_MID - H_CENTER) * ratio_h >> 9);
        else lens_h = H_MID + ((H_EDGE - H_MID) * (ratio_h - 512) >> 9);

        dist_v = klabs(rel_z);
        max_v = (long)tilesizy[sprite[*hitsprite].picnum] * sprite[*hitsprite].yrepeat << 2;
        if (max_v < 1) max_v = 1;
        ratio_v = (dist_v << 10) / max_v;
        if (ratio_v > 1024) ratio_v = 1024;
        if (ratio_v < 512) lens_v = V_CENTER + ((V_MID - V_CENTER) * ratio_v >> 9);
        else lens_v = V_MID + ((V_EDGE - V_MID) * (ratio_v - 512) >> 9);

        lvx = scale(lvx, lens_h, 512);
        nvz = scale(nvz, lens_v, 512);

        // --- STEP 4: RE-PROJECT TO WORLD SPACE (ANCHORED TO CAMERA) ---
        // Mirror angle relative to the window plane (cam_anchor)
        out_ang = (sprite[cam_anchor].ang + 1024) & 2047;
        
        if (sprite[*hitsprite].xrepeat > 0)
            lx = scale(lx, sprite[cam_anchor].xrepeat, sprite[*hitsprite].xrepeat);

        // Calculate world coordinates relative to CAMERA (the window surface)
        rotatepoint(0L, 0L, -lx, -ly, (short)(sprite[cam_anchor].ang), &tx, &ty);
        rotatepoint(0L, 0L, lvx, lvy, out_ang, &tvx, &tvy);

        // SPAWN exactly at the CAMERA1 plane to avoid hitting internal walls
        nxs = sprite[cam_anchor].x + tx;
        nys = sprite[cam_anchor].y + ty;
        nzs = sprite[cam_anchor].z + rel_z; 
        nvx = tvx; nvy = tvy;

        // SECTOR must come from the Target (Dest) to ensure bullet is "inside" the room
        nsect = sprite[target_exit].sectnum;
        updatesector(nxs, nys, &nsect);
        if (nsect < 0) nsect = sprite[target_exit].sectnum;

        // Push-out from the window plane
        nxs += (nvx >> 8); nys += (nvy >> 8); nzs += (nvz >> 8);

        // THE RECURSIVE SHOT
        hitscan(nxs, nys, nzs, nsect, nvx, nvy, nvz, hitsect, hitwall, hitsprite, hitx, hity, hitz, cliptype);
        
        // Pass vectors for potential daisy-chaining
        vx = nvx; vy = nvy; vz = nvz;
    }
}

// ============================ PRTL TELEPORTATION CODE FINISH ============================







// ===================================     UNUSED CODE START    ============================
//void hitscanprtlNOCORRECTION(long xs,long ys,long zs,short sectnum,long vx,long vy,long vz,
//                  short *hitsect, short *hitwall, short *hitsprite,
//                  long *hitx, long *hity, long *hitz, unsigned long cliptype)
//{
//    long depth, nxs, nys, nzs, nvx, nvy, ntx, nty;
//    short target_exit, angdiff;
//
//    depth = 0;
//
//    // 1. Call the standard engine hitscan
//    hitscan(xs,ys,zs,sectnum,vx,vy,vz,hitsect,hitwall,hitsprite,hitx,hity,hitz,cliptype);
//
//    // 2. Recursive portal check (max. 2 transitions for 486SX)
//    while (depth < 2 && *hitsprite >= 0 &&
//           sprite[*hitsprite].picnum >= PORTAL0 && sprite[*hitsprite].picnum <= PORTAL31)
//    {
//        // Here hittype is available! Use your T11 (index 10)
//        target_exit = (short)hittype[*hitsprite].temp_data[10];
//
//        if (target_exit < 0 || target_exit >= MAXSPRITES) break;
//
//        depth++;
//
//        // Calculate angle difference (+180 degrees)
//        angdiff = (sprite[target_exit].ang - sprite[*hitsprite].ang + 1024) & 2047;
//
//        // Transform the ray vector
//        rotatepoint(0L, 0L, vx, vy, angdiff, &nvx, &nvy);
//        vx = nvx; vy = nvy; // Update for the next pass
//
//        // Transform the hit point to the exit point
//        rotatepoint(0L, 0L, *hitx - sprite[*hitsprite].x,
//                   *hity - sprite[*hitsprite].y, angdiff, &ntx, &nty);
//
//        nxs = sprite[target_exit].x + ntx;
//        nys = sprite[target_exit].y + nty;
//        nzs = sprite[target_exit].z + (*hitz - sprite[*hitsprite].z);
//
//        // PUSH-OUT: Move the start of the new ray slightly forward from the exit
//        nxs += (sintable[(sprite[target_exit].ang + 512) & 2047] >> 8);
//        nys += (sintable[sprite[target_exit].ang & 2047] >> 8);
//
//        // Run hitscan again from the exit point
//        hitscan(nxs, nys, nzs, sprite[target_exit].sectnum, nvx, nvy, vz,
//                hitsect, hitwall, hitsprite, hitx, hity, hitz, cliptype);
//    }
//}
//
// ===================================     UNUSED CODE FINISH    ============================


