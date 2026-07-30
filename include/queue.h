/* Compatibility shim.
 *
 * Newer NuttX relocated the sys/queue singly/doubly linked list helpers to
 * <nuttx/queue.h>. PX4 (e.g. src/drivers/drv_hrt.h) still includes the classic
 * top-level <queue.h>. Provide the shim so unmodified PX4 sources compile
 * against this NuttX tree.
 */

#ifndef __INCLUDE_QUEUE_H
#define __INCLUDE_QUEUE_H

#include <nuttx/queue.h>

#endif /* __INCLUDE_QUEUE_H */
