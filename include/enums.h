#ifndef ENUMS_H
#define ENUMS_H

#include <stdbool.h>

/**
 * @brief Video region identifiers.
 */
typedef enum
{
    REGION_JPN,        ///< Japan (NTSC)
    REGION_USA,        ///< USA (NTSC)
    REGION_EUR,        ///< Europe (PAL)
    REGION_BRA,        ///< Brazil (PAL-M)
    REGION_INVALID = 0x80 ///< Invalid/unknown region
} region_t;

#endif // ENUMS_H
       // End of enums.h