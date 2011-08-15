/*
  A       +Anode      --A--
  F                  |     |
  -       B          F     B
  E       G          |     |
  -       C           --G---
  -       -          |     |
          D          E     C
                     |     |
                      --D--
*/
#define PPORT_BASE 0x378
extern "C" short Inp32 (short portaddr);
extern "C" void  Out32 (short portaddr, short datum);

#define a  (~0x01)
#define b  (~0x02)
#define c  (~0x04)
#define d  (~0x08)
#define e  (~0x10)
#define f  (~0x20)
#define g  (~0x40)

unsigned char digits[10] =
   { (a&b&c&d&e&f),  // 0
     (b&c),          // 1
     (a&b&d&e&g),    // 2
     (a&b&c&d&g),    // 3
     (b&c&f&g),      // 4
     (a&c&d&f&g),    // 5
     (a&c&d&e&f&g),  // 6
     (a&b&c),        // 7
     (a&b&c&d&e&f&g),// 8
     (a&b&c&d&f&g)   // 9
  };

#define displayNumber(n)  Out32(PPORT_BASE,digits[(n)])



