/*
 * CH32V003(CH32V003F4P6)用 タイマーライブラリ
 * Mylib_Timer.c
 *
 *  Created on: Sep 25, 2024
 *      Author: yuki(denshi-1996)
 */

#include "Mylib_Timer.h"
#include "Mylib_GPIO.h"
#include "ch32v00x.h"

//Timer1 PWM_初期化設定
void TIM1_PWM_init(void) {

    //TIM1_CH3をPC5に割り当て (注：別のポートも変更になるので注意すること)
    AFIO->PCFR1 |= (0b11 << 6);

    //TIM1　有効
    RCC->APB2PCENR |= RCC_TIM1EN;

    //Update Event Generation有効
    TIM1->SWEVGR |= TIM_UG;

    //TIM1 CHメイン出力有効
    TIM1->BDTR |= TIM_MOE;

    //TIM1_CH1 (PD2) 出力許可
    //TIM1->CCER |= TIM_CC1E;

//    //TIM1_CH1 PWMモード1
//    TIM1->CHCTLR1 = bit_replace(TIM1->CHCTLR1, 0b110, 3, 4);
//    //TIM1_CH1 プリロード有効
//    TIM1->CHCTLR1 |= TIM_OC1PE;

    //TIM1_CH3 PWMモード1
    TIM1->CHCTLR2 = bit_replace(TIM1->CHCTLR2, 0b110, 3, 4);
    //TIM1_CH3 プリロード有効
    TIM1->CHCTLR2 |= TIM_OC3PE;

    //TIM1オートリロードプリロード有効
    TIM1->CTLR1 |= TIM_ARPE;

    //1MHz (@8MHz)
    TIM1->PSC = 7;

    //1kHz動作
    TIM1->ATRLR = 1000;

    //TIM1->CH1CVR = 500;

    //TIM1 カウント有効
    TIM1->CTLR1 |= TIM_CEN;
}


//TIM1_PWMを用いたtone関数 (TIM1_CH1 PD2)
//freq:周波数
void tone(int freq) {
    //freqに入力された周波数にセット
    TIM1->ATRLR = 1000000 / freq;
    //Duty比を50%にセット
    TIM1->CH3CVR = 1000000 / freq / 2;

    //TIM1_CH3 (PC5) 出力許可
    TIM1->CCER |= TIM_CC3E;
}

//toneを止める関数
void no_tone(void) {
    //TIM1_CH1 (PD2) 出力禁止
    TIM1->CCER &= ~TIM_CC3E;
}


//Timer2　割り込み設定　初期化
void TIM2_interrupt_init(void) {
    //TIM2有効 (TIM2EN)
    RCC->APB1PCENR |= (1 << 0);

    //TIM2 イベント設定 (UG)
    TIM2->SWEVGR |= (1 << 0);

    //aオートリロードプレロード有効(ARPE)
    TIM2->CTLR1 |= (1 << 7);
    //TIM2 Update割り込み有効(UIE)
    TIM2->DMAINTENR |= (1 << 0);

    //TIM2プリスケーラ
    //TIM2->PSC = 47; //(PLL使用時 @48MHz)
    TIM2->PSC = 23; //(a初期状態 @24MHz)
    //TIM2->PSC = 7; //@8MHz

    //TIM2 オートリロードレジスタ
    //(1kHz周期　1ms)
    //TIM2->ATRLR = 1000;
    TIM2->ATRLR = 1000;

    //TIM2 NVIC 割り込み有効
    NVIC_EnableIRQ(TIM2_IRQn);

    //TIM2カウント有効(CEN)
    TIM2->CTLR1 |= (1 << 0);
}
