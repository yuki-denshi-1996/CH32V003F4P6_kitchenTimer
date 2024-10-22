/*
 * Mylib_Sleep.c
 *
 *  Created on: Oct 9, 2024
 *      Author: user
 */

#include "Mylib_Sleep.h"

void standByModeInit(void){
    //LSI有効化 (standByモードではLSIを使用するため)
    RCC->RSTSCKR |= RCC_LSION;

    RCC->APB1PCENR |= RCC_PWREN; //PWRユニット有効
}
//StandBy(DeepSleep)モードに入る
//q各種割り込みで解除
void standByMode(void){

}
