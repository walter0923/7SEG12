
#define SEGA    0xFE
#define SEGB    0xFD
#define SEGC    0xFB
#define SEGD    0xF7
#define SEGE    0xEF
#define SEGF    0xDF
#define SEGG    0xBF

#define NUM0    (SEGA & SEGB & SEGC & SEGD & SEGE & SEGF)
#define NUM1    (SEGB & SEGC)
#define NUM2    (SEGA & SEGB & SEGD & SEGE & SEGG)
#define NUM3    (SEGA & SEGB & SEGC & SEGD & SEGG)
#define NUM4    (SEGB & SEGC & SEGF & SEGG)
#define NUM5    (SEGA & SEGC & SEGD & SEGF & SEGG)
#define NUM6    (SEGA & SEGC & SEGD & SEGE & SEGF & SEGG)
#define NUM7    (SEGA & SEGB & SEGC)
#define NUM8    (SEGA & SEGB & SEGC & SEGD & SEGE & SEGF & SEGG)
#define NUM9    (SEGA & SEGB & SEGC & SEGF & SEGG)
#define DASH    (SEGG)
#define NON     0xFF
char number[12] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, DASH, NON};