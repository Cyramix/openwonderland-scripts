#define VERBOSE
#undef LG3D

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

Copyright 1985, 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

******************************************************************/

/* NOTE: this file compiles without this, but then gets the wrong X_BYTE_ORDER definition */
#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include "inputstr.h"
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_LATIN2
#define XK_LATIN3
#define XK_LATIN4
#define XK_LATIN8
#define XK_LATIN9
#define XK_CYRILLIC
#define XK_GREEK
#define XK_ARMENIAN
#define XK_CAUCASUS
#define XK_VIETNAMESE
#define XK_XKB_KEYS
#include <X11/keysym.h>
#include "windowstr.h"
#include "remwin.h"
#include "protocol.h"
#include "lk201kbd.h"

/*
** Accept the event if its client has been assigned control 
** or if nobody has control.
*/
 
#define ACCEPT_EVENT(pScrPriv, clientId) \
    ((pScrPriv)->controller == clientId || \
     (pScrPriv)->controller == -1)


#define KEYSYM_INDEX(i, keySyms) \
    ((i - keySyms->minKeyCode) * keySyms->mapWidth)

typedef struct _rwKeyMapEntry {
    /* the keysym associated with this entry - 0 indicates not in use */
    KeySym keySym;

    /*  the last time this keysym was used */
    unsigned long lastTime;
} rwKeyMapEntry;

rwKeyMapEntry keyMap[MAP_LENGTH];

/* the keyboard defined in InitInput */
DeviceIntPtr rwinKeyboard;

void rwinInitKeyboard(DeviceIntPtr kbdDev) {
    rwinKeyboard = kbdDev;

#ifdef VERBOSE
    ErrorF("rwinInitKeyboard %p\n", kbdDev);
#endif
}

#define MIN_KEY   8
#define MAX_KEY   255
#define MAP_WIDTH 4

void rwinInitKeyMap() {
    /* populate initial keymap with common values */ 
    static KeySym const initialKeymap[] = {
        /* 0   */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 5   */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol, 
        /* 10  */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 15  */ XK_space, XK_exclam, XK_quotedbl, XK_numbersign, XK_dollar,
        /* 20  */ XK_percent, XK_ampersand, XK_apostrophe, XK_quoteright, XK_parenleft,
        /* 25  */ XK_parenright, XK_asterisk, XK_plus, XK_comma, XK_minus,
        /* 30  */ XK_period, XK_slash, XK_0, XK_1, XK_2,
        /* 35  */ XK_3, XK_4, XK_5, XK_6, XK_7,
        /* 40  */ XK_8, XK_9, XK_colon, XK_semicolon, XK_less,
        /* 45  */ XK_equal, XK_greater, XK_question, XK_at, XK_a,
        /* 50  */ XK_b, XK_c, XK_d, XK_e, XK_f,
        /* 55  */ XK_g, XK_h, XK_i, XK_j, XK_k, 
        /* 60  */ XK_l, XK_m, XK_n, XK_o, XK_p,
        /* 65  */ XK_q, XK_r, XK_s, XK_t, XK_u,
        /* 70  */ XK_v, XK_w, XK_x, XK_y, XK_z,
        /* 75  */ XK_A, XK_B, XK_C, XK_D, XK_E,
        /* 80  */ XK_F, XK_G, XK_H, XK_I, XK_J, 
        /* 85  */ XK_K, XK_L, XK_M, XK_N, XK_O,
        /* 90  */ XK_P, XK_Q, XK_R, XK_S, XK_T,
        /* 95  */ XK_U, XK_V, XK_W, XK_X, XK_Y,
        /* 100 */ XK_Z, XK_bracketleft, XK_backslash, XK_bracketright, XK_asciicircum, 
        /* 105 */ XK_underscore, XK_grave, XK_quoteleft, XK_braceleft, XK_bar, 
        /* 110 */ XK_braceright, XK_asciitilde, NoSymbol, NoSymbol, NoSymbol,
        /* 115 */ XK_BackSpace, XK_Tab, XK_Linefeed, XK_Clear, XK_Return,
        /* 120 */ XK_Pause, XK_Scroll_Lock, XK_Sys_Req, XK_Escape, XK_Delete,
        /* 125 */ XK_Home, XK_Left, XK_Up, XK_Right, XK_Down,
        /* 130 */ XK_Prior, XK_Page_Up, XK_Next, XK_Page_Down, XK_End,
        /* 135 */ XK_Begin, XK_Select, XK_Print, XK_Execute, XK_Insert,
        /* 140 */ XK_Undo, XK_Redo, XK_Menu, XK_Find, XK_Cancel,
        /* 145 */ XK_Help, XK_Break, XK_Mode_switch, XK_script_switch, XK_Num_Lock,
        /* 150 */ XK_KP_Space, XK_KP_Tab, XK_KP_Enter, XK_KP_F1, XK_KP_F2,
        /* 155 */ XK_KP_F3, XK_KP_F4, XK_KP_Home, XK_KP_Left, XK_KP_Up,
        /* 160 */ XK_KP_Right, XK_KP_Down, XK_KP_Prior, XK_KP_Page_Up, XK_KP_Next,
        /* 165 */ XK_KP_Page_Down, XK_KP_End, XK_KP_Begin, XK_KP_Insert, XK_KP_Delete, 
        /* 170 */ XK_KP_Equal, XK_KP_Multiply, XK_KP_Add, XK_KP_Separator, XK_KP_Subtract,
        /* 175 */ XK_KP_Decimal, XK_KP_Divide, XK_KP_0, XK_KP_1, XK_KP_3,
        /* 180 */ XK_KP_4, XK_KP_5, XK_KP_6, XK_KP_7, XK_KP_8, 
        /* 185 */ XK_KP_9, XK_F1, XK_F2, XK_F3, XK_F4,
        /* 190 */ XK_F5, XK_F6, XK_F7, XK_F8, XK_F9,
        /* 195 */ XK_F10, XK_F11, XK_F12, NoSymbol, NoSymbol,  
        /* 200 */ XK_Shift_L, XK_Shift_R, XK_Control_L, XK_Control_R, XK_Caps_Lock,
        /* 205 */ XK_Meta_L, XK_Meta_R, XK_Alt_L, XK_Alt_R, XK_Super_L,
        /* 210 */ XK_Super_R, XK_Hyper_L, XK_Hyper_R, NoSymbol, NoSymbol,
        /* 215 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 220 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 225 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 230 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 235 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 240 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 245 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 250 */ NoSymbol, NoSymbol, NoSymbol, NoSymbol, NoSymbol,
        /* 255 */ NoSymbol
    }; 

    int           i;
    KeySymsRec    keySyms;
    CARD8         modMap[MAP_LENGTH];
    KeySym        map[MAP_LENGTH * MAP_WIDTH];
    KeySymsPtr    pks = &keySyms;
    unsigned long initialTime = GetTimeInMillis();

#ifdef VERBOSE
    ErrorF("rwinInitKeyMap %p\n", rwinKeyboard);
#endif

    /* make sure all values are filled with NoSymbol
       to start */
    memset(&modMap, NoSymbol, MAP_LENGTH);
    memset(&map, NoSymbol, MAP_LENGTH * MAP_WIDTH);
   
    /* set up the keysyms map */ 
    pks->minKeyCode = MIN_KEY;
    pks->maxKeyCode = MAX_KEY;
    pks->mapWidth = MAP_WIDTH;
    pks->map = map;

    /* populate the various maps */
	for (i = 0; i < MAP_LENGTH; i++) {
		KeySym cur = initialKeymap[i];

        /* fill in our map with this symbol */ 
        keyMap[i].keySym = cur;
        keyMap[i].lastTime = (cur == NoSymbol) ? 0 : initialTime;

        /* ignore values less than the minimum key code */
        if (i < pks->minKeyCode) {
            continue;
        }

        /* add it to the real keymap */
        pks->map[KEYSYM_INDEX(i, pks)] = cur;
        pks->map[KEYSYM_INDEX(i, pks) + 1] = cur;

        /* add it to the modmap if necessary */
        switch (cur) {
            case XK_Shift_L:
            case XK_Shift_R:
                modMap[i] = ShiftMask;
                keyMap[i].lastTime = ULONG_MAX;
                break;
            case XK_Control_L:
            case XK_Control_R:
                modMap[i] = ControlMask;
                keyMap[i].lastTime = ULONG_MAX;
                break;
            case XK_Caps_Lock:
                modMap[i] = LockMask;
                keyMap[i].lastTime = ULONG_MAX;
                break;
            case XK_Multi_key:
                modMap[i] = Mod1Mask;
                keyMap[i].lastTime = ULONG_MAX;
                break;
        }
    }

    /* copy the keysyms and modmap into the current keyboard */ 
    SetKeySymsMap(&rwinKeyboard->key->curKeySyms, pks);
    memmove(rwinKeyboard->key->modifierMap, modMap, MAP_LENGTH);
}

/*
** Map a keysym sent by the remote instance into a keycode.
** This method searches the existing keycode-to-keymap
** bindings and finds the keycode matching the given keysym.
** If no binding is found for the given keysym, a new 
** binding is created, using the first empty keycode.
** In the case where there are no empty keycodes, the
** least recently used key will be ejected from the map
** and replaced with the given keycode.
*/
static KeyCode
KeysymToKeycode(KeySym keySym, ClientPtr client) {
	register int i, min;
	unsigned long minTime = ULONG_MAX - 1; 
	unsigned long curTime;
	KeySymsPtr pks = &inputInfo.keyboard->key->curKeySyms;

	min = -1;

	/* search each key until we find a match.  As we go
	   through, also track the lowest time value we have seen
	   so far. If we don't find a match, we will evict the
	   minimum value and replace it.
	 */
	for (i = pks->minKeyCode; i < pks->maxKeyCode; i++) {
		if (keyMap[i].keySym == keySym) {
			/* found a match -- update the time in the LRU cache */
			curTime = GetTimeInMillis();
			if (curTime > keyMap[i].lastTime) {
				keyMap[i].lastTime = curTime;
			}

#ifdef VERBOSE
			ErrorF("KeySymToKeyMap found code %d for 0x%x\n",
				   i, (int) keySym);
#endif

			/* return the code */
			return (KeyCode) i;         
		} 

		/* if we got here, the current code was not a match
		   update the minimum so far for future eviction
		 */
		if (keyMap[i].lastTime < minTime) {
			minTime = keyMap[i].lastTime;
			min = i;
		}
	}

    /* if we made it here, there was no match in the map.
       Pick the least recently used key, and remap it.
	 */
    if (min < 0) {
		/* no remapping candiates (!) */
        return 0;
    }

#ifdef VERBOSE
    ErrorF("Remap key %d from 0x%x to 0x%x, last update %ld\n",
           min, (int) keyMap[min].keySym, (int) keySym, keyMap[min].lastTime);
#endif

	/* update our record of the key */
	keyMap[min].keySym = keySym;
    keyMap[min].lastTime = GetTimeInMillis();

    /* create the new mapping. Make sure to fill in both the lower
       and upper case values (even if they are the same), to guarantee
       the right value is returned no matter what modifiers are selected 
	*/
    pks->map[KEYSYM_INDEX(min, pks)] = keySym;
    pks->map[KEYSYM_INDEX(min, pks) + 1] = keySym;

    SendMappingNotify(MappingKeyboard, min, 1, client);

    /* return the remapped key */
    return min;
}

static void
injectKeyEvent (ScreenPtr pScreen, Bool isPressed, KeySym keySym)
{
    DeviceIntPtr pKeyboard = inputInfo.keyboard;
    xEvent event;
    ClientPtr client = rwFindWindowManagerClient(pScreen);

    event.u.u.detail = KeysymToKeycode(keySym, client);

#ifdef VERBOSE
            ErrorF("injectKeyEvent: keySym = 0x%x, keyCode = %d pressed = %d\n",
                   (int) keySym, (int) event.u.u.detail, (int) isPressed);
#endif


    if (isPressed) {
	event.u.u.type = KeyPress;
    } else {
	event.u.u.type = KeyRelease;
    }
    event.u.keyButtonPointer.time = GetTimeInMillis();
    event.u.keyButtonPointer.state = 0;

    (*pKeyboard->public.processInputProc)(&event, pKeyboard, 1);
}

static void
injectPointerEvent (ScreenPtr pScreen, int wid, int mask, int x, int y)
{
    RemwinScreenPrivPtr pScrPriv = REMWIN_GET_SCRPRIV(pScreen);
    DeviceIntPtr pPointer = inputInfo.pointer;
    unsigned long when = GetTimeInMillis();
    xEvent event;
    int button;
    int rootX, rootY;
    WindowPtr pWin;

    /*ErrorF("Received ptr from WL: wid = %d, xy = %d, %d\n", wid, x, y);*/

    /* 
    ** Calculate absolutize event coordinates. These must be valid in the 
    ** event for proper sprite window processing to occur.
    */
    pWin = LookupIDByType(wid, RT_WINDOW);
    if (pWin == NULL) {
	ErrorF("WARNING: cannot find pointer event window %d\n", wid); 
	return;
    }
	
    rootX = x + pWin->drawable.x;
    rootY = y + pWin->drawable.y;
    /*
    ErrorF("%d\n", pWin->drawable.id);
    ErrorF("xywh = %d, %d, %d, %d\n", 
	   pWin->drawable.x, pWin->drawable.y,
	   pWin->drawable.width, pWin->drawable.height);
    ErrorF("rootXY = %d, %d\n", rootX, rootY);
    */

    /* New */
    pWin = pWin->parent;
    while (pWin != NULL) {
	rootX += pWin->drawable.x;
	rootY += pWin->drawable.y;
	/*
	ErrorF("%d\n", pWin->drawable.id);
	ErrorF("xywh = %d, %d, %d, %d\n", 
	       pWin->drawable.x, pWin->drawable.y,
	       pWin->drawable.width, pWin->drawable.height);
	ErrorF("rootXY = %d, %d\n", rootX, rootY);
	*/
	pWin = pWin->parent;
    }

    event.u.u.type = MotionNotify;
#ifdef LG3D
    event.u.keyButtonPointer.eventX = x;
    event.u.keyButtonPointer.eventY = y;
    event.u.keyButtonPointer.event = wid;
#endif /* LG3D */
    event.u.keyButtonPointer.rootX = rootX;
    event.u.keyButtonPointer.rootY = rootY;
    event.u.keyButtonPointer.time = when;;
    (*inputInfo.pointer->public.processInputProc)(&event, inputInfo.pointer, 1);

    for (button = 0; button < 5; button++) {
	int buttonMask = 1 << button;
	int changeMask = mask ^ pScrPriv->controllerButtonMask;
	if (changeMask & buttonMask) {
	    if (mask & buttonMask) {
		event.u.u.type = ButtonPress;
		event.u.keyButtonPointer.time = when;
		event.u.u.detail = button + 1;
	    } else {
		event.u.u.type = ButtonRelease;
		event.u.keyButtonPointer.time = when;
		event.u.u.detail = button + 1;
	    }
	    (*pPointer->public.processInputProc)(&event, pPointer, 1);
	}
    }

    pScrPriv->controllerButtonMask = mask;
}

/*
** The window manager client is the client which has set
** SubstructureRedirect on the root window. We know this 
** because the Appshare window manager does this.
*/

ClientPtr
rwFindWindowManagerClient (ScreenPtr pScreen) 
{
    WindowPtr pRootWin = WindowTable[pScreen->myNum];
    OtherClients *pOthClient = wOtherClients(pRootWin);

    while (pOthClient != NULL) {
	if ((pOthClient->mask & SubstructureRedirectMask)) {
	    return clients[CLIENT_ID(pOthClient->resource)];
	}
	pOthClient = pOthClient->next;
    }

    FatalError("Remote Window: Cannot find window manager client");
}


static void
winSetSize (WindowPtr pWin, int clientId, int w, int h) 
{
    ScreenPtr pScreen = pWin->drawable.pScreen;
    RemwinScreenPrivPtr pScrPriv = REMWIN_GET_SCRPRIV(pScreen);
    int vlist[2];
    int ret;

    if (pScrPriv->pWmClient == NULL) {
	pScrPriv->pWmClient = rwFindWindowManagerClient(pScreen);
    }

    /* Inform clients that an interactive resize is occuring */
    pScrPriv->configuringClient = clientId;

    vlist[0] = w;
    vlist[1] = h;
    ret = ConfigureWindow(pWin, (CWWidth | CWHeight), (XID *) vlist, pScrPriv->pWmClient);
    if (ret != Success) {
	ErrorF("Resize window failed, error = %d\n", ret);
	return;
    }

    pScrPriv->configuringClient = -1;
}

static void
winToFront (WindowPtr pWin, int clientId) 
{
    ScreenPtr pScreen = pWin->drawable.pScreen;
    RemwinScreenPrivPtr pScrPriv = REMWIN_GET_SCRPRIV(pScreen);
    WindowPtr pRootWin = WindowTable[pScreen->myNum];
    WindowPtr pFirst = pRootWin->firstChild;
    int vlist[2];
    int ret;

    if (pFirst == pWin) {
	/* This window is the only child; Nothing to do */
	return;
    }

    if (pFirst == NULL) {
	ErrorF("winToFront called when root window has no children\n");
	return;
    }

    if (pScrPriv->pWmClient == NULL) {
	pScrPriv->pWmClient = rwFindWindowManagerClient(pScreen);
    }

    /* Inform clients that an interactive restack is occuring */
    pScrPriv->configuringClient = clientId;

    vlist[0] = pFirst->drawable.id;
    vlist[1] = Above;
    ret = ConfigureWindow(pWin, (CWSibling|CWStackMode), (XID *) vlist, pScrPriv->pWmClient);
    if (ret != Success) {
	ErrorF("winToFront failed, error = %d\n", ret);
	return;
    }

    pScrPriv->configuringClient = -1;
}

static void
destroyWindow (WindowPtr pWin) 
{
    if (pWin->parent != NULL) {
	FreeResource(pWin->drawable.id, RT_NONE);
    }
}

/*
** Handle client input messages.
*/

void
rwinHandler (ScreenPtr pScreen)
{
    RemwinScreenPrivPtr pScrPriv = REMWIN_GET_SCRPRIV(pScreen);
    RwcommPtr pComm = pScrPriv->pComm;
    int msgType;
    int n;

    msgType = RWCOMM_NEXT_MESSAGE_TYPE_READ(pComm);
    /*ErrorF("msgType = %d\n", msgType);*/

    switch (msgType) {

    case CLIENT_MESSAGE_TYPE_KEY: {
	char buf[KEY_EVENT_MESSAGE_SIZE];
	Bool isPressed;
	KeySym keySym;
	int clientId;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1, 
			     KEY_EVENT_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}
	
	clientId = KEY_EVENT_MESSAGE_GET_CLIENTID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    isPressed = KEY_EVENT_MESSAGE_GET_ISPRESSED(buf);
	    keySym = KEY_EVENT_MESSAGE_GET_KEYSYM(buf);

	    swapl(&keySym, n);

#ifdef VERBOSE
	    ErrorF("KeyEvent: isPressed = %d, keySym = 0x%x, clientId = %d\n", 
		   isPressed, (int)keySym, clientId);
#endif

	    injectKeyEvent(pScreen, isPressed, keySym);
	}
	break;
    }

    case CLIENT_MESSAGE_TYPE_POINTER: {
	char buf[POINTER_EVENT_MESSAGE_SIZE];
	int mask;
	short x, y;
	int wid;
	int clientId;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     POINTER_EVENT_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = POINTER_EVENT_MESSAGE_GET_CLIENTID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    mask = POINTER_EVENT_MESSAGE_GET_MASK(buf);
	    x = POINTER_EVENT_MESSAGE_GET_X(buf);
	    y = POINTER_EVENT_MESSAGE_GET_Y(buf);
	    wid = POINTER_EVENT_MESSAGE_GET_WID(buf);

	    swaps(&x, n);
	    swaps(&y, n);
	    swapl(&wid, n);

#ifdef VERBOSE
	    ErrorF("PointerEvent: mask = 0x%x, x,y = %d, %d, wid = %d, clientId = %d\n", 
		   mask, x, y, wid, clientId);
#endif

	    injectPointerEvent(pScreen, wid, mask, x, y);
	}
	break;
    }

    case CLIENT_MESSAGE_TYPE_TAKE_CONTROL: {
	char buf[TAKE_CONTROL_MESSAGE_SIZE];
	int clientId;
	Bool steal;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     TAKE_CONTROL_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = TAKE_CONTROL_MESSAGE_GET_CLIENTID(buf);
	steal = TAKE_CONTROL_MESSAGE_GET_STEAL(buf);

	swapl(&clientId, n);

#ifdef VERBOSE
	ErrorF("TakeControl: clientId = %d, steal = %d\n", 
	       clientId, steal);
#endif

	rwTakeControl(pScreen, clientId, steal);

	break;
    }

    case CLIENT_MESSAGE_TYPE_RELEASE_CONTROL: {
	char buf[RELEASE_CONTROL_MESSAGE_SIZE];
	int clientId;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     RELEASE_CONTROL_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = RELEASE_CONTROL_MESSAGE_GET_CLIENTID(buf);

	swapl(&clientId, n);

#ifdef VERBOSE
	ErrorF("ReleaseControl: clientId = %d\n", clientId);
#endif

	rwReleaseControl(pScreen, clientId);

	break;
    }

    case CLIENT_MESSAGE_TYPE_SET_WINDOW_TITLE: {
	char hdrBuf[SET_WINDOW_TITLE_MESSAGE_SIZE];
	char *buf;
	int wid;
	int strLen;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, hdrBuf + 1,
			     SET_WINDOW_TITLE_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	wid = SET_WINDOW_TITLE_MESSAGE_GET_WID(hdrBuf);
	strLen = SET_WINDOW_TITLE_MESSAGE_GET_STRLEN(hdrBuf);

	swapl(&wid, n);
	swapl(&strLen, n);

#ifdef VERBOSE
	ErrorF("Incoming SetWindowTitle: wid = %d, strLen = %d\n", wid, strLen);
#endif

	buf = xalloc(strLen);
	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf, strLen)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

#ifdef VERBOSE
        { char *outBuf = xalloc(strLen+1);
	  int i;
	  for (i = 0; i < strLen; i++) {
	      outBuf[i] = buf[i];
	  }
	  outBuf[strLen] = '\0';
	  ErrorF("Incoming title = %s\n", outBuf);
	}
#endif

	if (!rwoutSetWindowTitleWrite(pScreen, wid, strLen, buf)) {
	    ErrorF("Could not send SetWindowTitle message\n");
	    return;
	}
	
	xfree(buf);

	break;
    }

    // TODO: eventually allow this when don't have control
    case CLIENT_MESSAGE_TYPE_WIN_SET_USER_DISPL: {
	char buf[WIN_SET_USER_DISPL_MESSAGE_SIZE];
	int clientId;
	int widUnswapped;
	int ixUnswapped, iyUnswapped, izUnswapped;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     WIN_SET_USER_DISPL_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = WIN_SET_USER_DISPL_MESSAGE_GET_CLIENT_ID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    widUnswapped = WIN_SET_USER_DISPL_MESSAGE_GET_WID(buf);
	    ixUnswapped = WIN_SET_USER_DISPL_MESSAGE_GET_IX(buf);
	    iyUnswapped = WIN_SET_USER_DISPL_MESSAGE_GET_IY(buf);
	    izUnswapped = WIN_SET_USER_DISPL_MESSAGE_GET_IZ(buf);

#ifdef VERBOSE
	    ErrorF("Incoming WinSetUserDispl: clientId = %d\n", clientId);
#endif
	    /* 
	    ** Just echo this information back to the master client 
	    ** for rebroadcast to all slaves
	    */
	    if (!rwoutWinSetUserDisplWrite(pScreen, clientId, widUnswapped, 
					   ixUnswapped, iyUnswapped, izUnswapped)) {
		ErrorF("Could not send WinSetUserDispl message\n");
		return;
	    }
	}
	break;
    }

    // TODO: eventually allow this when don't have control
    case CLIENT_MESSAGE_TYPE_WIN_SET_SIZE: {
	char buf[WIN_SET_SIZE_MESSAGE_SIZE];
	int clientId;
	int wid;
	int w, h;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     WIN_SET_SIZE_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = WIN_SET_SIZE_MESSAGE_GET_CLIENT_ID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    wid = WIN_SET_SIZE_MESSAGE_GET_WID(buf);
	    w = WIN_SET_SIZE_MESSAGE_GET_WIDTH(buf);
	    h = WIN_SET_SIZE_MESSAGE_GET_HEIGHT(buf);

	    swapl(&wid, n);
	    swapl(&w, n);
	    swapl(&h, n);

#ifdef VERBOSE
	    ErrorF("ResizeWindow: clientId = %d, wid = %d, wh = %d, %d\n", 
		   clientId, wid, w, h);
#endif
	
	    WindowPtr pWin = LookupIDByType(wid, RT_WINDOW);
	    if (pWin == NULL) {
		ErrorF("Could not find window to resize, wid = %d\n", wid);
		return;
	    }
	    
	    winSetSize(pWin, clientId, w, h);
	}
	break;
    }

    // TODO: eventually allow this when don't have control
    case CLIENT_MESSAGE_TYPE_WIN_SET_ROTATE_Y: {
	char buf[WIN_SET_ROTATE_Y_MESSAGE_SIZE];
	int clientId;
	int widUnswapped;
	int irotyUnswapped;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     WIN_SET_ROTATE_Y_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = WIN_SET_ROTATE_Y_MESSAGE_GET_CLIENT_ID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    widUnswapped = WIN_SET_ROTATE_Y_MESSAGE_GET_WID(buf);
	    irotyUnswapped = WIN_SET_ROTATE_Y_MESSAGE_GET_IROTY(buf);

#ifdef VERBOSE
	    ErrorF("Incoming WinSetRotateY: clientId = %d\n", clientId);
#endif
	    /* 
	    ** Just echo this information back to the master client 
	    ** for rebroadcast to all slaves
	    */
	    if (!rwoutWinSetRotateYWrite(pScreen, clientId, widUnswapped, 
					 irotyUnswapped)) {
		ErrorF("Could not send WinSetRotateY message\n");
		return;
	    }
	}
	break;
    }

    // TODO: eventually allow this when don't have control
    case CLIENT_MESSAGE_TYPE_WIN_TO_FRONT: {
	char buf[WIN_TO_FRONT_MESSAGE_SIZE];
	int clientId;
	int wid;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     WIN_TO_FRONT_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = WIN_TO_FRONT_MESSAGE_GET_CLIENT_ID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {

	    wid = WIN_TO_FRONT_MESSAGE_GET_WID(buf);
	    swapl(&wid, n);

#ifdef VERBOSE
	    ErrorF("Incoming WinToFront: clientId = %d, wid = %d\n", clientId, wid);
#endif

	    WindowPtr pWin = LookupIDByType(wid, RT_WINDOW);
	    if (pWin == NULL) {
		ErrorF("Could not find window to resize, wid = %d\n", wid);
		return;
	    }
	    
	    winToFront(pWin, clientId);
	}
	break;
    }

    case CLIENT_MESSAGE_TYPE_DESTROY_WINDOW: {
	char buf[DESTROY_WINDOW_MESSAGE_SIZE];
	int wid;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
		     DESTROY_WINDOW_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	wid = DESTROY_WINDOW_MESSAGE_GET_WID(buf);
	swapl(&wid, n);

#ifdef VERBOSE
	ErrorF("DestroyWindow: wid = %d\n", wid);
#endif
	
	WindowPtr pWin = LookupIDByType(wid, RT_WINDOW);
	if (pWin == NULL) {
	    ErrorF("Could not find window to destroy, wid = %d\n", wid);
	    return;
	}

	destroyWindow(pWin);
	break;;
    }

    case CLIENT_MESSAGE_TYPE_SLAVE_CLOSE_WINDOW: {
	char buf[SLAVE_CLOSE_WINDOW_MESSAGE_SIZE];
	int clientId, wid, n;

	if (!RWCOMM_NEXT_MESSAGE_BUFFER_READ(pComm, buf + 1,
			     SLAVE_CLOSE_WINDOW_MESSAGE_SIZE -1)) {
	    ErrorF("Error reading next message buffer\n");
	    return;
	}

	clientId = WIN_SET_SIZE_MESSAGE_GET_CLIENT_ID(buf);
	swapl(&clientId, n);

	if (ACCEPT_EVENT(pScrPriv, clientId)) {
	    wid = WIN_SET_SIZE_MESSAGE_GET_WID(buf);
	    swapl(&wid, n);

#ifdef VERBOSE
	    ErrorF("SlaveCloseWindow: clientId = %d, wid = %d\n", clientId, wid);
#endif
	
	    WindowPtr pWin = LookupIDByType(wid, RT_WINDOW);
	    if (pWin == NULL) {
		ErrorF("Could not find window to resize, wid = %d\n", wid);
		return;
	    }
	    
	    rwoutSlaveCloseWindowWrite(pScreen, clientId, wid);
	}

	break;
    }

    /* TODO */
    case CLIENT_MESSAGE_TYPE_HELLO: 
	FatalError("rwinHandler: client message hello is not yet implemented\n");
	
    case CLIENT_MESSAGE_TYPE_INVALID:
	// Client has disconnected
	ErrorF("Exitting.\n");
	exit(0);
        break;

    default:
	FatalError("rwinHandler: unknown message type %d\n", msgType);

    }
}

#ifdef LG3D
int lgeDisplayServerIsAlive = 0;

Window lgeDisplayServerPRWsList[MAXSCREENS];
WindowPtr pLgeDisplayServerPRWWinsList[MAXSCREENS];
static int numRegisteredScr = 0;

static Bool first = TRUE;
#endif /* LG3D */

void
rwinInitScreen (ScreenPtr pScr) 
{
#ifdef LG3D
    WindowPtr pRootWin;
    int i;

    if (first) {
	first = FALSE;

	lgeDisplayServerIsAlive = 1;

	for (i = 0; i < MAXSCREENS; i++) {
	    lgeDisplayServerPRWsList[i] = INVALID;
	    pLgeDisplayServerPRWWinsList[i] = NULL;
	}
    }

    pRootWin = WindowTable[pScr->myNum];
    lgeDisplayServerPRWsList[pScr->myNum] = pRootWin->drawable.id;
    pLgeDisplayServerPRWWinsList[pScr->myNum] = pRootWin;;
    numRegisteredScr = 1;
#endif /* LG3D */
}

#ifdef LG3D
int
IsWinLgePRWOne(int win) 
{
    int i;
	
    for (i=0; i < numRegisteredScr; i++)
    {
	if(lgeDisplayServerPRWsList[i] == win)
            return win;
    }	
    return INVALID;
}

WindowPtr
GetLgePRWWinFor(int win) 
{
    int i;
	
    for (i=0; i < numRegisteredScr; i++)
    {
	if(lgeDisplayServerPRWsList[i] == win)
	     return pLgeDisplayServerPRWWinsList[i];			
    }	
    return NULL;
}
#endif /* LG3D */
