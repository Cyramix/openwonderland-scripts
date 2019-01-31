/*****************************************************************

Copyright 2008 Sun Microsystems, Inc.

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

/*
** The XRemwin Client Manager keeps track of which application clients
** are connected. After all connected clients disconnect it institutes 
** an orderly shutdown of the Xremwin server. This shutdown process is
** cancelled if a new client connects during this time. We do this to 
** handle some apps (such as Open Office) which sometimes connect for
** a brief instant to do initialization and then disconnect and reconnect.
** 
** During the first phase of the shutdown process we first quickly take away 
** control from any wonderland client which has control. We do this, by default, 
** after two seconds but can be changed by setting the REMWIN_CONTROL_LOSS_TIMEOUT
** environment variable (in seconds). Thus, if the server is really about to go 
** down, this allows any controlling wonderland client to quickly resume 
** controlling the world. In order to time this, we use a timer called the 
** "control loss" timer which counts down to zero.
**
** After we take away control, we start a second "shutdown" timer which again
** counts down to zero. When this timer hits zero the server is finally shutdown.
** By default, the shutdown timer expires 28 seconds after the control loss timer
** expires. This value can be changed by setting the REMWIN_SHUT_DOWN_TIMEOUT
** environment variable (in seconds).
**
** The reason why we need to shut down in this way is to handle clients which
** connect, disconnect and reconnect (perhaps) multiple times at the start of 
** an app session. If we were to shut down the server shortly after there were
** no connected clients these applications would fail to start properly. So we
** need to give a reasonable amount of time for the server to linger until it
** knows that no more clients are going to connect. 
**
** So why don't we just simply let the server exit after 30 seconds? Why do
** we take away control? The reason is because the application code
** running in the wonderland client doesn't exit until the server goes away.
** So if you are controlling an app and hit the close button on the window
** frame the app will go away but your input devices will continue to be in 
** "app mode" and you will be unable to move your avatar. This is why we
** take away control quickly.
**
** But doesn't this cause us to lose control of an app when we don't want to?
** Most of the time this shouldn't be a problem. Most of the disconnect/reconnect
** behavior of apps occurs when the app is first starting up and the user has not
** yet had a chance to take control.
*/

#include <unistd.h>
#include "dixstruct.h"
#include "misc.h"
#include "remwin.h"

#define DEBUG(format) do { \
    if (pClientMgr->debug) ErrorF(format); \
} while (0)

#define DEBUG_ARG(format, arg) do { \
    if (pClientMgr->debug) ErrorF((format), (arg)); \
} while (0)

#define DEBUG_STATE(format, state) do {	\
    if (pClientMgr->debug) { \
        ErrorF(format); \
        printState(state); \
    } \
} while (0)

#define DEBUG_ACTION(format, action) do { \
    if (pClientMgr->debug) { \
        ErrorF(format); \
        printAction(action); \
    } \
} while (0)

static int getIntFromEnvVar (char *envVarName, int def);
static void clientConnect (RemwinClientManagerPtr pClientMgr, ClientPtr pClient);
static void clientDisconnect (RemwinClientManagerPtr pClientMgr, ClientPtr pClient);
static void applyAction (RemwinClientManagerPtr pClientMgr, int action);
static void printState (int state);
static void printAction (int action);
static void error (char *message);
static void errorIllegalState (RemwinClientManagerPtr pClientMgr, int action);
static void errorIllegalActionInState (RemwinClientManagerPtr pClientMgr, int action);

/**
 * Create a new client manager.
 */
RemwinClientManagerPtr
rwclientmgrCreate (ScreenPtr pScreen) 
{
    RemwinClientManagerPtr pClientMgr;
    int numSecs;
    int i;

    pClientMgr = (RemwinClientManagerPtr) xalloc(sizeof(RemwinClientManagerRec));
    if (pClientMgr == NULL) {
        return NULL;
    }

    pClientMgr->numClientsConnected = 0;
    pClientMgr->connected = (Bool *) xalloc(MAXCLIENTS * sizeof(Bool));
    if (pClientMgr->connected == NULL) {
        xfree(pClientMgr);
        return NULL;
    }
    for (i = 0; i < MAXCLIENTS; i++) {
        pClientMgr->connected[i] = FALSE;
    }

    numSecs = getIntFromEnvVar("REMWIN_CONTROL_LOSS_TIMEOUT", REMWIN_CONTROL_LOSS_TIMEOUT_DEFAULT);
    pClientMgr->armTicksControlLoss = numSecs * (int)MAX_OUTPUT_RATE;
    /*DEBUG_ARG("armTicksControlLoss = %d\n", pClientMgr->armTicksControlLoss);*/

    numSecs = getIntFromEnvVar("REMWIN_SHUT_DOWN_TIMEOUT", REMWIN_SHUT_DOWN_TIMEOUT_DEFAULT);
    pClientMgr->armTicksShutDown = numSecs * (int)MAX_OUTPUT_RATE;;
    /*DEBUG_ARG("armTicksShutDown = %d\n", pClientMgr->armTicksShutDown);*/
						   
    pClientMgr->pScreen = pScreen;
    pClientMgr->state = CLIENT_MANAGER_STATE_INITIAL;
    pClientMgr->timerCountControlLoss = 0;
    pClientMgr->timerCountShutDown = 0;

    int value = getIntFromEnvVar("REMWIN_CLIENT_MANAGER_DEBUG", 0);
    pClientMgr->debug = value ? TRUE : FALSE;

    return pClientMgr;
}

/**
 * Deallocate a client manager.
 */
void
rwclientmgrDestroy (RemwinClientManagerPtr pClientMgr) 
{
    xfree(pClientMgr->connected);  
    xfree(pClientMgr);
}

static int
getIntFromEnvVar (char *envVarName, int def) 
{
    char *envVarValue = getenv(envVarName);

    if (envVarValue == NULL || *envVarValue == '\0') {
        return def;
    }

    int value = atoi(envVarValue);
    ErrorF("Environment variable $%s value = %d\n", envVarName, value);

    return value;
}

void
rwclientmgrClientStateChange (RemwinClientManagerPtr pClientMgr, ClientPtr pClient) 
{
    if (pClient->clientGone) {

        /* A client has disconnected */
	if (!pClientMgr->connected[pClient->index]) {
	    ErrorF("XRemWin Client Manager: Internal error: spurious disconnect when client already disconnected\n");
	    return;
	}

	clientDisconnect(pClientMgr, pClient);

    } else {

	if (pClientMgr->connected[pClient->index]) {
	    /* The client state change was not related to the connection */
	    return;
	}

        /* A new client has connected */
	clientConnect(pClientMgr, pClient);
    }
}

/**
 * Record that a new client has connected and perform the connect action.
 */
static void
clientConnect (RemwinClientManagerPtr pClientMgr, ClientPtr pClient) 
{
    pClientMgr->connected[pClient->index] = TRUE;
    pClientMgr->numClientsConnected++;
    applyAction(pClientMgr, CLIENT_MANAGER_ACTION_CLIENT_CONNECTED);
}

/**
 * Record that a client has disconnected and perform the disconnect action.
 */
static void
clientDisconnect (RemwinClientManagerPtr pClientMgr, ClientPtr pClient) 
{
    pClientMgr->connected[pClient->index] = FALSE;
    pClientMgr->numClientsConnected--;
    if (pClientMgr->numClientsConnected <= 0) {
        applyAction(pClientMgr, CLIENT_MANAGER_ACTION_LAST_CLIENT_DISCONNECTED);
    }
}

/**
 * On every clock tick decrement any timer that is active and perform the 
 * proper action if the timer expires.
 */
void
rwclientmgrTimerTick (RemwinClientManagerPtr pClientMgr) 
{
    if (pClientMgr->state == CLIENT_MANAGER_STATE_NO_CLIENTS_CONTROL_LOSS_PENDING) {

        /* The control-loss timer is running. Decrement its count */
        pClientMgr->timerCountControlLoss--;
        if (pClientMgr->timerCountControlLoss <= 0) {
            applyAction(pClientMgr, CLIENT_MANAGER_ACTION_CONTROL_LOSS_TIMEOUT);
        }

    } else if (pClientMgr->state == CLIENT_MANAGER_STATE_NO_CLIENTS_SHUT_DOWN_PENDING) {

        /* The shutdown timer is running. Decrement its count */
        if (pClientMgr->timerCountShutDown <= 0) {
	    applyAction(pClientMgr, CLIENT_MANAGER_ACTION_SHUT_DOWN_TIMEOUT);
	}
	pClientMgr->timerCountShutDown--;

    } else {
        /* No timer is active */
    }
}

/**
 * Client Manager State Machine: Calculate the next state based on the action.
 * Call any appropriate routines.
 */
static void
applyAction (RemwinClientManagerPtr pClientMgr, int action) 
{
    DEBUG_STATE("State Before: ", pClientMgr->state);
    DEBUG_ACTION("Action: ", action);

    switch (pClientMgr->state) {

    case CLIENT_MANAGER_STATE_INITIAL:
        switch (action) {

	case CLIENT_MANAGER_ACTION_CLIENT_CONNECTED:
  	    pClientMgr->state = CLIENT_MANAGER_STATE_HAS_CLIENTS;
	    break;

	default:
	    errorIllegalActionInState(pClientMgr, action);
	}
	break;

    case CLIENT_MANAGER_STATE_HAS_CLIENTS:
        switch (action) {

	case CLIENT_MANAGER_ACTION_CLIENT_CONNECTED:
	    /* No change */
	    break;

	case CLIENT_MANAGER_ACTION_LAST_CLIENT_DISCONNECTED:
  	    /* Arm the control loss timer */
	    pClientMgr->timerCountControlLoss = pClientMgr->armTicksControlLoss;
	    pClientMgr->state = CLIENT_MANAGER_STATE_NO_CLIENTS_CONTROL_LOSS_PENDING;
	    break;

	default:
	    errorIllegalActionInState(pClientMgr, action);
	}
        break;

    case CLIENT_MANAGER_STATE_NO_CLIENTS_CONTROL_LOSS_PENDING:
        switch (action) {

	case CLIENT_MANAGER_ACTION_CLIENT_CONNECTED:
	    /* This stops the timer */
	    pClientMgr->state = CLIENT_MANAGER_STATE_HAS_CLIENTS;
	    DEBUG("Control Loss Timer Stopped\n");
	    break;

	case CLIENT_MANAGER_ACTION_CONTROL_LOSS_TIMEOUT:
	    /* The control-loss timer has expired */
	    
	    /* Take away control from the current controller */
	    rwTakeAwayControl(pClientMgr->pScreen);
	    DEBUG("Control Taken Away\n");

	    /* Arm the shutdown timer */
	    pClientMgr->timerCountShutDown = pClientMgr->armTicksShutDown;
	    pClientMgr->state = CLIENT_MANAGER_STATE_NO_CLIENTS_SHUT_DOWN_PENDING;
	    break;

	default:
	    errorIllegalActionInState(pClientMgr, action);
	}
        break;

    case CLIENT_MANAGER_STATE_NO_CLIENTS_SHUT_DOWN_PENDING:
        switch (action) {

	case CLIENT_MANAGER_ACTION_CLIENT_CONNECTED:
	    /* This stops the timer */
	    DEBUG("Shut Down Timer Stopped\n");
	    pClientMgr->state = CLIENT_MANAGER_STATE_HAS_CLIENTS;
	    break;

	case CLIENT_MANAGER_ACTION_SHUT_DOWN_TIMEOUT:
	    /* The shutdown timer has expired. We now shut down the server */
	    DEBUG("Initiate Shutdown\n");
	    rwShutdown(pClientMgr->pScreen);
	    /* Note: we never get here */
	    break;

	default:
	    errorIllegalActionInState(pClientMgr, action);
	}
        break;

    default:
        errorIllegalState(pClientMgr, action);
    }

    DEBUG_STATE("State After: ", pClientMgr->state);
}

static void
printState (int state) 
{
    ErrorF("state = ");
    switch (state) {
    case CLIENT_MANAGER_STATE_INITIAL:
        ErrorF("INITIAL");
        break;  
    case CLIENT_MANAGER_STATE_HAS_CLIENTS:
        ErrorF("HAS_CLIENTS");
        break;
    case CLIENT_MANAGER_STATE_NO_CLIENTS_CONTROL_LOSS_PENDING:
        ErrorF("CONTROL_LOSS_PENDING");
        break;
    case CLIENT_MANAGER_STATE_NO_CLIENTS_SHUT_DOWN_PENDING:
        ErrorF("SHUT_DOWN_PENDING");
        break;
    default:
        ErrorF("*** Illegal State ***");
    }
    ErrorF("\n");
}

static void
printAction (int action) 
{
    ErrorF("action = ");
    switch (action) {
    case CLIENT_MANAGER_ACTION_CLIENT_CONNECTED:
        ErrorF("CLIENT_CONNECTED");
        break;
    case CLIENT_MANAGER_ACTION_LAST_CLIENT_DISCONNECTED:
        ErrorF("LAST_CLIENT_DISCONNECTED");
        break;
    case CLIENT_MANAGER_ACTION_CONTROL_LOSS_TIMEOUT:
        ErrorF("CONTROL_LOSS_TIMEOUT");
        break;
    case CLIENT_MANAGER_ACTION_SHUT_DOWN_TIMEOUT:
        ErrorF("SHUT_DOWN_TIMEOUT");
        break;
    default:
        ErrorF("*** Illegal Action ***");
    }
    ErrorF("\n");
}

static void
error (char *message) 
{
    ErrorF("Xremwin ClientManager Error: %s\n", message);
}

static void
errorIllegalState (RemwinClientManagerPtr pClientMgr, int action) 
{
  error("Illegal state");
  printState(pClientMgr->state);
  printAction(action);
}

static void
errorIllegalActionInState (RemwinClientManagerPtr pClientMgr, int action) 
{
  error("Illegal action in state");
  printState(pClientMgr->state);
  printAction(action);
}

