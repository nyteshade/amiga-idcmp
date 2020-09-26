# amiga-idcmp

## Overview

Working with Amiga Intuition windows can require a significant amount of boiler plate, especially when it comes to working with 
IDCMP in C. So I wrote this once so I do not have to continue rewriting the same code over and over again. `amiga-idcmp` is a
single header (.h) and source code file (.c). 

It provides a few useful bits for consumption in your code.

 - `ALL_IDCMP_EVENTS` is a `#define` that includes all IDCMP_ flags at one time. 
 - `IDCMPMouseButton` is a enum type that names button defines in a more readable way, `RIGHT_MOUSE_UP` vs. `MENUUP` for example
 - ` IDCMPState` is a tri state enum that defines a finished, continue and no-change set of states for the continued parsing
   of IDCMP messages for a given Window
 - `IDCMPEvents` is the big one; it is a struct that contains function pointers to handlers for each event type. More on this
   shortly.
 - `InitializeIDCMPEvents()` is a function that, given a pointer to an `IDCMPEvents` struct, will set its data to zeroes and make
   it ready for consumption by `ProcessIDCMPMessage()`
 - `ApplyIDCMPBasics()` for now, is a function that, given a pointer to an `IDCMPEvents` struct, will set its `closeWindow` handler
   to return  STATE_FINISHED and cause `HandleIDCMP()` to return.
 - `HandleIDCMP()` is a function that can be called to start parsing messages for a window using handlers supplied by an instance
   of `IDCMPEvents`. 
 - `ProcessIDCMPMessage()` is a function that is called when a new `IntuiMessage` is received. It returns an instance of ` IDCMPState`
   to let, usually `HandleIDCMP()` know whether or not it should continue listening for messages.
 
 ## Examples
 
 Below is an example bit of code that opens a window, attaches a custom handler for mouseMove, employs the default closeWindow handler
 and that is basically it.
 
 ```c
#include <intuition/intuition.h>

#include <exec/exec.h>
#include <exec/libraries.h>
#include <exec/types.h>

#include <proto/intuition.h>
#include <proto/exec.h>

#include <stdio.h>
#include <string.h>

#include "idcmp.h"

 IDCMPState 
printMouseCoords(struct Window *window, struct IntuiMessage *message, WORD x, WORD y) {
   printf("x %ld  y %ld\n", x, y);
   return  STATE_NO_CHANGE;
}

int
main(int argc, char **argv) {
   struct Window *window;
   struct IntuitionBase *IntuitionBase;
   IDCMPEvents events;

   InitializeIDCMPEvents(&events);
   ApplyIDCMPBasics(&events);
   events.mouseMove = printMouseCoords;
   
   IntuitionBase = (struct IntuitionBase *)OpenLibrary((STRPTR)"intuition.library", 37L);
   if (!IntuitionBase) {
      KPrintF("Failed to open library\n");
      return 5;
   }

   window = OpenWindowTags(
      NULL, 
      WA_Width, 300, 
      WA_Height, 180, 
      WA_Left, 10, 
      WA_Top, 20, 
      WA_Title, "Hello Amiga", 
      WA_Flags, 
         WFLG_CLOSEGADGET | WFLG_DEPTHGADGET | WFLG_DRAGBAR | 
         WFLG_REPORTMOUSE | WFLG_GIMMEZEROZERO | WFLG_HASZOOM,
      WA_IDCMP, ALL_IDCMP_EVENTS,
      TAG_END
   );

   if (!window) {
      CloseLibrary((struct Library *)IntuitionBase);
      return 5;
   }

   HandleIDCMP(&events, window,  STATE_CONTINUE);
   CloseWindow(window);
   CloseLibrary((struct Library *)IntuitionBase);

   return 0;
}
```

## Breakdown

Let's look at the most important snippets in that example. 

```c
   IDCMPEvents events;
```

In this bit, we create a new `IDCMPEvents` struct on the stack. This struct is a container for all the event handlers that might be 
invoked by the `HandleIDCMP()` function. It is highly recommended that `InitializeIDCMPEvents()` is called on each new instance before
it is used. The handlers are checked for non-null status before being invoked and if the memory is not zeroed, function pointers may 
be invoked when in fact they just point at random locations in memory. This will usually cause a crash.

In order to use this, as mentioned above, two things need to happen. One we need to initalize it, and two we need to assign the function
pointers within to point at functions that respond to the event. In the above example, we also invoke a method called `ApplyIDCMPBasics()` 
which only creates a basic `closeWindow()` handler at the moment. 

```c
   InitializeIDCMPEvents(&events);
   ApplyIDCMPBasics(&events);
```

Next we actually want to do something custom with out mouse move messages. Custom in this case is simply printing out the location, 
but in order to do this we need to do two things. One we need to write a function to print out the coordinates and two we need to 
assign the `mouseMove()` handler to the function we've defined.

Here is our mouse move handler. 

```c
 IDCMPState 
printMouseCoords(Window *window, IntuiMessage *message, WORD x, WORD y) {
   printf("x %ld  y %ld\n", x, y);
   return  STATE_NO_CHANGE;
}
```

Note that it returns a ` IDCMPState` enum value. Typically, this means that if you return `TRUE` or ` STATE_FINISHED`, that
`HandleIDCMP()` will return and move on. ` STATE_CONTINUE` will overwrite any other message in which ` STATE_FINISHED` might have
been returned previously and ` STATE_NO_CHANGE` is coded such that whatever `HandleIDCMP()`'s previous state was, that it will not
be changed.

Assigning the handler is easy and is done as a single assignment statement.

```c
events.mouseMove = printMouseCoords;
```

Finally, once your window is opened and your `IDCMPEvents` structure is pointing to the handlers you want to provide code for, simply
invoke `HandleIDCMP()` to start the message loop processing. If you want to write your own `UserPort->mp_SigBit` handling but still 
want to use this code, you can invoke `ProcessIDCMPMessage()` manually. See the `HandleIDCMP` function in idcmp.c. 

```c
HandleIDCMP(&events, window,  STATE_CONTINUE);
```

### Screenshot

<figure><img src="https://github.com/nyteshade/amiga-idcmp/blob/master/Example.png?raw=true"><figcaption>Example</figcaption></figure>
