/*
 * mpu.h
 *
 *  Created on: Mar 31, 2020
 *      Author: JaYniL~LM10
 */

#ifndef MPU_H_
#define MPU_H_

#define MPU_CONFIG_PRIV_DEFAULT     4
#define MPU_CONFIG_HARDFLT_NMI      2
#define MPU_CONFIG_NONE             0

//*****************************************************************************
//
// Flags for the region size to be passed to MPURegionSet.
//
//*****************************************************************************
#define MPU_RGN_SIZE_32B            (4 << 1)
#define MPU_RGN_SIZE_64B            (5 << 1)
#define MPU_RGN_SIZE_128B           (6 << 1)
#define MPU_RGN_SIZE_256B           (7 << 1)
#define MPU_RGN_SIZE_512B           (8 << 1)

#define MPU_RGN_SIZE_1K             (9 << 1)
#define MPU_RGN_SIZE_2K             (10 << 1)
#define MPU_RGN_SIZE_4K             (11 << 1)
#define MPU_RGN_SIZE_8K             (12 << 1)
#define MPU_RGN_SIZE_16K            (13 << 1)
#define MPU_RGN_SIZE_32K            (14 << 1)
#define MPU_RGN_SIZE_64K            (15 << 1)
#define MPU_RGN_SIZE_128K           (16 << 1)
#define MPU_RGN_SIZE_256K           (17 << 1)
#define MPU_RGN_SIZE_512K           (18 << 1)

#define MPU_RGN_SIZE_1M             (19 << 1)
#define MPU_RGN_SIZE_2M             (20 << 1)
#define MPU_RGN_SIZE_4M             (21 << 1)
#define MPU_RGN_SIZE_8M             (22 << 1)
#define MPU_RGN_SIZE_16M            (23 << 1)
#define MPU_RGN_SIZE_32M            (24 << 1)
#define MPU_RGN_SIZE_64M            (25 << 1)
#define MPU_RGN_SIZE_128M           (26 << 1)
#define MPU_RGN_SIZE_256M           (27 << 1)
#define MPU_RGN_SIZE_512M           (28 << 1)

#define MPU_RGN_SIZE_1G             (29 << 1)
#define MPU_RGN_SIZE_2G             (30 << 1)
#define MPU_RGN_SIZE_4G             (31 << 1)

//*****************************************************************************
//
// Flags for the permissions to be passed to MPURegionSet.
//
//*****************************************************************************
#define MPU_RGN_PERM_EXEC           0x00000000
#define MPU_RGN_PERM_NOEXEC         0x10000000
#define MPU_RGN_PERM_PRV_NO_USR_NO  0x00000000
#define MPU_RGN_PERM_PRV_RW_USR_NO  0x01000000
#define MPU_RGN_PERM_PRV_RW_USR_RO  0x02000000
#define MPU_RGN_PERM_PRV_RW_USR_RW  0x03000000
#define MPU_RGN_PERM_PRV_RO_USR_NO  0x05000000
#define MPU_RGN_PERM_PRV_RO_USR_RO  0x06000000

//*****************************************************************************
//
// Flags for the sub-region to be passed to MPURegionSet.
//
//*****************************************************************************
#define MPU_SUB_RGN_DISABLE_0       0x00000100
#define MPU_SUB_RGN_DISABLE_1       0x00000200
#define MPU_SUB_RGN_DISABLE_2       0x00000400
#define MPU_SUB_RGN_DISABLE_3       0x00000800
#define MPU_SUB_RGN_DISABLE_4       0x00001000
#define MPU_SUB_RGN_DISABLE_5       0x00002000
#define MPU_SUB_RGN_DISABLE_6       0x00004000
#define MPU_SUB_RGN_DISABLE_7       0x00008000
#define MPU_SUB_RGN_ENABLE_ALL      0x00000000
#define MPU_SUB_RGB_DISABLE_ALL     0x0000FF00
//*****************************************************************************
//
// Flags to enable or disable a region, to be passed to MPURegionSet.
//
//*****************************************************************************
#define MPU_RGN_ENABLE              1
#define MPU_RGN_DISABLE             0

//*****************************************************************************
//
#define FLASH_MEM_TEX_S_C_B             0x00020000
#define PERIPHERAL_TEX_S_C_B           (0b101 <<16)
#define SRAM_TEX_S_C_B                 (0b110 <<16)
#define PERIPH_BASE_ADD                 0x40000000
#define SRAM_BASE_ADD_1                 0x20000000
#define SRAM_BASE_ADD_2                 0x20004000
#define SRAM_BASE_ADD_3                 0x20008000

#define MPU_CTRL 0xE000ED94
#define MPU_RNR  0xE000ED98
#define MPU_RBAR 0xE000ED9C
#define MPU_RASR 0xE000EDA0

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************

uint32_t *address_range;

void MPU_init();


#endif /* MPU_H_ */
