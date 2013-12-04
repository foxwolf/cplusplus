/*****************************************************************************
 * JVLC.java: JNI interface for vlc Java Bindings
 *****************************************************************************
 * Copyright (C) 1998-2005 the VideoLAN team
 *
 * Authors: Filippo Carone <filippo@carone.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/* These are a must*/
#include <jni.h>
#include <vlc/vlc.h>
#include <vlc/libvlc.h>
#include <jawt.h>
#include <jawt_md.h>

#ifndef WIN32
#include <X11/Xlib.h> // for Xlibs graphics functions
#endif

#include <stdio.h>    // for printf
#include <stdlib.h>   // for malloc

/* JVLC internal imports, generated by gcjh */
#include "../includes/JVLCCanvas.h"

jlong getJVLCInstance (JNIEnv *env, jobject _this);

JNIEXPORT void JNICALL Java_org_videolan_jvlc_JVLCCanvas_paint (JNIEnv *env, jobject canvas, jobject graphics) {

  JAWT awt;
  JAWT_DrawingSurface* ds;
  JAWT_DrawingSurfaceInfo* dsi;
#ifdef WIN32
  JAWT_Win32DrawingSurfaceInfo* dsi_win;
#else
  JAWT_X11DrawingSurfaceInfo* dsi_x11;
  GC gc;
#endif
  
  jint lock;
    
  libvlc_drawable_t drawable;
  libvlc_exception_t *exception = ( libvlc_exception_t * ) malloc( sizeof( libvlc_exception_t ));
  libvlc_exception_init( exception );

  /* Get the AWT */
  awt.version = JAWT_VERSION_1_3;
  if (JAWT_GetAWT(env, &awt) == JNI_FALSE) {
    printf("AWT Not found\n");
    return;
  }

  /* Get the drawing surface */
  ds = awt.GetDrawingSurface(env, canvas);
  if (ds == NULL) {
    printf("NULL drawing surface\n");
    return;
  }

  /* Lock the drawing surface */
  lock = ds->Lock(ds);
  if((lock & JAWT_LOCK_ERROR) != 0) {
    printf("Error locking surface\n");
    awt.FreeDrawingSurface(ds);
    return;
  }

  /* Get the drawing surface info */
  dsi = ds->GetDrawingSurfaceInfo(ds);
  if (dsi == NULL) {
    printf("Error getting surface info\n");
    ds->Unlock(ds);
    awt.FreeDrawingSurface(ds);
    return;
  }


#ifdef WIN32
  /* Get the platform-specific drawing info */
  dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;

  /* Now paint */

  drawable = reinterpret_cast<int>(dsi_win->hwnd);
  long vlcInstance = getJVLCInstance( env, canvas );
  libvlc_video_set_parent( (libvlc_instance_t *) vlcInstance, drawable, exception );

#else // UNIX
  /* Get the platform-specific drawing info */
  dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;

  /* Now paint */
  gc = XCreateGC(dsi_x11->display, dsi_x11->drawable, 0, 0);
  XSetBackground(dsi_x11->display, gc, 0);
  
  drawable = dsi_x11->drawable;
  long vlcInstance = getJVLCInstance( env, canvas );
  libvlc_video_set_parent( (libvlc_instance_t *)vlcInstance, drawable, exception );

  XFreeGC(dsi_x11->display, gc);

#endif

  /* Free the drawing surface info */
  ds->FreeDrawingSurfaceInfo(dsi);

  /* Unlock the drawing surface */
  ds->Unlock(ds);

  /* Free the drawing surface */
  awt.FreeDrawingSurface(ds);
}

/*
 * Utility functions
 */
jlong getJVLCInstance (JNIEnv *env, jobject _this) {
    /* get the id field of object */
    jclass    canvascls   = env->GetObjectClass(_this);
    jmethodID canvasmid   = env->GetMethodID(canvascls, "getJVLC", "()Lorg/videolan/jvlc/JVLC;");
    jobject   canvasjvlc  = env->CallObjectMethod(_this, canvasmid);
    jclass    cls   = env->GetObjectClass(canvasjvlc);
    jmethodID mid   = env->GetMethodID(cls, "getInstance", "()J");
    jlong     field = env->CallLongMethod(canvasjvlc, mid);
    return field;
}
