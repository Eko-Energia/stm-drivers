/**
  ******************************************************************************
  * @file    pcb_cells_can.c
  * @author  Bartosz Rychlicki
  * @author  AGH Eko-Energia

  * @Title   Firmware for Cells' PCB's CAN peripheral
  *
  * @brief   This file contains functions' bodies, that implement functions logic and provide optimized workflow of PCB Cell's CAN peripheral
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------  */
#include "pcb_cells_can.h"

/* Variables ----------------------------------------------------------------------------------  */
static const CAN_DataGetter_t CAN2_Therm_Handlers[9] = {
    PCBCells_CAN_Get_CAN2_Data_Therm1,
    PCBCells_CAN_Get_CAN2_Data_Therm2,
    PCBCells_CAN_Get_CAN2_Data_Therm3,
    PCBCells_CAN_Get_CAN2_Data_Therm4,
    PCBCells_CAN_Get_CAN2_Data_Therm5,
    PCBCells_CAN_Get_CAN2_Data_Therm6,
    PCBCells_CAN_Get_CAN2_Data_Therm7,
    PCBCells_CAN_Get_CAN2_Data_Therm8,
    PCBCells_CAN_Get_CAN2_Data_Therm9
};

static uint8_t framesAscendingBaseId  = 1;  // base unity number of id for ascending  organization of frames

static uint8_t framesDescendingBaseId = 9;  // base unity number of id for descending organization of frames

extern PCBCells_TypeDef pcbCells;
/* Functions' bodies --------------------------------------------------------------------------  */

HAL_StatusTypeDef PCBCells_CAN_InitFrames(PCBCells_TypeDef* pc){

	// Init object for CAn frame
	CAN_ScheduledMsg msg;

	// basic configuration for all frames
	msg.period_ms = PCBCELLS_CAN_THERM_PERIOD;
	msg.header.DLC = PCBCELLS_CAN_THERM_DLC;
	msg.header.IDE = CAN_ID_STD;
	msg.header.RTR = CAN_RTR_DATA;

	for(int i = 0; i < 9; ++i){

		// configuration frames ID
		if(PCBCells_CAN_GetID(pc, i, &msg.header.StdId) != HAL_OK){
			return HAL_ERROR;
		}


		if(PCBCells_CAN_ConfigData(pc, &msg, i) != HAL_OK){
			return HAL_ERROR;
		}

		// Adding frame to buffer
		if(CAN_AddScheduledMessage(msg, &pc->pccan.CAN_msgBuffer) != HAL_OK){
			return HAL_ERROR;
		}
	}

	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_CAN_GetID(PCBCells_TypeDef* pc, uint8_t frameIndex, uint32_t* Id){

	// check if correct framesIndex was given
	if(frameIndex >= 9){
		return HAL_ERROR;
	}

	// Calculating Id for ascending or descending order of IDs assignment
	*Id = PCBCELLS_CAN_THERM_ID_BASE + pc->packetIndex * 10
	       + ((pc->packetIndex % 2 != 0) ? framesAscendingBaseId : framesDescendingBaseId);


	// Checking if correct Id was assigned
	if(*Id > (PCBCELLS_CAN_THERM_ID_BASE + pc->packetIndex * 10 + 9) || *Id < (PCBCELLS_CAN_THERM_ID_BASE + pc->packetIndex * 10 + 1)){
		return HAL_ERROR;
	}

	// checking ID value range
	if(framesAscendingBaseId > 9 || framesDescendingBaseId < 1){
		return HAL_ERROR;
	}

	// locking reserved ID and changing values of indicators
	framesAscendingBaseId++;
	framesDescendingBaseId--;
	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_CAN_ConfigData(PCBCells_TypeDef* pc, CAN_ScheduledMsg* msg, uint8_t thermId){

	// organizing proper assignment
	msg->GetData = CAN2_Therm_Handlers[thermId];

	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_CAN_SendFrames(PCBCells_TypeDef* pc){

	// Send scheduled messages on CAN bus
	CAN_HandleScheduled(&pc->pccan.hcan, &pc->pccan.CAN_msgBuffer);

	return HAL_OK;
}


HAL_StatusTypeDef PCBCells_CAN_ScaleValue(PCBCells_TypeDef* pc, uint8_t thermIndex, float* realValue){

	// checking if given thermistor index is correct
	if(thermIndex >= 9){
		return HAL_ERROR;
	}

	uint8_t binaryType = 0; 	// Init of variable that will store calculated binary type of temperature, its type will be correct to those which should be sent via CAN

	// Calculating binary type of temperature | refers to: binaryType = (realValue - offset)/ gain;
	binaryType = (*realValue + PCBCELLS_CAN_THERM_OFFSET) / PCBCELLS_CAN_THERM_GAIN;

	// assigning calculated binary type of temperature to correct index in array (stored in PCBCells_TypeDef), index of array is equal to thermIndex - 1
	pc->pcadc.PCBCells_temperatures[thermIndex] = binaryType;

	return HAL_OK;
}

void PCBCells_CAN_PackCAN2Temps(uint8_t* data, uint8_t thermId){

	// assigning data for CAN frame from PCBCells object's array's element
	switch (thermId) {
	    case 0:		// first therm
	        *data = pcbCells.pcadc.PCBCells_temperatures[0];
	        break;
	    case 1:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[1];
	        break;
	    case 2:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[2];
	        break;
	    case 3:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[3];
	        break;
	    case 4:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[5];
	        break;
	    case 5:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[6];
	        break;
	    case 6:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[7];
	        break;
	    case 7:
	    	*data = pcbCells.pcadc.PCBCells_temperatures[8];
	        break;
	    case 8:		// ninth therm
	    	*data = pcbCells.pcadc.PCBCells_temperatures[4];
	        break;
	    default:

	    	// return error status
	        return;
	}
}


void PCBCells_CAN_Get_CAN2_Data_Therm1(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 0);}
void PCBCells_CAN_Get_CAN2_Data_Therm2(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 1);}
void PCBCells_CAN_Get_CAN2_Data_Therm3(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 2);}
void PCBCells_CAN_Get_CAN2_Data_Therm4(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 3);}
void PCBCells_CAN_Get_CAN2_Data_Therm5(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 4);}
void PCBCells_CAN_Get_CAN2_Data_Therm6(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 5);}
void PCBCells_CAN_Get_CAN2_Data_Therm7(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 6);}
void PCBCells_CAN_Get_CAN2_Data_Therm8(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 7);}
void PCBCells_CAN_Get_CAN2_Data_Therm9(uint8_t *data){ PCBCells_CAN_PackCAN2Temps(data, 8);}
