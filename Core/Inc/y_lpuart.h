#ifndef Y_LPUART_H
#define Y_LPUART_H


#include <stdint.h>


#define YPERI_BASE  0x40000000UL
#define YLPUART1    ((YLPUART_instance *) (YPERI_BASE + 0x8000U))


typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
    volatile uint32_t PRESC;
} YLPUART_instance;


typedef enum
{
    YLPUART_ok    = 0,
    YLPUART_error = 0,
} YLPUART_returnStatus;

typedef enum
{
    YLPUART_disable = 0,
    YLPUART_enable  = 1
} YLPUART_enableDisable;

typedef enum
{
    YLPUART_ready = 0,
    YLPUART_busy  = 1,
    YLPUART_off   = 2
} YLPUART_status;

typedef enum
{
    YLPUART_data7 = 0,
    YLPUART_data8 = 1,
    YLPUART_data9 = 2
} YLPUART_dataLength;

typedef enum
{
    YLPUART_stop1 = 0,
    YLPUART_stop2 = 1,
} YLPUART_stopBits;

typedef enum
{
    /*
     * NOTE: using my own hard-coded values as I couldn't find device headers
     * for them from STM
     */
    YLPUART_pclk   = 0U,
    YLPUART_sysclk = 1U,
    YLPUART_hsi    = 2U,
    YLPUART_lse    = 3U
} YLPUART_kernelClock;


typedef struct
{
    YLPUART_enableDisable   transmitter;
    YLPUART_enableDisable   receiver;
    YLPUART_enableDisable   singleSample;
    uint8_t                 prescaler;
} YLPUART_advancedConfig;

typedef struct
{
    uint32_t                baudRate;
    YLPUART_dataLength      dataLength;
    YLPUART_enableDisable   parityEnable;
    YLPUART_kernelClock     kernelClock;
} YLPUART_config;


typedef struct
{
    YLPUART_instance*       instance;
    YLPUART_status          status;
    YLPUART_config*         config;
    YLPUART_advancedConfig* advancedConfig;
    uint16_t                mask;
} YLPUART_typeDef;


YLPUART_returnStatus ylpuart_setConfig      (YLPUART_typeDef*);                             /* handler */
YLPUART_returnStatus ylpuart_transmit       (YLPUART_typeDef*, uint8_t*, uint16_t);         /* handler, data, size */
YLPUART_returnStatus ylpuart_receive        (YLPUART_typeDef*, uint8_t*, uint16_t);         /* handler, data, size */


#endif


/* example usage */

/*
 * [ ] ylpuart_config(instance, config);
 * [ ] ylpuart_transmit(instance, data, size)
 * [ ] ylpuart_receive_one(instance)
 * [ ] ylpuart_receive(instance, data, size)
 * [ ] ylpuart_receive_match(instance, match)
 * [ ] ylpuart_receive_it(instance, data, size)
 * [ ] ylpuart_receive_match_it(instance, match)
 * [ ] while (ylpuart_busy(instance)); // wait
 */
