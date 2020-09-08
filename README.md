# amiga-idcmp

## Overview

Working with Amiga Intuition windows can require a significant amount of boiler plate, especially when it comes to working with 
IDCMP in C. So I wrote this once so I do not have to continue rewriting the same code over and over again. `amiga-idcmp` is a
single header (.h) and source code file (.c). 

It provides a few useful bits for consumption in your code.

 - `ALL_IDCMP_EVENTS` is a `#define` that includes all IDCMP_ flags at one time. 
 - `IDCMPMouseButton` is a enum type that names button defines in a more readable way, `RIGHT_MOUSE_UP` vs. `MENUUP` for example
 - `MessageLoopState` is a tri state enum that defines a finished, continue and no-change set of states for the continued parsing
   of IDCMP messages for a given Window
 - `IDCMPEvents` is the big one; it is a struct that contains function pointers to handlers for each event type. More on this
   shortly.
 - `InitializeIDCMPEvents()` is a function that, given a pointer to an `IDCMPEvents` struct, will set its data to zeroes and make
   it ready for consumption by `ProcessIDCMPMessage()`
 - `ApplyIDCMPBasics()` for now, is a function that, given a pointer to an `IDCMPEvents` struct, will set its `closeWindow` handler
   to return LOOPSTATE_FINISHED and cause `HandleIDCMP()` to return.
 - `HandleIDCMP()` is a function that can be called to start parsing messages for a window using handlers supplied by an instance
   of `IDCMPEvents`. 
 - `ProcessIDCMPMessage()` is a function that is called when a new `IntuiMessage` is received. It returns an instance of `MessageLoopState`
   to let, usually `HandleIDCMP()` know whether or not it should continue listening for messages.
 
 
