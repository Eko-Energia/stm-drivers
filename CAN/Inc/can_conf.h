/**
  ******************************************************************************
  * @file    can_conf.h
  * @author  AGH Eko-Energia

  * @Title   CAN Driver configuration file

  * @brief   This file contains defines, flags and macros that should be set (or configured) by user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_CAN_CONF_H_
#define INC_CAN_CONF_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/* Public constant macros -------------------------------------------------------------------------------------------- */

/*
 * Transmission policy configuration section
 */

/**
 * Automatic retransmission (bxCAN NART bit), applied by CAN_Init().
 *
 * 1 = enabled  (default) - the peripheral retries a frame that lost arbitration
 *                          or hit a bus error until it is acknowledged.
 * 0 = disabled           - a frame is transmitted once; a failed frame is dropped
 *                          and its mailbox is released immediately.
 *
 * @note With retransmission enabled a frame that is never acknowledged (no other
 *       node on the bus, missing termination, bit-timing mismatch) occupies its
 *       mailbox indefinitely. bxCAN has only 3 TX mailboxes, so three such frames
 *       block every further transmission. CAN_HandleScheduled() recovers from
 *       that case via CAN_CONF_TX_FAIL_LIMIT below.
 */
#define CAN_CONF_AUTO_RETRANSMISSION (1U)

/**
 * Consecutive failed enqueue attempts of a single scheduled message before
 * CAN_HandleScheduled() aborts all pending TX requests to unblock the mailboxes.
 *
 * Counts periods, not loop iterations: with a 1000 ms message and a limit of 3
 * the abort is issued after ~3 s of a wedged peripheral. Set to 0 to disable
 * the recovery and only skip the missed slots.
 */
#define CAN_CONF_TX_FAIL_LIMIT (3U)


#ifdef __cplusplus
}
#endif

#endif /* INC_CAN_CONF_H_ */
