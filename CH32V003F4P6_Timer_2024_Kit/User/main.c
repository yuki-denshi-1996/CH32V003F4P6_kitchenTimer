/*
 * SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
 * SPDX-License-Identifier: BSD 3-Clause
 */

#include "Timer_2024_Kit.h"
#include <ch32v00x.h>
#include "Mylib_GPIO.h"
#include "Mylib_Timer.h"

//q作った基板チェック用のプログラム 今回は面倒なので、同じファイルに統合することにする
#define TEST_MODE 0

void Standby_init(void);
void Standby(void);

void beep(int freq, int len); //Beep音を鳴らす関数 freq:周波数 | len:ながさ(ms単位)

void seg_output(int num, int val, int dot);
void seg_num(int num);
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile unsigned int cnt0 = 0, cnt1 = 0, cnt_sw_min = 0, cnt_sw_sec = 0,
        cnt_sw_ss = 0; //sw関連
volatile unsigned int cnt_sw_fast1 = 0, cnt_sw_fast2 = 0; //sw関連
volatile unsigned int cnt_time = 0, cnt_time_flash = 0, cnt_time_beep = 0; //Timeカウント関連
volatile unsigned int cnt_sleep = 0; //sleep時間関連
volatile unsigned int cnt_beep = 0; //Beep音関係

#if TEST_MODE
int Test_cnt999 = 0;
#endif

volatile unsigned int g_seg_num = 0, g_seg_val = 0, g_seg_dot = 0;

volatile unsigned int g_beep_en = 0, g_beep_len = 0;

//CH32V003動作周波数 24MHz
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

    // aフラッシュメモリが初期値の場合、リセット
    if (FLASH_read(FAST1_TIME_ADRESS) == 0xFFFF) {
        FLASH_write(FAST1_TIME_ADRESS, 0);
    }
    if (FLASH_read(FAST2_TIME_ADRESS) == 0xFFFF) {
        FLASH_write(FAST2_TIME_ADRESS, 0);
    }

    //TESTプログラム
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
    //Timerプログラム (Test用ではないので注意)
    while(1) {
        static int mode = TIME_PUSH_SET;
        static int timer_min = 0,timer_sec = 0,timer_sec_half = 0; //Timerの分, 秒, ハーフ(タイマー中央LEDの点滅用
        static int sw_min_val = 0, sw_sec_val = 0, sw_ss_val = 0, sw_fast1_val = 0, sw_fast2_val = 0;//a短押しされたら1
        static int sw_min_first = 0, sw_sec_first = 0, sw_ss_first = 0, sw_fast1_first = 0, sw_fast2_first = 0;//a1度のみ1になる(ラッチ)
        static int long_sw_min_val = 0, long_sw_sec_val = 0, long_sw_ss_val = 0, long_sw_fast1_val = 0, long_sw_fast2_val = 0;//a長押しされたら1に
        static int sw_min_tmp = 0, sw_sec_tmp = 0, sw_ss_tmp = 0, sw_fast1_tmp = 0, sw_fast2_tmp = 0;//a一時的に記憶
        static int beep_cnt = 0;//END時にBEEP音がどこでなっているのか記憶する用
        static int start_min = 0, start_sec = 0;//終了時に開始したときの時間を覚えておくように

        //SW_MIN スイッチ用プログラム
        if(digitalRead(SW_MIN) == 0) {
            if(sw_min_tmp == 1) {
                //a長押し検知
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
            cnt_sleep = 0; //SWを操作していたらスリープタイマーリセット
        }
        else {
            sw_min_tmp = 0;
            sw_min_val = 0;
            long_sw_min_val = 0;
        }

        //SW_SEC スイッチ用プログラム
        if(digitalRead(SW_SEC) == 0) {
            if(sw_sec_tmp == 1) {
                //a長押し検知
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
            cnt_sleep = 0; //SWを操作していたらスリープタイマーリセット
        }
        else {
            sw_sec_tmp = 0;
            sw_sec_val = 0;
            long_sw_sec_val = 0;
        }

        //SW_START_STOP スイッチ用プログラム
        if(digitalRead(SW_START_STOP) == 0) {
            if(sw_ss_tmp == 1) {
                //a長押し検知
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
            cnt_sleep = 0; //SWを操作していたらスリープタイマーリセット
        }
        else {
            sw_ss_tmp = 0;
            sw_ss_val = 0;
            long_sw_ss_val = 0;
        }

        //SW_FAST1 スイッチ用プログラム
        if(digitalRead(SW_FAST1) == 0) {
            if(sw_fast1_tmp == 1) {
                //a長押し検知
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
            cnt_sleep = 0; //SWを操作していたらスリープタイマーリセット
        }
        else {
            sw_fast1_tmp = 0;
            sw_fast1_val = 0;
            long_sw_fast1_val = 0;
        }

        //SW_FAST2 スイッチ用プログラム
        if(digitalRead(SW_FAST2) == 0) {
            if(sw_fast2_tmp == 1) {
                //a長押し検知
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
            cnt_sleep = 0; //SWを操作していたらスリープタイマーリセット
        }
        else {
            sw_fast2_tmp = 0;
            sw_fast2_val = 0;
            long_sw_fast2_val = 0;
        }

        switch(mode) {
            //Timerをボタンでセットしてもらうところ
            case TIME_PUSH_SET:

            //SW_MINが短押しされた場合
            if(sw_min_first == 1) {
                sw_min_first = 0; //リセット
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_min++;//MINを増やす
                //もしTimer_minが100を超えたら
                if(timer_min >= 100) {
                    timer_min = 0; //0に戻す
                }
            }

            //SW_SECが短押しされた場合
            if(sw_sec_first == 1) {
                sw_sec_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_sec++;
                //もしTimer_secが60sを超えたら
                if(timer_sec >= 60) {
                    timer_sec = 0; //0に戻す
                    timer_min++;
                    //もし100minを超えたら
                    if(timer_min >= 100) {
                        timer_min = 0; //0に戻す
                    }
                }
            }

            //SW_START_STOPが短押しされたら
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

            //SW_MINが長押しされた場合 かつ　繰り返し
            if(long_sw_min_val == 1 && cnt_sw_min >= (LONG_PUSH_TIME + LONG_PUSH_TO_TIME)) {
                cnt_sw_min = LONG_PUSH_TIME;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_min++;
            }

            //SW_SECが長押しされた場合　かつ　繰り返し
            if(long_sw_sec_val == 1 && cnt_sw_sec >= (LONG_PUSH_TIME + LONG_PUSH_TO_TIME)) {
                cnt_sw_sec = LONG_PUSH_TIME;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                timer_sec++;
            }

            //SW_SECとSW_MINが同時に押された場合
            if(sw_sec_val == 1 && sw_min_val == 1) {
                //Timeをリセット
                timer_min = 0;
                timer_sec = 0;

                //beep(BEEP_FREQ,BEEP_PUSH_TIME);

                //q長押し用のカウンタをリセット
                cnt_sw_min = 0;
                cnt_sw_sec = 0;
            }

            //SW_FAST関連
            //FAST1
            //先ほどまで押されていたが、離したことを検知
            if(sw_fast1_val == 0 && sw_fast1_first == 1) {
                sw_fast1_first = 0;
                //a一定時間押し続けていないことを検知 (つまり短押し)
                if(cnt_sw_fast1 <= FAST_SW_LONG) {
                    uint16_t tmp_eeprom_data = FLASH_read(FAST1_TIME_ADRESS);
                    timer_min = tmp_eeprom_data / 100;
                    timer_sec = tmp_eeprom_data % 100;
                    beep(BEEP_FREQ, BEEP_PUSH_TIME);
                }
            }

            //長押しされたことを検知
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
                //a一定時間押し続けていないことを検知 (つまり短押し)
                if(cnt_sw_fast2 <= FAST_SW_LONG) {
                    uint16_t tmp_eeprom_data = FLASH_read(FAST2_TIME_ADRESS);
                    timer_min = tmp_eeprom_data / 100;
                    timer_sec = tmp_eeprom_data % 100;
                    beep(BEEP_FREQ, BEEP_PUSH_TIME);
                }
            }

            //長押しされたことを検知
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

            g_seg_val = HIGH; //7seg表示

            //SEG出力
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;

            break;//TIME_PUSH_SET

            case MODE_TIMER:

            //500ms経過したら
            if(cnt_time >= 500) {
                timer_sec_half++; //500ms経過としてインクリメント
                cnt_time = 0;
            }
            //カウントが2になった = 1000ms経過
            if(timer_sec_half >= 2) {
                timer_sec_half = 0;
                timer_sec--; //secをデクリメント
                //Timer_secが負になったら
                if(timer_sec < 0) {
                    timer_min--;
                    timer_sec = 59;
                }

                //どちらも0 = つまり時間終了
                if(timer_min == 0 && timer_sec == 0) {
                    mode = MODE_TIMER_END;
                    cnt_time_flash = 0;
                    cnt_time_beep = 0;
                    beep_cnt = 0;
                }
            }

            //SW処理
            //SW_START_STOPが短押しされたら
            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_TIMER_STOP;
                cnt_time_flash = 0;
            }

            //SEG出力
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_val = 1;

            if(timer_sec_half == 1) {
                g_seg_dot = 0;
            } else {
                g_seg_dot = 1;
            }

            break;                //MODE_TIMER

            case MODE_TIMER_STOP:
            //a基本的にタイマーが一時停止されて、点滅するところ

            //SEG点滅処理
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg点灯
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg滅
            }

            //SW処理
            //SW_START_STOPが短押しされたら
            if(sw_min_val == 1 || sw_sec_val == 1) {
                mode = TIME_PUSH_SET;
            }

            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_TIMER; //Timerカウント開始
                cnt_time = 0;
                timer_sec_half = 0;
            }

            //seg出力
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;

            break;//MODE_TIMER_STOP

            case MODE_TIMER_END:

            //SEG点滅処理
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg点灯
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg滅
            }

            //音処理
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
                mode = TIME_PUSH_SET; //最初に戻す
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

            //SW処理
            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_STOPWATCH_STOP;
                cnt_time_flash = 0;
            }

            //SEG出力
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_val = 1;

            if(timer_sec_half == 1) {
                g_seg_dot = 0;
            } else {
                g_seg_dot = 1;
            }

            break; //MODE_STOPWATCH

            case MODE_STOPWATCH_STOP:
            //SEG点滅処理
            if(cnt_time_flash > 500 && cnt_time_flash < 1000) {
                g_seg_val = 1; //seg点灯
            }
            else if(cnt_time_flash >= 1000) {
                cnt_time_flash = 0;
            }
            else {
                g_seg_val = 0; //seg滅
            }

            //SW処理
            //SW_START_STOPが短押しされたら
            if(sw_min_val == 1 || sw_sec_val == 1) {
                mode = TIME_PUSH_SET;
            }

            if(sw_ss_first == 1) {
                sw_ss_first = 0;
                beep(BEEP_FREQ,BEEP_PUSH_TIME);
                mode = MODE_STOPWATCH; //STOPWATCHカウント開始
                cnt_time = 0;
                timer_sec_half = 0;
            }

            //seg出力
            g_seg_num = (timer_min * 100) + timer_sec;
            g_seg_dot = 1;
            break;//MODE_STOPWATCH_STOP

            case MODE_SLEEP:
            g_seg_val = 0;//segmentのLED消灯
            Standby();//sleep
            cnt_sleep = 0;
            mode = MODE_SLEEP_END;
            cnt_time = 0;
            break;//MODE_SLEEP
            case MODE_SLEEP_END:
            //SWボタンを離すまで、ここで待機
            //20ms待つ
            if(cnt_time >= 20) {
                //SWがすべて離されたら
                if(sw_min_val == 0 && sw_sec_val == 0 && sw_ss_val == 0) {
                    mode = TIME_PUSH_SET;

                    //SWを離しても反応しないように
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

//Standby 初期化関数
//Standbyのピン設定などもここに記載
void Standby_init(void) {
    RCC->APB1PCENR |= RCC_APB1Periph_PWR; //PWR 有効
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO; //AFIO 有効

    RCC->RSTSCKR |= RCC_LSION; //LSI有効
    while((RCC->RSTSCKR & RCC_LSIRDY) == 0);
    //LSIが有効になるまで
}

//Standby実行関数
//Standby_initを動作させること
//Standbyはイベントで解除
void Standby(void) {

    //sleep 少しでも消費電力を削減するため、ハイインピーダンスに
    pinMode(SEG_CA1, INPUT);
    pinMode(SEG_CA2, INPUT);
    pinMode(SEG_CA3, INPUT);
    pinMode(SEG_CA4, INPUT);

    //StandByモード
    PWR->CTLR &= ~PWR_CTLR_PDDS;
    PWR->CTLR |= PWR_CTLR_PDDS;

    //SLEEPDEEP
    NVIC->SCTLR |= (1 << 2);

    //NVIC->SCTLR |= (1 << 1); //SLEEPONEXIT(すべての動作が終了したらlow-power modeへ)
    NVIC_DisableIRQ(TIM2_IRQn); //TIM2割り込み禁止
    __WFE();
    //NVIC->SCTLR &= ~(1 << 1);

    //pinModeをもとに戻す
    pinMode(SEG_CA1, OD_OUTPUT);
    pinMode(SEG_CA2, OD_OUTPUT);
    pinMode(SEG_CA3, OUTPUT);
    pinMode(SEG_CA4, OUTPUT);

    NVIC_EnableIRQ(TIM2_IRQn); //TIM2割り込み有効
}

//Beep音生成関数
//freq:周波数 | len:長さ
void beep(int freq, int len) {
    tone(freq);
    //Timer割り込み処理で停止させる
    cnt_beep = 0;
    g_beep_len = len;
    g_beep_en = 1;
}

//7segに出力するための関数
//num:表示したい数字｜val:点灯させるか(1 or 0)|dot:DOTを表示させるか(1 or 0)
void seg_output(int num, int val, int dot) {
    static int carry = 1;

    //seg_dot点灯用
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

// TIM2 割り込み
// interrupt(割り込み)周期　(1ms割り込み)
void TIM2_IRQHandler(void) {
    // TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    // TIM2 割り込み解除(UIF)
    TIM2->INTFR &= ~(1 << 0);

    seg_output(g_seg_num, g_seg_val, g_seg_dot);
    cnt0++; //7seg_ダイナミック用
    cnt1++; //aチャタリング対策用
    cnt_sw_min++; //MINボタン長押し検出用
    cnt_sw_sec++; //SECボタン長押し検出量
    cnt_sw_ss++; //SSボタン長押し検出用
    cnt_sw_fast1++; //FAST1ボタン長押し検出量
    cnt_sw_fast2++; //FAST2ボタン長押し検出量
    cnt_beep++; //BEEP用
    cnt_time++; //Timer用
    cnt_time_flash++; //7セグ点滅用
    cnt_time_beep++; //音用
    cnt_sleep++; //Sleep用
#if TEST_MODE
    Test_cnt999++; //test用
#else
    //Beep音を止める処理
    if (g_beep_en == 1) {
        if (cnt_beep >= g_beep_len) {
            no_tone(); //Beep音停止
            g_beep_en = 0;
        }
    }
#endif
}
