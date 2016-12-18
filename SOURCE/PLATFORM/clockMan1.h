/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : clockMan1.h
**     Project     : CAN_MASTER_Example
**     Processor   : S32K144_100
**     Component   : fsl_clock_manager
**     Version     : Component S32K144_SDK01, Driver 01.00, CPU db: 3.00.000
**     Repository  : S32K144_SDK01
**     Compiler    : IAR ARM C Compiler
**     Date/Time   : 2016-08-20, 08:11, # CodeGen: 5
**     Contents    :
**         CLOCK_SYS_SetScgConfiguration     - scg_status_t CLOCK_SYS_SetScgConfiguration(const scg_config_t * scgConfig);
**         CLOCK_SYS_SetPccConfiguration     - void CLOCK_SYS_SetPccConfiguration(const pcc_config_t * peripheralClockConfig);
**         CLOCK_SYS_SetSimConfiguration     - void CLOCK_SYS_SetSimConfiguration(const sim_clock_config_t * simClockConfig);
**         CLOCK_SYS_Init                    - clock_manager_error_code_t CLOCK_SYS_Init(clock_manager_user_config_t const...
**         CLOCK_SYS_UpdateConfiguration     - clock_manager_error_code_t CLOCK_SYS_UpdateConfiguration(uint8_t...
**         CLOCK_SYS_SetConfiguration        - clock_manager_error_code_t CLOCK_SYS_Se...
**         CLOCK_SYS_GetCurrentConfiguration - uint8_t CLOCK_SYS_GetCurrentConfiguration(void);
**         CLOCK_SYS_GetErrorCallback        - clock_manager_callback_user_config_t* CLOCK_SYS_GetErrorCallback(void);
**         CLOCK_SYS_GetFreq                 - clock_manager_error_code_t CLOCK_SYS_GetFreq(clock_names_t clockName,uint32_t...
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file clockMan1.h
** @version 01.00
*/         
/*!
**  @addtogroup clockMan1_module clockMan1 module documentation
**  @{
*/         
#ifndef __clockMan1_H
#define __clockMan1_H
/* MODULE clockMan1. */

#include <stdbool.h>
#include <stdint.h>

/* Include inherited beans */
#include "Cpu.h"

/*! @brief User configuration structure 0 */
extern clock_manager_user_config_t clockMan1_InitConfig0;

/*! @brief Count of user configuration structures */
#define FSL_CLOCK_MANAGER_CONFIG_CNT 1U

/*! @brief Array of pointers to User configuration structures */
extern clock_manager_user_config_t const *g_clockManConfigsArr[];

/*! @brief Count of peripheral clock user configurations */
#define NUM_OF_CONFIGURED_PERIPHERAL_CLOCKS_0 23U


/*! @brief Count of user Callbacks */
#define FSL_CLOCK_MANAGER_CALLBACK_CNT 0U

/*! @brief Array of User callbacks */
extern clock_manager_callback_user_config_t *g_clockManCallbacksArr[];
#endif
/* ifndef __clockMan1_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the Freescale S32K series of microcontrollers.
**
** ###################################################################
*/

