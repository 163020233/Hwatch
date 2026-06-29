/**
* @file acl_mdl.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2025. All rights reserved.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef INC_EXTERNAL_ACL_ACL_MODEL_H_
#define INC_EXTERNAL_ACL_ACL_MODEL_H_

#include <stddef.h>
#include <stdint.h>

#include "acl_base.h"
#include "acl_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACL_DIM_ENDPOINTS        2
#define ACL_MAX_DIM_CNT          128
#define ACL_MAX_TENSOR_NAME_LEN  128
#define ACL_MAX_BATCH_NUM        128
#define ACL_MAX_HW_NUM           128
#define ACL_MAX_SHAPE_COUNT      128
#define ACL_INVALID_NODE_INDEX   0xFFFFFFFF

#define ACL_MDL_LOAD_FROM_FILE            1
#define ACL_MDL_LOAD_FROM_FILE_WITH_MEM   2
#define ACL_MDL_LOAD_FROM_MEM             3
#define ACL_MDL_LOAD_FROM_MEM_WITH_MEM    4
#define ACL_MDL_LOAD_FROM_FILE_WITH_Q     5
#define ACL_MDL_LOAD_FROM_MEM_WITH_Q      6

/* used for ACL_MDL_WORKSPACE_MEM_OPTIMIZE */
#define ACL_WORKSPACE_MEM_OPTIMIZE_DEFAULT 0
#define ACL_WORKSPACE_MEM_OPTIMIZE_INPUTOUTPUT 1

// for model stream
#define ACL_MODEL_STREAM_FLAG_HEAD    0x00000000U // first stream
#define ACL_MODEL_STREAM_FLAG_DEFAULT 0x7FFFFFFFU

typedef struct aclmdlDataset aclmdlDataset;
typedef struct aclmdlDesc aclmdlDesc;
typedef struct aclmdlAIPP aclmdlAIPP;
typedef struct aclAippExtendInfo aclAippExtendInfo;
typedef struct aclmdlConfigHandle aclmdlConfigHandle;
typedef struct aclmdlExecConfigHandle aclmdlExecConfigHandle;

typedef enum {
    ACL_MDL_PRIORITY_INT32 = 0,
    ACL_MDL_LOAD_TYPE_SIZET,
    ACL_MDL_PATH_PTR, /**< pointer to model load path with deep copy */
    ACL_MDL_MEM_ADDR_PTR, /**< pointer to model memory with shallow copy */
    ACL_MDL_MEM_SIZET,
    ACL_MDL_WEIGHT_ADDR_PTR, /**< pointer to weight memory of model with shallow copy */
    ACL_MDL_WEIGHT_SIZET,
    ACL_MDL_WORKSPACE_ADDR_PTR, /**< pointer to worksapce memory of model with shallow copy */
    ACL_MDL_WORKSPACE_SIZET,
    ACL_MDL_INPUTQ_NUM_SIZET,
    ACL_MDL_INPUTQ_ADDR_PTR, /**< pointer to inputQ with shallow copy */
    ACL_MDL_OUTPUTQ_NUM_SIZET,
    ACL_MDL_OUTPUTQ_ADDR_PTR, /**< pointer to outputQ with shallow copy */
    ACL_MDL_WORKSPACE_MEM_OPTIMIZE,
    ACL_MDL_WEIGHT_PATH_PTR, /**< pointer to weight path with deep copy */
    ACL_MDL_MODEL_DESC_PTR, /**< pointer to model desc of model with shallow copy */
    ACL_MDL_MODEL_DESC_SIZET,
    ACL_MDL_KERNEL_PTR, /**< pointer to kernel bin of model with shallow copy */
    ACL_MDL_KERNEL_SIZET,
    ACL_MDL_KERNEL_ARGS_PTR, /**< pointer to kernel args of model with shallow copy */
    ACL_MDL_KERNEL_ARGS_SIZET,
    ACL_MDL_STATIC_TASK_PTR, /**< pointer to static task desc of model with shallow copy */
    ACL_MDL_STATIC_TASK_SIZET,
    ACL_MDL_DYNAMIC_TASK_PTR, /**< pointer to dynamic task desc of model with shallow copy */
    ACL_MDL_DYNAMIC_TASK_SIZET,
    ACL_MDL_MEM_MALLOC_POLICY_SIZET,
    ACL_MDL_FIFO_PTR, /**< pointer to fifo memory of model with shallow copy */
    ACL_MDL_FIFO_SIZET
} aclmdlConfigAttr;

typedef enum {
    ACL_MDL_STREAM_SYNC_TIMEOUT = 0,
    ACL_MDL_EVENT_SYNC_TIMEOUT,
    ACL_MDL_WORK_ADDR_PTR, /**< param */
    ACL_MDL_WORK_SIZET, /**< param */
    ACL_MDL_MPAIMID_SIZET, /**< param reserved */
    ACL_MDL_AICQOS_SIZET, /**< param reserved */
    ACL_MDL_AICOST_SIZET, /**< param reserved */
    ACL_MDL_MEC_TIMETHR_SIZET /**< param reserved */
} aclmdlExecConfigAttr;

typedef struct aclmdlIODims {
    char name[ACL_MAX_TENSOR_NAME_LEN]; /**< tensor name */
    size_t dimCount; /**< dim array count */
    int64_t dims[ACL_MAX_DIM_CNT]; /**< dim data array */
} aclmdlIODims;

typedef struct aclmdlExeOMDesc {
    size_t workSize;
    size_t weightSize;
    size_t modelDescSize;
    size_t kernelSize;
    size_t kernelArgsSize;
    size_t staticTaskSize;
    size_t dynamicTaskSize;
    size_t fifoTaskSize;
    size_t reserved[8];
} aclmdlExeOMDesc;

/**
 * @ingroup AscendCL
 * @brief Create data of type aclmdlDesc
 *
 * @retval the aclmdlDesc pointer
 */
ACL_FUNC_VISIBILITY aclmdlDesc *aclmdlCreateDesc(void);

/**
 * @ingroup AscendCL
 * @brief destroy data of type aclmdlDesc
 *
 * @param modelDesc [IN]   Pointer to almdldlDesc to be destroyed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlDestroyDesc(aclmdlDesc *modelDesc);

/**
 * @ingroup AscendCL
 * @brief Get aclmdlDesc data of the model according to the model ID
 *
 * @param  modelDesc [OUT]   aclmdlDesc pointer
 * @param  modelId [IN]      model id
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetDesc(aclmdlDesc *modelDesc, uint32_t modelId);

/**
 * @ingroup AscendCL
 * @brief Get aclmdlDesc data of the model according to the model path
 *
 * @param  modelDesc [OUT]   aclmdlDesc pointer
 * @param  modelPath [IN]    model path
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetDescFromFile(aclmdlDesc *modelDesc, const char *modelPath);

/**
 * @ingroup AscendCL
 * @brief Get aclmdlDesc data of the model according to the model and modelSize
 *
 * @param  modelDesc [OUT]   aclmdlDesc pointer
 * @param  model [IN]        model pointer
 * @param  modelSize [IN]    model size
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetDescFromMem(aclmdlDesc *modelDesc, const void *model, size_t modelSize);

/**
 * @ingroup AscendCL
 * @brief Get the number of the inputs of
 *        the model according to data of aclmdlDesc
 *
 * @param  modelDesc [IN]   aclmdlDesc pointer
 *
 * @retval input size with aclmdlDesc
 */
ACL_FUNC_VISIBILITY size_t aclmdlGetNumInputs(aclmdlDesc *modelDesc);

/**
 * @ingroup AscendCL
 * @brief Get the number of the output of
 *        the model according to data of aclmdlDesc
 *
 * @param  modelDesc [IN]   aclmdlDesc pointer
 *
 * @retval output size with aclmdlDesc
 */
ACL_FUNC_VISIBILITY size_t aclmdlGetNumOutputs(aclmdlDesc *modelDesc);

/**
 * @ingroup AscendCL
 * @brief Get the size of the specified input according to
 *        the data of type aclmdlDesc
 *
 * @param  modelDesc [IN]  aclmdlDesc pointer
 * @param  index [IN] the size of the number of inputs to be obtained,
 *         the index value starts from 0
 *
 * @retval Specify the size of the input
 */
ACL_FUNC_VISIBILITY size_t aclmdlGetInputSizeByIndex(aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief Get the size of the specified output according to
 *        the data of type aclmdlDesc
 *
 * @param modelDesc [IN]   aclmdlDesc pointer
 * @param index [IN]  the size of the number of outputs to be obtained,
 *        the index value starts from 0
 *
 * @retval Specify the size of the output
 */
ACL_FUNC_VISIBILITY size_t aclmdlGetOutputSizeByIndex(aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief Create config handle of execute
 *
 * @retval the aclmdlCreateExecConfigHandle pointer
 */
ACL_FUNC_VISIBILITY aclmdlExecConfigHandle *aclmdlCreateExecConfigHandle(void);

/**
 * @ingroup AscendCL
 * @brief Destroy config handle of model execute
 *
 * @param  handle [IN]  Pointer to aclmdlExecConfigHandle to be destroyed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlDestroyExecConfigHandle(const aclmdlExecConfigHandle *handle);

/**
 * @ingroup AscendCL
 * @brief Create data of type aclmdlDataset
 *
 * @retval the aclmdlDataset pointer
 */
ACL_FUNC_VISIBILITY aclmdlDataset *aclmdlCreateDataset(void);

/**
 * @ingroup AscendCL
 * @brief destroy data of type aclmdlDataset
 *
 * @param  dataset [IN]  Pointer to aclmdlDataset to be destroyed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlDestroyDataset(const aclmdlDataset *dataset);

/**
 * @ingroup AscendCL
 * @brief Add aclDataBuffer to aclmdlDataset
 *
 * @param dataset [OUT]    aclmdlDataset address of aclDataBuffer to be added
 * @param dataBuffer [IN]  aclDataBuffer address to be added
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlAddDatasetBuffer(aclmdlDataset *dataset, aclDataBuffer *dataBuffer);

/**
 * @ingroup AscendCL
 * @brief Get the number of aclDataBuffer in aclmdlDataset
 *
 * @param dataset [IN]   aclmdlDataset pointer
 *
 * @retval the number of aclDataBuffer
 */
ACL_FUNC_VISIBILITY size_t aclmdlGetDatasetNumBuffers(const aclmdlDataset *dataset);

/**
 * @ingroup AscendCL
 * @brief Get the aclDataBuffer in aclmdlDataset by index
 *
 * @param dataset [IN]   aclmdlDataset pointer
 * @param index [IN]     the index of aclDataBuffer
 *
 * @retval Get successfully, return the address of aclDataBuffer
 * @retval Failure return NULL
 */
ACL_FUNC_VISIBILITY aclDataBuffer *aclmdlGetDatasetBuffer(const aclmdlDataset *dataset, size_t index);

/**
 * @ingroup AscendCL
 * @brief Execute model synchronous inference until the inference result is returned
 *
 * @param  modelId [IN]   ID of the model to perform inference
 * @param  input [IN]     Input data for model inference
 * @param  output [OUT]   Output data for model inference
 * @param  stream [IN]   stream
 * @param  handle [IN]   config of model execute
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlExecuteV2(uint32_t modelId, const aclmdlDataset *input, aclmdlDataset *output,
                                             aclrtStream stream, const aclmdlExecConfigHandle *handle);

/**
 * @ingroup AscendCL
 * @brief Execute model asynchronous inference until the inference result is returned
 *
 * @param  modelId [IN]   ID of the model to perform inference
 * @param  input [IN]     Input data for model inference
 * @param  output [OUT]   Output data for model inference
 * @param  stream [IN]   stream
 * @param  handle [IN]   config of model execute
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY  aclError aclmdlExecuteAsyncV2(uint32_t modelId, const aclmdlDataset *input, aclmdlDataset *output,
                                                   aclrtStream stream, const aclmdlExecConfigHandle *handle);
/**
 * @ingroup AscendCL
 * @brief unload model with model id
 *
 * @param  modelId [IN]   model id to be unloaded
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlUnload(uint32_t modelId);

/**
 * @ingroup AscendCL
 * @brief Get the weight memory size and working memory size
 * required for model execution according to the model file
 *
 * @param  fileName [IN]     Model path to get memory information
 * @param  workSize [OUT]    The amount of working memory for model executed
 * @param  weightSize [OUT]  The amount of weight memory for model executed
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlQuerySize(const char *fileName, size_t *workSize, size_t *weightSize);

/**
 * @ingroup AscendCL
 * @brief Get the size of each partition and working memory size
 * required for model execution according to the model file
 *
 * @param  fileName [IN]          Model path to get memory information
 * @param  aclmdlExeOMDesc [OUT]  The size of each partition and working memory size
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlQueryExeOMDesc(const char *fileName, aclmdlExeOMDesc *mdlPartitionSize);

/**
 * @ingroup AscendCL
 * @brief get input dims info
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]  input tensor index
 * @param dims [OUT]  dims info
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclmdlGetInputDimsV2
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetInputDims(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims);

/**
 * @ingroup AscendCL
 * @brief get output dims info
 *
 * @param modelDesc [IN] model description
 * @param index [IN]     output tensor index
 * @param dims [OUT]     dims info
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetOutputDims(const aclmdlDesc *modelDesc, size_t index, aclmdlIODims *dims);

/**
 * @ingroup AscendCL
 * @brief get input name by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]      intput tensor index
 *
 * @retval input tensor name,the same life cycle with modelDesc
 */
ACL_FUNC_VISIBILITY const char *aclmdlGetInputNameByIndex(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get output name by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]      output tensor index
 *
 * @retval output tensor name,the same life cycle with modelDesc
 */
ACL_FUNC_VISIBILITY const char *aclmdlGetOutputNameByIndex(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get input format by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]      intput tensor index
 *
 * @retval input tensor format
 */
ACL_FUNC_VISIBILITY aclFormat aclmdlGetInputFormat(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get output format by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]      output tensor index
 *
 * @retval output tensor format
 */
ACL_FUNC_VISIBILITY aclFormat aclmdlGetOutputFormat(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get input data type by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]  intput tensor index
 *
 * @retval input tensor data type
 */
ACL_FUNC_VISIBILITY aclDataType aclmdlGetInputDataType(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get output data type by index
 *
 * @param modelDesc [IN]  model description
 * @param index [IN]  output tensor index
 *
 * @retval output tensor data type
 */
ACL_FUNC_VISIBILITY aclDataType aclmdlGetOutputDataType(const aclmdlDesc *modelDesc, size_t index);

/**
 * @ingroup AscendCL
 * @brief get input tensor index by name
 *
 * @param modelDesc [IN]  model description
 * @param name [IN]    intput tensor name
 * @param index [OUT]  intput tensor index
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetInputIndexByName(const aclmdlDesc *modelDesc, const char *name, size_t *index);

/**
 * @ingroup AscendCL
 * @brief get output tensor index by name
 *
 * @param modelDesc [IN]  model description
 * @param name [IN]  output tensor name
 * @param index [OUT]  output tensor index
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlGetOutputIndexByName(const aclmdlDesc *modelDesc, const char *name, size_t *index);

/**
 * @ingroup AscendCL
 * @brief load model with config
 *
 * @param handle [IN]    pointer to model config handle
 * @param modelId [OUT]  pointer to model id
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
*/
ACL_FUNC_VISIBILITY aclError aclmdlLoadWithConfig(const aclmdlConfigHandle *handle, uint32_t *modelId);

/**
 * @ingroup AscendCL
 * @brief create model config handle of type aclmdlConfigHandle
 *
 * @retval the aclmdlConfigHandle pointer
 *
 * @see aclmdlDestroyConfigHandle
*/
ACL_FUNC_VISIBILITY aclmdlConfigHandle *aclmdlCreateConfigHandle(void);

/**
 * @ingroup AscendCL
 * @brief destroy data of type aclmdlConfigHandle
 *
 * @param handle [IN]   pointer to model config handle
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see aclmdlCreateConfigHandle
 */
ACL_FUNC_VISIBILITY aclError aclmdlDestroyConfigHandle(aclmdlConfigHandle *handle);

/**
 * @ingroup AscendCL
 * @brief set config for model load
 *
 * @param handle [OUT]    pointer to model config handle
 * @param attr [IN]       config attr in model config handle to be set
 * @param attrValue [IN]  pointer to model config value
 * @param valueSize [IN]  memory size of attrValue
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlSetConfigOpt(aclmdlConfigHandle *handle, aclmdlConfigAttr attr,
    const void *attrValue, size_t valueSize);

/**
 * @ingroup AscendCL
 * @brief set config for model execute
 *
 * @param handle [OUT]    pointer to model execute config handle
 * @param attr [IN]       config attr in model execute config handle to be set
 * @param attrValue [IN]  pointer to model execute config value
 * @param valueSize [IN]  memory size of attrValue
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclmdlSetExecConfigOpt(aclmdlExecConfigHandle *handle, aclmdlExecConfigAttr attr,
                                                    const void *attrValue, size_t valueSize);

/**
 * @ingroup AscendCL
 * @brief get real tensor name from modelDesc
 *
 * @param modelDesc [IN]  pointer to modelDesc
 * @param name [IN]       tensor name
 *
 * @retval the pointer of real tensor name
 * @retval Failure return NULL
 */
ACL_FUNC_VISIBILITY const char *aclmdlGetTensorRealName(const aclmdlDesc *modelDesc, const char *name);

#ifdef __cplusplus
}
#endif

#endif // INC_EXTERNAL_ACL_ACL_MODEL_H_
