/**
  ******************************************************************************
  * @file    pcb_cells_can.h
  * @author  Bartosz Rychlicki
  * @author  AGH Eko-Energia

  * @Title   Firmware for Cells' PCB's CAN
  *
  * @brief   This file contains common macros and functions' prototypes to provide high quality workflow of CAN peripheral embedded in Cells' PCBs in active mode.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef PCB_CELLS_CAN_H_
#define PCB_CELLS_CAN_H_

/* Includes ----------------------------------------------------------------------------------  */
#include "PCB_Cells_types.h"


/* Variables ---------------------------------------------------------------------------------  */
extern PCBCells_TypeDef pcbCells;


/* Typedefs ----------------------------------------------------------------------------------  */
typedef void (*CAN_DataGetter_t)(uint8_t *data);

/* Functions prototypes ----------------------------------------------------------------------- */
HAL_StatusTypeDef PCBCells_CAN_InitFrames(PCBCells_TypeDef* pc);

HAL_StatusTypeDef PCBCells_CAN_GetID(PCBCells_TypeDef* pc, uint8_t framesIndex, uint32_t* Id);

HAL_StatusTypeDef PCBCells_CAN_ConfigData(PCBCells_TypeDef* pc, CAN_ScheduledMsg* msg, uint8_t thermId);

HAL_StatusTypeDef PCBCells_CAN_SendFrames(PCBCells_TypeDef* pc);

HAL_StatusTypeDef PCBCells_CAN_ScaleValue(PCBCells_TypeDef* pc, uint8_t thermIndex, float* realValue);

void 			  PCBCells_CAN_PackCAN2Temps(uint8_t* data, uint8_t thermId);

void 	  PCBCells_CAN_Get_CAN2_Data_Therm1(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm2(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm3(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm4(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm5(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm6(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm7(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm8(uint8_t *data);
void 	  PCBCells_CAN_Get_CAN2_Data_Therm9(uint8_t *data);

/* Macros ------------------------------------------------------------------------------------- */
#define PCBCELLS_CAN_THERM_OFFSET  (0.0f)
#define PCBCELLS_CAN_THERM_GAIN    (0.390625f)

#define PCBCELLS_CAN_THERM_PERIOD  (200)
#define PCBCELLS_CAN_THERM_DLC     (1)

#define PCBCELLS_CAN_THERM_ID_BASE (200)

#endif /* PCB_CELLS_CAN_H_ */
