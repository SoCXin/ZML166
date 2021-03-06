/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 环境光传感器 TSL2560 例程，通过触发模式实现
 *
 * - 实验现象：
 *   1. 在门限触发中断中获取数据，并通过串口打印
 *
 * \internal
 * \par Modification history
 * - 1.00 20-04-26  fzb, first implementation
 * \endinternal
 */

#include "ametal.h"
#include "am_vdebug.h"
#include "am_sensor.h"
#include "am_sensor_tsl2560.h"
#include "am_delay.h"

/** \brief TSL2560传感器的ID */
const static int __tsl2560_id[2] = {AM_TSL2560_CHAN_1, AM_TSL2560_CHAN_2};

/** \brief TSL2560传感器数据 */
static am_sensor_val_t __tsl2560_data[2];

/**
 * \brief 通道1，CH0的回调函数
 */
static void __pfn_ch0 (void *p_arg, uint32_t trigger_src)
{
    am_sensor_handle_t handle = (am_sensor_handle_t)p_arg;
    if (trigger_src & AM_SENSOR_TRIGGER_THRESHOLD) {
        am_sensor_data_get(handle, &__tsl2560_id[0], 1, &__tsl2560_data[0]);
        am_kprintf("The channel %d,type is CH0, value is: %d !\r\n",
                   __tsl2560_id[0],
                   __tsl2560_data[0].val);
    }
}

/**
 * \brief 通道2，CH1的回调函数
 */
static void __pfn_ch1 (void *p_arg, uint32_t trigger_src)
{
    am_sensor_handle_t handle = (am_sensor_handle_t)p_arg;
    if (trigger_src & AM_SENSOR_TRIGGER_THRESHOLD) {
        am_sensor_data_get(handle, &__tsl2560_id[1], 1, &__tsl2560_data[1]);
        am_kprintf("The channel %d,type is CH1, value is: %d !\r\n",
                   __tsl2560_id[1],
                   __tsl2560_data[1].val);
    }
}

/**
 * \brief 例程入口
 */
void demo_std_tsl2560_int_entry (am_sensor_handle_t handle)
{
    /* 使能两路通道 */
    am_sensor_enable(handle, __tsl2560_id, 2, __tsl2560_data);

    /* 设置通道0，1的触发回调函数 */
    am_sensor_trigger_cfg(handle,
                          __tsl2560_id[0],
                          AM_SENSOR_TRIGGER_THRESHOLD,
                          __pfn_ch0,
                          (void*)handle);
    am_sensor_trigger_cfg(handle,
                          __tsl2560_id[1],
                          AM_SENSOR_TRIGGER_THRESHOLD,
                          __pfn_ch1,
                          (void*)handle);

    /* 打开通道0，1的触发模式 */
    am_sensor_trigger_on(handle, __tsl2560_id[0]);
    am_sensor_trigger_on(handle, __tsl2560_id[1]);

    while (1) {

    }
}

/* end of file */
