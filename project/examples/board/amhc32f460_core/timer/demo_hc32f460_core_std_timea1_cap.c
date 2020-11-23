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
 * \brief TIMA1 ��ʱ�� CAP ���̣�ͨ����׼�ӿ�ʵ��
 *
 * - �������裺
 *   1. ʹ�öŰ��ߣ��� PIOE_2 �� PIOE_9 ���ӡ�
 *
 * - ʵ������
 *   1. TIMEA1 ͨ�� PIOE_2 ������� 2KHz �� PWM��
 *   2. TIMEA1 ��������ͨ�� 2 ʹ�� PIOE_9 ���Ų���
 *   3. ���ڴ�ӡ�����ò����ܵõ��� PWM �źŵ����ں�Ƶ�ʡ�
 *
 * \note
 *    
 *       
 *
 * \par Դ����
 * \snippet demo_hc32f460_std_tim0_cap.c src_hc32f460_std_tim0_cap
 *
 * \internal
 * \par Modification history
 * - 1.00 19-09-20  zp, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_hc32f460_std_tim0_cap
 * \copydoc demo_hc32f460_std_tim0_cap.c
 */

/** [src_hc32f460_std_tim0_cap] */
#include "ametal.h"
#include "am_vdebug.h"
#include "am_hc32f460_timea_cap.h"
#include "am_hc32f460_inst_init.h"
#include "demo_std_entries.h"
#include "demo_hc32f460_core_entries.h"

/**
 * \brief �������
 */
void demo_hc32f460_core_std_timea1_cap_entry (void)
{
    am_pwm_handle_t timea3_pwm_handle = am_hc32f460_timea3_pwm_inst_init();
    am_cap_handle_t timea1_cap_handle = am_hc32f460_timea1_cap_inst_init();
    
    AM_DBG_INFO("demo hc32f460_core std timea1 cap!\r\n");

    /* TIM0 ���Ƶ��Ϊ 2KHz �� PWM */
    am_pwm_config(timea3_pwm_handle, TIMERA_CH5, 500000 / 2, 500000);
    am_pwm_enable(timea3_pwm_handle, TIMERA_CH5);

    demo_std_timer_cap_entry(timea1_cap_handle, TIMERA_CH1);
}
/** [src_hc32f460_std_tim0_cap] */

/* end of file */