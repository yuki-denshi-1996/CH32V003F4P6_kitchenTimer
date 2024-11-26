<!---
 SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
 SPDX-License-Identifier: BSD 3-Clause
--->
# 目次
- [目次](#目次)
- [はじめに](#はじめに)
- [gitのディレクトリ構成](#gitのディレクトリ構成)
- [基板に使用した部品等](#基板に使用した部品等)
- [標準プログラムでの使い方](#標準プログラムでの使い方)
- [プログラムの変更](#プログラムの変更)
- [仕様](#仕様)
- [ライセンス](#ライセンス)
- [連絡先](#連絡先)


# はじめに
これはCH32V003F4P6でタイマーを作ろうというプロジェクトです。<BR>

# gitのディレクトリ構成
CH32V003F4P6_Timer_2024_Kit ディレクトリにプログラムが入っています。MounRiver Studioにて開発しています。プロジェクトごと読み込んでください。<BR>
timer_V1 ディレクトリに基板データが入っています。KiCad 8.0で設計しています。timer_V1.kicad_pro を読み込んでください。<BR>

# 基板に使用した部品等
ここに出しているものは、あくまでも推奨しているものです。自由に変更していただいて構いません。<BR>
ここに書いているものは、すべて秋月電子通商様で購入できる部品となっています。<BR>
|   部品番号  |  部品名   |  リンク   | 備考|
| --- | --- | --- | ---|
| BZ1  |  圧電スピーカー   |  https://akizukidenshi.com/catalog/g/g104118/   | |
|    C1 |   セラミックコンデンサ 0.1uF  | https://akizukidenshi.com/catalog/g/g113582/  |  |
|     D1|   SBD  |   https://akizukidenshi.com/catalog/g/g116384/  |  |
| F1  |   リセッタブルヒューズ  |  https://akizukidenshi.com/catalog/g/g101354/   | |
|  J2  |   電源コネクタ (XH-2コネクタ)  |  https://akizukidenshi.com/catalog/g/g112247/   |  |
|  Q1-Q4   |   DTA123E (デジタルトランジスタ)  |  https://akizukidenshi.com/catalog/g/g112465/   |  |
| R1-R8, R10 |   抵抗 1kΩ (1/6Wサイズ)  |  https://akizukidenshi.com/catalog/g/g116102/   | |
|    SW1-SW6 |   タクトスイッチ  | https://akizukidenshi.com/catalog/g/g101282/    |  |
|     U2|  E40364-IFOW (時計用7セグメントLED)   |   https://akizukidenshi.com/catalog/g/g114426/  |  |
| U3  |   CH32V003F4P6 (マイコン)  |  https://akizukidenshi.com/catalog/g/g118061/   | |

# 標準プログラムでの使い方
ここでは、プログラムを改変していないことを前提条件として、使い方を記します。<BR>

まず電源を入れます。<BR>

MINスイッチで分数を, SECスイッチで秒数を変更することができます。<BR>
スタート・ストップをする場合には、START/STOPスイッチを押してください。<BR>
クリア(0分0秒に戻す)場合には、MINボタンとSECボタンを同時におしてください。<BR>

次にFASTボタンの説明です。<BR>
このタイマーには時刻を記憶できるボタンが2つ付いています。<BR>
登録したい時間をMIN/SECスイッチで設定したあと、FASTスイッチをピッとなるまで長押しします<BR>
次回以降は、登録した方のボタンを押すだけで、時間がセットされます<BR>
FASTボタンは2つありますので、2つの時間を記憶しておけます。<BR>

最後にストップウォッチモードの説明です。<BR>
0分0秒の状態でSTART/STOPを押すと、ストップウォッチになり、時間がカウントされます。<BR>

なお、動作していない状態(時間を測定していない状態)でしばらく放置しておくと、自動的に消灯し、スリープモードに入ります。<BR>
復帰する場合には、MIN/SEC/START・STOPボタンのいずれかを押してください。<BR>

# プログラムの変更
使用するピンは、圧電スピーカーが接続されているピン(PC5)を除き、自由に変更できます。<BR>
変更する場合は、プログラムソースにあるUser/Timer_2024_Kit.hの宣言を変更してください。<BR>

````diff
//PIN定義
-#define SEG_A PC2
+#define SEG_A PD1
#define SEG_B PC4
#define SEG_C PD3
#define SEG_D PC7
#define SEG_E PC6
#define SEG_F PC3
#define SEG_G PD4
#define SEG_DOT PD2
````

注意：PA1, PA2はプッシュプル出力/(プルアップ・プルダウンを使用しない)入力のみ可能です。基本的には変更しないことをおすすめします。

また、タイマーのビープ音は、Timer_2024_Kit.hのBEEP_FREQで変更できます。使用したい周波数を入れてください。
````diff
//BEEP音の高さ
-#define BEEP_FREQ 2000
+#define BEEP_FREQ 2500 //2500Hzに設定
````
その他、細かい設定もTimer_2024_Kit.hにて変更可能です。

# 仕様
電源は乾電池3本 (1.5V x 3 = 4.5V)を使用することを前提としていますが、USB電源等5Vでも使えます。

# ライセンス
このプログラムにはBSD 3-Clauseライセンスが適用されています。<BR>
詳しくは[3条項BSDライセンス](https://licenses.opensource.jp/BSD-3-Clause/BSD-3-Clause.html)をご覧ください。

# 連絡先
バグや追加機能の実装等はGithubのissueに書き込んでください。<BR>
ライセンス・再配布・その他問い合わせについては、下のメールアドレスまたはGoogle Formからどうぞ。<BR>
<BR>
メールアドレス:matsukawa.software＠gmail.com (アットマークを半角にしてください)<BR>
[Google Form お問い合わせフォーム](https://denshi1996.com/?page_id=68#toc1)<BR>

©️2024 - Yuki (denshi-1996) 以上