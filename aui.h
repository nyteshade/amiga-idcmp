#ifndef AUI_H
#define AUI_H

#include <exec/types.h>

typedef struct AUISize {
  WORD width;
  WORD height;
} AUISize;

typedef struct AUIPoint {
   WORD x;
   WORD y;
} AUIPoint;

#endif