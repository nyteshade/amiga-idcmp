#include <intuition/intuition.h>
#include <exec/exec.h>

#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include <clib/alib_stdio_protos.h>

#include "idcmp.h"

struct IntuitionBase* IntuitionBase;

VOID PrepEnvironment(VOID);
VOID ShutdownEnvironment(VOID);
VOID ApplyIDCMPExtras(IDCMPEvents* idcmp);

IDCMPState GadgetPressed(
  IDCMPWindow *window, 
  IDCMPMessage *message,
  IDCMPGadget *gadget
);

int main(int argc, char **argv) {
  IDCMPWindow* window = NULL;
  IDCMPEvents events;

  PrepEnvironment();
  
  InitializeIDCMPEvents(&events);
  ApplyIDCMPBasics(&events);
  ApplyIDCMPExtras(&events);

  window = OpenWindowTags(NULL,
    WA_Title, "Sample Window",
    WA_Flags, WFLGS_SIMPLE,
    WA_IDCMP, ALL_IDCMP_EVENTS,
    WA_Left, 13,
    WA_Top, 13,
    WA_Width, 320,
    WA_Height, 200,
    TAG_DONE
  );

  if (window) {
    HandleIDCMP(&events, window, FALSE);
  }

  ShutdownEnvironment();
  return 0;
}

VOID PrepEnvironment() {
  IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 0L);
  if (!IntuitionBase) {
    ShutdownEnvironment();
  }
  else {
    printf("Intuition library opened\n");
  }
}

VOID ShutdownEnvironment() {
  if (IntuitionBase) {
    CloseLibrary((struct Library *)IntuitionBase);
  }
}

IDCMPState GadgetPressed(
  IDCMPWindow *window, 
  IDCMPMessage *message,
  IDCMPGadget *gadget
) {
  printf("A gadget has been pressed\n");

  return STATE_NO_CHANGE;
}

VOID ApplyIDCMPExtras(IDCMPEvents* idcmp) {
  idcmp->GadgetDown = GadgetPressed;
}