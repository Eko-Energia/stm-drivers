# CAN_DRIVER
The driver is designed to simplify the usage of CAN bus communication on STM32F303 (might work on other MCUs). Features  of the module are designed specificaly to be used in PERLA - a solar car built by AGH Eko-Energia

## Limitations
The module currently sets filter banks to accept every incoming frame by default. (TBD)

## Configuration

User settable options live in `Inc/can_conf.h`.

| Macro | Default | Meaning |
| --- | --- | --- |
| `CAN_CONF_AUTO_RETRANSMISSION` | `1U` (enabled) | Transmission policy applied by `CAN_Init()`. `1` clears the bxCAN `NART` bit, so the peripheral retries a frame that lost arbitration or hit a bus error until it is acknowledged. `0` sets `NART`, so a frame is sent once and its mailbox is released even if the attempt failed. |
| `CAN_CONF_TX_FAIL_LIMIT` | `3U` | Consecutive failed enqueue attempts of one scheduled message before `CAN_HandleScheduled()` aborts all pending TX requests. Counts periods, not loop iterations. `0` disables the recovery. |

`CAN_Init()` writes `NART` between `HAL_CAN_ConfigFilter()` and `HAL_CAN_Start()`, which is the only window where the bit is writable, and mirrors the value into `hcan.Init.AutoRetransmission` so a later `HAL_CAN_Init()` does not revert it. The value configured in CubeMX is therefore overridden by this driver.

### Choosing a transmission policy

Automatic retransmission is the default because it is what the bus normally wants: a frame that loses arbitration is resent instead of dropped.

It has one failure mode worth knowing about. bxCAN has **three** TX mailboxes. A frame that is never acknowledged - single node on the bus, missing 120 Ω termination, bit timing mismatch - is retried by hardware forever and never releases its mailbox. After three such frames every `HAL_CAN_AddTxMessage()` fails. `CAN_HandleScheduled()` handles this in two ways: a failed enqueue costs one skipped period instead of a retry on every main loop iteration, and after `CAN_CONF_TX_FAIL_LIMIT` consecutive failures the pending requests are aborted so the queue can drain.

For a pure periodic status frame, where the next period supersedes stale data anyway, `CAN_CONF_AUTO_RETRANSMISSION = 0` is the safer choice - a failed frame is dropped immediately and can never block a mailbox.

## Usage

### Initialization
Requires HAL CAN instance to be imported from main.c as well as a buffer `CAN_scheduledMsgList`
```C
// app.c

extern CAN_HandleTypeDef;

struct CAN_scheduledMsgList buffer;

void app_main(void)
{
    CAN_Init(&hcan);
    ...
}

```
Unlimited number of buffers can be created, to change maximum number of messages for one buffer edit `CAN_MAX_MSG`

### GetData
Creating a frame requires a function that will fill the outgoing buffer with data on every call. 

```C
static void chargerGetData(uint8_t *data, void *context)
{
	// preserve one decimal place
	float maxCurrent = maxChargerCurrent * 10;
	float maxVoltage = MAX_CHARGER_VOLTAGE * 10;

	uint16_t maxChargerCurrentInt = (uint16_t) maxCurrent;
	uint16_t maxVoltageInt = (uint16_t) maxVoltage;

	data[0] = GET_BYTE(maxVoltageInt, 0);
	data[1] = GET_BYTE(maxVoltageInt, 1);
	data[2] = GET_BYTE(maxChargerCurrentInt, 0);
	data[3] = GET_BYTE(maxChargerCurrentInt, 1);
}
```


### Adding a frame to the buffer
```C
struct CAN_scheduledMsg chargerComms;

chargerComms.header.extId = CANID_RCD_STATIC_CHARGER1COMMS;
chargerComms.header.DLC = 3;
chargerComms.header.IDE = CAN_ID_EXT;
chargerComms.header.RTR = CAN_RTR_DATA;
chargerComms.lastTick = 0;
chargerComms.periodMs = 1000;
chargerComms.getData = chargerGetData;

CAN_AddScheduledMsg(chargerComms, &CAN_buffer);
```

### Handling messages added to the buffer

The function should propably be on the end of the main loop
```C
CAN_HandleScheduled(&hcan, &buffer);
```

Timing notes:
- The due check is `(now - lastTick) >= periodMs`, which stays correct across the 32 bit `HAL_GetTick()` wrap (~49.7 days).
- On success `lastTick` advances by whole periods, so the cadence does not drift with the execution time of the send. If a message falls more than one period behind it resynchronises to the current tick instead of emitting a catch-up burst.
- On a failed enqueue the message is re-armed for the next period and the remaining messages in the list are still processed. `txFailCount` in `CAN_scheduledMsg` is managed by the driver, do not write it.

### Removing a frame
```C
CAN_RemoveScheduledMsg(CANID_RCD_STATIC_CHARGER1COMMS);
```




# TO-DO
- [ ] Add non-periodic frame handling (period = 0), send once on handle and delete
- [ ] Handling on bus errors and generic error messages
- [ ] Filter configuration
- [ ] Handling of received messages