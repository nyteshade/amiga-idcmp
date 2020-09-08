#ifndef IDCMP_H
#define IDCMP_H 1

#include <intuition/intuition.h>
#include <proto/intuition.h>

#define ALL_IDCMP_EVENTS \
   IDCMP_ACTIVEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_CLOSEWINDOW | \
   IDCMP_DELTAMOVE | IDCMP_DISKINSERTED | IDCMP_DISKREMOVED | \
   IDCMP_GADGETDOWN | IDCMP_GADGETHELP | IDCMP_GADGETUP | IDCMP_IDCMPUPDATE | \
   IDCMP_INACTIVEWINDOW | IDCMP_INTUITICKS | IDCMP_LONELYMESSAGE | \
   IDCMP_MENUHELP | IDCMP_MENUPICK | IDCMP_MENUVERIFY | IDCMP_MOUSEBUTTONS | \
   IDCMP_MOUSEMOVE | IDCMP_NEWPREFS | IDCMP_NEWSIZE | IDCMP_RAWKEY | \
   IDCMP_REFRESHWINDOW | IDCMP_REQCLEAR | IDCMP_REQSET | IDCMP_REQVERIFY | \
   IDCMP_SIZEVERIFY | IDCMP_VANILLAKEY | IDCMP_WBENCHMESSAGE

typedef enum IDCMPMouseButton {
   LEFT_MOUSE_UP = SELECTUP,
   LEFT_MOUSE_DOWN = SELECTDOWN,
   MIDDLE_MOUSE_UP = MIDDLEUP,
   MIDDLE_MOUSE_DOWN = MIDDLEDOWN,
   RIGHT_MOUSE_UP = MENUUP,
   RIGHT_MOUSE_DOWN = MENUDOWN,
   NO_BUTTON = 0
} IDCMPMouseButton;

typedef enum MessageLoopState {
   LOOPSTATE_FINISHED = TRUE,
   LOOPSTATE_CONTINUE = FALSE,
   LOOPSTATE_NO_CHANGE = 2
} MessageLoopState;

typedef struct IDCMPEvents {
	MessageLoopState
   (*activeWindow)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*changeWindow)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*closeWindow)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*deltaMove)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*diskInserted)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*diskRemoved)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*gadgetDown)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*gadgetHelp)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*gadgetUp)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*idcmpUpdate)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*inactiveWindow)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*intuiTicks)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*lonelyMessage)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*menuHelp)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*menuPick)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*menuVerify)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*mouseButtons)(
      struct Window *window, 
      struct IntuiMessage *message, 
      IDCMPMouseButton buttons
   );

   MessageLoopState
   (*mouseMove)(
      struct Window *window, 
      struct IntuiMessage *message, 
      WORD x, 
      WORD y
   );

   MessageLoopState
   (*newPrefs)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*newSize)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*rawKey)(struct Window *window, struct IntuiMessage *message);
   
   MessageLoopState
   (*refreshWindow)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*reqClear)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*reqSet)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*reqVerify)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*sizeVerify)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*vanillaKey)(struct Window *window, struct IntuiMessage *message);

   MessageLoopState
   (*workbenchMessage)(struct Window *window, struct IntuiMessage *message);
} IDCMPEvents;

void 
InitializeIDCMPEvents(IDCMPEvents *events);

void 
ApplyIDCMPBasics(IDCMPEvents *events);

void
HandleIDCMP(
   IDCMPEvents *events, 
   struct Window *window, 
   MessageLoopState initialDone
);

MessageLoopState 
ProcessIDCMPMessage(IDCMPEvents *events, struct Window *window);

#endif