/*
 * CH32V003(CH32V003F4P6)�� �����ީ`�饤�֥��
 * Mylib_Timer.c
 *
 *  Created on: Sep 25, 2024
 *      Author: yuki(denshi-1996)
 */

#include "Mylib_Timer.h"
#include "Mylib_GPIO.h"
#include "ch32v00x.h"

//Timer1 PWM_���ڻ��O��
void TIM1_PWM_init(void) {

    //TIM1_CH3��PC5�˸�굱�� (ע���e�Υݩ`�Ȥ����ˤʤ�Τ�ע�⤹�뤳��)
    AFIO->PCFR1 |= (0b11 << 6);

    //TIM1���Є�
    RCC->APB2PCENR |= RCC_TIM1EN;

    //Update Event Generation�Є�
    TIM1->SWEVGR |= TIM_UG;

    //TIM1 CH�ᥤ������Є�
    TIM1->BDTR |= TIM_MOE;

    //TIM1_CH1 (PD2) �����S��
    //TIM1->CCER |= TIM_CC1E;

//    //TIM1_CH1 PWM��`��1
//    TIM1->CHCTLR1 = bit_replace(TIM1->CHCTLR1, 0b110, 3, 4);
//    //TIM1_CH1 �ץ��`���Є�
//    TIM1->CHCTLR1 |= TIM_OC1PE;

    //TIM1_CH3 PWM��`��1
    TIM1->CHCTLR2 = bit_replace(TIM1->CHCTLR2, 0b110, 3, 4);
    //TIM1_CH3 �ץ��`���Є�
    TIM1->CHCTLR2 |= TIM_OC3PE;

    //TIM1���`�ȥ��`�ɥץ��`���Є�
    TIM1->CTLR1 |= TIM_ARPE;

    //1MHz (@8MHz)
    TIM1->PSC = 7;

    //1kHz����
    TIM1->ATRLR = 1000;

    //TIM1->CH1CVR = 500;

    //TIM1 ��������Є�
    TIM1->CTLR1 |= TIM_CEN;
}


//TIM1_PWM���ä���tone�v�� (TIM1_CH1 PD2)
//freq:�ܲ���
void tone(int freq) {
    //freq���������줿�ܲ����˥��å�
    TIM1->ATRLR = 1000000 / freq;
    //Duty�Ȥ�50%�˥��å�
    TIM1->CH3CVR = 1000000 / freq / 2;

    //TIM1_CH3 (PC5) �����S��
    TIM1->CCER |= TIM_CC3E;
}

//tone��ֹ����v��
void no_tone(void) {
    //TIM1_CH1 (PD2) ������ֹ
    TIM1->CCER &= ~TIM_CC3E;
}


//Timer2������z���O�������ڻ�
void TIM2_interrupt_init(void) {
    //TIM2�Є� (TIM2EN)
    RCC->APB1PCENR |= (1 << 0);

    //TIM2 ���٥���O�� (UG)
    TIM2->SWEVGR |= (1 << 0);

    //a���`�ȥ��`�ɥץ��`���Є�(ARPE)
    TIM2->CTLR1 |= (1 << 7);
    //TIM2 Update����z���Є�(UIE)
    TIM2->DMAINTENR |= (1 << 0);

    //TIM2�ץꥹ���`��
    //TIM2->PSC = 47; //(PLLʹ�Õr @48MHz)
    TIM2->PSC = 23; //(a����״�B @24MHz)
    //TIM2->PSC = 7; //@8MHz

    //TIM2 ���`�ȥ��`�ɥ쥸����
    //(1kHz���ڡ�1ms)
    //TIM2->ATRLR = 1000;
    TIM2->ATRLR = 1000;

    //TIM2 NVIC ����z���Є�
    NVIC_EnableIRQ(TIM2_IRQn);

    //TIM2��������Є�(CEN)
    TIM2->CTLR1 |= (1 << 0);
}
