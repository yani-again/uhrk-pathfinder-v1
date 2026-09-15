#include "y_lpuart.h"
#include "stm32g031xx.h"
#include <stddef.h>


#define HSI_RAW     16000000    /* 16 MHz */
#define LSI_RAW     32000       /* 32 KHz */
#define LSE_RAW     32768       /* 32.768 KHz */


static uint32_t get_sysclk(void);
static uint32_t get_pclk(void);


YLPUART_returnStatus ylpuart_setConfig(YLPUART_typeDef* handler)
{
    uint16_t lpuart_presc_values[16] = {1, 2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256, 256, 256, 256, 256};

    /* no configuration if controller is busy */
    if (handler->status == YLPUART_busy)
    {
        return YLPUART_error;
    }

    /* disable controller, as per RM0444 requirements to modify some bits only
     * when UE = 0
     */
    handler->status = YLPUART_off;
    handler->instance->CR1 &= ~USART_CR1_UE;

    /* Enable peripheral clock */
    RCC->APBENR1 |= RCC_APBENR1_LPUART1EN;

    if (handler->advancedConfig != NULL)
    {
        switch (handler->advancedConfig->receiver)
        {
            case YLPUART_disable:
                handler->instance->CR1 &= ~USART_CR1_RE;
                break;
            case YLPUART_enable:
                handler->instance->CR1 |= USART_CR1_RE;
                break;
            default:
                return YLPUART_error;
                break;
        }

        switch (handler->advancedConfig->transmitter)
        {
            case YLPUART_disable:
                handler->instance->CR1 &= ~USART_CR1_TE;
                break;
            case YLPUART_enable:
                handler->instance->CR1 |= USART_CR1_TE;
                break;
            default:
                return YLPUART_error;
                break;
        }

        switch (handler->advancedConfig->singleSample)
        {
            case YLPUART_disable:
                handler->instance->CR3 &= ~USART_CR3_ONEBIT;
                break;
            case YLPUART_enable:
                handler->instance->CR3 |= USART_CR3_ONEBIT;
                break;
            default:
                return YLPUART_error;
                break;
        }

        handler->instance->PRESC = handler->advancedConfig->prescaler & USART_PRESC_PRESCALER_Msk;
    }

    switch (handler->config->dataLength)
    {
        case YLPUART_data7:
            handler->instance->CR1 |= USART_CR1_M1;
            handler->instance->CR1 &= ~USART_CR1_M0;
            break;
        case YLPUART_data8:
            handler->instance->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
            break;
        case YLPUART_data9:
            handler->instance->CR1 &= ~USART_CR1_M1;
            handler->instance->CR1 |= USART_CR1_M0;
            break;
        default:
            return YLPUART_error;
            break;
    }

    switch (handler->config->parityEnable)
    {
        case YLPUART_disable:
            handler->instance->CR1 &= ~USART_CR1_PCE;
            break;
        case YLPUART_enable:
            handler->instance->CR1 |= USART_CR1_PCE;
            break;
    }

    /* kernel clock config */
    RCC->CCIPR |= (handler->config->kernelClock << RCC_CCIPR_LPUART1SEL_Pos);

    /* LPUARTDIV = (lpuart_ker_ck_pres * 256) / baud_rate */
    uint32_t lpuartdiv;
    uint32_t lpuart_ker_ck;
    uint32_t lpuart_ker_ck_pres;

    /* 
     * NOTE: YLPUART does not set up & configure clocks - its job is to do LPUART communication, not
     * fix user errors and mistakes
     */
    if (handler->config->kernelClock == YLPUART_pclk)
    {
        /* kernel clock = PCLK */
        lpuart_ker_ck = get_pclk();
    }
    else if (handler->config->kernelClock == YLPUART_sysclk)
    {
        /* kernel clock = SYSCLK */
        /*
         * Note: no support yet for:
         * - HSE
         * - PLLRCLK
         * - LSI
         */
        lpuart_ker_ck = get_sysclk();
    }
    else if (handler->config->kernelClock == YLPUART_hsi)
    {
        /* kernel clock = HSI */
        /* raw HSI, NOT HSISYS */
        lpuart_ker_ck = HSI_RAW;
    }
    else if (handler->config->kernelClock == YLPUART_lse)
    {
        /* kernel clock = LSE */
        lpuart_ker_ck = LSI_RAW;
    }
    else
    {
        /* nothing to do */
    }

    lpuart_ker_ck_pres = lpuart_ker_ck / lpuart_presc_values[(handler->instance->PRESC & USART_PRESC_PRESCALER_Msk) >> USART_PRESC_PRESCALER_Pos];

    /* finally enable controller */
    handler->instance->CR1 |= USART_CR1_UE;

    return YLPUART_ok;
}

YLPUART_returnStatus ylpuart_transmit(YLPUART_typeDef* handler, uint8_t* pData, uint16_t dataSize)
{
    /* no transfers if controller is busy */
    if (handler->status == YLPUART_busy)
    {
        return YLPUART_error;
    }

    handler->status = YLPUART_busy;

    for (; dataSize > 0; --dataSize)
    {
        /* wait for last transaction to finish */
        while (!(handler->instance->ISR & USART_ISR_TXE_TXFNF))
            ;

        handler->instance->TDR = *pData;
        ++pData;
    }

    /* wait for transaction to complete before releasing status */
    while (!(handler->instance->ISR & USART_ISR_TC))
        ;
    handler->status = YLPUART_ready;

    return YLPUART_ok;
}

YLPUART_returnStatus ylpuart_receive(YLPUART_typeDef* handler, uint8_t* pData, uint16_t dataSize)
{
    /* no transfers if controller is busy */
    if (handler->status == YLPUART_busy)
    {
        return YLPUART_error;
    }

    handler->status = YLPUART_busy;

    for (; dataSize > 0; --dataSize)
    {
        /* wait for data to be available */
        while (!(handler->instance->ISR & USART_ISR_RXNE_RXFNE))
            ;

        /* mask is used to remove parity bit from data */
        *pData = handler->instance->RDR & handler->mask;
        ++pData;
    }

    handler->status = YLPUART_ready;

    return YLPUART_ok;
}


static uint32_t get_sysclk(void)
{
    uint8_t  rcc_hsidiv_values[8]    = {1, 2, 4, 8, 16, 32, 64, 128};
    uint32_t sysclk;

    switch ((RCC->CFGR >> RCC_CFGR_SW_Pos) & RCC_CFGR_SW_Msk)
    {
        case (RCC_CFGR_SW_HSISYS):
            /* HSISYS */
            sysclk = HSI_RAW / rcc_hsidiv_values[(RCC->CR & RCC_CR_HSIDIV_Msk) >> RCC_CR_HSIDIV_Pos];
            break;
        case (RCC_CFGR_SW_HSE):
            /* HSE */
            sysclk = 0;
            break;
        case (RCC_CFGR_SW_PLLRCLK):
            /* PLLRCLK */
            sysclk = 0;
            break;
        case (RCC_CFGR_SW_LSI):
            /* LSI */
            sysclk = 0;
            break;
        case (RCC_CFGR_SW_LSE):
            /* LSE */
            sysclk = LSE_RAW;
            break;
        default:
            /* error */
            sysclk = 0;
            break;
    }

    return sysclk;
}

static uint32_t get_pclk(void)
{
    uint32_t pclk;
    uint16_t ahb_presc_values[16] = {1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    uint8_t  apb_presc_values[8]  = {1, 1, 1, 1, 2, 4, 8, 16};

    pclk = get_sysclk();
    pclk /= ahb_presc_values[(RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos];
    pclk /= apb_presc_values[(RCC->CFGR & RCC_CFGR_PPRE_Msk) >> RCC_CFGR_PPRE_Pos];

    return pclk;
}
