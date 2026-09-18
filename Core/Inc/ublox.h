#ifndef UBLOX_H
#define UBLOX_H


#include <stdint.h>


/* UBX datatypes */
typedef uint8_t     UBX_U1;
typedef int8_t      UBX_I1;
typedef uint8_t     UBX_X1;
typedef uint8_t     UBX_E1;
typedef uint16_t    UBX_U2;
typedef int16_t     UBX_I2;
typedef uint16_t    UBX_X2;
typedef uint16_t    UBX_E2;
typedef uint32_t    UBX_U4;
typedef int32_t     UBX_I4;
typedef uint32_t    UBX_X4;
typedef uint32_t    UBX_E4;
typedef uint8_t     UBX_L;

/* preamble */
#define UBX_PREAMBLE    ((UBX_U2) 0xB562)
#define UBX_PREAMBLE_A  ((UBX_U1) 0xB5)
#define UBX_PREAMBLE_B  ((UBX_U1) 0x62)

/* UBX message class & IDs */
#define UBX_MGA_ANO         0x1320
#define UBX_MGA_INI         0x1340
#define UBX_MON_GNSS        0x0A28
#define UBX_MON_VER         0x0A04
#define UBX_MON_HW3         0x0A37
#define UBX_MON_RXR         0x0A21
#define UBX_NAV_PVT         0x0107
#define UBX_NAV_DOP         0x0104
#define UBX_NAV_RESETODO    0x0110

/* CFG messages */
/* CFG_I2C */
#define CFG_I2C_Address             0x20510001
#define CFG_I2C_ExtendedTimeout     0x10510002
#define CFG_I2C_Enabled             0x10510003

/* CFG_I2CINPROT */
#define CFG_I2CINPROT_UBX           0x10710001
#define CFG_I2CINPROT_NMEA          0x10710002

/* CFG_I2COUTPROT */
#define CFG_I2COUTPROT_UBX          0x10720001
#define CFG_I2COUTPROT_NMEA         0x10720002

/* CFG_UART1 */
#define CFG_UART1_BaudRate          0x40520001
#define CFG_UART1_StopBits          0x20520002
#define CFG_UART1_DataBits          0x20520003
#define CFG_UART1_Parity            0x20520004
#define CFG_UART1_Enabled           0x10520005

/* CFG_UART1INPROT */
#define CFG_UART1INPROT_UBX         0x10730001
#define CFG_UART1INPROT_NMEA        0x10730002

/* CFG_UART1OUTPROT */
#define CFG_UART1OUTPROT_UBX        0x10730001
#define CFG_UART1OUTPROT_NMEA       0x10730002

/* CFG_HW */
#define CFG_HW_ANT_CFG_VOLCTRL      0x10a3002e


/* functions */
uint16_t UBX_CalculateChecksum(uint8_t *pData, uint16_t length);


#endif
