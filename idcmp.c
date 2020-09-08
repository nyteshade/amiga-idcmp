#include <proto/exec.h>
#include <proto/intuition.h>
#include <string.h>

#include "support/gcc8_c_support.h"
#include "idcmp.h"

#define CLEARBIT(number, bit)    (number &= ~(1 << bit))
#define TOGGLEBIT(number, bit)   (number ^= 1 << bit)
#define SETBIT(number, bit)      (number |= 1 << bit)

/**
 * Predefined basic function that ensures the application message loop exits
 * when the close gadget is pressed. 
 * 
 * @param window an instance of `struct Window *` that represents the open 
 * window being acted upon
 * @param message an instance of `struct IntuiMessage *` indicating the message
 * with the `IDCMP_CLOSEWINDOW` class.
 * @returns `LOOPSTATE_FINISHED` which is a value that modifies the `done` 
 * state of the active message loop
 */
MessageLoopState
__idcmp_events_close_window__(
   struct Window *window, 
   struct IntuiMessage *message
) {
   return LOOPSTATE_FINISHED;
}

/**
 * Given a pointer to a `IDCMPEvents` structure, this method will, at the very
 * least, zero all the values within. This makes it easy and safe to use the 
 * structure with partially filled out method invocations. 
 * 
 * @param events a pointer to a `IDCMPEvents` structure
 */
void 
InitializeIDCMPEvents(IDCMPEvents *events) {
   memset(events, 0L, sizeof(IDCMPEvents));
}

/**
 * For those folks not wanting to write their own closeWindow handler, this
 * method will apply a basic one that terminates the active message loop.
 * 
 * @param events a pointer to a `IDCMPEvents` structure
 */
void 
ApplyIDCMPBasics(IDCMPEvents *events) {
   events->closeWindow = __idcmp_events_close_window__;
}

/**
 * This function performs the `Wait` calls on the `UserPort`'s `mp_SigBit`
 * property. It does so as long as each call to `ProcessIDCMPMessage` 
 * returns `TRUE`. To perform the work at hand, three values are required
 * from the invoking user; a pointer to a `IDCMPEvents` structure that 
 * contains all the function handlers for the various types of events, a
 * pointer to a `Window` structure that refer to the window being handled
 * and an initial `BOOL` value to determine the starting `done` state. If 
 * the initial done state is TRUE, indicating the loop is done then loop 
 * will process only once; unless some handler returns FALSE explicitly
 */
void
HandleIDCMP(
   IDCMPEvents *events, 
   struct Window *window, 
   MessageLoopState initialDone
) {
   MessageLoopState done = initialDone;

   do {
      ULONG signals = Wait(1L << window->UserPort->mp_SigBit);
      if (signals & (1L << window->UserPort->mp_SigBit)) {
         MessageLoopState state = ProcessIDCMPMessage(events, window);

         if (state != LOOPSTATE_NO_CHANGE) {
            done = state;
         }
      }
   }
   while (done != LOOPSTATE_FINISHED);
}

/**
 * The primary loop provided by this small library. The `HandleIDCMP` 
 * function will receive a pointer to a `IDCMPEvents` structure defining
 * all the desired handlers and a pointer to a `Window` structure to know 
 * which window we are working on.
 * 
 * @param events a pointer to a `IDCMPEvents` structure containing all the
 * handlers for use when working with Intuition windows
 * @param window a pointer to a `Window` structure for which the IDCMP events 
 * pertain to
 * @param intialDone a TRUE or FALSE value that denotes the number of times
 * the messages will be 
 */
MessageLoopState 
ProcessIDCMPMessage(
   IDCMPEvents *events, 
   struct Window *window
) {
   struct IntuiMessage *message;
   USHORT code;
   SHORT mousex, mousey;
   ULONG class;
   IDCMPMouseButton buttons = NO_BUTTON;

   while (NULL != (message = (struct IntuiMessage *)GetMsg(window->UserPort))) {
      class = message->Class;
      code = message->Code;
      mousex = message->MouseX;
      mousey = message->MouseY;

      ReplyMsg((struct Message *)message);

      switch (class) {
         case IDCMP_ACTIVEWINDOW:
            if (events->activeWindow) {
               return events->activeWindow(window, message);
            }
            break;

         case IDCMP_CHANGEWINDOW:
            if (events->changeWindow) {
               return events->changeWindow(window, message);
            }
            break;

         case IDCMP_CLOSEWINDOW:
            if (events->closeWindow) {
               return events->closeWindow(window, message);
            }
            break;

         case IDCMP_DELTAMOVE:
            if (events->deltaMove) {
               return events->deltaMove(window, message);
            }
            break;

         case IDCMP_DISKINSERTED:
            if (events->diskInserted) {
               return events->diskInserted(window, message);
            }
            break;

         case IDCMP_DISKREMOVED:
            if (events->diskRemoved) {
               return events->diskRemoved(window, message);
            }
            break;

         case IDCMP_GADGETDOWN:
            if (events->gadgetDown) {
               return events->gadgetDown(window, message);
            }
            break;

         case IDCMP_GADGETHELP:
            if (events->gadgetHelp) {
               return events->gadgetHelp(window, message);
            }
            break;

         case IDCMP_GADGETUP:
            if (events->gadgetUp) {
               return events->gadgetUp(window, message);
            }
            break;

         case IDCMP_IDCMPUPDATE:
            if (events->idcmpUpdate) {
               return events->idcmpUpdate(window, message);
            }
            break;

         case IDCMP_INACTIVEWINDOW:
            if (events->inactiveWindow) {
               return events->inactiveWindow(window, message);
            }
            break;

         case IDCMP_INTUITICKS:
            if (events->intuiTicks) {
               return events->intuiTicks(window, message);
            }
            break;

         case IDCMP_LONELYMESSAGE:
            if (events->lonelyMessage) {
               return events->lonelyMessage(window, message);
            }
            break;

         case IDCMP_MENUHELP:
            if (events->menuHelp) {
               return events->menuHelp(window, message);
            }
            break;

         case IDCMP_MENUPICK:
            if (events->menuPick) {
               return events->menuPick(window, message);
            }
            break;


         case IDCMP_MENUVERIFY:
            if (events->menuVerify) {
               return events->menuVerify(window, message);
            }
            break;

         case IDCMP_MOUSEBUTTONS:
            switch (code) {
               case SELECTUP:
                  if ((buttons & LEFT_MOUSE_DOWN) == LEFT_MOUSE_DOWN) {
                     buttons ^= LEFT_MOUSE_DOWN;
                  }

                  buttons |= LEFT_MOUSE_UP;
                  break;
               
               case SELECTDOWN:
                  if ((buttons & LEFT_MOUSE_UP) == LEFT_MOUSE_UP) {
                     buttons ^= LEFT_MOUSE_UP;
                  }

                  buttons |= LEFT_MOUSE_DOWN;
                  break;
               
               case MENUUP:
                  if ((buttons & RIGHT_MOUSE_DOWN) == RIGHT_MOUSE_DOWN) {
                     buttons ^= RIGHT_MOUSE_DOWN;
                  }

                  buttons |= RIGHT_MOUSE_UP;
                  break;
               
               case MENUDOWN:
                  if ((buttons & RIGHT_MOUSE_UP) == LEFT_MOUSE_UP) {
                     buttons ^= LEFT_MOUSE_UP;
                  }

                  buttons |= LEFT_MOUSE_DOWN;
                  break;

               case MIDDLEDOWN:
                  if ((buttons & MIDDLE_MOUSE_UP) == MIDDLE_MOUSE_UP) {
                     buttons ^= MIDDLE_MOUSE_UP;
                  }

                  buttons |= MIDDLE_MOUSE_DOWN;
                  break;

               case MIDDLEUP:
                  if ((buttons & MIDDLE_MOUSE_DOWN) == MIDDLE_MOUSE_DOWN) {
                     buttons ^= MIDDLE_MOUSE_DOWN;
                  }

                  buttons |= MIDDLE_MOUSE_UP;
                  break;

               default:
                  buttons |= code;
                  break;
            }
            
            if (events->mouseButtons) {
               return events->mouseButtons(window, message, buttons);
            }
            break;

         case IDCMP_MOUSEMOVE:
            if (events->mouseMove) {
               BOOL gz = (window->Flags & WFLG_GIMMEZEROZERO) == WFLG_GIMMEZEROZERO;
               WORD x = gz ? window->GZZMouseX : window->MouseX;
               WORD y = gz ? window->GZZMouseY : window->MouseY;

               return events->mouseMove(window, message, x, y);
            }
            break;

         case IDCMP_NEWPREFS:
            if (events->newPrefs) {
               return events->newPrefs(window, message);
            }
            break;

         case IDCMP_NEWSIZE:
            if (events->newSize) {
               return events->newSize(window, message);
            }
            break;

         case IDCMP_RAWKEY:
            if (events->rawKey) {
               return events->rawKey(window, message);
            }
            break;

         case IDCMP_REFRESHWINDOW:
            if (events->refreshWindow) {
               return events->refreshWindow(window, message);
            }
            break;

         case IDCMP_REQCLEAR:
            if (events->reqClear) {
               return events->reqClear(window, message);
            }
            break;

         case IDCMP_REQSET:
            if (events->reqSet) {
               return events->reqSet(window, message);
            }
            break;

         case IDCMP_REQVERIFY:
            if (events->reqVerify) {
               return events->reqVerify(window, message);
            }
            break;

         case IDCMP_SIZEVERIFY:
            if (events->sizeVerify) {
               return events->sizeVerify(window, message);
            }
            break;

         case IDCMP_VANILLAKEY:
            if (events->vanillaKey) {
               return events->vanillaKey(window, message);
            }
            break;

         case IDCMP_WBENCHMESSAGE:
            if (events->workbenchMessage) {
               return events->workbenchMessage(window, message);
            }
            break;
                  
         default:
            KPrintF("Unknown IDCMP message (%ld)\n", class);
            break;
      }
   }

   return LOOPSTATE_NO_CHANGE;
}