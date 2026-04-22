// portals.c
// to be included in game.c

//-------------------------------------------------------------------------
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
along with this program; if not, write to the Free Softw4:53 20.06.2025are
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

// =========================== Draw PORTAL0 - START  =======================================
void SE40_DrawPortal0(int spnum, long x, long y, long z, short a, short h, long smoothratio)
{
    long i=0, j=0, k=0;
    long offx, offy, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat;
    unsigned char old_xr, old_yr;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    //T11 (temp_data[10]):camera indexes (camindexportal).
    //T12 (temp_data[11]):activation stats and portal-camera spin counters.

    if (sprite[spnum].picnum == PORTAL0)
    {
        // 1. Target: tile sizes
        target_w = (long)tilesizx[PORTAL0];
        target_h = (long)tilesizy[PORTAL0];

        // 2. Calc repeats (sprite size)
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 3. HARD-BIND to CAMERA1 (Portal Camera)
        // Use new "tempy" T11 (index 10)
        k = hittype[spnum].temp_data[10]; 

        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = k; // remember in T11
                    break;
                }
            }
        }

        // If portal camera found - render
        if (k >= 0 && k < MAXSPRITES)
        {
            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - start
            old_cstat = sprite[spnum].cstat;
            sprite[spnum].cstat |= 32768;
            if (portalsprite1 >= 0) sprite[portalsprite1].cstat |= 32768;
            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - finish

            // prevent conflict with original security monitors (VIEWSCREEN)
            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL0, target_w, target_h);
            
            // Render the world in PORTAL0 tile
            drawrooms(sprite[k].x, sprite[k].y, sprite[k].z, sprite[k].ang, 100L, sprite[k].sectnum);
            animatesprites(sprite[k].x, sprite[k].y, sprite[k].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL0); // rotate the tile contents 90 degrees right 

            setviewback(); // prevents screen from shreding into horizontal scanlines
            
            // Restore regular camsprite immediately
            camsprite = backup_camsprite;

            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - start
            sprite[spnum].cstat = old_cstat; 
            if (portalsprite1 >= 0) sprite[portalsprite1].cstat &= ~32768;
            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - finish
        }

        // 4. Restore: bring sprite parameters back
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
    drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
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
    long i, l, angdiff;

    if (totalclock & 12) return; // Throttle-hop-optimization: update portal only each 12th frame.

    i = headspritestat[128];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL0)
        {
            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 620 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
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
            //     SE40_Draw(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                    SE40_DrawPortal0(i, x, y, z, a, h, smoothratio);
                break;
        }
        i = nextspritestat[i];
    }
}

// =========================== Draw PORTAL0 - FINISH  ======================================








// =========================== Draw PORTAL1 - START  =======================================
void SE40_DrawPortal1(int spnum, long x, long y, long z, short a, short h, long smoothratio)
{
    long i=0, j=0, k=0;
    long offx, offy, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat;
    unsigned char old_xr, old_yr;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

    //T11 (temp_data[10]):camera indexes (camindexportal).
    //T12 (temp_data[11]):activation stats and portal-camera spin counters.

    if (sprite[spnum].picnum == PORTAL1)
    {
        // 1. Target: tile sizes
        target_w = (long)tilesizx[PORTAL1];
        target_h = (long)tilesizy[PORTAL1];

        // 2. Calc repeats (sprite size)
        new_xr = (long)old_xr * (xdim / target_w);
        new_yr = (long)old_yr * (ydim / target_h);
        sprite[spnum].xrepeat = (new_xr > 255) ? 255 : (unsigned char)new_xr;
        sprite[spnum].yrepeat = (new_yr > 255) ? 255 : (unsigned char)new_yr;

        // 3. HARD-BIND to CAMERA1 (Portal Camera)
        // Use new "tempy" T11 (index 10)
        k = hittype[spnum].temp_data[10]; 

        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = k; // remember in T11
                    break;
                }
            }
        }

        // If portal camera found - render
        if (k >= 0 && k < MAXSPRITES)
        {
            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - start
            old_cstat = sprite[spnum].cstat;
            sprite[spnum].cstat |= 32768;
            if (portalsprite0 >= 0) sprite[portalsprite0].cstat |= 32768;
            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - finish

            // prevent conflict with original security monitors (VIEWSCREEN)
            backup_camsprite = camsprite;
            camsprite = -1;

            setviewtotile(PORTAL1, target_w, target_h);
            
            // Render the world in PORTAL1 tile
            drawrooms(sprite[k].x, sprite[k].y, sprite[k].z, sprite[k].ang, 100L, sprite[k].sectnum);
            animatesprites(sprite[k].x, sprite[k].y, sprite[k].ang, smoothratio);
            drawmasks();
            squarerotatetile(PORTAL1); // rotate the tile contents 90 degrees right 

            setviewback(); // prevents screen from shreding into horizontal scanlines
            
            // Restore regular camsprite immediately
            camsprite = backup_camsprite;

            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - start
            sprite[spnum].cstat = old_cstat; 
            if (portalsprite0 >= 0) sprite[portalsprite0].cstat &= ~32768;
            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - finish
        }

        // 4. Restore: bring sprite parameters back
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
    drawrooms(offx+sprite[i].x,offy+sprite[i].y,z,a,h,sprite[i].sectnum);
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
    long i, l, angdiff;

    if (totalclock & 12) return; // Throttle-hop-optimization: update portal only each 12th frame.

    i = headspritestat[129];
    while (i >= 0)
    {
        if (sprite[i].picnum == PORTAL1)
        {
            // visibility determination start
            l = getangle(sprite[i].x - ps[screenpeek].posx, sprite[i].y - ps[screenpeek].posy);
            angdiff = klabs(((l - ps[screenpeek].ang + 1024) & 2047) - 1024);

            // if diff is less than 512 (fov90+cansee). 512 is 90degrees but awake them eariler at ang of 620
            if (angdiff < 620 && cansee(ps[screenpeek].posx, ps[screenpeek].posy, ps[screenpeek].posz-(24<<8), 
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
            //     SE40_Draw(i, x, y, a, smoothratio);
            //     break;
            case 42:
            case 43:
            case 44:
            case 45:
                if (ps[screenpeek].cursectnum == sprite[i].sectnum)
                    SE40_DrawPortal1(i, x, y, z, a, h, smoothratio);
                break;
        }
        i = nextspritestat[i];
    }
}

// =========================== Draw PORTAL1 - FINISH  ======================================