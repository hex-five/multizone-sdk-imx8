/* Copyright(C) 2020 Hex Five Security, Inc. - All Rights Reserved */

#ifndef HEXFIVE_PLATFORM_H
#define HEXFIVE_PLATFORM_H


/************************************************************************************
* 			               IMX8QM-MEK PLATFORM GENERIC			
************************************************************************************/

#define CPU_FREQ                        (264000000)              /*!< Hz */
#define RTC_FREQ                        (264000000)              /*!< Hz */

#define MPU_REGIONS                     (8)

#define UART_IRQn                       LPUART_IRQn

#define LED                             0
#define LED_PIN                         0

#define BTN                             0
#define BTN_PIN                         0
#define BTN_IRQn                        0


#define UART_RX_OFFSET                  (0x08)
#define UART_TX_OFFSET                  (0x09)
#define UART_RX                         (1 << UART_RX_OFFSET)
#define UART_TX                         (1 << UART_TX_OFFSET)

#define USART_BAUDRATE                  (115200)



/************************************************************************************
* 			            IMX8QM (CM4) PERIPHERALS SPECIFIC				
************************************************************************************/

#define PERIPH_BASE                 (0x40000000UL)              /*!< AHB/ABP Peripherals                */


/* Low Power Universal Asynchronous Receiver/Transmitter (LPUART) */
#define LPUART_BASE                 (PERIPH_BASE + 0x1220000)
#define LPUART_REG(offset)          _REG32(LPUART_BASE, offset)

#define LPUART_VERID                (0x00)                      /*!< Version ID Register                */
#define LPUART_PARAM                (0x04)                      /*!< Parameter Register                 */
#define LPUART_GLOBAL               (0x08)                      /*!< LPUART Global Register             */
#define LPUART_PINCFG               (0x0C)                      /*!< LPUART Pin Configuration Register  */
#define LPUART_BAUD                 (0x10)                      /*!< LPUART Baud Rate Register          */
#define LPUART_STAT                 (0x14)                      /*!< LPUART Status Register             */
#define LPUART_CTRL                 (0x18)                      /*!< LPUART Control Register            */
#define LPUART_DATA                 (0x1C)                      /*!< LPUART Data Register               */
#define LPUART_MATCH                (0x20)                      /*!< LPUART Match Address Register      */
#define LPUART_MODIR                (0x24)                      /*!< LPUART Modem IrDA Register         */
#define LPUART_FIFO                 (0x28)                      /*!< LPUART FIFO Register               */
#define LPUART_WATER                (0x2C)                      /*!< LPUART Watermark Register          */

#define LPUART_VERID_VAL            (0x04010003)

#define LPUART_GLOBAL_RST           (1 << 1)

#define LPUART_BAUD_SBR_MASK        (0x1FFF)
#define LPUART_BAUD_SBNS_MASK       (1 << 13)
#define LPUART_BAUD_OSR_MASK        (0x1F000000)
#define LPUART_BAUD_SBNS_M10        (1 << 29)

#define LPUART_STAT_RDRF            (1 << 21)                   /*!< Receive Data Register Full Flag    */
#define LPUART_STAT_TC              (1 << 22)                   /*!< Transmission Complete Flag         */
#define LPUART_STAT_TRDE            (1 << 23)                   /*!< Transmit Data Register Empty Flag  */

#define LPUART_CTRL_PT_MASK         (1 << 0)                    /*!< Parity Type Mask                   */
#define LPUART_CTRL_PT_E            (0 << 0)                    /*!< Parity Type - Even parity          */
#define LPUART_CTRL_PT_O            (1 << 0)                    /*!< Parity Type - Odd parity           */
#define LPUART_CTRL_PE_MASK         (1 << 1)                    /*!< Parity Enable Mask                 */
#define LPUART_CTRL_PE_N            (0 << 1)                    /*!< Parity Enable - No parity          */
#define LPUART_CTRL_PE_P            (1 << 1)                    /*!< Parity Enable - Parity enable      */
#define LPUART_CTRL_M_MASK          (1 << 4)                    /*!< 9-Bit or 8-Bit Mode Select Mask    */
#define LPUART_CTRL_M_8B            (0 << 4)                    /*!< 9-Bit or 8-Bit Mode Select - 8-Bit */
#define LPUART_CTRL_M_9B            (1 << 4)                    /*!< 9-Bit or 8-Bit Mode Select - 9-Bit */
#define LPUART_CTRL_RE_MASK         (1 << 18)                   /*!< Receiver Enable Mask               */
#define LPUART_CTRL_RE_D            (0 << 18)                   /*!< Receiver Enable - disable          */
#define LPUART_CTRL_RE_E            (1 << 18)                   /*!< Receiver Enable - enable           */
#define LPUART_CTRL_TE_MASK         (1 << 19)                   /*!< Transmitter Enable Mask            */
#define LPUART_CTRL_TE_D            (0 << 19)                   /*!< Transmitter Enable - disable       */
#define LPUART_CTRL_TE_E            (1 << 19)                   /*!< Transmitter Enable - enable        */

#define LPUART_CTRL_RXFE            (1 << 3)
#define LPUART_CTRL_TXFE            (1 << 7)
#define LPUART_CTRL_RXFLUSH         (1 << 14)
#define LPUART_CTRL_TXFLUSH         (1 << 15)

#define LPUART_IRQn                 (00)



/************************************************************************************
* 			                CORTEX-M4 ARCH SPECIFIC				
************************************************************************************/

/* System Control Space */
#define SCS_BASE                    (0xE000E000UL)              /*!< System Control Space (0xE000E000-0xE000EFFF) */
#define MISC_BASE                   (SCS_BASE + 0x0000UL)       /*!< Misc System Control Registers (0xE000E000-0xE000E00F) */ 
#define SYSTICK_BASE                (SCS_BASE + 0x0010UL)       /*!< SysTick Timer (0xE000E010-0xE000E01F) */
#define NVIC_BASE                   (SCS_BASE + 0x0100UL)       /*!< Nested Vectored Interrupt Controller (0xE000E100-) */
#define SCB_BASE                    (SCS_BASE + 0x0D00UL)       /*!< System Control Block (0xE000E0D0-) */
#define MPU_BASE                    (SCS_BASE + 0x0D90UL)       /*!< Memory Protection Unit (0xE000ED90-) */

/* System Control Block (SCB) */
#define SCB_CPUID                   (SCB_BASE + 0x00UL)         /*!< CPUID Base Register */
#define SCB_ICSR                    (SCB_BASE + 0x04UL)         /*!< Interrupt Control and State Register */
#define SCB_VTOR                    (SCB_BASE + 0x08UL)         /*!< Vector Table Offset Register */
#define SCB_AIRCR                   (SCB_BASE + 0x0CUL)         /*!< Application Interrupt and Reset Control Register */
#define SCB_SCR                     (SCB_BASE + 0x10UL)         /*!< System Control Register */
#define SCB_CCR                     (SCB_BASE + 0x14UL)         /*!< Configuration and Control Register */
#define SCB_SHPR1                   (SCB_BASE + 0x18UL)         /*!< System Handler Priority Register 1 */
#define SCB_SHPR2                   (SCB_BASE + 0x1CUL)         /*!< System Handler Priority Register 2 */
#define SCB_SHPR3                   (SCB_BASE + 0x20UL)         /*!< System Handler Priority Register 3 */
#define SCB_SHCSR                   (SCB_BASE + 0x24UL)         /*!< System Handler Control and State Register */
#define SCB_CFSR                    (SCB_BASE + 0x28UL)         /*!< Configurable Fault Status Register */
#define SCB_HFSR                    (SCB_BASE + 0x2CUL)         /*!< HardFault Status Register */
#define SCB_DFSR                    (SCB_BASE + 0x30UL)         /*!< Debug Fault Status Register */
#define SCB_MMFAR                   (SCB_BASE + 0x34UL)         /*!< MemManage Fault Address Register */
#define SCB_BFAR                    (SCB_BASE + 0x38UL)         /*!< BusFault Address Register */
#define SCB_AFSR                    (SCB_BASE + 0x3CUL)         /*!< Auxiliary Fault Status Register */
#define SCB_CPACR                   (SCB_BASE + 0x88UL)         /*!< Coprocessor Access Control Register */

/* Memory Protection Unit (MPU) */
#define MPU_TYPE                    (MPU_BASE + 0x00UL)         /*!< MPU Type Register */
#define MPU_CTRL                    (MPU_BASE + 0x04UL)         /*!< MPU Control Register */
#define MPU_RNR                     (MPU_BASE + 0x08UL)         /*!< MPU Region Number Register */
#define MPU_RBAR                    (MPU_BASE + 0x0CUL)         /*!< MPU Region Base Address Register */
#define MPU_RASR                    (MPU_BASE + 0x10UL)         /*!< MPU Region Attribute and Size Register */
#define MPU_RBAR1                   (MPU_BASE + 0x14UL)         /*!< MPU Region Base Address Alias1 Register */
#define MPU_RASR1                   (MPU_BASE + 0x18UL)         /*!< MPU Region Attribute and Size Alias1 Register */
#define MPU_RBAR2                   (MPU_BASE + 0x1CUL)         /*!< MPU Region Base Address Alias2 Register */
#define MPU_RASR2                   (MPU_BASE + 0x20UL)         /*!< MPU Region Attribute and Size Alias2 Register */
#define MPU_RBAR3                   (MPU_BASE + 0x24UL)         /*!< MPU Region Base Address Alias3 Register */
#define MPU_RASR3                   (MPU_BASE + 0x28UL)         /*!< MPU Region Attribute and Size Alias3 Register */



/************************************************************************************
* 			                    HELPERS				
************************************************************************************/

#define _REG64(base, offset)        (*(volatile uint64_t *)((base) + (offset)))
#define _REG32(base, offset)        (*(volatile uint32_t *)((base) + (offset)))
#define _REG16(base, offset)        (*(volatile uint16_t *)((base) + (offset)))
#define _REG8(base, offset)         (*(volatile uint8_t *)((base) + (offset)))


#endif /* HEXFIVE_PLATFORM_H */
