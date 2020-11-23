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
 * \brief TRNG ������������̣�ͨ�� HW ��ӿ�ʵ��
 *
 * - ʵ������
 *
 *   TRNG ģ���ϵ����һ�����������ϵ����������Աȣ������ͬ��TRNG�����������ȷ����ӡ
 *   ��Trng generate tset success!\r\n��,��֮��ӡ ��Trng generate tset failed!\r\n����
 *
 * \note
 *    1. ����۲촮�ڴ�ӡ�ĵ�����Ϣ����Ҫ�� PIOA_10 �������� PC ���ڵ� TXD��
 *       PIOA_9 �������� PC ���ڵ� RXD��
 *    2. ������Դ���ʹ���뱾������ͬ����Ӧ�ں�������ʹ�õ�����Ϣ�������
 *      ���磺AM_DBG_INFO()����
 *
 * \par Դ����
 * \snippet demo_zsl42x_hw_trng_dac.c src_zsl42x_hw_trng_generate
 *
 *
 * \internal
 * \par Modification History
 * - 1.00 19-10-12  ly, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_zsl42x_hw_trng_generate
 * \copydoc demo_zsl42x_hw_trng_generate.c
 */

/** [src_zsl42x_hw_trng_generate] */
#include "ametal.h"
#include "am_zsl42x.h"
#include "am_int.h"
#include "am_delay.h"
#include "am_board.h"
#include "am_zsl42x_trng.h"
#include "hw/amhw_zsl42x_trng.h"

/*******************************************************************************
  ȫ�ֱ���
*******************************************************************************/
static amhw_zsl42x_trng_t       *gp_hw_trng   = NULL;  /**< \brief trng ���� */

/**< \brief �ϵ��������������� */
static uint32_t rng_data1[2] = {0};

/**< \brief ���ϵ��������������� */
static uint32_t rng_data2[2] = {0};

/**
 * \brief �������������
 */
static void rng_generate (uint8_t env_flag)
{

    /* �����Դ��· */
    amhw_zsl42x_trng_cir_enable (gp_hw_trng);

    if (env_flag == AM_ZSL42x_TRNG_POWER_ON){
        /* ѡ������װ�س�ʼֵ */
        amhw_zsl42x_trng_load_set (gp_hw_trng, AM_TRUE);
    }else{
        /* ѡ������װ�س�ʼֵ */
        amhw_zsl42x_trng_load_set (gp_hw_trng, AM_FALSE);
    }

    /* ѡ������ʽ */
    amhw_zsl42x_trng_load_set (gp_hw_trng, AM_TRUE);

    /* ѡ����λ���� */
    amhw_zsl42x_trng_load_set (gp_hw_trng, AMHW_ZSL42x_TRNG_CNT_256);

    /* ������������� */
    amhw_zsl42x_trng_run (gp_hw_trng);

    /* �ȴ�������� */
    while (amhw_zsl42x_trng_stu_check (gp_hw_trng)){

        ;
    }

    if (env_flag == AM_ZSL42x_TRNG_POWER_ON){
        /* ѡ������װ�س�ʼֵ */
        amhw_zsl42x_trng_load_set (gp_hw_trng, AM_FALSE);
    }else{
        ;
    }

    /* ѡ������ʽ */
    amhw_zsl42x_trng_load_set (gp_hw_trng, AM_FALSE);

    /* ѡ����λ���� */
    amhw_zsl42x_trng_load_set (gp_hw_trng, AMHW_ZSL42x_TRNG_CNT_64);

    /* ������������� */
    amhw_zsl42x_trng_run (gp_hw_trng);

    /* �ȴ�������� */
    while (amhw_zsl42x_trng_stu_check (gp_hw_trng)){

        ;
    }

    if (env_flag == AM_ZSL42x_TRNG_POWER_ON){
        /* ��ȡ64λ������� */
        rng_data1[0] = amhw_zsl42x_trng_low32_data_read (gp_hw_trng);
        rng_data1[1] = amhw_zsl42x_trng_high32_data_read (gp_hw_trng);
    }else{
        /* ��ȡ64λ������� */
        rng_data2[0] = amhw_zsl42x_trng_low32_data_read (gp_hw_trng);
        rng_data2[1] = amhw_zsl42x_trng_high32_data_read (gp_hw_trng);
    }

    /* �ر����Դ��· */
    amhw_zsl42x_trng_cir_disable (gp_hw_trng);
}

/**
 * \brief �������
 */
void demo_zsl42x_hw_trng_generate_entry (void *p_hw_trng)
{
    gp_hw_trng  = (amhw_zsl42x_trng_t *)p_hw_trng;

    /* ��һ���ϵ��������� */
    rng_generate (AM_ZSL42x_TRNG_POWER_ON);

    /* �������Ӧ��Ϊȫ0��ȫ1 */
    if ( (rng_data1[0] != 0) &&
         (rng_data1[1] != 0) &&
         (rng_data1[0] != 0xFFFFFFFF) &&
         (rng_data1[1] != 0xFFFFFFFF)){
        ;
    }
    else{
        am_kprintf ("Trng generate tset failed!\r\n");
    }

    /* ���ϵ��������� */
    rng_generate (AM_ZSL42x_TRNG_POWER_NON);

    if ((rng_data1[0] != rng_data2[0]) && (rng_data1[1] != rng_data2[1])){

        am_kprintf ("Trng generate tset success!\r\n");
    }else{

        am_kprintf ("Trng generate tset failed!\r\n");
    }

    AM_FOREVER {
        ;
    }
}

/* end of file */
