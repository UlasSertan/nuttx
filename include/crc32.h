/* Compatibility shim.
 *
 * Newer NuttX relocated crc32 helpers to <nuttx/crc32.h>. PX4 sources still
 * include the classic top-level <crc32.h>. Provide the shim so unmodified PX4
 * code compiles against this NuttX tree.
 */

#ifndef __INCLUDE_CRC32_H
#define __INCLUDE_CRC32_H

#include <nuttx/crc32.h>

#endif /* __INCLUDE_CRC32_H */
