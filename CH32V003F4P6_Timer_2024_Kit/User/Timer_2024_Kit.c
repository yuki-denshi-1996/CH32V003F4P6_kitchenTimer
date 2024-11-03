/*
* SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
* SPDX-License-Identifier: BSD 3-Clause
*/

/*
 * Pin.c
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki(denshi-1996)
 */

#include "ch32v00x.h"
#include "Timer_2024_Kit.h"

//FLASH書き込み関数
//Address:アドレス   | Data:書き込むデータ
void FLASH_write(volatile uint32_t Address, volatile uint16_t Data) {
    //FLASHがLOCK状態だったら解除
    if (FLASH->CTLR & FLASH_CTLR_LOCK) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }

    //スタンダードプログラミング有効
    FLASH->CTLR |= FLASH_CTLR_PG;

    *(volatile uint16_t *)Address = Data;

    //FLASHがBUSYでなくなるまで待機
    while(FLASH->STATR & FLASH_STATR_BSY);
    FLASH->STATR |= FLASH_FLAG_EOP;

    FLASH->CTLR &= ~FLASH_CTLR_PG;

    FLASH->CTLR |= FLASH_CTLR_LOCK;
}

//FLASH読み出し関数
uint16_t FLASH_read(volatile uint32_t Address){
    volatile uint16_t *Data = (uint16_t*)Address;
    return *Data;
}

void FLASH_Erase(volatile uint32_t Address){
    //FLASHがLOCK状態だったら解除
        if (FLASH->CTLR & FLASH_CTLR_LOCK) {
            FLASH->KEYR = 0x45670123;
            FLASH->KEYR = 0xCDEF89AB;
        }

        //標準削除モード
        FLASH->CTLR |= FLASH_CTLR_PER;
        //削除する先頭ページアドレス指定
        FLASH->ADDR = Address;
        //削除開始
        FLASH->CTLR |= FLASH_CTLR_STRT;

        //削除が完了するまで待機
        while(FLASH->STATR & FLASH_STATR_BSY);

        FLASH->STATR |= FLASH_FLAG_EOP;

        //終了
        FLASH->CTLR &= ~FLASH_CTLR_PER;

}
