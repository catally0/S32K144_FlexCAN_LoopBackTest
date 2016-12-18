/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if !defined(__FSL_EDMA_DRIVER_H__)
#define __FSL_EDMA_DRIVER_H__

#include "fsl_device_registers.h"
#include "fsl_edma_request.h"
#include "fsl_edma_hal.h"
#include "fsl_dmamux_hal.h"
#include "fsl_clock_manager.h"
#include <stddef.h>

/*!
 * @addtogroup edma_driver
 * @{
 */

/*! @brief Array for the eDMA module register base address. */
extern DMA_Type * const g_edmaBase[DMA_INSTANCE_COUNT];

/*! @brief Array for DMAMUX module register base address. */
extern DMAMUX_Type * const g_dmamuxBase[DMAMUX_INSTANCE_COUNT];

/*! @brief Array for eDMA channel interrupt vector number. */
extern const IRQn_Type g_edmaIrqId[FSL_FEATURE_EDMA_MODULE_CHANNELS];

/*! @brief Variables storing the clock names for DMA operations, as implemented in the SoC. */
extern clock_names_t dmaClockName;
extern clock_names_t dmamuxClockName;

#if defined FSL_FEATURE_EDMA_HAS_ERROR_IRQ
/*! @brief Array for eDMA module's error interrupt vector number. */
extern const IRQn_Type g_edmaErrIrqId[DMA_INSTANCE_COUNT];
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief Macro for the memory size needed for the software TCD.
 *
 * Software TCD is aligned to 32 bytes. To make sure the software TCD can meet the
 * eDMA module requirement, allocate memory with extra 32 bytes.
 */
#define STCD_SIZE(number)           ((number + 1) * 32)
#define STCD_ADDR(address)          (edma_software_tcd_t *)(((uint32_t)address + 32) & ~0x1FU)

/*!
 * @brief Macro for accessing the least significant bit of the ERR register.
 *
 * The erroneous channels are retrieved from ERR register by subsequently right
 * shifting all the ERR bits + "AND"-ing the result with this mask.
 */
#define EDMA_ERR_LSB_MASK           1U

/*!
 * @brief The user configuration structure for the eDMA driver.
 *
 * Use an instance of this structure with the EDMA_DRV_Init() function. This allows the user to configure
 * settings of the EDMA peripheral with a single function call.
 */
typedef struct EDMAUserConfig {
    edma_channel_arbitration_t chnArbitration;  /*!< eDMA channel arbitration. */
    bool notHaltOnError;                        /*!< Any error causes the HALT bit to set. Subsequently, all service requests are ignored until the HALT bit is cleared. */
} edma_user_config_t;

/*! @brief Error code for the eDMA Driver. */
typedef enum _edma_status {
    EDMA_STATUS_SUCCESS = 0U,
    EDMA_STATUS_INVALID_ARGUMENT = 1U,  /*!< Parameter is invalid. */
    EDMA_STATUS_FAIL = 2U,              /*!< Failed operation. */
    EDMA_STATUS_CLOCK_GATED_OFF = 3U    /*!< Clock is disabled. */
} edma_status_t;

/*!
 * @brief Channel status for eDMA channel.
 *
 * A structure describing the eDMA channel status. The user can get the status by callback parameter
 * or by calling EDMA_DRV_getStatus() function.
 */
typedef enum _edma_chn_status {
    EDMA_CHN_NORMAL = 0U,           /*!< eDMA channel is occupied. */
    EDMA_CHN_IDLE,                  /*!< eDMA channel is idle. */
    EDMA_CHN_ERROR                  /*!< An error occurs in the eDMA channel. */
} edma_chn_status_t;

/*!
 * @brief Definition for the eDMA channel callback function.
 *
 * Prototype for the callback function registered in the eDMA driver.
 */
typedef void (*edma_callback_t)(void *parameter, edma_chn_status_t status);

/*! @brief Data structure for the eDMA channel state. */
typedef struct EDMAChnState {
    uint8_t channel;                     /*!< Virtual channel indicator. */
    edma_callback_t callback;            /*!< Callback function pointer for the eDMA channel. It will
                                              be called at the eDMA channel complete and eDMA channel
                                              error. */
    void *parameter;                     /*!< Parameter for the callback function pointer. */
    volatile edma_chn_status_t status;   /*!< eDMA channel status. */
} edma_chn_state_t;

/*!
 * @brief The user configuration structure for the an eDMA driver channel.
 *
 * Use an instance of this structure with the EDMA_DRV_ChannelInit() function. This allows the user to configure
 * settings of the EDMA channel with a single function call.
 */
typedef struct EDMAChannelConfig {
    edma_channel_priority_t priority;  /*!< eDMA channel priority - only used when channel arbitration mode is 'Fixed priority'. */
    uint8_t channel;                   /*!< eDMA channel number - use EDMA_ANY_CHANNEL for dynamic allocation */
    dma_request_source_t source;       /*!< Selects the source of the DMA request for this channel */
    edma_callback_t callback;          /*!< Callback that will be registered for this channel */
    void * callbackParam;              /*!< Parameter passed to the channel callback */
} edma_channel_config_t;

/*! @brief enum type for channel allocation.  */
typedef enum _edma_chn_state_type {
    EDMA_INVALID_CHANNEL = 0xFFU, /*!< Macros indicate the failure of the channel request. */
    EDMA_ANY_CHANNEL = 0xFEU      /*!< Macros used when requesting channel dynamically. */
} edma_chn_state_type_t;

/*! @brief A type for the DMA transfer. */
typedef enum _edma_transfer_type {
    EDMA_TRANSFER_PERIPH2MEM,        /*!< Transfer from peripheral to memory */
    EDMA_TRANSFER_MEM2PERIPH,        /*!< Transfer from memory to peripheral */
    EDMA_TRANSFER_MEM2MEM,           /*!< Transfer from memory to memory */
} edma_transfer_type_t;

/*! @brief Data structure for configuring a discrete memory transfer. */
typedef struct EDMAScatterGatherList {
    uint32_t address;           /*!< Address of buffer. */
    uint32_t length;            /*!< Length of buffer. */
    edma_transfer_type_t type;  /*!< Type of the DMA transfer */
} edma_scatter_gather_list_t;

/*!
 * @brief Runtime state structure for the eDMA driver.
 *
 * This structure holds data that is used by the eDMA peripheral driver to manage
 * multi eDMA channels.
 * The user passes the memory for this run-time state structure and the eDMA
 * driver populates the members.
 */
typedef struct EDMAState {
#if (USE_RTOS)
    mutex_t lock;       /*!< Lock for channel allocation and release. */
#endif
    edma_chn_state_t * volatile chn[FSL_FEATURE_EDMA_MODULE_CHANNELS];     /*!< Pointer array storing channel state. */
} edma_state_t;

/*!
 * @brief eDMA loop transfer configuration.
 *
 * This structure configures the basic minor/major loop attributes.
 */
typedef struct EDMALoopConfig {
    uint32_t majorLoopIterationCount;       /*!< Number of major loop iterations. */
    bool srcOffsetEnable;                   /*!< Selects whether the minor loop offset is applied to the
                                                 source address upon minor loop completion. */
    bool dstOffsetEnable;                   /*!< Selects whether the minor loop offset is applied to the
                                                 destination address upon minor loop completion. */
    int32_t minorLoopOffset;                /*!< Sign-extended offset applied to the source or destination address
                                                 to form the next-state value after the minor loop completes. */
    bool minorLoopChnLinkEnable;            /*!< Enables channel-to-channel linking on minor loop complete. */
    uint8_t minorLoopChnLinkNumber;         /*!< The number of the next channel to be started by DMA
                                                 engine when minor loop completes. */
    bool majorLoopChnLinkEnable;            /*!< Enables channel-to-channel linking on major loop complete. */
    uint8_t majorLoopChnLinkNumber;         /*!< The number of the next channel to be started by DMA
                                                 engine when major loop completes. */
} edma_loop_transfer_config_t;
/*!
 * @brief eDMA transfer size configuration.
 *
 * This structure configures the basic source/destination transfer attribute.
 */
typedef struct EDMATransferConfig {
    uint32_t srcAddr;                                 /*!< Memory address pointing to the source data. */
    uint32_t destAddr;                                /*!< Memory address pointing to the destination data. */
    edma_transfer_size_t srcTransferSize;             /*!< Source data transfer size. */
    edma_transfer_size_t destTransferSize;            /*!< Destination data transfer size. */
    int16_t srcOffset;                                /*!< Sign-extended offset applied to the current source address to
                                                           form the next-state value as each source read/write is completed. */
    int16_t destOffset;                               /*!< Sign-extended offset applied to the current destination address to
                                                           form the next-state value as each source read/write is completed. */
    int32_t srcLastAddrAdjust;                        /*!< Last source address adjustment. */
    int32_t destLastAddrAdjust;                       /*!< Last destination address adjustment. Note here it is only
                                                           valid when scatter/gather feature is not enabled. */
    edma_modulo_t srcModulo;                          /*!< Source address modulo. */
    edma_modulo_t destModulo;                         /*!< Destination address modulo. */
    uint32_t minorByteTransferCount;                  /*!< Number of bytes to be transferred in each service request of the channel. */
    bool scatterGatherEnable;                         /*!< Enable scatter gather feature. */
    uint32_t scatterGatherNextDescAddr;               /*!< The address of the next descriptor to be used, when scatter/gather feature is enabled.
                                                           Note this value is not used when scatter/gather feature is disabled. */
    bool interruptEnable;                             /*!< Enable the interrupt request when the major loop count completes */
    edma_loop_transfer_config_t *loopTransferConfig;  /*!< Pointer to loop transfer configuration structure (defines minor/major loop attributes)
                                                           Note: this field is only used when minor loop mapping is enabled from DMA configuration. */
} edma_transfer_config_t;

/*! @brief eDMA TCD */
typedef struct EDMASoftwareTcd {
    uint32_t SADDR;
    uint16_t SOFF;
    uint16_t ATTR;
    uint32_t NBYTES;
    uint32_t SLAST;
    uint32_t DADDR;
    uint16_t DOFF;
    uint16_t CITER;
    uint32_t DLAST_SGA;
    uint16_t CSR;
    uint16_t BITER;
} edma_software_tcd_t;
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
  * @name eDMA Peripheral Driver
  * @{
  */

/*!
  * @name eDMA peripheral driver module level functions
  * @{
  */

/*!
 * @brief Initializes the eDMA module.
 *
 * This function initializes the run-time state structure to provide the eDMA channel allocation
 * release, protect, and track the state for channels. This function also resets the eDMA modules,
 * initializes the module to user-defined settings and default settings.
 * This is an example to set up the edma_state_t and the edma_user_config_t parameters and to call
 * the EDMA_DRV_Init function by passing in these parameters.
   @code
   edma_state_t state;    <- The user simply allocates memory for this structure.
   edma_user_config_t userConfig;    <- The user fills out members for this structure.

   userConfig.chnArbitration = EDMA_ARBITRATION_ROUND_ROBIN;
   userCOnfig.notHaltOnError = false;    <- The default setting is false, means eDMA halt on error.

   EDMA_DRV_Init(&state, &userConfig);

   @endcode
 *
 * @param edmaState The pointer to the eDMA peripheral driver state structure. The user passes
 * the memory for this run-time state structure and the eDMA peripheral driver populates the
 * members. This run-time state structure keeps track of the eDMA channels status. The memory must
 * be kept valid before calling the EDMA_DRV_DeInit.
 * @param userConfig User configuration structure for eDMA peripheral drivers. The user populates the
 * members of this structure and passes the pointer of this structure into the function.
 *
 * @return An eDMA error codes or kStatus_EDMA_Success.
 */
edma_status_t EDMA_DRV_Init(edma_state_t *edmaState, const edma_user_config_t *userConfig);

/*!
 * @brief De-initializes the eDMA module.
 *
 * This function resets the eDMA module to reset state and disables the interrupt to the core.
 *
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_Deinit(void);

/* @} */

/*!
  * @name eDMA peripheral driver channel management functions
  * @{
  */

/*!
 * @brief Initializes an eDMA channel.
 *
 * This function initializes the run-time state structure for a eDMA channel, baed on user
 * configuration. It will request the channel, set up the channel priority and install the
 * callback.
 *
 * @param edmaChannelState Pointer to the eDMA channel state structure. The user passes
 * the memory for this run-time state structure and the eDMA peripheral driver populates the
 * members. This run-time state structure keeps track of the eDMA channel status. The memory must
 * be kept valid before calling the EDMA_DRV_ReleaseChannel.
 * @param edmaChannelConfig User configuration structure for eDMA channel. The user populates the
 * members of this structure and passes the pointer of this structure into the function.
 *
 * @return An eDMA error codes or kStatus_EDMA_Success.
 */
edma_status_t EDMA_DRV_ChannelInit(edma_chn_state_t *edmaChannelState, const edma_channel_config_t *edmaChannelConfig);

/*!
 * @brief Requests an eDMA channel dynamically or statically.
 *
 * This function allocates the eDMA channel according to the required channel allocation and
 * corresponding to the eDMA hardware request, initializes the channel state memory provided by user and fills
 * out the members. This functions also sets up the hardware request configuration according to the
 * user's requirements.
 *
 * This function provides two ways to allocate an eDMA channel: statically and dynamically.
 * In a static allocation, the user provides the required channel number and eDMA driver tries to allocate the
 * required channel to the user. If the channel is not occupied, the eDMA driver is
 * successfully assigned to the user. If the channel is already occupied, the user
 * gets the return value EDMA_INVALID_CHANNEL.
 * This is an example to request a channel statically:
    @code
    uint32_t channelNumber = 14;  <- Try to allocate the channel 14
    edma_chn_state_t chn; <- The user simply allocates memory for this structure.

    if (EDMA_INVALID_CHANNEL == EDMA_DRV_RequestChannel(channel, EDMA_REQ_DMAMUX_ALWAYS_ENABLED0, chn))
    {
        printf("request channel %d failed!\n", channel);
    }

    @endcode
 * In a dynamic allocation, any of the free eDMA channels are available for use. eDMA driver
 * assigns the first free channel to the user.
 * This is an example for user to request a channel dynamically :
    @code
    uint32_t channel;    <- Store the allocated channel number.
    edma_chn_state_t chn;    <- The user simply allocates memory for this structure.

    channel = EDMA_DRV_RequestChannel(EDMA_ANY_CHANNEL, EDMA_REQ_DMAMUX_ALWAYS_ENABLED0, chn);

    if (channel == EDMA_INVALID_CHANNEL)
    {
        printf("request channel %d failed!\n", channel);
    }
    else
    {
        printf("Channel %d is successfully allocated! /n", channel);
    }

    @endcode
 *
 * @param channel Requested channel number. If the channel is assigned with the EDMA_ANY_CHANNEL, the eDMA driver
 * allocates the channel dynamically. If the channel is assigned with a valid channel number, the eDMA driver
 * allocates that channel.
 * @param source eDMA hardware request number.
 * @param chn The pointer to the eDMA channel state structure. The user passes the memory for this
 * run-time state structure. The eDMA peripheral driver populates the members. This run-time
 * state structure keeps tracks of the eDMA channel status. The memory must be kept valid before
 * calling the EDMA_DRV_ReleaseChannel().
 *
 * @return Successfully allocated channel number or the EDMA_INVALID_CHANNEL indicating that the request has
 * failed.
 */
uint8_t EDMA_DRV_RequestChannel(uint8_t channel, dma_request_source_t source, edma_chn_state_t *chn);

/*!
 * @brief Releases an eDMA channel.
 *
 * This function stops the eDMA channel and disables the interrupt of this channel. The channel state
 * structure can be released after this function is called.
 *
 * @param chn The pointer to the channel state structure.
 *
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_ReleaseChannel(edma_chn_state_t *chn);

/* @} */

/*!
  * @name eDMA peripheral driver transfer setup functions
  * @{
  */

/*!
 * @brief Copies the channel configuration to the TCD registers.
 *
 * @param chn Pointer to the channel state structure.
 * @param config Pointer to the channel configuration structure.
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_PushConfigToReg(edma_chn_state_t *chn, edma_transfer_config_t *config);

/*!
 * @brief Copies the channel configuration to the software TCD structure.
 *
 * This function copies the properties from the channel configuration to the software TCD structure; the address
 * of the software TCD can be used to enable scatter/gather operation (pointer to the next TCD).

 * @param chn Pointer to the channel state structure.
 * @param config Pointer to the channel configuration structure.
 * @param stcd Pointer to the software TCD structure.
 */
void EDMA_DRV_PushConfigToSTCD(edma_chn_state_t *chn, edma_transfer_config_t *config, edma_software_tcd_t *stcd);

/*!
 * @brief Configures a simple single block data transfer with DMA.
 *
 * This function configures the descriptor for a single block transfer.
 * The function considers contiguous memory blocks, thus it configures the TCD
 * source/destination offset fields to cover the data buffer without gaps,
 * according to "transferSize" parameter (the offset is equal to the number of
 * bytes transferred in a source read/destination write).
 *
 * NOTE: For memory-to-peripheral or peripheral-to-memory transfers, make sure
 * the transfer size is equal to the data buffer size of the peripheral used,
 * otherwise only truncated chunks of data may be transferred (e.g. for a
 * communication IP with an 8-bit data register the transfer size should be 1B,
 * whereas for a 32-bit data register, the transfer size should be 4B). The
 * rationale of this constraint is that, on the peripheral side, the address
 * offset is set to zero, allowing to read/write data from/to the peripheral
 * in a single source read/destination write operation.
 *
 * @param chn Pointer to the channel state structure.
 * @param type Transfer type (M->M, P->M, M->P).
 * @param srcAddr A source register address or a source memory address.
 * @param destAddr A destination register address or a destination memory address.
 * @param transferSize The number of bytes to be transferred on every DMA write/read.
 * Source/Dest share the same write/read size.
 * @param dataBufferSize The total number of bytes to be transferred.
 *
 * @return An error code of EDMA_STATUS_SUCCESS
 */
edma_status_t EDMA_DRV_ConfigSingleBlockTransfer(
                            edma_chn_state_t *chn, edma_transfer_type_t type,
                            uint32_t srcAddr, uint32_t destAddr,
                            edma_transfer_size_t transferSize, uint32_t dataBufferSize);

/*!
 * @brief Configures the DMA transfer in loop mode.
 *
 * This function configures the DMA transfer in a loop chain. The user passes a block of memory into this
 * function that configures the loop transfer properties (minor/major loop count, address offsets, channel linking).
 * The DMA driver copies the configuration to TCD registers, only when the loop properties are set up correctly
 * and minor loop mapping is enabled for the eDMA module.
 *
 * @param chn Pointer to the channel state structure.
 * @param transferConfig Pointer to the transfer configuration strucutre; this structure defines fields for setting
 * up the basic transfer and also a pointer to a memory strucure that defines the loop chain properties (minor/major).
 *
 * @return An error code of EDMA_STATUS_SUCCESS
 */
edma_status_t EDMA_DRV_ConfigLoopTransfer(edma_chn_state_t *chn, edma_transfer_config_t * transferConfig);

/*!
 * @brief Configures the DMA transfer in a scatter-gather mode.
 *
 * This function configures the descriptors into a single-ended chain. The user passes blocks of memory into
 * this function. The interrupt is triggered only when the last memory block is completed. The memory block
 * information is passed with the edma_scatter_gather_list_t data structure, which can tell
 * the memory address and length.
 * The DMA driver configures the descriptor for each memory block, transfers the descriptor from the
 * first one to the last one, and stops.
 *
 * @param chn Pointer to the channel state structure.
 * @param stcd Array of empty software TCD structures. The user must prepare this memory block. The "stcd"
 * buffer must align with 32 bytes; if not, an error occurs in the eDMA driver. Thus, the required
 * memory size for "stcd" is equal to (tcdCount + 1) * size_of(edma_software_tcd_t); the driver will take
 * care of the memory alignment if the provided memory buffer is big enough. For proper allocation of the
 * "stcd" buffer it is recommended to use STCD_SIZE macro.
 * @param transferSize The number of bytes to be transferred on every DMA write/read.
 * @param bytesOnEachRequest Bytes to be transferred in each DMA request.
 * @param srcList Data structure storing the address, length and type of transfer (M->M, M->P, P->M) for
 * the bytes to be transferred for source memory blocks. If the source memory is peripheral, the length
 * is not used.
 * @param destList Data structure storing the address, length and type of transfer (M->M, M->P, P->M) for
 * the bytes to be transferred for destination memory blocks. In the memory-to-memory transfer mode, the
 * user must ensure that the length of the destination scatter gather list is equal to the source
 * scatter gather list. If the destination memory is a peripheral register, the length is not used.
 * @param tcdCount The number of TCD memory blocks contained in the scatter gather list.
 *
 * @return An error code of EDMA_STATUS_SUCCESS
 */
edma_status_t EDMA_DRV_ConfigScatterGatherTransfer(
                        edma_chn_state_t *chn, edma_software_tcd_t *stcd,
                        edma_transfer_size_t transferSize, uint32_t bytesOnEachRequest,
                        edma_scatter_gather_list_t *srcList, edma_scatter_gather_list_t *destList,
                        uint8_t tcdCount);

/* @} */

/*!
  * @name eDMA Peripheral driver channel operation functions
  * @{
  */
/*!
 * @brief Starts an eDMA channel.
 *
 * This function enables the eDMA channel DMA request.
 *
 * @param chn Pointer to the channel state structure.
 *
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_StartChannel(edma_chn_state_t *chn);

/*!
 * @brief Stops the eDMA channel.
 *
 * This function disables the eDMA channel DMA request.
 *
 * @param chn Pointer to the channel state structure.
 *
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_StopChannel(edma_chn_state_t *chn);

/* @} */

/*!
  * @name eDMA Peripheral callback and interrupt functions
  * @{
  */

/*!
 * @brief Registers the callback function and the parameter for eDMA channel.
 *
 * This function registers the callback function and the parameter into the eDMA channel state structure.
 * The callback function is called when the channel is complete or a channel error occurs. The eDMA
 * driver passes the channel status to this callback function to indicate whether it is caused by the
 * channel complete event or the channel error event.
 *
 * To un-register the callback function, set the callback function to "NULL" and call this
 * function.
 *
 * @param chn The pointer to the channel state structure.
 * @param callback The pointer to the callback function.
 * @param parameter The pointer to the callback function's parameter.
 *
 * @return An eDMA error codes or EDMA_STATUS_SUCCESS.
 */
edma_status_t EDMA_DRV_InstallCallback(edma_chn_state_t *chn, edma_callback_t callback, void *parameter);

/*!
 * @brief IRQ Handler for eDMA channel interrupt.
 *
 * This function is provided as the default flow for eDMA channel interrupt. This function clears the
 * status and calls the callback functions.
 * The user can add this function into the hardware interrupt entry and implement a
 * custom interrupt action function.
 *
 * @param channel Channel number.
 */
void EDMA_DRV_IRQHandler(uint8_t channel);

/*!
 * @brief ERROR IRQ Handler for eDMA channel interrupt.
 *
 * This function is provided as the default action for eDMA module error interrupt. This function
 * clears status, stops the error on a eDMA channel, and calls the eDMA channel callback function if the
 * error eDMA channel is already requested.
 * The user can add this function into the eDMA error interrupt entry and implement a custom
 * interrupt action function.
 */
void EDMA_DRV_ErrorIRQHandler(void);

/* @} */

/*!
  * @name eDMA Peripheral driver miscellaneous functions
  * @{
  */
/*!
 * @brief Gets the eDMA channel status.
 *
 * @param chn Pointer to the channel state structure.
 *
 * @return Channel status.
 */
static inline edma_chn_status_t EDMA_DRV_GetChannelStatus(edma_chn_state_t *chn)
{
    return chn->status;
}

/* @} */


/* @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* __FSL_EDMA_DRIVER_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/

