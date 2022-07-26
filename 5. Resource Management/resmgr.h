#ifndef RESMGR_H
#define RESMGR_H

/**
 * A side value indicating a resource manager instance exists on the client side.
 */
#define RESMGR_SIDE_CLIENT 0

/**
 * A side value indicating a resource manager instance exists on the server side.
 */
#define RESMGR_SIDE_SERVER 1

/**
 * State indicating a resource manger instance is not running.
 */
#define RESMGR_NOT_RUNNING 0

/**
 * State indicating a resource manager instance is running.
 */
#define RESMGR_RUNNING 1

/**
 * The fixed number of system resources available.
 */
#define NUM_RESOURCE_CLASSES 20

typedef struct __resmgr_mem_s __resmgr_mem_s;

typedef struct
{
    /** The executing side. */
    int side;

    /** Nonzero if currently running, otherwise zero. */
    int running;

    /** The ID of the shared memory segment used. */
    int shmid;

    /** The ID of the semaphore set protecting the internal memory. */
    int semid;

    /** Nonzero if in verbose mode, otherwise zero. */
    int verbose;

    /** Internal shared memory structure. */
    __resmgr_mem_s* __mem;
} resmgr_s;

/**
 * Create a resource manager instance.
 */
#define resmgr_new(side) resmgr_construct(malloc(sizeof(resmgr_s)), side)

/**
 * Destroy a resource manager instance.
 */
#define resmgr_delete(resmgr) free(resmgr_destruct(resmgr))

/**
 * Construct a resource manager instance.
 *
 * @param resmgr The resource manager instance
 * @param side The executing side
 * @return The resource manager instance, constructed
 */
resmgr_s* resmgr_construct(resmgr_s* resmgr, int side);

/**
 * Destruct a resource manager instance.
 *
 * @param resmgr The resource manager instance
 * @return The resource manager, destructed
 */
resmgr_s* resmgr_destruct(resmgr_s* resmgr);

/**
 * Lock a resource manager instance for exclusive access. This blocks if already locked.
 *
 * @param resmgr The resource manager instance
 * @return Zero on success, otherwise nonzero
 */
int resmgr_lock(resmgr_s* resmgr);

/**
 * Unlock a locked resource manager instance.
 *
 * @param resmgr The resource manager instance
 * @return Zero on success, otherwise nonzero
 */
int resmgr_unlock(resmgr_s* resmgr);

/**
 * Update a resource manager.
 *
 * Server-side only.
 *
 * @param resmgr The resource manager instance
 */
void resmgr_update(resmgr_s* resmgr);

/**
 * Resolve any and all deadlocks in a system.
 *
 * Server-side only.
 *
 * @param resmgr The resource manager instance
 */
void resmgr_resolve_deadlocks(resmgr_s* resmgr);

/**
 * Request an instance of a particular resource class.
 *
 * @param resmgr The resource manager instance
 * @param res The resource class
 * @return Zero on success, otherwise nonzero
 */
int resmgr_request(resmgr_s* resmgr, int res);

/**
 * Release an instance of a particular resource class.
 *
 * @param resmgr The resource manager instance
 * @param res The resource class
 * @return Zero on success, otherwise nonzero
 */
int resmgr_release(resmgr_s* resmgr, int res);

/**
 * Count how many instances of a resource class the calling process has acquired.
 *
 * @param resmgr The resource manager instance
 * @param res The resource class
 * @return The number of instances
 */
int resmgr_count(resmgr_s* resmgr, int res);

/**
 * Dump statistics to stderr.
 *
 * @param resmgr The resource manager instance
 */
void resmgr_dump(resmgr_s* resmgr);

#endif // #ifndef RESMGR_H
