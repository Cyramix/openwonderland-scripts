
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
** Client-to-Server Messages
*/

#define CLIENT_MESSAGE_TYPE_INVALID     	0
#define CLIENT_MESSAGE_TYPE_KEY			1
#define CLIENT_MESSAGE_TYPE_POINTER		2
#define CLIENT_MESSAGE_TYPE_TAKE_CONTROL	3
#define CLIENT_MESSAGE_TYPE_RELEASE_CONTROL	4
#define CLIENT_MESSAGE_TYPE_SET_WINDOW_TITLE	5
#define CLIENT_MESSAGE_TYPE_WIN_SET_USER_DISPL  6
#define CLIENT_MESSAGE_TYPE_WIN_SET_SIZE        7
#define CLIENT_MESSAGE_TYPE_WIN_SET_ROTATE_Y    8
#define CLIENT_MESSAGE_TYPE_WIN_TO_FRONT        9
#define CLIENT_MESSAGE_TYPE_DESTROY_WINDOW	10
#define CLIENT_MESSAGE_TYPE_SLAVE_CLOSE_WINDOW	11
#define CLIENT_MESSAGE_TYPE_HELLO	        12

typedef struct keyeventmessage_struct {
    CARD8 	msgType;			
    CARD8       isPressed;
    CARD16      pad;
    CARD32      keysym;
    CARD32      clientId;
} KeyEventMessage;

#define KEY_EVENT_MESSAGE_SIZE sizeof(KeyEventMessage)

#define KEY_EVENT_MESSAGE_GET_ISPRESSED(pBuf) \
    ((KeyEventMessage*)(pBuf))->isPressed

#define KEY_EVENT_MESSAGE_GET_KEYSYM(pBuf) \
    ((KeyEventMessage*)(pBuf))->keysym

#define KEY_EVENT_MESSAGE_GET_CLIENTID(pBuf) \
    ((KeyEventMessage*)(pBuf))->clientId

typedef struct pointereventmessage_struct {
    CARD8 	msgType;
    CARD8       mask;
    CARD16      x;
    CARD16      y;
    CARD16      pad;
    CARD32      wid;
    CARD32      clientId;
} PointerEventMessage;

#define POINTER_EVENT_MESSAGE_SIZE sizeof(PointerEventMessage)

#define POINTER_EVENT_MESSAGE_GET_MASK(pBuf) \
    ((PointerEventMessage *)(pBuf))->mask

#define POINTER_EVENT_MESSAGE_GET_X(pBuf) \
    ((PointerEventMessage*)(pBuf))->x

#define POINTER_EVENT_MESSAGE_GET_Y(pBuf) \
    ((PointerEventMessage*)(pBuf))->y

#define POINTER_EVENT_MESSAGE_GET_WID(pBuf) \
    ((PointerEventMessage*)(pBuf))->wid

#define POINTER_EVENT_MESSAGE_GET_CLIENTID(pBuf) \
    ((PointerEventMessage*)(pBuf))->clientId

typedef struct takecontrolmessage_struct {
    CARD8 	msgType;			
    CARD8       steal;
    CARD16      pad;
    CARD32      clientId;
} TakeControlMessage;

#define TAKE_CONTROL_MESSAGE_SIZE sizeof(TakeControlMessage)

#define TAKE_CONTROL_MESSAGE_GET_STEAL(pBuf) \
    ((((TakeControlMessage*)(pBuf))->steal == 1) ? TRUE : FALSE)

#define TAKE_CONTROL_MESSAGE_GET_CLIENTID(pBuf) \
    ((TakeControlMessage*)(pBuf))->clientId

typedef struct releasecontrolmessage_struct {
    CARD8 	msgType;			
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
} ReleaseControlMessage;

#define RELEASE_CONTROL_MESSAGE_SIZE sizeof(ReleaseControlMessage)

#define RELEASE_CONTROL_MESSAGE_GET_CLIENTID(pBuf) \
    ((ReleaseControlMessage*)(pBuf))->clientId

/* Same structure used for both server and client messages */

typedef struct setwindowtitlemessage_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      wid;
    CARD32      strLen;
    /* Followed by strLen bytes */
} SetWindowTitleMessage;

#define SET_WINDOW_TITLE_MESSAGE_SIZE sizeof(SetWindowTitleMessage)

#define SET_WINDOW_TITLE_MESSAGE_SET_TYPE(pBuf, mType) \
    ((SetWindowTitleMessage *)(pBuf))->msgType = (mType)

#define SET_WINDOW_TITLE_MESSAGE_GET_WID(pBuf) \
    ((((SetWindowTitleMessage*)(pBuf))->wid))

#define SET_WINDOW_TITLE_MESSAGE_SET_WID(pBuf, widval) \
    ((SetWindowTitleMessage *)(pBuf))->wid = (widval)

#define SET_WINDOW_TITLE_MESSAGE_GET_STRLEN(pBuf) \
    ((((SetWindowTitleMessage*)(pBuf))->strLen))

#define SET_WINDOW_TITLE_MESSAGE_SET_STRLEN(pBuf, len) \
    ((SetWindowTitleMessage *)(pBuf))->strLen = (len)

/* Same structure used for both server and client messages */

typedef struct winsetuserdispl_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
    CARD32      wid;
    CARD32      ix;
    CARD32      iy;
    CARD32      iz;
} WinSetUserDisplMessage;

#define WIN_SET_USER_DISPL_MESSAGE_SIZE sizeof(WinSetUserDisplMessage)

#define WIN_SET_USER_DISPL_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WinSetUserDisplMessage *)(pBuf))->msgType = (mType)

#define WIN_SET_USER_DISPL_MESSAGE_GET_CLIENT_ID(pBuf) \
    ((((WinSetUserDisplMessage*)(pBuf))->clientId))

#define WIN_SET_USER_DISPL_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((WinSetUserDisplMessage *)(pBuf))->clientId = (cid)

#define WIN_SET_USER_DISPL_MESSAGE_GET_WID(pBuf) \
    ((((WinSetUserDisplMessage*)(pBuf))->wid))

#define WIN_SET_USER_DISPL_MESSAGE_SET_WID(pBuf, widval) \
    ((WinSetUserDisplMessage *)(pBuf))->wid = (widval)

#define WIN_SET_USER_DISPL_MESSAGE_GET_IX(pBuf) \
    ((((WinSetUserDisplMessage*)(pBuf))->ix))

#define WIN_SET_USER_DISPL_MESSAGE_SET_IX(pBuf, ixval) \
    ((WinSetUserDisplMessage *)(pBuf))->ix = (ixval)

#define WIN_SET_USER_DISPL_MESSAGE_GET_IY(pBuf) \
    ((((WinSetUserDisplMessage*)(pBuf))->iy))

#define WIN_SET_USER_DISPL_MESSAGE_SET_IY(pBuf, iyval) \
    ((WinSetUserDisplMessage *)(pBuf))->iy = (iyval)

#define WIN_SET_USER_DISPL_MESSAGE_GET_IZ(pBuf) \
    ((((WinSetUserDisplMessage*)(pBuf))->iz))

#define WIN_SET_USER_DISPL_MESSAGE_SET_IZ(pBuf, izval) \
    ((WinSetUserDisplMessage *)(pBuf))->iz = (izval)

typedef struct winsetsize_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
    CARD32      wid;
    CARD32      width;
    CARD32      height;
} WinSetSizeMessage;

#define WIN_SET_SIZE_MESSAGE_SIZE sizeof(WinSetSizeMessage)

#define WIN_SET_SIZE_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WinSetSizeMessage *)(pBuf))->msgType = (mType)

#define WIN_SET_SIZE_MESSAGE_GET_CLIENT_ID(pBuf) \
    ((((WinSetSizeMessage*)(pBuf))->clientId))

#define WIN_SET_SIZE_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((WinSetSizeMessage *)(pBuf))->clientId = (cid)

#define WIN_SET_SIZE_MESSAGE_GET_WID(pBuf) \
    ((((WinSetSizeMessage*)(pBuf))->wid))

#define WIN_SET_SIZE_MESSAGE_SET_WID(pBuf, widval) \
    ((WinSetSizeMessage *)(pBuf))->wid = (widval)

#define WIN_SET_SIZE_MESSAGE_GET_WIDTH(pBuf) \
    ((((WinSetSizeMessage*)(pBuf))->width))

#define WIN_SET_SIZE_MESSAGE_SET_WIDTH(pBuf, w) \
    ((WinSetSizeMessage *)(pBuf))->width = (w)

#define WIN_SET_SIZE_MESSAGE_GET_HEIGHT(pBuf) \
    ((((WinSetSizeMessage*)(pBuf))->height))

#define WIN_SET_SIZE_MESSAGE_SET_HEIGHT(pBuf, h) \
    ((WinSetSizeMessage *)(pBuf))->height = (h)

/* Same structure used for both server and client messages */

typedef struct winsetrotatey_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
    CARD32      wid;
    CARD32      iroty;
} WinSetRotateYMessage;

#define WIN_SET_ROTATE_Y_MESSAGE_SIZE sizeof(WinSetRotateYMessage)

#define WIN_SET_ROTATE_Y_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WinSetRotateYMessage *)(pBuf))->msgType = (mType)

#define WIN_SET_ROTATE_Y_MESSAGE_GET_CLIENT_ID(pBuf) \
    ((((WinSetRotateYMessage*)(pBuf))->clientId))

#define WIN_SET_ROTATE_Y_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((WinSetRotateYMessage *)(pBuf))->clientId = (cid)

#define WIN_SET_ROTATE_Y_MESSAGE_GET_WID(pBuf) \
    ((((WinSetRotateYMessage*)(pBuf))->wid))

#define WIN_SET_ROTATE_Y_MESSAGE_SET_WID(pBuf, widval) \
    ((WinSetRotateYMessage *)(pBuf))->wid = (widval)

#define WIN_SET_ROTATE_Y_MESSAGE_GET_IROTY(pBuf) \
    ((((WinSetRotateYMessage*)(pBuf))->iroty))

#define WIN_SET_ROTATE_Y_MESSAGE_SET_IROTY(pBuf, irotyval) \
    ((WinSetRotateYMessage *)(pBuf))->iroty = (irotyval)

typedef struct wintofront_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
    CARD32      wid;
} WinToFrontMessage;

#define WIN_TO_FRONT_MESSAGE_SIZE sizeof(WinToFrontMessage)

#define WIN_TO_FRONT_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WinToFrontMessage *)(pBuf))->msgType = (mType)

#define WIN_TO_FRONT_MESSAGE_GET_CLIENT_ID(pBuf) \
    ((((WinToFrontMessage*)(pBuf))->clientId))

#define WIN_TO_FRONT_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((WinToFrontMessage *)(pBuf))->clientId = (cid)

#define WIN_TO_FRONT_MESSAGE_GET_WID(pBuf) \
    ((((WinToFrontMessage*)(pBuf))->wid))

#define WIN_TO_FRONT_MESSAGE_SET_WID(pBuf, widval) \
    ((WinToFrontMessage *)(pBuf))->wid = (widval)

typedef struct slaveclosewin_struct {
    CARD8 	msgType;
    CARD8       pad1;
    CARD16      pad2;
    CARD32      clientId;
    CARD32      wid;
} SlaveCloseWindowMessage;

#define SLAVE_CLOSE_WINDOW_MESSAGE_SIZE sizeof(SlaveCloseWindowMessage)

#define SLAVE_CLOSE_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((SlaveCloseWindowMessage *)(pBuf))->msgType = (mType)

#define SLAVE_CLOSE_WINDOW_MESSAGE_GET_CLIENT_ID(pBuf) \
    ((((SlaveCloseWindowMessage*)(pBuf))->clientId))

#define SLAVE_CLOSE_WINDOW_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((SlaveCloseWindowMessage *)(pBuf))->clientId = (cid)

#define SLAVE_CLOSE_WINDOW_MESSAGE_GET_WID(pBuf) \
    ((((SlaveCloseWindowMessage*)(pBuf))->wid))

#define SLAVE_CLOSE_WINDOW_MESSAGE_SET_WID(pBuf, widval) \
    ((SlaveCloseWindowMessage *)(pBuf))->wid = (widval)

typedef struct hellomessage_struct {
    CARD8 	msgType;
} HelloMessage;

#define HELLO_MESSAGE_SIZE sizeof(HelloWindowMessage)

/*
** Server to Client Messages
*/

#define SERVER_MESSAGE_TYPE_CREATE_WINDOW    		0
#define SERVER_MESSAGE_TYPE_DESTROY_WINDOW   		1
#define SERVER_MESSAGE_TYPE_SHOW_WINDOW      		2
#define SERVER_MESSAGE_TYPE_CONFIGURE_WINDOW 		3
#define SERVER_MESSAGE_TYPE_POSITION_WINDOW  		4
#define SERVER_MESSAGE_TYPE_RESTACK_WINDOW 		5
#define SERVER_MESSAGE_TYPE_WINDOW_SET_DECORATED  	6
#define SERVER_MESSAGE_TYPE_WINDOW_SET_BORDER_WIDTH  	7
#define SERVER_MESSAGE_TYPE_WINDOW_SET_USER_DISPL 	8
#define SERVER_MESSAGE_TYPE_WINDOW_SET_ROTATE_Y         9
#define SERVER_MESSAGE_TYPE_SLAVE_CLOSE_WINDOW          10
#define SERVER_MESSAGE_TYPE_BEEP	 		11
#define SERVER_MESSAGE_TYPE_DISPLAY_PIXELS	 	12
#define SERVER_MESSAGE_TYPE_COPY_AREA    	 	13
#define SERVER_MESSAGE_TYPE_CONTROLLER_STATUS	 	14
#define SERVER_MESSAGE_TYPE_DISPLAY_CURSOR 		15
#define SERVER_MESSAGE_TYPE_MOVE_CURSOR                 16
#define SERVER_MESSAGE_TYPE_SHOW_CURSOR                 17
#define SERVER_MESSAGE_TYPE_SET_WINDOW_TITLE            18
#define SERVER_MESSAGE_TYPE_WELCOME                 	19
/* PERFTEST - not used by server                        20 */
#define SERVER_MESSAGE_TYPE_PING                 	21
/* SERVER_DISCONNECT          	                        22 */

typedef struct {
    CARD8 	msgType;
    CARD8 	decorated;		
    CARD16 	borderWidth;
    CARD32 	wid;
    CARD16 	x;
    CARD16 	y;
    CARD32 	wAndBorder;  /* Includes 2 * bw */
    CARD32 	hAndBorder;  /* Includes 2 * bw */
} CreateWindowMessage;

#define CREATE_WINDOW_MESSAGE_SIZE sizeof(CreateWindowMessage)

#define CREATE_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((CreateWindowMessage *)(pBuf))->msgType = (mType)

#define CREATE_WINDOW_MESSAGE_SET_DECORATED(pBuf, decor) \
    ((CreateWindowMessage *)(pBuf))->decorated = (decor)

#define CREATE_WINDOW_MESSAGE_SET_BORDER_WIDTH(pBuf, bw) \
    ((CreateWindowMessage *)(pBuf))->borderWidth = (bw)

#define CREATE_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((CreateWindowMessage *)(pBuf))->wid = (windowId)

#define CREATE_WINDOW_MESSAGE_SET_X(pBuf, xval) \
    ((CreateWindowMessage *)(pBuf))->x = (xval)

#define CREATE_WINDOW_MESSAGE_SET_Y(pBuf, yval) \
    ((CreateWindowMessage *)(pBuf))->y = (yval)

#define CREATE_WINDOW_MESSAGE_SET_WANDBORDER(pBuf, wandb) \
    ((CreateWindowMessage *)(pBuf))->wAndBorder = (wandb)

#define CREATE_WINDOW_MESSAGE_SET_HANDBORDER(pBuf, handb) \
    ((CreateWindowMessage *)(pBuf))->hAndBorder = (handb)

/* Same structure used for both server and client messages */

typedef struct {
    CARD8 	msgType;			
    CARD8 	pad1;		
    CARD16 	pad2;
    CARD32 	wid;
} DestroyWindowMessage;

#define DESTROY_WINDOW_MESSAGE_SIZE sizeof(DestroyWindowMessage)

#define DESTROY_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((DestroyWindowMessage *)(pBuf))->msgType = (mType)

#define DESTROY_WINDOW_MESSAGE_GET_WID(pBuf) \
    ((((DestroyWindowMessage*)(pBuf))->wid))

#define DESTROY_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((DestroyWindowMessage *)(pBuf))->wid = (windowId)

typedef struct {
    CARD8 	msgType;
    CARD8 	show;		
    CARD8 	transient;
    CARD8 	pad;
    CARD32 	wid;
} ShowWindowMessage;

#define SHOW_WINDOW_MESSAGE_SIZE sizeof(ShowWindowMessage)

#define SHOW_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((ShowWindowMessage *)(pBuf))->msgType = (mType)

#define SHOW_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((ShowWindowMessage *)(pBuf))->wid = (windowId)

#define SHOW_WINDOW_MESSAGE_SET_SHOW(pBuf, showit) \
    ((ShowWindowMessage *)(pBuf))->show = (showit)

#define SHOW_WINDOW_MESSAGE_SET_TRANSIENT(pBuf, trans) \
    ((ShowWindowMessage *)(pBuf))->transient = (trans)

typedef struct {
    CARD8 	msgType;
    CARD8 	pad1;
    CARD16	pad2;
    CARD32      clientId;
    CARD32 	wid;
    CARD16 	x;
    CARD16 	y;
    CARD32 	wAndBorder;    /* Includes 2 * bw */
    CARD32 	hAndBorder;    /* Includes 2 * bw */
    CARD32 	sibid;
} ConfigureWindowMessage;

#define CONFIGURE_WINDOW_MESSAGE_SIZE sizeof(ConfigureWindowMessage)

#define CONFIGURE_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((ConfigureWindowMessage *)(pBuf))->msgType = (mType)

#define CONFIGURE_WINDOW_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((ConfigureWindowMessage *)(pBuf))->clientId = (clientId)

#define CONFIGURE_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((ConfigureWindowMessage *)(pBuf))->wid = (windowId)

#define CONFIGURE_WINDOW_MESSAGE_SET_X(pBuf, xval) \
    ((ConfigureWindowMessage *)(pBuf))->x = (xval)

#define CONFIGURE_WINDOW_MESSAGE_SET_Y(pBuf, yval) \
    ((ConfigureWindowMessage *)(pBuf))->y = (yval)

#define CONFIGURE_WINDOW_MESSAGE_SET_WANDBORDER(pBuf, wandb) \
    ((ConfigureWindowMessage *)(pBuf))->wAndBorder = (wandb)

#define CONFIGURE_WINDOW_MESSAGE_SET_HANDBORDER(pBuf, handb) \
    ((ConfigureWindowMessage *)(pBuf))->hAndBorder = (handb)

#define CONFIGURE_WINDOW_MESSAGE_SET_SIBID(pBuf, siblingId) \
    ((ConfigureWindowMessage *)(pBuf))->sibid = (siblingId)

typedef struct  {
    CARD8 	msgType;
    CARD8 	pad1;
    CARD16 	pad2;
    CARD32      clientId;
    CARD32 	wid;
    CARD16 	x;
    CARD16 	y;
} PositionWindowMessage;

#define POSITION_WINDOW_MESSAGE_SIZE sizeof(PositionWindowMessage)

#define POSITION_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((PositionWindowMessage *)(pBuf))->msgType = (mType)

#define POSITION_WINDOW_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((PositionWindowMessage *)(pBuf))->clientId = (clientId)

#define POSITION_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((PositionWindowMessage *)(pBuf))->wid = (windowId)

#define POSITION_WINDOW_MESSAGE_SET_X(pBuf, xval) \
    ((PositionWindowMessage *)(pBuf))->x = (xval)

#define POSITION_WINDOW_MESSAGE_SET_Y(pBuf, yval) \
    ((PositionWindowMessage *)(pBuf))->y = (yval)

typedef struct  {
    CARD8 	msgType;
    CARD8 	pad1;
    CARD16 	pad2;
    CARD32      clientId;
    CARD32 	wid;
    CARD32 	sibid;
} RestackWindowMessage;

#define RESTACK_WINDOW_MESSAGE_SIZE sizeof(RestackWindowMessage)

#define RESTACK_WINDOW_MESSAGE_SET_TYPE(pBuf, mType) \
    ((RestackWindowMessage *)(pBuf))->msgType = (mType)

#define RESTACK_WINDOW_MESSAGE_SET_CLIENT_ID(pBuf, cid) \
    ((RestackWindowMessage *)(pBuf))->clientId = (clientId)

#define RESTACK_WINDOW_MESSAGE_SET_WID(pBuf, windowId) \
    ((RestackWindowMessage *)(pBuf))->wid = (windowId)

#define RESTACK_WINDOW_MESSAGE_SET_SIBID(pBuf, sid) \
    ((RestackWindowMessage *)(pBuf))->sibid = (sid)

typedef struct {
    CARD8 	msgType;
    CARD8 	decorated;
    CARD16 	pad;
    CARD32 	wid;
} WindowSetDecoratedMessage;

#define WINDOW_SET_DECORATED_MESSAGE_SIZE sizeof(WindowSetDecoratedMessage)

#define WINDOW_SET_DECORATED_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WindowSetDecoratedMessage *)(pBuf))->msgType = (mType)

#define WINDOW_SET_DECORATED_MESSAGE_SET_DECORATED(pBuf, decor) \
    ((WindowSetDecoratedMessage *)(pBuf))->decorated = (decor)

#define WINDOW_SET_DECORATED_MESSAGE_SET_WID(pBuf, windowId) \
    ((WindowSetDecoratedMessage *)(pBuf))->wid = (windowId)

typedef struct {
    CARD8 	msgType;
    CARD8 	pad;
    CARD16 	borderWidth;
    CARD32 	wid;
} WindowSetBorderWidthMessage;

#define WINDOW_SET_BORDER_WIDTH_MESSAGE_SIZE sizeof(WindowSetBorderWidthMessage)

#define WINDOW_SET_BORDER_WIDTH_MESSAGE_SET_TYPE(pBuf, mType) \
    ((WindowSetBorderWidthMessage *)(pBuf))->msgType = (mType)

#define WINDOW_SET_BORDER_WIDTH_MESSAGE_SET_BORDER_WIDTH(pBuf, bw) \
    ((WindowSetBorderWidthMessage *)(pBuf))->borderWidth = (bw)

#define WINDOW_SET_BORDER_WIDTH_MESSAGE_SET_WID(pBuf, windowId) \
    ((WindowSetBorderWidthMessage *)(pBuf))->wid = (windowId)

typedef struct {
    CARD8 	msgType;
} BeepMessage;

#define BEEP_MESSAGE_SIZE sizeof(BeepMessage)

#define BEEP_MESSAGE_SET_TYPE(pBuf, mType) \
    ((BeepMessage *)(pBuf))->msgType = (mType)

#define DISPLAY_PIXELS_ENCODING_UNCODED    0
#define DISPLAY_PIXELS_ENCODING_RLE24      1

/* 
** Note: Don't use xRectangle because x and y are constrained
** to fit in shorts because they are clipped to the screen.
*/

typedef struct {
    CARD8 	msgType;
    CARD8 	encodingType;
    CARD16      x;
    CARD32 	wid;
    CARD16      y;
    CARD16      width;
    CARD16      height;
    CARD16      pad;
} DisplayPixelsMessage;

#define DISPLAY_PIXELS_MESSAGE_SIZE sizeof(DisplayPixelsMessage)

#define DISPLAY_PIXELS_MESSAGE_SET_TYPE(pBuf, mType) \
    ((DisplayPixelsMessage *)(pBuf))->msgType = (mType)

#define DISPLAY_PIXELS_MESSAGE_SET_WID(pBuf, windowId) \
    ((DisplayPixelsMessage *)(pBuf))->wid = (windowId)

#define DISPLAY_PIXELS_MESSAGE_SET_X(pBuf, xvalue) \
    ((DisplayPixelsMessage *)(pBuf))->x = (xvalue)

#define DISPLAY_PIXELS_MESSAGE_SET_Y(pBuf, yvalue) \
    ((DisplayPixelsMessage *)(pBuf))->y = (yvalue)

#define DISPLAY_PIXELS_MESSAGE_SET_WIDTH(pBuf, wvalue) \
    ((DisplayPixelsMessage *)(pBuf))->width = (wvalue)

#define DISPLAY_PIXELS_MESSAGE_SET_HEIGHT(pBuf, hvalue) \
    ((DisplayPixelsMessage *)(pBuf))->height = (hvalue)

#define DISPLAY_PIXELS_MESSAGE_SET_ENCODING(pBuf, encoding) \
    ((DisplayPixelsMessage *)(pBuf))->encodingType = (encoding)

typedef struct {
    CARD8 	msgType;
    CARD8 	pad1;		
    CARD16 	pad2;
    CARD32 	wid;
    CARD32 	srcx;
    CARD32 	srcy;
    CARD32 	width;
    CARD32 	height;
    CARD32 	dstx;
    CARD32 	dsty;
} CopyAreaMessage;

#define COPY_AREA_MESSAGE_SIZE sizeof(CopyAreaMessage)

#define COPY_AREA_MESSAGE_SET_TYPE(pBuf, mType) \
    ((CopyAreaMessage *)(pBuf))->msgType = (mType)

#define COPY_AREA_MESSAGE_SET_WID(pBuf, windowId) \
    ((CopyAreaMessage *)(pBuf))->wid = (windowId)

#define COPY_AREA_MESSAGE_SET_SRCX(pBuf, sx) \
    ((CopyAreaMessage *)(pBuf))->srcx = (sx)

#define COPY_AREA_MESSAGE_SET_SRCY(pBuf, sy) \
    ((CopyAreaMessage *)(pBuf))->srcy = (sy)

#define COPY_AREA_MESSAGE_SET_WIDTH(pBuf, w) \
    ((CopyAreaMessage *)(pBuf))->width = (w)

#define COPY_AREA_MESSAGE_SET_HEIGHT(pBuf, h) \
    ((CopyAreaMessage *)(pBuf))->height = (h)

#define COPY_AREA_MESSAGE_SET_DSTX(pBuf, dx) \
    ((CopyAreaMessage *)(pBuf))->dstx = (dx)

#define COPY_AREA_MESSAGE_SET_DSTY(pBuf, dy) \
    ((CopyAreaMessage *)(pBuf))->dsty = (dy)

typedef struct controllerstatusmessage_struct {
    CARD8 	msgType;			
    CARD8       status;
    CARD16      pad;
    CARD32      clientId;
} ControllerStatusMessage;

/* The attempt of the specified client to take control has been refused */
#define CONTROLLER_STATUS_REFUSED        0

/* The specified client has lost control */
#define CONTROLLER_STATUS_LOST           1

/* The specified client has gained control */
#define CONTROLLER_STATUS_GAINED         2

#define CONTROLLER_STATUS_MESSAGE_SIZE sizeof(ControllerStatusMessage)

#define CONTROLLER_STATUS_MESSAGE_SET_TYPE(pBuf, mType) \
    ((ControllerStatusMessage *)(pBuf))->msgType = (mType)

#define CONTROLLER_STATUS_MESSAGE_SET_STATUS(pBuf, stat) \
    ((ControllerStatusMessage*)(pBuf))->status = (stat)

#define CONTROLLER_STATUS_MESSAGE_SET_CLIENTID(pBuf, cid) \
    ((ControllerStatusMessage*)(pBuf))->clientId = (cid)

typedef struct displaycursormessage_struct {

    CARD8 	msgType;			
    CARD8       pad1;
    CARD16      pad2;
    CARD16      width;
    CARD16      height;
    CARD16      xhot;
    CARD16      yhot;

    /* Followed by (width * height) 32-bit pixels */

} DisplayCursorMessage;

#define DISPLAY_CURSOR_MESSAGE_SIZE sizeof(DisplayCursorMessage)

#define DISPLAY_CURSOR_MESSAGE_SET_TYPE(pBuf, mType) \
    ((DisplayCursorMessage *)(pBuf))->msgType = (mType)

#define DISPLAY_CURSOR_MESSAGE_SET_WIDTH(pBuf, w) \
    ((DisplayCursorMessage *)(pBuf))->width = (w)

#define DISPLAY_CURSOR_MESSAGE_SET_HEIGHT(pBuf, h) \
    ((DisplayCursorMessage *)(pBuf))->height = (h)

#define DISPLAY_CURSOR_MESSAGE_SET_XHOT(pBuf, xh) \
    ((DisplayCursorMessage *)(pBuf))->xhot = (xh)

#define DISPLAY_CURSOR_MESSAGE_SET_YHOT(pBuf, yh) \
    ((DisplayCursorMessage *)(pBuf))->yhot = (yh)

typedef struct movecursormessage_struct {
    CARD8 	msgType;			
    CARD8       pad1;
    CARD16      pad2;
    CARD32      wid;
    CARD32      x;
    CARD32      y;
} MoveCursorMessage;

#define MOVE_CURSOR_MESSAGE_SIZE sizeof(MoveCursorMessage)

#define MOVE_CURSOR_MESSAGE_SET_TYPE(pBuf, mType) \
    ((MoveCursorMessage *)(pBuf))->msgType = (mType)

#define MOVE_CURSOR_MESSAGE_SET_WID(pBuf, windowId) \
    ((MoveCursorMessage *)(pBuf))->wid = (windowId)

#define MOVE_CURSOR_MESSAGE_SET_X(pBuf, cx) \
    ((MoveCursorMessage *)(pBuf))->x = (cx)

#define MOVE_CURSOR_MESSAGE_SET_Y(pBuf, cy) \
    ((MoveCursorMessage *)(pBuf))->y = (cy)

typedef struct showcursormessage_struct {
    CARD8 	msgType;			
    CARD8       show;
} ShowCursorMessage;

#define SHOW_CURSOR_MESSAGE_SIZE sizeof(ShowCursorMessage)

#define SHOW_CURSOR_MESSAGE_SET_TYPE(pBuf, mType) \
    ((ShowCursorMessage *)(pBuf))->msgType = (mType)

#define SHOW_CURSOR_MESSAGE_SET_SHOW(pBuf, showit) \
    ((ShowCursorMessage *)(pBuf))->show = (showit)

#endif /* PROTOCOL_H */
