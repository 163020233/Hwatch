/**
* @file acl_rt.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2025. All rights reserved.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef INC_EXTERNAL_ACL_ACL_RT_H_
#define INC_EXTERNAL_ACL_ACL_RT_H_

#include <stdint.h>
#include <stddef.h>
#include "acl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

// Current version is 1.15.0
#define ACL_MAJOR_VERSION              1
#define ACL_MINOR_VERSION              15
#define ACL_PATCH_VERSION              0

// for create stream
#define ACL_STREAM_FAST_LAUNCH  0x00000001U
#define ACL_STREAM_FAST_SYNC    0x00000002U
#define ACL_STREAM_PERSISTENT   0x00000004U
#define ACL_STREAM_HUGE         0x00000008U
#define ACL_STREAM_CPU_SCHEDULE 0x00000010U

#define ACL_STREAM_WAIT_VALUE_GEQ 0x00000000U
#define ACL_STREAM_WAIT_VALUE_EQ  0x00000001U
#define ACL_STREAM_WAIT_VALUE_AND 0x00000002U
#define ACL_STREAM_WAIT_VALUE_NOR 0x00000003U

#define ACL_CONTINUE_ON_FAILURE 0x00000000U
#define ACL_STOP_ON_FAILURE     0x00000001U

// for device get capability
#define ACL_DEV_FEATURE_SUPPORT     0x00000001
#define ACL_DEV_FEATURE_NOT_SUPPORT 0x00000000

#define ACL_RT_NOTIFY_EXPORT_FLAG_DEFAULT                0x0UL
#define ACL_RT_NOTIFY_EXPORT_FLAG_DISABLE_PID_VALIDATION 0x02UL

#define ACL_RT_NOTIFY_IMPORT_FLAG_DEFAULT            0x0UL
#define ACL_RT_NOTIFY_IMPORT_FLAG_ENABLE_PEER_ACCESS 0x02UL

#define ACL_RT_IPC_MEM_EXPORT_FLAG_DEFAULT                0x0UL
#define ACL_RT_IPC_MEM_EXPORT_FLAG_DISABLE_PID_VALIDATION 0x1UL

#define ACL_RT_IPC_MEM_IMPORT_FLAG_DEFAULT            0x0UL
#define ACL_RT_IPC_MEM_IMPORT_FLAG_ENABLE_PEER_ACCESS 0x1UL

#define ACL_RT_VMM_EXPORT_FLAG_DEFAULT                0x0UL
#define ACL_RT_VMM_EXPORT_FLAG_DISABLE_PID_VALIDATION 0x1UL

typedef enum aclrtRunMode {
    ACL_DEVICE,
    ACL_HOST,
} aclrtRunMode;

typedef enum aclrtCallbackBlockType {
    ACL_CALLBACK_NO_BLOCK,
    ACL_CALLBACK_BLOCK,
} aclrtCallbackBlockType;

typedef enum aclrtMemcpyKind {
    ACL_MEMCPY_HOST_TO_HOST,
    ACL_MEMCPY_HOST_TO_DEVICE,
    ACL_MEMCPY_DEVICE_TO_HOST,
    ACL_MEMCPY_DEVICE_TO_DEVICE,
    ACL_MEMCPY_DEFAULT,
    ACL_MEMCPY_HOST_TO_BUF_TO_DEVICE,
    ACL_MEMCPY_INNER_DEVICE_TO_DEVICE,
    ACL_MEMCPY_INTER_DEVICE_TO_DEVICE,
} aclrtMemcpyKind;

typedef enum aclrtMemMallocPolicy {
    ACL_MEM_MALLOC_HUGE_FIRST,
    ACL_MEM_MALLOC_HUGE_ONLY,
    ACL_MEM_MALLOC_NORMAL_ONLY,
    ACL_MEM_MALLOC_HUGE_FIRST_P2P,
    ACL_MEM_MALLOC_HUGE_ONLY_P2P,
    ACL_MEM_MALLOC_NORMAL_ONLY_P2P,
    ACL_MEM_MALLOC_HUGE1G_ONLY,
    ACL_MEM_MALLOC_HUGE1G_ONLY_P2P,
    ACL_MEM_TYPE_LOW_BAND_WIDTH   = 0x0100,
    ACL_MEM_TYPE_HIGH_BAND_WIDTH  = 0x1000,
    ACL_MEM_ACCESS_USER_SPACE_READONLY = 0x100000,
} aclrtMemMallocPolicy;

typedef enum aclrtMemAttr {
    ACL_DDR_MEM,
    ACL_HBM_MEM,
    ACL_DDR_MEM_HUGE,
    ACL_DDR_MEM_NORMAL,
    ACL_HBM_MEM_HUGE,
    ACL_HBM_MEM_NORMAL,
    ACL_DDR_MEM_P2P_HUGE,
    ACL_DDR_MEM_P2P_NORMAL,
    ACL_HBM_MEM_P2P_HUGE,
    ACL_HBM_MEM_P2P_NORMAL,
    ACL_HBM_MEM_HUGE1G,
    ACL_HBM_MEM_P2P_HUGE1G,
} aclrtMemAttr;

typedef enum {
    ACL_RT_STREAM_WORK_ADDR_PTR = 0, /**< pointer to model work addr */
    ACL_RT_STREAM_WORK_SIZE, /**< pointer to model work size */
    ACL_RT_STREAM_FLAG,
    ACL_RT_STREAM_PRIORITY,
} aclrtStreamConfigAttr;

typedef struct aclrtStreamConfigHandle {
    void* workptr;
    size_t workSize;
    size_t flag;
    uint32_t priority;
} aclrtStreamConfigHandle;

typedef void (*aclrtCallback)(void *userData);

#define MAX_MEM_UCE_INFO_ARRAY_SIZE 128
#define UCE_INFO_RESERVED_SIZE 14

#define ACL_RT_BINARY_MAGIC_ELF_AICORE      0x43554245U
#define ACL_RT_BINARY_MAGIC_ELF_VECTOR_CORE 0x41415246U
#define ACL_RT_BINARY_MAGIC_ELF_CUBE_CORE   0x41494343U

#define ACL_RT_CMO_MAX_BARRIER_NUM 6U

/**
 * @ingroup AscendCL
 * @brief The thread that handles the callback function on the Stream
 *
 * @param threadId [IN] thread ID
 * @param stream [IN]   stream handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtSubscribeReport(uint64_t threadId, aclrtStream stream);

/**
 * @ingroup AscendCL
 * @brief Add a callback function to be executed on the host
 *        to the task queue of the Stream
 *
 * @param fn [IN]   Specify the callback function to be added
 *                  The function prototype of the callback function is:
 *                  typedef void (*aclrtCallback)(void *userData);
 * @param userData [IN]   User data to be passed to the callback function
 * @param blockType [IN]  callback block type
 * @param stream [IN]     stream handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtLaunchCallback(aclrtCallback fn, void *userData, aclrtCallbackBlockType blockType,
                                                 aclrtStream stream);

/**
 * @ingroup AscendCL
 * @brief After waiting for a specified time, trigger callback processing
 *
 * @par Function
 *  The thread processing callback specified by
 *  the aclrtSubscribeReport interface
 *
 * @param timeout [IN]   timeout value
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtSubscribeReport
 */
ACL_FUNC_VISIBILITY aclError aclrtProcessReport(int32_t timeout);

/**
 * @ingroup AscendCL
 * @brief Cancel thread registration,
 *        the callback function on the specified Stream
 *        is no longer processed by the specified thread
 *
 * @param threadId [IN]   thread ID
 * @param stream [IN]     stream handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtUnSubscribeReport(uint64_t threadId, aclrtStream stream);

/**
 * @ingroup AscendCL
 * @brief create context and associates it with the calling thread
 *
 * @par Function
 * The following use cases are supported:
 * @li If you don't call the aclrtCreateContext interface
 * to explicitly create the context,
 * the system will use the default context, which is implicitly created
 * when the aclrtSetDevice interface is called.
 * @li If multiple contexts are created in a process
 * (there is no limit on the number of contexts),
 * the current thread can only use one of them at the same time.
 * It is recommended to explicitly specify the context of the current thread
 * through the aclrtSetCurrentContext interface to increase.
 * the maintainability of the program.
 *
 * @param  context [OUT]    point to the created context
 * @param  deviceId [IN]    device to create context on
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtSetDevice | aclrtSetCurrentContext
 */
ACL_FUNC_VISIBILITY aclError aclrtCreateContext(aclrtContext *context, int32_t deviceId);

/**
 * @ingroup AscendCL
 * @brief destroy context instance
 *
 * @par Function
 * Can only destroy context created through aclrtCreateContext interface
 *
 * @param  context [IN]   the context to destroy
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtCreateContext
 */
ACL_FUNC_VISIBILITY aclError aclrtDestroyContext(aclrtContext context);

/**
 * @ingroup AscendCL
 * @brief set the context of the thread
 *
 * @par Function
 * The following scenarios are supported:
 * @li If the aclrtCreateContext interface is called in a thread to explicitly
 * create a Context (for example: ctx1), the thread's Context can be specified
 * without calling the aclrtSetCurrentContext interface.
 * The system uses ctx1 as the context of thread1 by default.
 * @li If the aclrtCreateContext interface is not explicitly created,
 * the system uses the default context as the context of the thread.
 * At this time, the aclrtDestroyContext interface cannot be used to release
 * the default context.
 * @li If the aclrtSetCurrentContext interface is called multiple times to
 * set the thread's Context, the last one prevails.
 *
 * @par Restriction
 * @li If the cevice corresponding to the context set for the thread
 * has been reset, you cannot set the context as the context of the thread,
 * otherwise a business exception will result.
 * @li It is recommended to use the context created in a thread.
 * If the aclrtCreateContext interface is called in thread A to create a context,
 * and the context is used in thread B,
 * the user must guarantee the execution order of tasks in the same stream
 * under the same context in two threads.
 *
 * @param  context [IN]   the current context of the thread
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtCreateContext | aclrtDestroyContext
 */
ACL_FUNC_VISIBILITY aclError aclrtSetCurrentContext(aclrtContext context);

/**
 * @ingroup AscendCL
 * @brief get the context of the thread
 *
 * @par Function
 * If the user calls the aclrtSetCurrentContext interface
 * multiple times to set the context of the current thread,
 * then the last set context is obtained
 *
 * @param  context [OUT]   the current context of the thread
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtSetCurrentContext
 */
ACL_FUNC_VISIBILITY aclError aclrtGetCurrentContext(aclrtContext *context);

/**
 * @ingroup AscendCL
 * @brief Specify the device to use for the operation
 * implicitly create the default context and the default stream
 *
 * @par Function
 * The following use cases are supported:
 * @li Device can be specified in the process or thread.
 * If you call the aclrtSetDevice interface multiple
 * times to specify the same device,
 * you only need to call the aclrtResetDevice interface to reset the device.
 * @li The same device can be specified for operation
 *  in different processes or threads.
 * @li Device is specified in a process,
 * and multiple threads in the process can share this device to explicitly
 * create a Context (aclrtCreateContext interface).
 * @li In multi-device scenarios, you can switch to other devices
 * through the aclrtSetDevice interface in the process.
 *
 * @param  deviceId [IN]  the device id
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtResetDevice |aclrtCreateContext
 */
ACL_FUNC_VISIBILITY aclError aclrtSetDevice(int32_t deviceId);

/**
 * @ingroup AscendCL
 * @brief Reset the current operating Device and free resources on the device,
 * including the default context, the default stream,
 * and all streams created under the default context,
 * and synchronizes the interface.
 * If the task under the default context or stream has not been completed,
 * the system will wait for the task to complete before releasing it.
 *
 * @par Restriction
 * @li The Context, Stream, and Event that are explicitly created
 * on the device to be reset. Before resetting,
 * it is recommended to follow the following interface calling sequence,
 * otherwise business abnormalities may be caused.
 * @li Interface calling sequence:
 * call aclrtDestroyEvent interface to release Event or
 * call aclrtDestroyStream interface to release explicitly created Stream->
 * call aclrtDestroyContext to release explicitly created Context->
 * call aclrtResetDevice interface
 *
 * @param  deviceId [IN]   the device id
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtResetDevice(int32_t deviceId);

/**
 * @ingroup AscendCL
 * @brief get target device of current thread
 *
 * @param deviceId [OUT]  the device id
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtGetDevice(int32_t *deviceId);

/**
 * @ingroup AscendCL
 * @brief get target side
 *
 * @param runMode [OUT]    the run mode
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtGetRunMode(aclrtRunMode *runMode);

/**
 * @ingroup AscendCL
 * @brief alloc memory on device, real alloc size is aligned to 32 bytes and padded with 32 bytes
 *
 * @par Function
 *  alloc for size linear memory on device
 *  and return a pointer to allocated memory by *devPtr
 *
 * @par Restriction
 * @li The memory requested by the aclrtMalloc interface needs to be released
 * through the aclrtFree interface.
 * @li Before calling the media data processing interface,
 * if you need to apply memory on the device to store input or output data,
 * you need to call acldvppMalloc to apply for memory.
 *
 * @param devPtr [OUT]  pointer to pointer to allocated memory on device
 * @param size [IN]     alloc memory size
 * @param policy [IN]   memory alloc policy
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtFree | acldvppMalloc | aclrtMallocCached
 */
ACL_FUNC_VISIBILITY aclError aclrtMalloc(void **devPtr,
                                         size_t size,
                                         aclrtMemMallocPolicy policy);

/**
 * @ingroup AscendCL
 * @brief free device memory
 *
 * @par Function
 *  can only free memory allocated through the aclrtMalloc interface
 *
 * @param  devPtr [IN]  Pointer to memory to be freed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtMalloc
 */
ACL_FUNC_VISIBILITY aclError aclrtFree(void *devPtr);

/**
 * @ingroup AscendCL
 * @brief synchronous memory replication between host and device
 *
 * @param dst [IN]       destination address pointer
 * @param destMax [IN]   Max length of the destination address memory
 * @param src [IN]       source address pointer
 * @param count [IN]     the length of byte to copy
 * @param kind [IN]      memcpy type
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtMemcpy(void *dst,
                                         size_t destMax,
                                         const void *src,
                                         size_t count,
                                         aclrtMemcpyKind kind);

/**
 * @ingroup AscendCL
 * @brief Initialize memory and set contents of memory to specified value
 *
 * @par Function
 *  The memory to be initialized is on the Host or device side,
 *  and the system determines whether
 *  it is host or device according to the address
 *
 * @param devPtr [IN]    Starting address of memory
 * @param maxCount [IN]  Max length of destination address memory
 * @param value [IN]     Set value
 * @param count [IN]     The length of memory
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtMemset(void *devPtr, size_t maxCount, int32_t value, size_t count);

/**
 * @ingroup AscendCL
 * @brief Create config handle of stream
 *
 * @retval the aclrtStreamConfigHandle pointer
 */
ACL_FUNC_VISIBILITY aclrtStreamConfigHandle *aclrtCreateStreamConfigHandle(void);

/**
 * @ingroup AscendCL
 * @brief Destroy config handle of model execute
 *
 * @param  handle [IN]  Pointer to aclrtStreamConfigHandle to be destroyed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtDestroyStreamConfigHandle(aclrtStreamConfigHandle *handle);

/**
 * @ingroup AscendCL
 * @brief set config for stream
 *
 * @param handle [OUT]    pointer to stream config handle
 * @param attr [IN]       config attr in stream config handle to be set
 * @param attrValue [IN]  pointer to stream config value
 * @param valueSize [IN]  memory size of attrValue
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtSetStreamConfigOpt(aclrtStreamConfigHandle *handle, aclrtStreamConfigAttr attr,
    const void *attrValue, size_t valueSize);

/**
 * @ingroup AscendCL
 * @brief  create stream instance
 *
 * @param  stream [OUT]   the created stream
 * @param  handle [IN]   the config of stream
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtCreateStreamV2(aclrtStream *stream, const aclrtStreamConfigHandle *handle);

/**
 * @ingroup AscendCL
 * @brief destroy stream instance
 *
 * @par Function
 * Can only destroy streams created through the aclrtCreateStream interface
 *
 * @par Restriction
 * Before calling the aclrtDestroyStream interface to destroy
 * the specified Stream, you need to call the aclrtSynchronizeStream interface
 * to ensure that the tasks in the Stream have been completed.
 *
 * @param stream [IN]  the stream to destroy
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtCreateStream | aclrtSynchronizeStream
 */
ACL_FUNC_VISIBILITY aclError aclrtDestroyStream(aclrtStream stream);

/**
 * @ingroup AscendCL
 * @brief block the host until all tasks
 * in the specified stream have completed
 *
 * @param  stream [IN]   the stream to wait
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtSynchronizeStream(aclrtStream stream);

/**
 * @ingroup AscendCL
 * @brief Obtain the free memory and total memory of specified attribute.
 * the specified memory include normal memory and huge memory.
 *
 * @param attr [IN]    the memory attribute of specified device
 * @param free [OUT]   the free memory of specified device
 * @param total [OUT]  the total memory of specified device.
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtGetMemInfo(aclrtMemAttr attr, size_t *free, size_t *total);

/**
 * @ingroup AscendCL
 * @brief The thread that handles the hostFunc function on the Stream
 *
 * @param hostFuncThreadId [IN] thread ID
 * @param exeStream        [IN] stream handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtSubscribeHostFunc(uint64_t hostFuncThreadId, aclrtStream exeStream);

/**
 * @ingroup AscendCL
 * @brief After waiting for a specified time, trigger hostFunc callback function processing
 *
 * @par Function
 *  The thread processing callback specified by the aclrtSubscribeHostFunc interface
 *
 * @param timeout [IN]   timeout value
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclrtSubscribeHostFunc
 */
ACL_FUNC_VISIBILITY aclError aclrtProcessHostFunc(int32_t timeout);

/**
 * @ingroup AscendCL
 * @brief Cancel thread registration,
 *        the hostFunc function on the specified Stream
 *        is no longer processed by the specified thread
 *
 * @param hostFuncThreadId [IN]   thread ID
 * @param exeStream        [IN]   stream handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtUnSubscribeHostFunc(uint64_t hostFuncThreadId, aclrtStream exeStream);

/**
 * @ingroup AscendCL
 * @brief query ACL interface version
 *
 * @param majorVersion[OUT] ACL interface major version
 * @param minorVersion[OUT] ACL interface minor version
 * @param patchVersion[OUT] ACL interface patch version
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclrtGetVersion(int32_t *majorVersion, int32_t *minorVersion, int32_t *patchVersion);

#ifdef __cplusplus
}
#endif

#endif // INC_EXTERNAL_ACL_ACL_RT_H_

