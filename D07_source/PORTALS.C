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

// ========================================================================
// DESIGN PRINCIPLES OF UNIVERSAL CAMERA XY MOVEMENT INSIDE PORTALS:
// DO: Always calculate movement in LOCAL coordinate space of the sprite.
// DO: Use 'angdiff' for relative view synchronization.
// DON'T: Use global dx/dy for parallax (it breaks on non-parallel walls).
// DON'T: Hardcode Picnums for axis inversion (use local rotation instead).
// DRAWROOMSPORTAL only moves X and Y of the camera, all the rest happen
//           inside SE40_DrawPortal#x because it's the only way it works.
// ========================================================================

void drawroomsportal(long ox, long oy, long p_z, short p_ang, long ohoriz,
                   short osect, spritetype *src_prtl, spritetype *dst_cam)
{
    long dx, dy, ratio, distp, angdiff, fdx, fdy;
    long local_x, local_y, tx, ty, final_x, final_y;
    long maxparaldist, slowed_horiz;
    short slowed_angle, final_sect;

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
            
        final_x = dst_cam->x + tx; final_y = dst_cam->y + ty;
    
        // 4. Collision Safety
        // DON'T: Let the camera leave the sector (prevents HOM/Black screen glitches).
        if (klabs(tx) + klabs(ty) > maxparaldist)
        {
            final_x = dst_cam->x + scale(tx, maxparaldist, klabs(tx) + klabs(ty));
            final_y = dst_cam->y + scale(ty, maxparaldist, klabs(tx) + klabs(ty));
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

    slowed_angle = (short)(dst_cam->ang + (angdiff >> 4)); //slower horizontal look
    slowed_horiz = 100 + ((ohoriz - 100) >> 3);            //slower vertical look
    
    // DO: Apply angdiff to p_ang for proper head rotation synchronization.
    // DON'T: Modify p_ang with hardcoded constants like 512 or 1536 here.
    //drawrooms(final_x, final_y, p_z, (short)((p_ang + angdiff) & 2047), ohoriz, final_sect);
    drawrooms(final_x, final_y, p_z, (short)(slowed_angle & 2047), slowed_horiz, final_sect);
}

// =========================== Draw PORTAL0 - START  =======================================

void SE40_DrawPortal0(int spnum, long x, long y, long z, short a, short h, long smoothratio)
{
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz;
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat;
    unsigned char old_xr, old_yr;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

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
        k = hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = k;
                    break;
                }
            }
        }

        // If portal camera found - render
        if (k >= 0 && k < MAXSPRITES)
        {
            // Safety: ensure camera sector is valid before rendering to prevent flicker
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - start
            old_cstat = sprite[spnum].cstat;
            sprite[spnum].cstat |= 32768;
            if (portalsprite1 >= 0) sprite[portalsprite1].cstat |= 32768;
            // hide PORTAL1 sprite tile itself inside PORTAL0 contents - finish

            // --- Sync camera look with player ---
            sprite[k].ang = ps[screenpeek].ang;

            // --- ADVANCED Z-TRACKING & CLAMPING START ---
            // nz: desired height based on player movement
            nz = hittype[k].temp_data[8] + ((ps[screenpeek].posz - sprite[spnum].z) >> 1);
            
            // Slope-aware Z-clamping for the camera sector
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            
            // Safe Z-bounds: prevents near-plane glitches on floor/ceiling
            if (nz < (ceilz_at_cam + 8L)) nz = ceilz_at_cam + 8L;
            if (nz > (floorz_at_cam - 8L)) nz = floorz_at_cam - 8L;
            
            // Emergency centering for low-profile sectors
            if ((floorz_at_cam - ceilz_at_cam) < 16L) nz = (floorz_at_cam + ceilz_at_cam) >> 1;
            
            sprite[k].z = nz;
            // --- ADVANCED Z-TRACKING & CLAMPING END ---

            // prevent conflict with original security monitors (VIEWSCREEN)
            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL0, target_w, target_h);
            
            // --- PORTAL RENDER ---
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                            sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, sprite[k].ang, smoothratio);
            drawmasks();
            
            // rotate the tile contents 90 degrees right every frame it's drawn
            squarerotatetile(PORTAL0); 

            setviewback(); 
            
            // Restore regular camsprite
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
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

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
    // ALL declarations MUST be at the top for Watcom C 11.0
    long i=0, j=0, k=0;
    long offx, offy, dxp, dyp, render_w, render_h;
    long floor1=0, floor2=0, ok=0, fofmode = 0, backup_camsprite;
    long nz, ceilz_at_cam, floorz_at_cam, angdiffprtl;
    long pushx, pushy, pushz;
    short target_w, target_h, new_xr = 0, new_yr = 0, old_cstat;
    unsigned char old_xr, old_yr;

    old_xr = sprite[spnum].xrepeat;
    old_yr = sprite[spnum].yrepeat;
    old_cstat = sprite[spnum].cstat;

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
        k = hittype[spnum].temp_data[10]; 
        if (k < 0 || k >= MAXSPRITES || sprite[k].picnum != CAMERA1)
        {
            k = -1;
            for(i=0; i<MAXSPRITES; i++)
            {
                if (sprite[i].picnum == CAMERA1 && sprite[i].lotag == sprite[spnum].hitag)
                {
                    k = i;
                    hittype[spnum].temp_data[10] = k;
                    break;
                }
            }
        }

        // If portal camera found - render
        if (k >= 0 && k < MAXSPRITES)
        {
            // Safety: ensure camera sector is valid before rendering to prevent flicker
            if (sprite[k].sectnum < 0) 
                updatesector(sprite[k].x, sprite[k].y, &sprite[k].sectnum);

            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - start
            old_cstat = sprite[spnum].cstat;
            sprite[spnum].cstat |= 32768;
            if (portalsprite0 >= 0) sprite[portalsprite0].cstat |= 32768;
            // hide PORTAL0 sprite tile itself inside PORTAL1 contents - finish

            // --- Sync camera look with player ---
            sprite[k].ang = ps[screenpeek].ang;

            // --- ADVANCED Z-TRACKING & CLAMPING START ---
            // nz: desired height based on player movement
            nz = hittype[k].temp_data[8] + ((ps[screenpeek].posz - sprite[spnum].z) >> 1);
            
            // Slope-aware Z-clamping for the camera sector
            getzsofslope(sprite[k].sectnum, sprite[k].x, sprite[k].y, &ceilz_at_cam, &floorz_at_cam);
            
            // Safe Z-bounds: prevents near-plane glitches on floor/ceiling
            if (nz < (ceilz_at_cam + 8L)) nz = ceilz_at_cam + 8L;
            if (nz > (floorz_at_cam - 8L)) nz = floorz_at_cam - 8L;
            
            // Emergency centering for low-profile sectors
            if ((floorz_at_cam - ceilz_at_cam) < 8L) nz = (floorz_at_cam + ceilz_at_cam) >> 1;
            
            sprite[k].z = nz;
            // --- ADVANCED Z-TRACKING & CLAMPING END ---

            // prevent conflict with original security monitors (VIEWSCREEN)
            backup_camsprite = camsprite;
            camsprite = -1; 

            setviewtotile(PORTAL1, target_w, target_h);
            
            // --- PORTAL RENDER ---
            drawroomsportal(ps[screenpeek].posx, ps[screenpeek].posy, sprite[k].z, 
                            sprite[k].ang, (long)h, sprite[spnum].sectnum, 
                            &sprite[spnum], &sprite[k]);

            animatesprites(sprite[k].x, sprite[k].y, sprite[k].ang, smoothratio);
            drawmasks();
            
            // rotate the tile contents 90 degrees right every frame it's drawn
            squarerotatetile(PORTAL1); 

            setviewback();
            
            // Restore regular camsprite
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
    long i, l, angdiff;

    if (totalclock & 6) return; // Throttle-hop-optimization: update portal only each 6th frame.

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
