/*
 * Mylib_Sleep.c
 *
 *  Created on: Oct 9, 2024
 *      Author: user
 */

#include "Mylib_Sleep.h"

void standByModeInit(void){
    //LSI�Є��� (standBy��`�ɤǤ�LSI��ʹ�ä��뤿��)
    RCC->RSTSCKR |= RCC_LSION;

    RCC->APB1PCENR |= RCC_PWREN; //PWR��˥å��Є�
}
//StandBy(DeepSleep)��`�ɤ����
//q���N����z�ߤǽ��
void standByMode(void){

}
