/*
 * Mylib_Sleep.h
 *
 *  Created on: Oct 9, 2024
 *      Author: user
 */

#ifndef USER_MYLIB_SLEEP_H_
#define USER_MYLIB_SLEEP_H_

#include <ch32v00x.h>

//standByモード使用するための初期化
void standByModeInit(void);
//StandBy(DeepSleep)モードに入る
//各種割り込みで解除
void standByMode(void);

#endif /* USER_MYLIB_SLEEP_H_ */
