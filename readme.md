<!---
 SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
 SPDX-License-Identifier: BSD 3-Clause
--->
# 目次
- [目次](#目次)
- [はじめに](#はじめに)
- [ライセンス](#ライセンス)
- [連絡先](#連絡先)
- [GPIO\_mylib](#gpio_mylib)
  - [void GPIOA\_init(), GPIOB\_init(), GPIOC\_init(), GPIOD\_init()](#void-gpioa_init-gpiob_init-gpioc_init-gpiod_init)
  - [int GPIO\_output\_speed(int speed)](#int-gpio_output_speedint-speed)
  - [void pinMode(int Pin, int val)](#void-pinmodeint-pin-int-val)
  - [unsigned char digitalRead(int Pin);](#unsigned-char-digitalreadint-pin)
  - [void digitalWrite(int Pin, int val)](#void-digitalwriteint-pin-int-val)

# はじめに
このプロジェクトはCH32V003F4P6用のライブラリです。<BR>
<BR>
MounRiver Studio標準コンパイラ (ライブラリシステム)で実行できるようになっています。<BR>
main.cに参考プログラムが書かれています。<BR>
<BR>
Userディレクトリの中に <BR> 
Mylib_XXXXX.h, <BR>
Mylib_XXXXX.c <BR>
というファイルが入っています。<BR>
使用する場合は、それぞれのソースファイルをインポートしてください。<BR>

# ライセンス
このプログラムにはBSD 3-Clauseライセンスが適用されています。<BR>
詳しくは[3条項BSDライセンス](https://licenses.opensource.jp/BSD-3-Clause/BSD-3-Clause.html)をご覧ください。

# 連絡先
バグや追加機能の実装等はGithubのissueに書き込んでください。<BR>
ライセンス・再配布・その他問い合わせについては、下のメールアドレスまたはGoogle Formからどうぞ。<BR>
<BR>
メールアドレス:matsukawa.software＠gmail.com (アットマークを半角にしてください)<BR>
[Google Form お問い合わせフォーム](https://denshi1996.com/?page_id=68#toc1)

# GPIO_mylib
GPIO関連のライブラリです。
## void GPIOA_init(), GPIOB_init(), GPIOC_init(), GPIOD_init()
GPIOxの初期化をします。各ポート操作をする前に実行します。<BR>

|        |         |  役割  |   定数|
| ------ | ------ | ------ | ------|
| 引数   | なし    |        |       |
| 戻り値 | なし    |        |       |
|        |        |       |        |
## int GPIO_output_speed(int speed)
GPIOの出力速度を指定します。初期値は2MHzに設定されています。
|        |         |  役割  |   定数|
| ------ | ------ | ------ | ------|
| 引数   |  speed    |   出力速度指定     |  OUTPUT_2MHz , OUTPUT_10MHz , OUTPUT_50MHz   |
| 戻り値 |  int型    |   引数を記憶し、それらの値を返却します    |    OUTPUT_2MHz , OUTPUT_10MHz , OUTPUT_50MHz   |
|        |        |       |        |

```
//出力速度を50MHzに設定
GPIO_ouptut_speed(OUTPUT_50Mhz);
```

## void pinMode(int Pin, int val)
GPIOの入出力を設定します。
|        |         |  役割  |   定数|
| ------ | ------ | ------ | ------|
| 引数   |  Pin    |   ピン指定    |  PxY (例：PA0, PB1, PC2, PD3)   |
|  |  val    |   GPIOの設定を指定    |    INPUT , INPUT_PULLUP , INPUT_PULLDOWN , INPUT_ANALOG , OUTPUT , OUTPUT_OPENDRAIN , OUTPUT_FUNC , OUPTUT_OPENDRAIN_FUNC  |
|    戻り値   |   なし     |       |        |
||||

|    定数名    |    役割     |
| ------ | ------ |
| INPUT  | 入力設定    | 
| INPUT_PULLUP | 内部プルアップを有効にした入力    |
|   INPUT_PULLDOWN     |    内部プルダウンを有効にした入力    |  
| INPUT_ANALOG  |アナログ入力(ADC)  | 
| OUTPUT | プッシュプル出力   |
|   OUTPUT_OPENDRAIN     |     オープンドレイン出力   |  
| OUTPUT_FUNC  | 別ペリフェラルからのプッシュプル出力(例:PWM)  | 
| OUTPUT_OPENDRAIN_FUNC | 別ペリフェラルからのオープンドレイン出力(例:PWM, I2C)    |
||| 

```
//PA1を出力に設定
pinMode(PA1, OUTPUT);
//PC0をプルアップ入力に設定
pinMode(PC0, INPUT_PULLUP);
```

## unsigned char digitalRead(int Pin);
指定したピンから、入力値を返します。 <BR>
（事前に、pinMode等で入力設定にしてください）
|        |         |  役割  |   定数|
| ------ | ------ | ------ | ------|
| 引数   | Pin    |    ピン指定   |  PxY(例:PA0, PB1, PC2, PD3)     |
| 戻り値 | unsigned char    |   GPIOの状態     |   1 or 0 <BR>(存在しないピンを指定した場合 0xFFが返されます)    |
|        |        |       |        |

```
//PA1をプルアップ入力に
pinMode(PA1, INPUT_PULLUP);

//PA0が0だったら
if(digitalRead(PA1) == 0){
    //PA2を1に
    digitalWrite(PA2, 1);
}
```

## void digitalWrite(int Pin, int val)
指定したピンの出力を変更します。<BR>
（事前にpinMode等で出力設定にしてください）

|        |         |  役割  |   定数|
| ------ | ------ | ------ | ------|
| 引数   | Pin    |   ピン指定     |   PxY(例:PA0, PB1, PC2, PD3)     |
|       |  val    |   GPIOの状態を指定   |  1 or 0    |
| 戻り値 | なし    |        |       |
|        |        |       |        |


|     pinMode   |   val    |  GPIOの状態  | 
| ------ | ------ | ------ |
| OUTPUT |  1  |    電源電圧 VDD (実際にはVDD - 0.4 [V]以上)    |       
| | 0  |    0[V] (実際は0.4[V]未満)    |      
|   OUTPUT_OPENDRAIN     |   1     |    0[V] (実際には0.4[V]未満)   |   
|       |   0    |  　オープン (ハイインピーダンス)    |   

```
//PA1を1(VDD)にする
digitalWrite(PA1, 1);
//PA2を0(0V)にする
digitalWrite(PA2, 0);
```
