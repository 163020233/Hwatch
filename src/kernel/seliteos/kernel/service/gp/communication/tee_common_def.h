/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All
 * rights reserved.
 * Description: tee common structure and macro defines.
 */

#ifndef _TEE_COMMON_H_
#define _TEE_COMMON_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uint32_t TEE_Result;

typedef struct {
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t clockSeqAndNode[8];
} TEE_UUID;

/*
 * The TEE_Identity structure defines the full identity of a Client:
 * - login is one of the TEE_LOGIN_XXX constants
 * - uuid contains the client UUID or Nil if not applicable
 */
typedef struct {
    uint32_t login;
    TEE_UUID uuid;
} TEE_Identity;

/*
 * This union describes one parameter passed by the Trusted Core Framework
 * to the entry points TA_OpenSessionEntryPoint or
 * TA_InvokeCommandEntryPoint or by the TA to the functions
 * TEE_OpenTASession or TEE_InvokeTACommand.
 *
 * Which of the field value or memref to select is determined by the
 * parameter type specified in the argument paramTypes passed to the entry
 * point.
 */
typedef union {
    struct {
        void *buffer;
        uint32_t size;
    } memref;
    struct {
        uint32_t a;
        uint32_t b;
    } value;
} TEE_Param;

/**
 * Return values. Type is TEE_Result
 *
 * TEE_SUCCESS                 The operation was successful.
 * TEE_ERROR_ACCESS_DENIED     Access privileges are not sufficient.
 * TEE_ERROR_BAD_PARAMETERS    Input parameters were invalid.
 * TEE_ERROR_BAD_STATE         Operation is not valid in the current state.
 * TEE_ERROR_NOT_IMPLEMENTED   The requested operation should exist but is not
 *                             yet implemented.
 * TEE_ERROR_OUT_OF_MEMORY     System ran out of resources.
 * TEE_ERROR_BUSY              The system is busy working on something else.
 * TEE_ERROR_COMMUNICATION     Communication with a remote party failed.
 * TEE_ERROR_SECURITY          A security fault was detected.
 * TEE_ERROR_SHORT_BUFFER      The supplied buffer is too short for the
 *                              generated output.
 * TEE_ERROR_BAD_SESSION_ID    The number of session is overflow.
 * TEE_ERROR_ITEM_NOT_FOUND    The requested data item is not found.
 * TEEC_ERROR_OVERFLOW          The stack or calculation is overflow.
 * TEE_FAILURE                 The operation was failed.
 */

/**
 *  Standard defined error codes.
 */
#define TEE_SUCCESS               0x00000000
#define TEE_ERROR_ACCESS_DENIED   0xFFFF0001
#define TEE_ERROR_BAD_PARAMETERS  0xFFFF0006
#define TEE_ERROR_BAD_STATE       0xFFFF0007
#define TEE_ERROR_NOT_IMPLEMENTED 0xFFFF0009
#define TEE_ERROR_OUT_OF_MEMORY   0xFFFF000C
#define TEE_ERROR_BUSY            0xFFFF000D
#define TEE_ERROR_COMMUNICATION   0xFFFF000E
#define TEE_ERROR_SECURITY        0xFFFF000F
#define TEE_ERROR_SHORT_BUFFER    0xFFFF0010
#define TEE_ERROR_BAD_SESSION_ID  0xFFFF0011
#define TEE_ERROR_ITEM_NOT_FOUND  0xFFFF0012
#define TEE_ERROR_OVERFLOW        0xFFFF0013
#define TEE_FAILURE               0xFFFFFFFF

/* Parameter Type Constants */
#define TEE_PARAM_TYPE_NONE          0
#define TEE_PARAM_TYPE_VALUE_INPUT   1
#define TEE_PARAM_TYPE_VALUE_OUTPUT  2
#define TEE_PARAM_TYPE_VALUE_INOUT   3
#define TEE_PARAM_TYPE_MEMREF_INPUT  5
#define TEE_PARAM_TYPE_MEMREF_OUTPUT 6
#define TEE_PARAM_TYPE_MEMREF_INOUT  7

/*
 * The macro TEE_PARAM_TYPES can be used to construct a value that you can
 * compare against an incoming paramTypes to check the type of all the
 * parameters in one comparison, like in the following example:
 * if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
 *                                  TEE_PARAM_TYPE_MEMREF_OUPUT,
 *                                  TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE)) {
 *      return TEE_ERROR_BAD_PARAMETERS;
 *  }
 */
#define TEE_PARAM_TYPES(t0, t1, t2, t3) \
    ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))

/*
 * The macro TEE_PARAM_TYPE_GET can be used to extract the type of a given
 * parameter from paramTypes if you need more fine-grained type checking.
 */
#define TEE_PARAM_TYPE_GET(t, i) ((((uint32_t)(t)) >> ((i)*4)) & 0xF)

/*
 * The macro TEE_PARAM_TYPE_SET can be used to load the type of a given
 * parameter from paramTypes without specifying all types (TEE_PARAM_TYPES)
 */
#define TEE_PARAM_TYPE_SET(t, i) (((uint32_t)(t)&0xF) << ((i)*4))

/* Not specified in the standard */
#define TEE_NUM_PARAMS      4
#define TEE_NUM_META_PARAMS 2

/*
 * This file defines the OP-TEE message protocol used to communicate
 * with an instance of OP-TEE running in secure world.
 */

/*****************************************************************************
 * Part 1 - formatting of messages
 *****************************************************************************/

#define TEE_MSG_ATTR_TYPE_NONE         0x0
#define TEE_MSG_ATTR_TYPE_VALUE_INPUT  0x1
#define TEE_MSG_ATTR_TYPE_VALUE_OUTPUT 0x2
#define TEE_MSG_ATTR_TYPE_VALUE_INOUT  0x3
#define TEE_MSG_ATTR_TYPE_RMEM_INPUT   0x5
#define TEE_MSG_ATTR_TYPE_RMEM_OUTPUT  0x6
#define TEE_MSG_ATTR_TYPE_RMEM_INOUT   0x7
#define TEE_MSG_ATTR_TYPE_TMEM_INPUT   0x9
#define TEE_MSG_ATTR_TYPE_TMEM_OUTPUT  0xa
#define TEE_MSG_ATTR_TYPE_TMEM_INOUT   0xb

#define TEE_MSG_ATTR_TYPE_MASK 0xF

/*
 * Meta parameter to be absorbed by the Secure OS and not passed
 * to the Trusted Application.
 *
 * Currently only used with TEE_MSG_CMD_OPEN_SESSION.
 */
#define TEE_MSG_ATTR_META 0x100

/**
 * struct tee_msg_param_tmem - temporary memory reference parameter
 * @buf_ptr:	Address of the buffer
 * @size:	Size of the buffer
 *
 * Secure and normal world communicates pointers as physical address
 * instead of the virtual address. This is because secure and normal world
 * have completely independent memory mapping. Normal world can even have a
 * hypervisor which need to translate the guest physical address (AKA IPA
 * in ARM documentation) to a real physical address before passing the
 * structure to secure world.
 */
struct tee_msg_param_tmem {
    uint64_t buf_ptr;
    uint64_t size;
};

/**
 * struct tee_msg_param_rmem - registered memory reference parameter
 * @offs:	Offset into shared memory reference
 * @size:	Size of the buffer
 * @shm_ref:	Shared memory reference, pointer to a struct TEE_SharedMemory
 */
struct tee_msg_param_rmem {
    uint64_t offs;
    uint64_t size;
    uint64_t shm_ref;
};

/**
 * struct tee_msg_param_value - opaque value parameter
 *
 * Value parameters are passed unchecked between normal and secure world.
 */
struct tee_msg_param_value {
    uint64_t a;
    uint64_t b;
    uint64_t c;
};

/**
 * struct tee_msg_param - parameter used together with struct tee_msg_arg
 * @attr:	attributes
 * @tmem:	parameter by temporary memory reference
 * @rmem:	parameter by registered memory reference
 * @fmem:	parameter by FF-A registered memory reference
 * @value:	parameter by opaque value
 *
 * @attr & TEE_MSG_ATTR_TYPE_MASK indicates if tmem, rmem or value is used in
 * the union. TEE_MSG_ATTR_TYPE_VALUE_* indicates value,
 * TEE_MSG_ATTR_TYPE_TMEM_* indicates @tmem and
 * TEE_MSG_ATTR_TYPE_RMEM_* or the alias PTEE_MSG_ATTR_TYPE_FMEM_* indicates
 * @rmem or @fmem depending on the conduit.
 * TEE_MSG_ATTR_TYPE_NONE indicates that none of the members are used.
 */
struct tee_msg_param {
    uint32_t attr;
    union {
        struct tee_msg_param_tmem tmem;
        struct tee_msg_param_rmem rmem;
        struct tee_msg_param_value value;
    } u;
};

/**
 * struct tee_msg_arg - call argument
 * @cmd: Command, one of TEE_MSG_CMD_* or TEE_MSG_RPC_CMD_*
 * @func: Trusted Application function, specific to the Trusted Application,
 *	     used if cmd == TEE_MSG_CMD_INVOKE_COMMAND
 * @session: In parameter for all TEE_MSG_CMD_* except
 *	     TEE_MSG_CMD_OPEN_SESSION where it's an output parameter instead
 * @cancel_id: Cancellation id, a unique value to identify this request
 * @ret: return value
 * @ret_origin: origin of the return value
 * @num_params: number of parameters supplied to the OS Command
 * @params: the parameters supplied to the OS Command
 *
 * All normal calls to Trusted OS uses this struct. If cmd requires further
 * information than what these fields hold it can be passed as a parameter
 * tagged as meta (setting the TEE_MSG_ATTR_META bit in corresponding
 * attrs field). All parameters tagged as meta have to come first.
 */
struct tee_msg_arg {
    uint32_t cmd;
    uint32_t func;
    uint32_t session;
    uint32_t ret;
    uint32_t ret_origin;
    uint32_t num_params;

    /* num_params tells the actual number of element in params */
    struct tee_msg_param params[];
};

/**
 * TEE_MSG_GET_ARG_SIZE - return size of struct tee_msg_arg
 *
 * @num_params: Number of parameters embedded in the struct tee_msg_arg
 *
 * Returns the size of the struct tee_msg_arg together with the number
 * of embedded parameters.
 */
#define TEE_MSG_GET_ARG_SIZE(num_params) \
    (sizeof(struct tee_msg_arg) + sizeof(struct tee_msg_param) * (num_params))

/*
 * Defines the maximum value of @num_params that can be passed to
 * TEE_MSG_GET_ARG_SIZE without a risk of crossing page boundary.
 */
#define TEE_MSG_MAX_NUM_PARAMS                                    \
    ((TEE_MSG_NONCONTIG_PAGE_SIZE - sizeof(struct tee_msg_arg)) / \
     sizeof(struct tee_msg_param))

/*****************************************************************************
 * Part 2 - requests from normal world
 *****************************************************************************/

/*
 * Do a secure call with struct tee_msg_arg as argument
 * The TEE_MSG_CMD_* below defines what goes in struct tee_msg_arg::cmd
 *
 * TEE_MSG_CMD_OPEN_SESSION opens a session to a Trusted Application.
 * The first two parameters are tagged as meta, holding two value
 * parameters to pass the following information:
 * param[0].u.value.a-b uuid of Trusted Application
 * param[1].u.value.a-b uuid of Client
 * param[1].u.value.c Login class of client TEE_MSG_LOGIN_*
 *
 * TEE_MSG_CMD_INVOKE_COMMAND invokes a command a previously opened
 * session to a Trusted Application.  struct tee_msg_arg::func is Trusted
 * Application function, specific to the Trusted Application.
 *
 * TEE_MSG_CMD_CLOSE_SESSION closes a previously opened session to
 * Trusted Application.
 */
#define TEE_MSG_CMD_OPEN_SESSION   0x0
#define TEE_MSG_CMD_INVOKE_COMMAND 0x1
#define TEE_MSG_CMD_CLOSE_SESSION  0x2

/* TEE_Identity Constants */
/* Operation is executed by a tee_kernel (secure) */
#define SKERN_IDENTITY ((TEE_Identity *)-1)
/* Operation is executed by a client (non-secure) */
#define NSKERN_IDENTITY (NULL)

/* Login Type Constants */
#define TEE_LOGIN_PUBLIC            0x00000000
#define TEE_LOGIN_USER              0x00000001
#define TEE_LOGIN_GROUP             0x00000002
#define TEE_LOGIN_APPLICATION       0x00000004
#define TEE_LOGIN_APPLICATION_USER  0x00000005
#define TEE_LOGIN_APPLICATION_GROUP 0x00000006
#define TEE_LOGIN_TRUSTED_APP       0xF0000000

/* Origin Code Constants */
#define TEE_ORIGIN_API         0x00000001
#define TEE_ORIGIN_COMMS       0x00000002
#define TEE_ORIGIN_TEE         0x00000003
#define TEE_ORIGIN_TRUSTED_APP 0x00000004

enum utee_entry_func {
    UTEE_ENTRY_FUNC_OPEN_SESSION = 0,
    UTEE_ENTRY_FUNC_CLOSE_SESSION,
    UTEE_ENTRY_FUNC_INVOKE_COMMAND,
};

#define DUMP_UUID(uuid)                                                     \
    do {                                                                    \
        PRINT_INFO("UUID is: %x-%x-%x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", \
                   (uuid).timeLow, (uuid).timeMid, (uuid).timeHiAndVersion, \
                   (uuid).clockSeqAndNode[0], (uuid).clockSeqAndNode[1],    \
                   (uuid).clockSeqAndNode[2], (uuid).clockSeqAndNode[3],    \
                   (uuid).clockSeqAndNode[4], (uuid).clockSeqAndNode[5],    \
                   (uuid).clockSeqAndNode[6], (uuid).clockSeqAndNode[7]);   \
    } while (0)

/**
 * struct TEE_SharedMemory - Memory to transfer data between a client
 * application and trusted code.
 *
 * @param buffer            The memory buffer which is to be, or has been,
 *                          shared with the TEE.
 * @param size              The size, in bytes, of the memory buffer.
 * @param flags             Bit-vector which holds properties of buffer.
 *                          The bit-vector can contain either or both of the
 *                          TEEC_MEM_INPUT and TEEC_MEM_OUTPUT flags.
 * @param id                The id of sharedMemory
 * @param alloced_size      The size, in bytes, of the memory buffer which is
 *                          alloced actually
 *
 * A shared memory block is a region of memory allocated in the context of the
 * client application memory space that can be used to transfer data between
 * that client application and a trusted application. The user of this struct
 * is responsible to populate the buffer pointer.
 */
typedef struct {
    void *buffer;
    size_t size;
    uint32_t flags;
    /*
     * Implementation-Defined
     */
    int id;
    size_t alloced_size;
} TEE_SharedMemory;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TEE_COMMON_H_ */
