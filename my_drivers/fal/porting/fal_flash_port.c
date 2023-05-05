/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-26     armink       the first version
 */

#include "elog.h"
#include <fal.h>
#include "main.h"
#include "string.h"

static int init(void);
static int read(long offset, uint8_t *buf, size_t size);
static int write(long offset, const uint8_t *buf, size_t size);
static int erase(long offset, size_t size);

const struct fal_flash_dev stm32_onchip_flash =
    {
        .name = "stm32_onchip",
        .addr = 0x08000000,
        .len = 512 * 1024,
        .blk_size = 2 * 1024,
        .ops = {init, read, write, erase},
        .write_gran = 32};

static int init(void)
{
    return 0;
    /* do nothing now */
}

static int read(long offset, uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = stm32_onchip_flash.addr + offset;
    for (i = 0; i < size; i++, addr++, buf++)
    {
        *buf = *(uint8_t *)addr;
    }

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t i;
    uint64_t write_data;
    uint64_t read_data;
    uint32_t addr = stm32_onchip_flash.addr + offset;

    HAL_FLASH_Unlock();
    for (i = 0; i < size; i += 8, buf += 8, addr += 8)
    {
        /* write data */
        memcpy(&write_data, buf, 8);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, write_data);
        read_data = *(uint64_t *)addr;
        /* check data */
        if (read_data != write_data)
        {
            return -1;
        }
    }
    HAL_FLASH_Lock();

    return size;
}

static int erase(long offset, size_t size)
{
    HAL_StatusTypeDef flash_status;
//    size_t erase_pages;
    uint32_t addr = stm32_onchip_flash.addr + offset;
    uint32_t PAGEError = 0;

    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.PageAddress = addr;
    EraseInitStruct.NbPages = size / 2048;
    if (size % 2048 != 0)
    {
        EraseInitStruct.NbPages++;
    }

    /* start erase */
    HAL_FLASH_Unlock();
    flash_status = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
    if (flash_status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();

    return size;
}
