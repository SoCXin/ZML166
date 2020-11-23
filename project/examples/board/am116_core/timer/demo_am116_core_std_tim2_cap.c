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
 * \brief TIM2 ��ʱ�� CAP ���̣�ͨ����׼�ӿ�ʵ��
 *
 * - �������裺
 *   1. ʹ�öŰ��ߣ��� PIOB_0 �� PIOA_0 ���ӡ�
 *
 * - ʵ������
 *   1. TIM3 ͨ�� PIOB_0 ������� 2KHz �� PWM��
 *   2. TIM2 ��������ͨ�� 1 ʹ�� PIOA_0 ���Ų���
 *   3. ���ڴ�ӡ�����ò����ܵõ��� PWM �źŵ����ں�Ƶ�ʡ�
 *
 * \note
 *    ����۲촮�ڴ�ӡ�ĵ�����Ϣ����Ҫ�� PIOA_9 �������� PC ���ڵ� RXD��
 *
 * \par Դ����
 * \snippet demo_am116_core_std_tim2_cap.c src_am116_core_std_tim2_cap
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-22  nwt, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_am116_core_std_tim2_cap
 * \copydoc demo_am116_core_std_tim2_cap.c
 */

/** [src_am116_core_std_tim2_cap] */
#include "ametal.h"
#include "am_pwm.h"
#include "am_vdebug.h"
#include "am_zlg116_inst_init.h"
#include "demo_std_entries.h"
#include "demo_am116_core_entries.h"

/**
 * \brief �������
 */
void demo_am116_core_std_tim2_cap_entry (void)
{
    am_pwm_handle_t pwm_handle = am_zlg116_tim3_pwm_inst_init();

    AM_DBG_INFO("demo am116_core std tim2 cap!\r\n");

    /* TIM3 ���Ƶ��Ϊ 2KHz �� PWM */
    am_pwm_config(pwm_handle, 2, 500000 / 2, 500000);
    am_pwm_enable(pwm_handle, 2);

    demo_std_timer_cap_entry(am_zlg116_tim2_cap_inst_init(), 0);
}
/** [src_am116_core_std_tim2_cap] */

/* end of file */