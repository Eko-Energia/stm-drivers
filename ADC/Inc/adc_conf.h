/**
  ******************************************************************************
  * @file    adc_conf.h
  * @author  Bartosz Rychlicki
  * @author  AGH Eko-Energia

  * @Title   ADC Driver configuration file

  * @brief   This file contains defines, flags and macros that should be sey (or configured) by user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_ADC_CONF_H_
#define INC_ADC_CONF_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/* Private constant macros -------------------------------------------------------------------------------------------- */
#define ADC1_USED_CHANNELS (3)  //< Macro defines number of channels for ADC1,
#define ADC2_USED_CHANNELS (3)  //< Macro defines number of channels for ADC2,

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_CONF_H_ */
