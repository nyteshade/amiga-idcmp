#define REXX_STORAGE_H TRUE

#include <exec/exec.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <string.h>

#include "idcmp.h"

/**
 * Predefined basic function that ensures the application message loop exits
 * when the close gadget is pressed. 
 * 
 * @param window an instance of `struct Window *` that represents the open 
 * window being acted upon
 * @param message an instance of `struct IntuiMessage *` indicating the message
 * with the `IDCMP_CLOSEWINDOW` class.
 * @returns ` STATE_FINISHED` which is a value that modifies the `done` 
 * state of the active message loop
 */
 IDCMPState
__idcmp_events_close_window__(
   struct Window *window, 
   struct IntuiMessage *message
) {
   if (window) {
      CloseWindow(window);
   }

   return  STATE_FINISHED;
}

void 
InitializeIDCMPEvents(IDCMPEvents *events) {
   memset(events, 0L, sizeof(IDCMPEvents));

   /* Prepare the exec list for the gadget handlers */
   events->GadgetEvents = AllocVec(sizeof(struct List), MEMF_ANY|MEMF_CLEAR);
   NewList(events->GadgetEvents);
}

void 
FreeIDCMPEvents(IDCMPEvents *events, BOOL freeOnlyContents) {
   struct List *list = events != NULL ? events->GadgetEvents : NULL;
   struct Node *node = NULL;

   if (list == NULL || events == NULL) {
      return;
   }

   while (!IsListEmpty(list) && (node = list->lh_Head)) {
      GadgetEventNode *gadgetNode = (GadgetEventNode *)node;
      RemHead(list);

      if (gadgetNode) {
         FreeVec(gadgetNode);
      }
   }

   if (!freeOnlyContents) {
      FreeVec(list);
   }
}

void 
ApplyIDCMPBasics(IDCMPEvents *events) {
   events->DismissWindow = __idcmp_events_close_window__;
}

void
HandleIDCMP(
   IDCMPEvents *events, 
   struct Window *window, 
    IDCMPState initialDone
) {
    IDCMPState done = initialDone;

   do {
      ULONG signals = Wait(1L << window->UserPort->mp_SigBit);
      if (signals & (1L << window->UserPort->mp_SigBit)) {
          IDCMPState state = ProcessIDCMPMessage(events, window);

         if (state !=  STATE_NO_CHANGE) {
            done = state;
         }
      }
   }
   while (done !=  STATE_FINISHED);
}

IDCMPState 
ProcessIDCMPMessage(
   IDCMPEvents *events, 
   struct Window *window
) {
   struct IntuiMessage *message;
   USHORT code;
   ULONG class;
   IDCMPMouseButton buttons = NO_BUTTON;

   while (NULL != (message = (struct IntuiMessage *)GetMsg(window->UserPort))) {
      class = message->Class;
      code = message->Code;

      ReplyMsg((struct Message *)message);

      switch (class) {
         case IDCMP_ACTIVEWINDOW:
            if (events->ActiveWindow) {
               return events->ActiveWindow(window, message);
            }
            break;

         case IDCMP_CHANGEWINDOW:
            if (events->ChangeWindow) {
               return events->ChangeWindow(window, message);
            }
            break;

         case IDCMP_CLOSEWINDOW:
            if (events->DismissWindow) {
               return events->DismissWindow(window, message);
            }
            break;

         case IDCMP_DELTAMOVE:
            if (events->DeltaMove) {
               return events->DeltaMove(window, message);
            }
            break;

         case IDCMP_DISKINSERTED:
            if (events->DiskInserted) {
               return events->DiskInserted(window, message);
            }
            break;

         case IDCMP_DISKREMOVED:
            if (events->DiskRemoved) {
               return events->DiskRemoved(window, message);
            }
            break;

         case IDCMP_GADGETDOWN:
            if (events->GadgetDown) {
               struct Gadget *gadget = (struct Gadget *)message->IAddress;
               return events->GadgetDown(window, message, gadget);
            }
            break;

         case IDCMP_GADGETHELP:
            if (events->GadgetHelp) {
               struct Gadget *gadget = (struct Gadget *)message->IAddress;
               return events->GadgetHelp(window, message, gadget);
            }
            break;

         case IDCMP_GADGETUP:
            if (events->GadgetUp) {
               struct Gadget *gadget = (struct Gadget *)message->IAddress;
               return events->GadgetUp(window, message, gadget);
            }
            break;

         case IDCMP_IDCMPUPDATE:
            if (events->IdcmpUpdate) {
               return events->IdcmpUpdate(window, message);
            }
            break;

         case IDCMP_INACTIVEWINDOW:
            if (events->InactiveWindow) {
               return events->InactiveWindow(window, message);
            }
            break;

         case IDCMP_INTUITICKS:
            if (events->IntuiTicks) {
               return events->IntuiTicks(window, message);
            }
            break;

         case IDCMP_LONELYMESSAGE:
            if (events->LonelyMessage) {
               return events->LonelyMessage(window, message);
            }
            break;

         case IDCMP_MENUHELP:
            if (events->MenuHelp) {
               return events->MenuHelp(window, message);
            }
            break;

         case IDCMP_MENUPICK:
            if (events->MenuPick) {
               return events->MenuPick(window, message);
            }
            break;


         case IDCMP_MENUVERIFY:
            if (events->MenuVerify) {
               return events->MenuVerify(window, message);
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
            
            if (events->MouseButtons) {
               return events->MouseButtons(window, message, buttons);
            }
            break;

         case IDCMP_MOUSEMOVE:
            if (events->MouseMove) {
               BOOL gz = (window->Flags & WFLG_GIMMEZEROZERO) == WFLG_GIMMEZEROZERO;
               WORD x = gz ? window->GZZMouseX : window->MouseX;
               WORD y = gz ? window->GZZMouseY : window->MouseY;

               return events->MouseMove(window, message, x, y);
            }
            break;

         case IDCMP_NEWPREFS:
            if (events->NewPrefs) {
               return events->NewPrefs(window, message);
            }
            break;

         case IDCMP_NEWSIZE:
            if (events->NewSize) {
               return events->NewSize(window, message);
            }
            break;

         case IDCMP_RAWKEY:
            if (events->RawKey) {
               return events->RawKey(window, message);
            }
            break;

         case IDCMP_REFRESHWINDOW:
            if (events->RefreshWindow) {
               return events->RefreshWindow(window, message);
            }
            break;

         case IDCMP_REQCLEAR:
            if (events->ReqClear) {
               return events->ReqClear(window, message);
            }
            break;

         case IDCMP_REQSET:
            if (events->ReqSet) {
               return events->ReqSet(window, message);
            }
            break;

         case IDCMP_REQVERIFY:
            if (events->ReqVerify) {
               return events->ReqVerify(window, message);
            }
            break;

         case IDCMP_SIZEVERIFY:
            if (events->SizeVerify) {
               return events->SizeVerify(window, message);
            }
            break;

         case IDCMP_VANILLAKEY:
            if (events->VanillaKey) {
               return events->VanillaKey(window, message);
            }
            break;

         case IDCMP_WBENCHMESSAGE:
            if (events->WorkbenchMessage) {
               return events->WorkbenchMessage(window, message);
            }
            break;
                  
         default:
            break;
      }
   }

   return  STATE_NO_CHANGE;
}

void ForEachGadget(
   IDCMPEvents *events, 
   void (*Looper)(IDCMPGadget *gadget, GadgetEventNode *node, IDCMPList *list)
) {
   struct List *list = events->GadgetEvents;
   struct Node *node = NULL;

   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      GadgetEventNode *gadgetNode = (GadgetEventNode *)node;

      Looper(gadgetNode->gadget, gadgetNode, list);
   }

}

void AddGadgetHandler(
   IDCMPEvents *events,
   IDCMPGadget *gadget,
   IDCMPGadgetHandler handler,
   GadgetEventType type
) {
   GadgetEventNode *node;

   if (!events || !gadget || !handler) { return; }

   node = AllocVec(sizeof(GadgetEventNode), MEMF_CLEAR|MEMF_ANY);
   if (!node) {
      return;
   }

   node->gadget = gadget;
   node->handler = handler;
   node->type = type;

   AddTail(events->GadgetEvents, (struct Node *)node);
}

void RemoveGadgetHandlersByType(
   IDCMPEvents *events,
   GadgetEventType type 
) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if ((eventNode->type & type) == type) {         
         Remove(node);
         FreeVec(eventNode);
      }
   }
}

void RemoveGadgetHandlersForId(
   IDCMPEvents *events,
   UWORD GadgetId
) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget->GadgetID == GadgetId) {         
         Remove(node);
         FreeVec(eventNode);
      }
   }
}

BOOL ContainsMatchingGadget(IDCMPEvents *events, IDCMPGadget *gadget) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return FALSE; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget == gadget) {
         return TRUE;
      }
   }

   return FALSE;
}

IDCMPGadgetHandler GetGadgetHandlerById(IDCMPEvents *events, UWORD gadgetId) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return NULL; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget->GadgetID == gadgetId) {
         return eventNode->handler;
      }
   }

   return NULL;
}

IDCMPGadgetHandler GetGadgetHandlerByPtr(
   IDCMPEvents *events, 
   IDCMPGadget *gadget
) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return NULL; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget == gadget) {
         return eventNode->handler;
      }
   }

   return NULL;
}

GadgetEventNode *GetGadgetNodeById(IDCMPEvents *events, UWORD gadgetId) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return NULL; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget->GadgetID == gadgetId) {
         return eventNode;
      }
   }

   return NULL;

}

GadgetEventNode *GetGadgetNodeByPtr(IDCMPEvents *events, IDCMPGadget *gadget) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return NULL; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget == gadget) {
         return eventNode;
      }
   }

   return NULL;
}

IDCMPGadget *FindGadgetById(IDCMPEvents *events, UWORD gadgetId) {
   GadgetEventNode *eventNode = NULL;   
   IDCMPList *list;
   struct Node *node;

   if (!events) { return NULL; }

   list = events->GadgetEvents;
   for (node = list->lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ) {
      eventNode = (GadgetEventNode *)node;
      if (eventNode->gadget && eventNode->gadget->GadgetID == gadgetId) {
         return eventNode->gadget;
      }
   }

   return NULL;
}
