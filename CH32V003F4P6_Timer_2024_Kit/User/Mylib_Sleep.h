/*
 * Mylib_Sleep.h
 *
 *  Created on: Oct 9, 2024
 *      Author: user
 */

#ifndef USER_MYLIB_SLEEP_H_
#define USER_MYLIB_SLEEP_H_

#include <ch32v00x.h>

//standBy��`��ʹ�ä��뤿��γ��ڻ�
void standByModeInit(void);
//StandBy(DeepSleep)��`�ɤ����
//���N����z�ߤǽ��
void standByMode(void);

#endif /* USER_MYLIB_SLEEP_H_ */
