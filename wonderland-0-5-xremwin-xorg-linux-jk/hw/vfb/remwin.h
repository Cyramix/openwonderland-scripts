
/*****************************************************************

Copyright 2007 Sun Microsystems, Inc.

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice(s) and this permission notice appear in all copies of
the Software and that both the above copyright notice(s) and this
permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder
shall not be used in advertising or otherwise to promote the sale, use
or other dealings in this Software without prior written authorization
of the copyright holder.

******************************************************************/

#ifndef REMWIN_H
#define REMWIN_H

#include "scrnintstr.h"
#include "regionstr.h"
#include "rwcomm.h"
#include "gcstruct.h"
#include "damage.h"

/* Don't send any pixel buffers larger than 16KB */
#define PIXEL_BUF_MAX_NUM_BYTES (1024*16)
#define PIXEL_BUF_MAX_NUM_PIXELS (PIXEL_BUF_MAX_NUM_BYTES/4)

/* 
** The RLE algorithm used never creates more pixels than
** is in the original buffer, so the max number of pixels
** provides an upper bound on the maximum number of runs.
*/
#define RLE_BUF_MAX_NUM_BYTES PIXEL_BUF_MAX_NUM_BYTES
#define RLE_BUF_MAX_NUM_PIXELS (RLE_BUF_MAX_NUM_BYTES/4)

/*
** The Client Manager. Used for figuring out when to quit.
*/

/* Take away control from current controller after 2 seconds with no clients connected */
#define REMWIN_CONTROL_LOSS_TIMEOUT_DEFAULT 	2  

/* Take away control from current controller after 28 more seconds after taking away control */
#define REMWIN_SHUT_DOWN_TIMEOUT_DEFAULT 	28

typedef struct remwin_client_manager *RemwinClientManagerPtr;
    
typedef struct remwin_client_manager {

    /* The X screen to which this manager belongs */
    ScreenPtr pScreen;

    /* Current State */    
    int state;

    /* Control-Loss Timer count */
    int timerCountControlLoss;

    /* Shutdown Timer */
    int timerCountShutDown;

    /* The number of ticks to run the control-loss timer */
    int armTicksControlLoss;

    /* The number of ticks to run the shutdown timer */
    int armTicksShutDown;

    /* 
    ** The number of clients (excluding the server client and the window manager 
    ** which are connected.
    */
    int numClientsConnected;

    /* A table of which clients are connected, indexed by the client index */
    Bool *connected;

    /* Debug flag; set by $REMWIN_CLIENT_MANAGER_DEBUG=1 */
    Bool debug;

} RemwinClientManagerRec;

/*
** The client manager state machine states.
*/

/* The initial state */
#define CLIENT_MANAGER_STATE_INITIAL				0

/* One or more clients are connected */
#define CLIENT_MANAGER_STATE_HAS_CLIENTS			1

/* No clients are connected and control-loss timer is running */
#define CLIENT_MANAGER_STATE_NO_CLIENTS_CONTROL_LOSS_PENDING	2

/* No clients are connected and shutdown timer is running */
#define CLIENT_MANAGER_STATE_NO_CLIENTS_SHUT_DOWN_PENDING	3

/*
** The client manager state machine actions.
*/

/* A new client has connected */
#define CLIENT_MANAGER_ACTION_CLIENT_CONNECTED			0

/* The last client has disconnected */
#define CLIENT_MANAGER_ACTION_LAST_CLIENT_DISCONNECTED		1

/* The control-loss timer has expired */
#define CLIENT_MANAGER_ACTION_CONTROL_LOSS_TIMEOUT		2

/* The shutdown timer has expired */
#define CLIENT_MANAGER_ACTION_SHUT_DOWN_TIMEOUT			3


/*
** The Remwin Screen Private
*/

typedef struct remwin_scr_priv_rec {
    RwcommPtr                   pComm;
    OsTimerPtr                  outputTimer;
    CloseScreenProcPtr		CloseScreen;
    ScreenWakeupHandlerProcPtr 	WakeupHandler;
    CreateWindowProcPtr		CreateWindow;
    RealizeWindowProcPtr	RealizeWindow;
    UnrealizeWindowProcPtr	UnrealizeWindow;
    DestroyWindowProcPtr	DestroyWindow;
    PositionWindowProcPtr	PositionWindow;
    ResizeWindowProcPtr		ResizeWindow;
    RestackWindowProcPtr	RestackWindow;
    ChangeWindowAttributesProcPtr ChangeWindowAttributes;
    CreateGCProcPtr             CreateGC;
    DisplayCursorProcPtr        DisplayCursor;
    SetCursorPositionProcPtr    SetCursorPosition;

    unsigned char *pPixelBuf;
    unsigned char *pRleBuf;

    /* List of managed o(top-level, drawable) windows */
    WindowPtr   pManagedWindows;

    /* 
    ** Which client currently has interactive control.
    ** -1 indicates no one has control.
    */
    int controller;

    /* Which pointer buttons the controller has pressed */
    int controllerButtonMask;
    
    /* The currently displayed cursor */
    CursorPtr pCursor;

    /* 
    ** The managed window the cursor is in. 
    ** NULL if cursor isn't shown 
    */
    WindowPtr pCursorWin;

    /* The cursor position (relative to pCursorWin) */
    int cursorX;
    int cursorY;

    /* The window manager client */
    ClientPtr pWmClient;

    /* 
    ** When this is -1 window moves and resizes are programmatic.
    ** (i.e. invoked by the application). When this is not -1
    ** window moves and resizes are being made by the user.
    */
    int configuringClient;

    /* True when any windows may have unsent dirty pixels */
    Bool windowsAreDirty;

    /* The client manager (figures out when to shutdown) */
    RemwinClientManagerPtr pClientMgr;

} RemwinScreenPrivRec, *RemwinScreenPrivPtr;

#define REMWIN_GET_SCRPRIV(pScreen) \
    ((RemwinScreenPrivPtr)((pScreen)->devPrivates[remwinScreenIndex].ptr))

#define REMWIN_GET_WINPRIV(pWin) \
    ((RemwinWindowPrivPtr)(pWin)->devPrivates[remwinWinIndex].ptr);

/* DELETE: no longer used 
#define REMWIN_SET_WINPRIV(pWin, pWinPriv) \
    (pWin)->devPrivates[remwinWinIndex].ptr = (pointer)pWinPriv;
*/

typedef struct remwin_window_priv_rec *RemwinWindowPrivPtr;

typedef struct _rwoutDamage {
    DamagePtr		pDamage;
    DrawablePtr		pDrawable;
    DamageReportLevel	level;
    XID			id;
    RemwinWindowPrivPtr pWinPriv;
} RwoutDamageRec, *RwoutDamagePtr;

typedef struct remwin_window_priv_rec {

    /* 
    ** The window has been dirtied. That is, there are updates to send
    ** to the client. (One or both of the regions has been updated).
    */
    Bool dirty;

    /* Whether the window has been mapped */
    Bool mapped;

    /* The area of the window that has been dirtied */
    RegionRec dirtyReg;

    /* The link to the next window (not winpriv!) */
    WindowPtr pWinNext;

    /* Pointer to the damage information for this window */
    RwoutDamagePtr pDamage;

} RemwinWindowPrivRec;

#define REMWIN_GET_GCPRIV(pGC) \
    ((RemwinGCPrivPtr)(pGC)->devPrivates[remwinGCIndex].ptr);

/* DELETE: no longer used 
#define REMWIN_SET_GCPRIV(pGC, pGCPriv) \
    (pWin)->devPrivates[remwinGCIndex].ptr = (pointer)pGCPriv;
*/

typedef struct remwin_gc_priv_rec *RemwinGCPrivPtr;

typedef struct remwin_gc_priv_rec {
    GCOps	    *wrapOps;	    /* wrapped ops */
    GCFuncs	    *wrapFuncs;	    /* wrapped funcs */
} RemwinGCPrivRec;

extern RemwinScreenPrivRec remwinScreenPriv;

extern void rwoutTimerCreate (ScreenPtr pScreen);
extern void rwoutTimerDestroy (ScreenPtr pScreen);

extern void rwinHandler (ScreenPtr pScreen);
extern void rwinInitKeyboard(DeviceIntPtr kbdDev);
extern void rwinInitKeyMap (void);

/* Maximum pixel output rate is 30 fps */
#define MAX_OUTPUT_RATE 30.0
#define TIMER_MS ((int)((1/MAX_OUTPUT_RATE) * 1000.0))

/* Wait 1 sec for a new connection to appear before terminating server */
#define MAX_WAIT_FOR_NEW_CONNECTION_COUNT (20 * (int)MAX_OUTPUT_RATE) 

extern Bool rwoutInitScreen (ScreenPtr pScreen);

extern void rwTakeControlInit (ScreenPtr pScreen);
extern void rwTakeControl (ScreenPtr pScreen, int clientId, Bool steal);
extern void rwReleaseControl (ScreenPtr pScreen, int clientId);

extern int remwinScreenIndex;

extern Bool rwoutSetWindowTitleWrite (ScreenPtr pScreen, int wid, int strLen, char *buf);

extern Bool rwoutWinSetUserDisplWrite (ScreenPtr pScreen, int clientId, int widUnswapped, 
				       int ixUnswapped, int iyUnswapped, int izUnswapped);

extern Bool rwoutWinSetRotateYWrite (ScreenPtr pScreen, int clientId, int widUnswapped, 
				     int irotyUnswapped);

extern Bool rwoutSlaveCloseWindowWrite (ScreenPtr pScreen, int clientId, int wid);

extern void rwoutBeep (int percent, DeviceIntPtr pDevice,
		       pointer ctrl, int unused);

extern void rwoutSyncClientOnConnect (ScreenPtr pScreen, RwcommClientPtr pCommClient);

extern Bool rlePixelsWrite (ScreenPtr pScreen, WindowPtr pWin, int x, int y, int w, int h);

extern Bool vfbRemoteWindow;

extern void rwinInitScreen (ScreenPtr pScreen);

extern int IsWinLgePRWOne(int win);
extern WindowPtr GetLgePRWWinFor(int win);

extern void rwoutDirtyWindow (WindowPtr pWin);

extern ClientPtr rwFindWindowManagerClient (ScreenPtr pScreen);

extern RemwinClientManagerPtr rwclientmgrCreate (ScreenPtr pScreen);

extern void rwclientmgrDestroy (RemwinClientManagerPtr pClientMgr);

extern void rwclientmgrClientStateChange (RemwinClientManagerPtr pClientMgr, ClientPtr pClient);

extern void rwclientmgrTimerTick (RemwinClientManagerPtr pClientMgr);

extern void rwTakeAwayControl (ScreenPtr pScreen);

extern void rwShutdown (ScreenPtr pScreen) ;

#endif /* REMWIN_H */
