/**
  ******************************************************************************
  * @file    pcb_cells_adc.c
  * @author  Bartosz Rychlicki
  * @author  AGH Eko-Energia

  * @Title   Firmware for Cells' PCB's ADC peripheral
  *
  * @brief   This file contains functions' bodies, that implement functions logic and provide optimized workflow of PCB Cell's ADC peripheral
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes -----------------------------------------------------------------------------------  */
#include "pcb_cells_adc.h"
#include "pcb_cells_can.h"


/* Variables ----------------------------------------------------------------------------------  */
// Array of ADC1's Channels' numbers
static uint8_t PCBCells_ADC1_Channels[5] = {
		ADC1_CH1, ADC1_CH2, ADC1_CH3, ADC1_CH4, ADC1_CH11
};

// Array of ADC2's Channels' numbers
static uint8_t PCBCells_ADC2_Channels[4] = {
		ADC2_CH1, ADC2_CH2, ADC2_CH3, ADC2_CH4
};

/* Functions' bodies --------------------------------------------------------------------------  */

HAL_StatusTypeDef PCBCells_ADC_ReadValues(PCBCells_TypeDef* pc){

	// Read ADC1 channels in order to calculate real temperatures
	if(PCBCells_ADC1_ReadChannels(pc) != HAL_OK){
		return HAL_ERROR;
	}

	// Read ADC2 channels in order to calculate real temperatures
	if(PCBCells_ADC2_ReadChannels(pc) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_ADC1_ReadChannels(PCBCells_TypeDef* pc){

	float    adc1Temperature_f = 0.0f;		// init  of variable which stores real temperature on exact thermistor

	// Reading all ADC1's active channels
	for(int i = 0; i < 5; ++i){

		if(PCBCells_ADC_ReadTemperature(pc, ADC1_INDEX, i, &adc1Temperature_f) != HAL_OK){
			return HAL_ERROR;
		}

		// scaling and assigning temperature to Cells's PCB's object's field
		if(PCBCells_CAN_ScaleValue(pc, (uint8_t)i, &adc1Temperature_f) != HAL_OK){
			return HAL_ERROR;
		}

	}

	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_ADC2_ReadChannels(PCBCells_TypeDef* pc){

	float    adc1Temperature_f = 0.0f;		// init  of variable which stores real temperature on exact thermistor

	// Reading all ADC1's active channels
	for(int i = 0; i < 4; ++i){

		if(PCBCells_ADC_ReadTemperature(pc, ADC2_INDEX, i, &adc1Temperature_f) != HAL_OK){
			return HAL_ERROR;
		}

		// scaling and assigning temperature to Cells's PCB's object's field
		if(PCBCells_CAN_ScaleValue(pc, (uint8_t)(i + ADC_THERM_OFFSET), &adc1Temperature_f) != HAL_OK){
			return HAL_ERROR;
		}

	}

	return HAL_OK;
}

HAL_StatusTypeDef PCBCells_ADC_ReadTemperature(PCBCells_TypeDef* pc, uint8_t adcIndex, uint8_t thermIndex, float* retval){

	float    Rt 		  = 0.0f;		// calculated resistance of thermistor
	float    voltagePin_f = 0.0f;		// real value of calculated voltage of pin
	uint16_t voltagePin_b = 0;			// binary value of calculated voltage of pin


	// switching operations for ADC1 and ADC2
	switch(adcIndex){

		// Handle calculating temperature, whose value is converted by one of ADC1's channels
		case ADC1_INDEX:

			// getting converted value of ADC1 channels on exact pin
			if(ADC_ReadChannel(&pc->pcadc.hadc1, &pc->pcadc.cadc1, &pc->pcadc.badc1, PCBCells_ADC1_Channels[thermIndex], &voltagePin_b) != HAL_OK){
				return HAL_ERROR;
			}

			// calculating real value of voltage on pin
			if(ADC_GetValue(&pc->pcadc.hadc1, &pc->pcadc.cadc1, &pc->pcadc.badc1, VCC_SUPPLY_VOLTAGE, PCBCells_ADC1_Channels[thermIndex], &voltagePin_f) != HAL_OK){
				return HAL_ERROR;
			}

			break;

			// Handle calculating temperature, whose value is converted by one of ADC2's channels
		case ADC2_INDEX:

			// getting converted value of ADC1 channels on exact pin
			if(ADC_ReadChannel(&pc->pcadc.hadc2, &pc->pcadc.cadc2, &pc->pcadc.badc1, PCBCells_ADC2_Channels[thermIndex], &voltagePin_b) != HAL_OK){
				return HAL_ERROR;
			}

			// calculating real value of voltage on pin
			if(ADC_GetValue(&pc->pcadc.hadc2, &pc->pcadc.cadc2, &pc->pcadc.badc1, VCC_SUPPLY_VOLTAGE, PCBCells_ADC2_Channels[thermIndex], &voltagePin_f) != HAL_OK){
				return HAL_ERROR;
			}

			break;

		// return error status, in case wrong ADC's id was given
		default:
			return HAL_ERROR;

		}

	// Calculating resistance of thermistor
	Rt = (float)10000.0f * (VCC_SUPPLY_VOLTAGE - voltagePin_f)/voltagePin_f;

	// calculating and calibrating temperature
	*retval = PCBCells_ADC_NTC_calibrateTemperature(PCBCells_ADC_NTC_GetTemperature(Rt) + 1.4f);

	return HAL_OK;
}


float PCBCells_ADC_NTC_calibrateTemperature(float measured){

    // calibrating points
    float T1 = 25.0f;  // real temperature
    float M1 = 28.0f;  // measured value
    float T2 = 36.6f;
    float M2 = 39.2f;

    // linear interpolation
    return T1 + (measured - M1) * (T2 - T1) / (M2 - M1);
}

float PCBCells_ADC_NTC_GetTemperature(float Rt){


	// table of temperatures
    static const float T[101] = {
        0,1,2,3,4,5,6,7,8,9,10,
        11,12,13,14,15,16,17,18,19,20,
        21,22,23,24,25,26,27,28,29,30,
        31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,
        51,52,53,54,55,56,57,58,59,60,
        61,62,63,64,65,66,67,68,69,70,
        71,72,73,74,75,76,77,78,79,80,
        81,82,83,84,85,86,87,88,89,90,
        91,92,93,94,95,96,97,98,99,100
    };


    // table of Resistances
    static const float R[101] = {
        27515,26344,25230,24169,23159,22197,21281,20407,19574,18780,
        18017,17300,16611,15953,15324,14724,14150,13602,13079,12578,
        12099,11642,11204,10785,10384,10000,9632,9280,8942,8619,
        8309,8012,7727,7453,7191,6940,6699,6467,6244,6030,
        5825,5628,5438,5256,5081,4913,4751,4595,4445,4301,
        4168,4029,3900,3776,3657,3542,3431,3324,3222,3122,
        3027,2935,2845,2760,2677,2597,2520,2445,2373,2304,
        2237,2172,2109,2049,1991,1934,1879,1826,1775,1725,
        1678,1632,1586,1543,1501,1461,1421,1383,1346,1310,
        1276,1242,1210,1179,1147,1118,1090,1061,1034,1008,
        983
    };




    // if out of range
    if (Rt >= R[0])  { return T[0]; }        // colder than 0°C
    if (Rt <= R[100]){ return T[100]; }      // warmer than 100°C



    // search for closes resistance
    for (int i = 0; i < 100; ++i) {

    	// checking if exact R is higher and lower to the closes resistance from table
        if (Rt <= R[i] && Rt >= R[i + 1]) {

        	// returning interpolated value of temperature
            return T[i] + (T[i + 1] - T[i]) * (Rt - R[i]) / (R[i + 1] - R[i]);

        }

    }


    //returning error value;
    return ((float)(-1.0f));
}
