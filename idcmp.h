#ifndef IDCMP_H
#define IDCMP_H 1

#include <intuition/intuition.h>
#include <exec/exec.h>

#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>

typedef struct Gadget IDCMPGadget;
typedef struct Window IDCMPWindow;
typedef struct Screen IDCMPScreen;
typedef struct IntuiMessage IDCMPMessage;
typedef struct List IDCMPList;

#define WFLGS_SIMPLE (\
   WFLG_SMART_REFRESH | WFLG_DRAGBAR | WFLG_CLOSEGADGET | \
	 WFLG_DEPTHGADGET | WFLG_ACTIVATE)

#define WFLGS_NORMAL (\
   WFLGS_SIMPLE | WFLG_GIMMEZEROZERO | WFLG_REPORTMOUSE | \
	 WFLG_SIZEGADGET)

#define ALL_IDCMP_EVENTS (\
   IDCMP_ACTIVEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_CLOSEWINDOW | \
   IDCMP_DELTAMOVE | IDCMP_DISKINSERTED | IDCMP_DISKREMOVED | \
   IDCMP_GADGETDOWN | IDCMP_GADGETHELP | IDCMP_GADGETUP | IDCMP_IDCMPUPDATE | \
   IDCMP_INACTIVEWINDOW | IDCMP_INTUITICKS | IDCMP_LONELYMESSAGE | \
   IDCMP_MENUHELP | IDCMP_MENUPICK | IDCMP_MENUVERIFY | IDCMP_MOUSEBUTTONS | \
   IDCMP_MOUSEMOVE | IDCMP_NEWPREFS | IDCMP_NEWSIZE | IDCMP_RAWKEY | \
   IDCMP_REFRESHWINDOW | IDCMP_REQCLEAR | IDCMP_REQSET | IDCMP_REQVERIFY | \
   IDCMP_SIZEVERIFY | IDCMP_VANILLAKEY | IDCMP_WBENCHMESSAGE)

typedef enum IDCMPMouseButton {
   LEFT_MOUSE_UP = SELECTUP,
   LEFT_MOUSE_DOWN = SELECTDOWN,
   MIDDLE_MOUSE_UP = MIDDLEUP,
   MIDDLE_MOUSE_DOWN = MIDDLEDOWN,
   RIGHT_MOUSE_UP = MENUUP,
   RIGHT_MOUSE_DOWN = MENUDOWN,
   NO_BUTTON = 0
} IDCMPMouseButton;

typedef enum  IDCMPState {
    STATE_FINISHED = TRUE,
    STATE_CONTINUE = FALSE,
    STATE_NO_CHANGE = 2
}  IDCMPState;

typedef enum GadgetEventType {
   GADGET_UP = 1,
   GADGET_DOWN = 2,
   GADGET_HELP = 4
} GadgetEventType;

/**
 * GadgetHandlers are function pointers that receive pointers to a gadget,
 * a window and the IntuiMessage structures present when a gadget is either
 * in the down, up or help states. Additionally the type denotes which of
 * those states were indicated. These are numbered to be operated on in a 
 * bitmask state.
 * 
 * @param gadget the gadget being evented
 * @param window the window pertaining to the gadget event
 * @param message the IntuiMessage structure indicating the event
 * @param type the gadget state; GADGET_UP, GADGET_DOWN or GADGET_HELP
 * @return TRUE if the clicking of this gadget should terminate the event
 * loop, FALSE if not.
 */
typedef BOOL (*IDCMPGadgetHandler)(
   IDCMPGadget *gadget, 
   IDCMPWindow *window,
   IDCMPMessage *message,
   GadgetEventType type
);

/**
 * The GadgetEventNode is a Exec List Node variant that contains several 
 * pieces specific to list of Gadget handler methods stored on each collective
 * IDCMPEvents structure instance.
 * 
 * Specifically it supports the matcher type, the event type, a pointer to 
 * the Gadget and a pointer to the function that should be executed.
 */
typedef struct GadgetEventNode {
   struct Node node;
   GadgetEventType type;
   IDCMPGadget *gadget;
   IDCMPGadgetHandler handler;
} GadgetEventNode;

/**
 * The IDCMPEvents structure contains each of the various IDCMP events that
 * one might listen for by providing a function pointer that can be assigned
 * to code in your project for a given window.
 * 
 * When an appropriate message comes through, the message and window pointers
 * are passed to the function pointer assigned to this instance of IDCMPEvents
 * 
 * It is worth noting that some event function pointers, receive additional
 * parameters when they are invoked such as mouse coordinates or references to
 * the gadgets they service and so on.
 */
typedef struct IDCMPEvents {
   struct List *GadgetEvents;

	IDCMPState (*ActiveWindow)(IDCMPWindow *window, IDCMPMessage *message);
   IDCMPState (*ChangeWindow)(IDCMPWindow *window, IDCMPMessage *message);
   IDCMPState (*DismissWindow)(IDCMPWindow *window, IDCMPMessage *message);
   IDCMPState (*DeltaMove)(IDCMPWindow *window, IDCMPMessage *message);
   IDCMPState (*DiskInserted)(IDCMPWindow *window, IDCMPMessage *message);
   IDCMPState (*DiskRemoved)(IDCMPWindow *window, IDCMPMessage *message);

   IDCMPState (*GadgetDown)(
      IDCMPWindow *window, 
      IDCMPMessage *message,
      IDCMPGadget *gadget
   );

    IDCMPState (*GadgetHelp)(
       IDCMPWindow *window, 
       IDCMPMessage *message,
       IDCMPGadget *gadget
   );

    IDCMPState (*GadgetUp)(
       IDCMPWindow *window, 
       IDCMPMessage *message,
       IDCMPGadget *gadget
   );

    IDCMPState (*IdcmpUpdate)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*InactiveWindow)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*IntuiTicks)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*LonelyMessage)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*MenuHelp)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*MenuPick)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*MenuVerify)(IDCMPWindow *window, IDCMPMessage *message);

    IDCMPState (*MouseButtons)(
      IDCMPWindow *window, 
      IDCMPMessage *message, 
      IDCMPMouseButton buttons
   );

    IDCMPState (*MouseMove)(
      IDCMPWindow *window, 
      IDCMPMessage *message, 
      WORD x, 
      WORD y
   );

    IDCMPState (*NewPrefs)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*NewSize)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*RawKey)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*RefreshWindow)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*ReqClear)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*ReqSet)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*ReqVerify)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*SizeVerify)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*VanillaKey)(IDCMPWindow *window, IDCMPMessage *message);
    IDCMPState (*WorkbenchMessage)(IDCMPWindow *window, IDCMPMessage *message);
} IDCMPEvents;

/**
 * Given a pointer to a `IDCMPEvents` structure, this method will, at the very
 * least, zero all the values within. This makes it easy and safe to use the 
 * structure with partially filled out method invocations. It will also 
 * allocate and intilize the exec list of gadgets for use with gadget handler
 * functions.
 * 
 * @param events a pointer to a `IDCMPEvents` structure
 */
void InitializeIDCMPEvents(IDCMPEvents *events);

/**
 * Frees the memory allocated for the gadget list and the IDCMPEvents object
 * itself, should the `freeOnlyContents` boolean value be `FALSE`.
 * 
 * @param events a pointer to a `struct IDCMPEvents` object
 * @param freeOnlyContents a boolena value that when false, will cause not
 * only the list of Gadgets in the structure be released but also the events
 * object itself.
 */
void FreeIDCMPEvents(IDCMPEvents *events, BOOL freeOnlyContents);

/**
 * For those folks not wanting to write their own dismiss window handler, this
 * method will apply a basic one that terminates the active message loop.
 * 
 * @param events a pointer to a `IDCMPEvents` structure
 */
void ApplyIDCMPBasics(IDCMPEvents *events);

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
IDCMPState ProcessIDCMPMessage(IDCMPEvents *events, IDCMPWindow *window);

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
 * 
 * @param events pointer to the `IDCMPEvents` structure with which to work
 * @param window pointer to the `Window` structure with which to work
 * @param initialDone TRUE if the loop should wait and run only once; FALSE
 * if the loop should run until a handler returns TRUE.
 */
void HandleIDCMP(
   IDCMPEvents *events, 
   IDCMPWindow *window, 
   IDCMPState initialDone
);

/**
 * A convenience function that walks the Exec list for gadget handlers and
 * 
 */
void ForEachGadget(
   IDCMPEvents *events, 
   void (*Looper)(IDCMPGadget *gadget, GadgetEventNode *node, IDCMPList *list)
);

/**
 * Creates a new GadgetHandler mapping so that when a particular Gadget is
 * evented (either by an up, down or help action), the supplied handler is
 * invoked.
 * 
 * @param events the IDCMPEvents object to add the handler to
 * @param gadget the gadget to register a handler for
 * @param handler the function pointer to invoke when the specified handler
 * is actioned
 * @param type some combination of GADGET_UP, GADGET_DOWN and GADGET_HELP
 */
void AddGadgetHandler(
   IDCMPEvents *events,
   IDCMPGadget *gadget,
   IDCMPGadgetHandler handler,
   GadgetEventType type
);

/**
 * This function will remove all gadget handlers installed, that match the
 * types specified from the supplied events list.
 * 
 * @param events IDCMPEvents object to remove the handlers from
 * @param type some combination of GADGET_UP, GADGET_DOWN and GADGET_HELP
 */
void RemoveGadgetHandlersByType(
   IDCMPEvents *events,
   GadgetEventType type 
);

/**
 * This function will remove all gadget handlers installed, that match the
 * GadgetId specified from the supplied events list.
 * 
 * @param events IDCMPEvents object to remove the handlers from
 * @param GadgetId the GadgetId property of the Gadget that matches the 
 * gadget to remove from the list.
 */
void RemoveGadgetHandlersForId(
   IDCMPEvents *events,
   UWORD GadgetId
);

/**
 * Looks through a given IDCMPEvents structure for a registered GadgetEventNode
 * and returns TRUE if one is found. FALSE is returned otherwise. Note that
 * function searches for exact pointer matches, it is not based on contents or
 * value matches.
 * 
 * @param events the `IDCMPEvents` structure to search
 * @param gadget the pointer to the `Gadget` structure to search for
 * @returns TRUE if the Gadget exists in the list; FALSE otherwise.
 */
BOOL ContainsMatchingGadget(IDCMPEvents *events, IDCMPGadget *gadget);

/**
 * Walk the list of GadgetEventNode structures and find the one with a matched
 * gadget id. If none exists, NULL is returned. Otherwise the specified
 * function pointer will be returned instead.
 * 
 * @param events the IDCMPEvents structure to search
 * @param gadgetId the id of the gadget to search for; if found, a reference
 * to the IDCMPGadgetHandler will be returned or NULL otherwise.
 */
IDCMPGadgetHandler GetGadgetHandlerById(IDCMPEvents *events, UWORD gadgetId);

/**
 * Walk the list of GadgetEventNode structures and find the one with a matched
 * gadget pointer. If none exists, NULL is returned. Otherwise the specified
 * function pointer will be returned instead.
 * 
 * @param events the IDCMPEvents structure to search
 * @param gadgetId the pointer of a gadget to search for; if found, a reference
 * to the IDCMPGadgetHandler will be returned or NULL otherwise.
 * @returns the requested function pointer or NULL if none exists
 */
IDCMPGadgetHandler GetGadgetHandlerByPtr(
   IDCMPEvents *events, 
   IDCMPGadget *gadget
);

/**
 * Walk the list of GadgetEventNode nodes and return the first that matches
 * the specified gadget id.
 * 
 * @param events the IDCMPEvents structure to search
 * @param gadgetId the id to match to any existing GadgetId values
 * @returns a pointer to the GadgetEventNode or NULL if none are found
 */
GadgetEventNode *GetGadgetNodeById(IDCMPEvents *events, UWORD gadgetId);

/**
 * Walk the list of GadgetEventNode nodes and return the first that matches
 * the specified gadget pointer.
 * 
 * @param events the IDCMPEvents structure to search
 * @param gadget the pointer to match to any existing Gadget values
 * @returns a pointer to the GadgetEventNode or NULL if none are found
 */
GadgetEventNode *GetGadgetNodeByPtr(IDCMPEvents *events, IDCMPGadget *gadget);

/**
 * Similar to, and relying on, the `GetGadgetNodeById` function, this 
 * function fetches the appropriate node and returns the pointer to the
 * actual gadget underneath.
 * 
 * @param events the `IDCMPEvents` structure to search
 * @param gadgetId the matching UWORD GadgetId stored in the associated
 * Gadget structure
 * @returns if the supplied gadget id matches that of the first in the list
 * to match, then the pointer to that represented Gadget is returned instead
 */
IDCMPGadget *FindGadgetById(IDCMPEvents *events, UWORD gadgetId);


#endif