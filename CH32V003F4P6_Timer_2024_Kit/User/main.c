/*
 * SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
 * SPDX-License-Identifier: BSD 3-Clause
 */

#include "Timer_2024_Kit.h"
#include <ch32v00x.h>
#include "Mylib_GPIO.h"
#include "Mylib_Timer.h"

//q��ä����ĥ����å��ѤΥץ���� ��������ݤʤΤǡ�Ʊ���ե���������礹�뤳�Ȥˤ���
#define TEST_MODE 0

void Standby_init(void);
void Standby(void);

void beep(int freq, int len); //Beep�����Ĥ餹�ؿ� freq:���ȿ� | len:�ʤ���(msñ��)

void seg_output(int num, int val, int dot);
void seg_num(int num);
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile unsigned int cnt0 = 0, cnt1 = 0, cnt_sw_min = 0, cnt_sw_sec = 0,
        cnt_sw_ss = 0; //sw��Ϣ
volatile unsigned int cnt_sw_fast1 = 0, cnt_sw_fast2 = 0; //sw��Ϣ
volatile unsigned int cnt_time = 0, cnt_time_flash = 0, cnt_time_beep = 0; //Time������ȴ�Ϣ
volatile unsigned int cnt_sleep = 0; //sleep���ִ�Ϣ
volatile unsigned int cnt_beep = 0; //Beep���ط�

#if TEST_MODE
int Test_cnt999 = 0;
#endif

volatile unsigned int g_seg_num = 0, g_seg_val = 0, g_seg_dot = 0;

volatile unsigned int g_beep_en = 0, g_beep_len = 0;

//CH32V003ư����ȿ� 24MHz
int main(void) {
    GPIOA_init();
    GPIOC_init();
    GPIOD_init();

    TIM1_PWM_init();
    TIM2_interrupt_init();
    Standby_init();

    pinMode(SEG_A, OD_OUTPUT);
    pinMode(SEG_B, OD_OUTPUT);
    pinMode(SEG_C, OD_OUTPUT);
    pinMode(SEG_D, OD_OUTPUT);
    pinMode(SEG_E, OD_OUTPUT);
    pinMode(SEG_F, OD_OUTPUT);
    pinMode(SEG_G, OD_OUTPUT);
    pinMode(SEG_DOT, OD_OUTPUT);

    pinMode(SEG_CA1, OD_OUTPUT);
    pinMode(SEG_CA2, OD_OUTPUT);
    pinMode(SEG_CA3, OUTPUT);
    pinMode(SEG_CA4, OUTPUT);

    pinMode(SW_START_STOP, INPUT_PULLUP);
    pinMode(SW_MIN, INPUT_PULLUP);
    pinMode(SW_SEC, INPUT_PULLUP);
    pinMode(SW_FAST1, INPUT_PULLUP);
    pinMode(SW_FAST2, INPUT_PULLUP);

    pinMode(BUZZER, F_OUTPUT);

    digitalWrite(SEG_CA1, HIGH);
    digitalWrite(SEG_CA2, HIGH);
    digitalWrite(SEG_CA3, HIGH);
    digitalWrite(SEG_CA4, HIGH);

    eventGPIO(SW_MIN, EVENT_FALL);
    eventGPIO(SW_SEC, EVENT_FALL);
    eventGPIO(SW_START_STOP, EVENT_FALL);

    // a�ե�å�����꤬����ͤξ�硢�ꥻ�å�
    if (FLASH_read(FAST1_TIME_ADRESS) == 0xFFFF) {
        FLASH_write(FAST1_TIME_ADRESS, 0);
    }
    if (FLASH_read(FAST2_TIME_ADRESS) == 0xFFFF) {
        FLASH_write(FAST2_TIME_ADRESS, 0);
    }

    //TEST�ץ����
#if TEST_MODE
    g_seg_val = HIGH;

    while(1) {
        if(digitalRead(SW_MIN) == 0) {
            g_seg_dot = HIGH;
        }
        if(digitalRead(SW_SEC) == 0) {
            g_seg_dot = LOW;
        }
        if(digitalRead(SW_START_STOP) == 0) {
            g_seg_val = HIGH;
        }
        if(digitalRead(SW_FAST1) == 0) {
            g_seg_val = LOW;
            tone(440);
        }
        if(digitalRead(SW_FAST2) == 0) {
            no_tone();
        }

        if(Test_cnt999 >= 1000) {
            Test_cnt999 = 0;
            g_seg_num++;
            if(g_seg_num >= 9999) {
                g_seg_num = 0;
            }
        }
    }
#else
    //Timer�ץ���� (Test�ѤǤϤʤ��Τ����)
    while(1) {
        static int mode = TIME_PUSH_SET;
        static int timer_min = 0,timer_sec = 0,timer_sec_half = 0; //Timer��ʬ, ��, �ϡ���(�����ޡ����LED��������
        static int sw_min_val = 0, sw_sec_val = 0, sw_ss_val = 0, sw_fast1_val = 0, sw_fast2_val = 0;//aû�������줿��1
        static int sw_min_first = 0, sw_sec_first = 0, sw_ss_first = 0, sw_fast1_first = 0, sw_fast2_first = 0;//a1�٤Τ�1�ˤʤ�(��å�)
        static int long_sw_min_val = 0, long_sw_sec_val = 0, long_sw_ss_val = 0, long_sw_fast1_val = 0, long_sw_fast2_val = 0;//aĹ�������줿��1��
        static int sw_min_tmp = 0, sw_sec_tmp = 0, sw_ss_tmp = 0, sw_fast1_tmp = 0, sw_fast2_tmp = 0;//a���Ū�˵���
        static int beep_cnt = 0;//END����BEEP�����ɤ��ǤʤäƤ���Τ�����������
        static int start_min = 0, start_sec = 0;//��λ���˳��Ϥ����Ȥ��λ��֤�Ф��Ƥ����褦��

        //SW_MIN �����å��ѥץ����
        if(digitalRead(SW_MIN) == 0) {
            if(sw_min_tmp == 1) {
                //aĹ��������
                if(cnt_sw_min >= CHATTERING_TIME) {
                    if(sw_min_val == 0) {
                        sw_min_first = 1;
                    }
                    sw_min_val = 1;
                }
                if(cnt_sw_min >= LONG_PUSH_TIME) {
                    long_sw_min_val = 1;
                }
            } else {
                sw_min_tmp = 1;
                cnt_sw_min = 0;
            }
            cnt_sleep = 0; //SW�����Ƥ����饹�꡼�ץ����ޡ��ꥻ�å�
        }
        else {
            sw_min_tmp = 0;
            sw_min_val = 0;
            long_sw_min_val = 0;
        }

        //SW_SEC �����å��ѥץ����
        if(digitalRead(SW_SEC) == 0) {
            if(sw_sec_tmp == 1) {
                //aĹ��������
                if(cnt_sw_sec >= CHATTERING_TIME) {
                    if(sw_sec_val == 0) {
                        sw_sec_first = 1;
                    }
                    sw_sec_val = 1;
                }
                if(cnt_sw_sec >= LONG_PUSH_TIME) {
                    long_sw_sec_val = 1;
                }
            } else {
                sw_sec_tmp = 1;
                cnt_sw_sec = 0;
            }
            cnt_sleep = 0; //SW�����Ƥ����饹�꡼�ץ����ޡ��ꥻ�å�
        }
        else {
            sw_sec_tmp = 0;
            sw_sec_val = 0;
            long_sw_sec_val = 0;
        }

        //SW_START_STOP �����å��ѥץ����
        if(digitalRead(SW_START_STOP) == 0) {
            if(sw_ss_tmp == 1) {
                //aĹ��������
                if(cnt_sw_ss >= CHATTERING_TIME) {
                    if(sw_ss_val == 0) {
                        sw_ss_first = 1;
                    }
                    sw_ss_val = 1;
                }
                if(cnt_sw_ss >= LONG_PUSH_TIME) {
                    long_sw_ss_val = 1;
                }
            } else {
                sw_ss_tmp = 1;
                cnt_sw_ss = 0;
            }
            cnt_sleep = 0; //SW�����Ƥ����饹�꡼�ץ����ޡ��ꥻ�å�
        }
        else {
            sw_ss_tmp = 0;
            sw_ss_val = 0;
            long_sw_ss_val = 0;
        }

        //SW_FAST1 �����å��ѥץ����
        if(digitalRead(SW_FAST1) == 0) {
            if(sw_fast1_tmp == 1) {
                //aĹ��������
                if(cnt_sw_fast1 >= CHATTERING_TIME) {
                    if(sw_fast1_val == 0) {
                        sw_fast1_first = 1;
                    }
                    sw_fast1_val = 1;
                }
                if(cnt_sw_fast1 >= LONG_PUSH_TIME_FAST) {
                    long_sw_fast1_val = 1;
                }
            } else {
                sw_fast1_tmp = 1;
                cnt_sw_fast1 = 0;
            }
            cnt_sleep = 0; //SW�����Ƥ����饹�꡼�ץ����ޡ��ꥻ�å�
        }
        else {
            sw_fast1_tmp = 0;
            sw_fast1_val = 0;
            long_sw_fast1_val = 0;
        }

        //SW_FAST2 �����å��ѥץ����
        if(digitalRead(SW_FAST2) == 0) {
            if(sw_fast2_tmp == 1) {
                //aĹ��������
                if(cnt_sw_fast2 >= CHATTERING_TIME) {
                    if(sw_fast2_val == 0) {
                        sw_fast2_first = 1;
                    }
                    sw_fast2_val = 1;
                }
                if(cnt_sw_fast2 >= LONG_PUSH_TIME_FAST) {
                    long_sw_fast2_val = 1;
                }
            } else {
                sw_fast2_tmp = 1;
                cnt_sw_fast2 = 0;
            }
            cnt_sleep = 0; //SW�����Ƥ����饹�꡼�ץ����ޡ��ꥻ�å�
        }
        else {
            sw_fast2_tmp = 0;
            sw_fast2_val = 0;
            long_sw_fast2_val = 0;
        }

        switch(mode) {
            //Timer��ܥ���ǥ��åȤ��Ƥ�餦�Ȥ���
            case TIME_PUSH_SET:

            //SW_MIN��û�������줿���
            if(sw_min_first == 1) {
                sw_min_first = 0; //�ꥻ�å�
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_min++;//MIN�����䤹
                //�⤷Timer_min��100��Ķ������
                if(timer_min >= 100) {
                    timer_min = 0; //0���᤹
                }
            }

            //SW_SEC��û�������줿���
            if(sw_sec_first == 1) {
                sw_sec_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_sec++;
                //�⤷Timer_sec��60s��Ķ������
                if(timer_sec >= 60) {
                    timer_sec = 0; //0���᤹
                    timer_min++;
                    //�⤷100min��Ķ������
                    if(timer_min >= 100) {
                        timer_min = 0; //0���᤹
                    }
                }
            }

            //SW_START_STOP��û�������줿��
            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                //STOPWATCH
                if(timer_min == 0 && timer_sec == 0) {
                    mode = MODE_STOPWATCH;
                    cnt_time = 0;
                    timer_sec_half = 0;
                } else {
                    start_min = timer_min;
                    start_sec = timer_sec;
                    mode = MODE_TIMER;
                    cnt_time = 0;
                    timer_sec_half = 0;
                }
            }

            //SW_MIN��Ĺ�������줿��� ���ġ������֤�
            if(long_sw_min_val == 1 && cnt_sw_min >= (LONG_PUSH_TIME + LONG_PUSH_TO_TIME)) {
                cnt_sw_min = LONG_PUSH_TIME;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_min++;
            }

            //SW_SEC��Ĺ�������줿��硡���ġ������֤�
            if(long_sw_sec_val == 1 && cnt_sw_sec >= (LONG_PUSH_TIME + LONG_PUSH_TO_TIME)) {
                cnt_sw_sec = LONG_PUSH_TIME;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_sec++;
            }

            //SW_SEC��SW_MIN��Ʊ���˲����줿���
            if(sw_sec_val == 1 && sw_min_val == 1) {
                //Time��ꥻ�å�
                timer_min = 0;
                timer_sec = 0;

                //beep(BEEP_FREQ,BEEP_PUSH_TIME);

                //qĹ�����ѤΥ����󥿤�ꥻ�å�
                cnt_sw_min = 0;
                cnt_sw_sec = 0;
            }

            //SW_FAST��Ϣ
            //FAST1
            //��ۤɤޤǲ�����Ƥ�������Υ�������Ȥ���
            if(sw_fast1_val == 0 && sw_fast1_first == 1) {
                sw_fast1_first = 0;
                //a������ֲ���³���Ƥ��ʤ����Ȥ��� (�Ĥޤ�û����)
                if(cnt_sw_fast1 <= FAST_SW_LONG) {
                    uint16_t tmp_eeprom_data = FLASH_read(FAST1_TIME_ADRESS);
                    timer_min = tmp_eeprom_data / 100;
                    timer_sec = tmp_eeprom_data % 100;
                    beep(BEEP_FREQ, BEEP_PUSH_TIME);
                }
            }

            //Ĺ�������줿���Ȥ���
            if(long_sw_fast1_val == 1 && sw_fast1_first == 1) {
                sw_fast1_first = 0;
                uint16_t tmp_eeprom_data = (timer_min * 100) + timer_sec;
                uint16_t tmp_fast2 = FLASH_read(FAST2_TIME_ADRESS);
                FLASH_Erase(FAST1_TIME_ADRESS);
                FLASH_write(FAST1_TIME_ADRESS, tmp_eeprom_data);
                FLASH_write(FAST2_TIME_ADRESS, tmp_fast2);
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            }

            //FAST2
            if(sw_fast2_val == 0 && sw_fast2_first == 1) {
                sw_fast2_first = 0;
                //a������ֲ���³���Ƥ��ʤ����Ȥ��� (�Ĥޤ�û����)
                if(cnt_sw_fast2 <= FAST_SW_LONG) {
                    uint16_t tmp_eeprom_data = FLASH_read(FAST2_TIME_ADRESS);
                    timer_min = tmp_eeprom_data / 100;
                    timer_sec = tmp_eeprom_data % 100;
                    beep(BEEP_FREQ, BEEP_PUSH_TIME);
                }
            }

            //Ĺ�������줿���Ȥ���
            if(long_sw_fast2_val == 1 && sw_fast2_first == 1) {
                sw_fast2_first = 0;
                uint16_t tmp_eeprom_data = (timer_min * 100) + timer_sec;
                uint16_t tmp_fast1 = FLASH_read(FAST1_TIME_ADRESS);
                FLASH_Erase(FAST1_TIME_ADRESS);
                FLASH_write(FAST2_TIME_ADRESS, tmp_eeprom_data);
                FLASH_write(FAST1_TIME_ADRESS, tmp_fast1);
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            }

            if(cnt_sleep >= SLEEP_TIME) {
                mode = MODE_SLEEP;
            }

            g_seg_val = HIGH; //7segɽ��

            //SEG����
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;

            break;//TIME_PUSH_SET

            case MODE_TIMER:

            //500ms�вᤷ����
            if(cnt_time >= 500) {
                timer_sec_half++; //500ms�в�Ȥ��ƥ��󥯥����
                cnt_time = 0;
            }
            //������Ȥ�2�ˤʤä� = 1000ms�в�
            if(timer_sec_half >= 2) {
                timer_sec_half = 0;
                timer_sec--; //sec��ǥ������
                //Timer_sec����ˤʤä���
                if(timer_sec < 0) {
                    timer_min--;
                    timer_sec = 59;
                }

                //�ɤ����0 = �Ĥޤ���ֽ�λ
                if(timer_min == 0 && timer_sec == 0) {
                    mode = MODE_TIMER_END;
                    cnt_time_flash = 0;
                    cnt_time_beep = 0;
                    beep_cnt = 0;
                }
            }

            //SW����
            //SW_START_STOP��û�������줿��
            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_TIMER_STOP;
                cnt_time_flash = 0;
            }

            //SEG����
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_val = 1;

            if(timer_sec_half == 1) {
                g_seg_dot = 0;
            } else {
                g_seg_dot = 1;
            }

            break;                //MODE_TIMER

            case MODE_TIMER_STOP:
            //a����Ū�˥����ޡ��������ߤ���ơ����Ǥ���Ȥ���

            //SEG���ǽ���
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg����
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg��
            }

            //SW����
            //SW_START_STOP��û�������줿��
            if(sw_min_val == 1 || sw_sec_val == 1) {
                mode = TIME_PUSH_SET;
            }

            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_TIMER; //Timer������ȳ���
                cnt_time = 0;
                timer_sec_half = 0;
            }

            //seg����
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;

            break;//MODE_TIMER_STOP

            case MODE_TIMER_END:

            //SEG���ǽ���
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg����
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg��
            }

            //������
            if(cnt_time_beep < END_BEEP_INTERVAL && beep_cnt == 0) {
                beep_cnt = 1;
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            } else if(END_BEEP_INTERVAL < cnt_time_beep && cnt_time_beep < END_BEEP_INTERVAL * 2 && beep_cnt == 1) {
                beep_cnt = 2;
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            } else if(END_BEEP_INTERVAL * 2 < cnt_time_beep && cnt_time_beep < END_BEEP_INTERVAL * 3 && beep_cnt == 2) {
                beep_cnt = 3;
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            }
            else if(END_BEEP_INTERVAL * 3 < cnt_time_beep && cnt_time_beep < END_BEEP_INTERVAL * 4 && beep_cnt == 3) {
                beep_cnt = 4;
                beep(BEEP_FREQ, BEEP_PUSH_TIME);
            }
            if(cnt_time_beep > END_BEEP_LEN) {
                cnt_time_beep = 0;
                beep_cnt = 0;
            }

            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = TIME_PUSH_SET; //�ǽ���᤹
                timer_min = start_min;
                timer_sec = start_sec;
            }

            break; //MODE_TIMER_END
            case MODE_STOPWATCH:
            if(cnt_time >= 500) {
                cnt_time = 0;
                timer_sec_half++;
                if(timer_sec_half >= 2) {
                    timer_sec_half = 0;
                    timer_sec++;
                    if(timer_sec >= 60) {
                        timer_min++;
                        timer_sec = 0;
                        if(timer_min >= 100) {
                            timer_min = 0;
                            mode = TIME_PUSH_SET;
                            beep(BEEP_FREQ, BEEP_PUSH_TIME);
                        }
                    }
                }
            }

            //SW����
            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_STOPWATCH_STOP;
                cnt_time_flash = 0;
            }

            //SEG����
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_val = 1;

            if(timer_sec_half == 1) {
                g_seg_dot = 0;
            } else {
                g_seg_dot = 1;
            }

            break; //MODE_STOPWATCH

            case MODE_STOPWATCH_STOP:
            //SEG���ǽ���
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg����
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg��
            }

            //SW����
            //SW_START_STOP��û�������줿��
            if(sw_min_val == 1 || sw_sec_val == 1) {
                mode = TIME_PUSH_SET;
            }

            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_STOPWATCH; //STOPWATCH������ȳ���
                cnt_time = 0;
                timer_sec_half = 0;
            }

            //seg����
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;
            break;//MODE_STOPWATCH_STOP

            case MODE_SLEEP:
            g_seg_val = 0;//segment��LED����
            Standby();//sleep
            cnt_sleep = 0;
            mode = MODE_SLEEP_END;
            cnt_time = 0;
            break;//MODE_SLEEP
            case MODE_SLEEP_END:
            //SW�ܥ����Υ���ޤǡ��������Ե�
            //20ms�Ԥ�
            if(cnt_time >= 20) {
                //SW�����٤�Υ���줿��
                if(sw_min_val == 0 && sw_sec_val == 0 && sw_ss_val == 0) {
                    mode = TIME_PUSH_SET;

                    //SW��Υ���Ƥ�ȿ�����ʤ��褦��
                    sw_min_first = 0;
                    sw_sec_first = 0;
                    sw_ss_first = 0;
                }
            }
            break; //MODE_SLEEP_END
        }

    }
#endif
}

//Standby ������ؿ�
//Standby�Υԥ�����ʤɤ⤳���˵���
void Standby_init(void) {
    RCC->APB1PCENR |= RCC_APB1Periph_PWR; //PWR ͭ��
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO; //AFIO ͭ��

    RCC->RSTSCKR |= RCC_LSION; //LSIͭ��
    while((RCC->RSTSCKR & RCC_LSIRDY) == 0);
    //LSI��ͭ���ˤʤ�ޤ�
}

//Standby�¹Դؿ�
//Standby_init��ư����뤳��
//Standby�ϥ��٥�Ȥǲ��
void Standby(void) {

    //sleep �����Ǥ�������Ϥ�︺���뤿�ᡢ�ϥ�����ԡ����󥹤�
    pinMode(SEG_CA1, INPUT);
    pinMode(SEG_CA2, INPUT);
    pinMode(SEG_CA3, INPUT);
    pinMode(SEG_CA4, INPUT);

    //StandBy�⡼��
    PWR->CTLR &= ~PWR_CTLR_PDDS;
    PWR->CTLR |= PWR_CTLR_PDDS;

    //SLEEPDEEP
    NVIC->SCTLR |= (1 << 2);

    //NVIC->SCTLR |= (1 << 1); //SLEEPONEXIT(���٤Ƥ�ư���λ������low-power mode��)
    NVIC_DisableIRQ(TIM2_IRQn); //TIM2�����߶ػ�
    __WFE();
    //NVIC->SCTLR &= ~(1 << 1);

    //pinMode���Ȥ��᤹
    pinMode(SEG_CA1, OD_OUTPUT);
    pinMode(SEG_CA2, OD_OUTPUT);
    pinMode(SEG_CA3, OUTPUT);
    pinMode(SEG_CA4, OUTPUT);

    NVIC_EnableIRQ(TIM2_IRQn); //TIM2������ͭ��
}

//Beep�������ؿ�
//freq:���ȿ� | len:Ĺ��
void beep(int freq, int len) {
    tone(freq);
    //Timer�����߽�������ߤ�����
    cnt_beep = 0;
    g_beep_len = len;
    g_beep_en = 1;
}

//7seg�˽��Ϥ��뤿��δؿ�
//num:ɽ��������������val:���������뤫(1 or 0)|dot:DOT��ɽ�������뤫(1 or 0)
void seg_output(int num, int val, int dot) {
    static int carry = 1;

    //seg_dot������
    if (dot == HIGH) {
        digitalWrite(SEG_DOT, LOW);
    } else {
        digitalWrite(SEG_DOT, HIGH);
    }

    if (cnt0 >= 1 && val == HIGH) {
        cnt0 = 0;
        switch (carry) {
        case 1:
            seg_num(-1);
            carry = 2;
            digitalWrite(SEG_CA1, HIGH);
            digitalWrite(SEG_CA2, HIGH);
            digitalWrite(SEG_CA3, HIGH);
            digitalWrite(SEG_CA4, LOW);
            seg_num((num / 1000));
            break;
        case 2:
            seg_num(-1);
            carry = 3;
            digitalWrite(SEG_CA1, HIGH);
            digitalWrite(SEG_CA2, HIGH);
            digitalWrite(SEG_CA3, LOW);
            digitalWrite(SEG_CA4, HIGH);
            seg_num(((num % 1000) / 100));
            break;
        case 3:
            seg_num(-1);
            carry = 4;
            digitalWrite(SEG_CA1, HIGH);
            digitalWrite(SEG_CA2, LOW);
            digitalWrite(SEG_CA3, HIGH);
            digitalWrite(SEG_CA4, HIGH);
            seg_num(((num % 100) / 10));
            break;
        case 4:
            seg_num(-1);
            carry = 1;
            digitalWrite(SEG_CA1, LOW);
            digitalWrite(SEG_CA2, HIGH);
            digitalWrite(SEG_CA3, HIGH);
            digitalWrite(SEG_CA4, HIGH);
            seg_num(num % 10);
            break;
        }
    } else {
        seg_num(-1);
        digitalWrite(SEG_CA1, HIGH);
        digitalWrite(SEG_CA2, HIGH);
        digitalWrite(SEG_CA3, HIGH);
        digitalWrite(SEG_CA4, HIGH);
    }
}

void seg_num(int num) {
    switch (num) {
    case -1:
        digitalWrite(SEG_A, HIGH);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, HIGH);
        digitalWrite(SEG_D, HIGH);
        digitalWrite(SEG_E, HIGH);
        digitalWrite(SEG_F, HIGH);
        digitalWrite(SEG_G, HIGH);
        break;
    case 0:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, LOW);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, HIGH);
        break;
    case 1:
        digitalWrite(SEG_A, HIGH);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, HIGH);
        digitalWrite(SEG_D, HIGH);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, HIGH);
        break;
    case 2:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, LOW);
        digitalWrite(SEG_C, HIGH);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, HIGH);
        digitalWrite(SEG_G, LOW);
        break;
    case 3:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, HIGH);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    case 4:
        digitalWrite(SEG_A, HIGH);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, HIGH);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    case 5:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, HIGH);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    case 6:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, LOW);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, HIGH);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    case 7:
        digitalWrite(SEG_A, HIGH);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, HIGH);
        break;
    case 8:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, LOW);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    case 9:
        digitalWrite(SEG_A, LOW);
        digitalWrite(SEG_B, HIGH);
        digitalWrite(SEG_C, LOW);
        digitalWrite(SEG_D, LOW);
        digitalWrite(SEG_E, LOW);
        digitalWrite(SEG_F, LOW);
        digitalWrite(SEG_G, LOW);
        break;
    }
}

// TIM2 ������
// interrupt(������)������(1ms������)
void TIM2_IRQHandler(void) {
    // TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    // TIM2 �����߲��(UIF)
    TIM2->INTFR &= ~(1 << 0);

    seg_output(g_seg_num, g_seg_val, g_seg_dot);
    cnt0++; //7seg_�����ʥߥå���
    cnt1++; //a���㥿����к���
    cnt_sw_min++; //MIN�ܥ���Ĺ����������
    cnt_sw_sec++; //SEC�ܥ���Ĺ����������
    cnt_sw_ss++; //SS�ܥ���Ĺ����������
    cnt_sw_fast1++; //FAST1�ܥ���Ĺ����������
    cnt_sw_fast2++; //FAST2�ܥ���Ĺ����������
    cnt_beep++; //BEEP��
    cnt_time++; //Timer��
    cnt_time_flash++; //7����������
    cnt_time_beep++; //����
    cnt_sleep++; //Sleep��
#if TEST_MODE
    Test_cnt999++; //test��
#else
    //Beep����ߤ�����
    if (g_beep_en == 1) {
        if (cnt_beep >= g_beep_len) {
            no_tone(); //Beep�����
            g_beep_en = 0;
        }
    }
#endif
}
