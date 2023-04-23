/*
 * Bullet Logger V9
 *      -- motion graph -- 
 * 
 *      v0, ve, impact time  measurement
 *      RL tilt & FR elevation angle            MPU6050
 *      temperature, humidity and pressure      BME280
 *      calender time                           RX8900
 * 
 *      LC115H case
 *      color LCD 2.4" 240x320
 *      Li-ION Battery
 *      Touch panel 
 *      microSD card
 *      RS485 LAN(1,2)
 *      WiFi ESP32-WROOM-32E ESO-NOW
 *
 * 
 *      Vdd = 3.3V 
 * 
 * 
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F57Q43
        Driver Version    :  2.00
 * 
 * 不明点           unicode 大きい
 *                  FATFSとFILの定義をheader.hへやらないとできない?
 
 * 
 * 2020.08.29   ver.0.00    BulletLogger3から移植
 *                          PIC18F26K83 -> PIC18F27Q43
 * 2020.08.29   ver.0.01    OK! DMA1が邪魔していてLCDが真っ白だった。
 * 2020.08.29   ver.0.02    SDカードFATfsオッケー。プログラムメモリー50% -> 73% 30kバイトほど使われる
 * 2020.08.30   ver.0.03    XC8最適化-O2(フリーでも2までは可)　SDへCSVデータ書き出し
 * 2020.08.31   ver.0.04    タッチパネル割込フラグ入った時だけ処理 
 * 2020.08.31   ver.0.05    SDカードcsvデータにヘッダ行を追加
 * 2020.09.02   ver.0.06    プログラムの見直し、まとめ
 * 2020.09.05   ver.0.07    モーションデータ別ファイルへ出力
 * 2020.09.08   ver.0.08    LCD SPI DMA転送　
 * 2020.09.12   ver.1.00    とりあえずの完成版
 * 2020.09.13   ver.1.01    スリープ(シャットダウン)機能追加
 * 2020.09.14   ver.1.02    割り込みと普通の両方から呼ぶサブルーチンは2つのプログラムが生成される
 *                          (POWER OFFサブからsprintfを呼んでいたため16kバイトもメモリを浪費していた)
 * 2020.09.15   ver.1.03    バッテリー電圧低下警告 2軸傾斜角度計算導入
 * 2020.09.16   ver.1.04    モーションセンサ　ゲイン補正 3軸傾斜計算
 * 2020.09.27   ver.1.05    固定文字列をconstにてプログラムメモリへ移動　RAMが2.5kも空いた???
 * 2021.01.03   ver.1.07    ヘッダファイル内での定義代入文を移動除去修正
 * 2021.01.11   ver.1.08    ファイル名下2桁に日付をいれた　センサー3-4距離を弾道計算値より修正
 * 2021.01.12   ver.1.09    測定中インジケータ追加　
 * 2021.01.13   ver.1.10    ローバッテリー検出テスト　いまいち、モーションブロック表示無し
 * 2021.01.16   ver.1.11    モーションブロックSDカード出力バグフィクス
 * 2021.01.16   ver.1.12    ショット時の角度がちょっと変?タイミング見直し
 * 
 * _V5 edition
 * 2021.01.20   ver.2.01    BulletLogger4から移植　MPU6050センサ部とモーション処理に分離しDMP導入前準備
 * 2021.01.22   ver.2.02    モーショングラフ表示　ブロック表示、ジャイロ表示廃止
 * 2021.01.25   ver.2.03    タイマー割り込み: VmeasureでSTOPしてから割込許可するとダメ。割込が入ってしまいタイムアウトになる
 * 2021.01.30   ver.2.04    SDカードエラー表示位置を他のウォーニングと統一
 * 2021.01.30   ver.2.05    SD SPI MODE3->MODE0変更。(問題なし)
 * 
 * _V6 edition
 * 2021.05.02   ver.6.01    BOXED_BulletLogger5から移植。　PIC18F57Q43 48pin TQFP
 * 2021.05.04   ver.6.02    MPU6050の調整値を合わせた(途中...)。バッテリー電圧、充電中の表示(プルアップ追加)
 * 2021.05.05   ver.6.03    取り付け角度補正値調整。。。。ボツ　　傾斜LEDに反映されない。
 * 2021.05.06   ver.6.04    取り付け角度補正値調整はMPU6050.cのaccel_offsetで行う。
 *                          マウントした状態でMPU6050.c #define TEST_YESにてオフセット値を測定しセット
 * 2021.05.10   ver.6.05    バックブースト電源をハイパワーモードに
 * 2021.05.11   ver.6.06    RTCCよりSOSCへ高精度32.768kHz入力。TMR3でテスト->VE_CONTROL1(LAN[2])へ出力Video_Sync 青LEDを点灯
 * 2021.05.12   ver.6.07    バッテリ電圧ADコンバータVref->FVR2.048V(+-4%)分圧1/3に変更
 * 2021.05.22   ver.6.08    ピッチ-0.5度を0に修正 -> 角度による簡易補正を追加 @MPU6050.c
 * 2021.05.23   ver.6.09    バッテリ低電圧パワーオフ(昇降圧コンバータなのでHLVDはうまく働かない)
 * 2021.07.11               ビデオシンクの1回目点灯を消灯するタイミングが遅れている（割り込み）->PWMハードで処理しては
 *                          タイマー1でのカウントをSMT1でやったらより正確（簡単）になるかも
 * 2021.07.24   ver.6.10    傾斜LED 0.45° → 0.20°
 * 2021.08.14   ver.6.11    モーションタイミングTMR3->TMR5へ変更(TMR3廃止の際に忘れていたバグフィクス)
 * 
 * _V7 edition
 * 2021.08.20   ver.7.01    玉センサ計測をSMTで行う。　->キャプチャモード　センサ信号->CLC1で合成
 * 2021.09.17               モーションデータが20msecほど遅れている???
 * 2021.09.20   ver.7.02    SMT動作OK。まだIOCと併用でデバグ。タイマーをいじったら動かなくなった。
 * 2021.09.21   ver.7.03    SMTキャプチャー値代入用rise_edge[],fall_edge[]の定義によって動かなくなる。
 *                          添え字の大きさ、宣言の位置？？　まだfileopenerrorがでてSD書き込みできない。
 *                          ->30以上でOKみたい。
 * 2021.09.21   ver.7.04    ビデオシンクLED TMR3->PWM3_16BIT (6Hz-duty7%)に変更。
 *                          TMR1,3,5を廃止
 * 2021.09.23   ver.7.05    vmeasure測定アルゴリズム変更。PT1~4_IOCは残す。
 * 2021.09.25   ver.7.06    TIMING_LOGを追加して割込のタイミング周りをデバッグ。SDへ出力。
 * 2021.09.27   ver.7.07    LCD表示関連を小分け処理に変更(スタート割込が入った後、残処理が長いとモーションデータの取得落ちが出るため)
 * 2021.09.28   ver.7.08    小分けの続き。I2Cクロック 100kHz -> 400kHz (MFINTOSC 500kHz -> HFINTOSC 2MHz)
 * 2021.10.02   ver.7.10    モーションセンサーデータをFIFOで読み出す。データ落ちがなくなった。
 *                          たまにFIFOオーバーフローが出る????????????
 * 2021.10.05   ver.7.11    グラフのショット赤点の位置を正確化
 * 2021.10.06   ver.7.12    傾斜角度表示平均サンプル数をできるだけ多く(リングバッファ数 → 表示周期間のデータ数に変更)
 * 2021.10.09   ver.7.13    ブレ計算の型指示不適当のため整数に丸まっていたところがあったので修正。
 *                          たまに傾斜角度の値が更新されないことがある。。。。不明
 * 2021.10.10   ver.7.14    グラフ表示をmotion_graph.cに分けた。横表示範囲を広げた。FIFO読込部修正。(まだ不明あり)
 * 2021.10.11   ver.7.15    とりあえず、モーションデータ最後まで読み込んだ後FIFOデータが残っているかをチェックして画面へ警告表示
 * 2021.10.13   ver.7.16    PT1~4センサーシーケンス異常の時タイムアウト。PT1以外が最初に入力した時は無視する。
 * 2021.10.17   ver.7.17    i2cルーチンを自家製に。1MHz x 1/4 = 250kHzまでしか動かない。　　(まえは2MHz x 1/5 = 400kHzでOK)
 * 2021.10.21   ver.7.18    モーションタイミング検証ビデオ240fpsに同期(PWM3_16BIT)
 * 2021.12.19   ver.7.19    RTC_disp()時のhourが更新表示されないバグフィクス
 * 2022.01.30   ver.7.20    ボタンを押したときにSDカードチェックできるように
 * 2022.01.30   ver.7.21    SDカードチェックはメインループで行う。約2秒毎。
 * 2022.02.01   ver.7.30    初速測定モードを作りたい*********************************
 * 
 * _V8 edition
 * 2022.04.19   ver.8.01    LANコネクタピンアサイン変更。PT1,2,3,4のプルアップを100kプルダウンに変更し、接続確認をできるようにした。
 * 2022.04.23   ver.8.02    RS485対応　UART1->2　ピン変更　TX2,RX2,TXDE(Tx Driver Enable)
 *                          VE_CONTROL1(Video-Sync LED) VE_CONTROL2
 *                          MCCが入り乱れてMCCmelodyのsystemというフォルダやファイルが邪魔をしていた。削除してOK
 * 2022.04.24   ver.8.03    モーション表示を切り離し。
 * 2022.04.24               受信テスト
 * 
 * _V8_2
 * 2022.04.25   ver.8.04    MCCをWIN10で修正。SM1T関連大幅変更。    IOC,CCP1,TMR1,CLC1,2,3,7,8
 * 2022.04.29   ver.8.05    I2C不調->250kHzでOK。(OSCFRQ=1:2MHz->=0:1MHz,I2CCON2 FMC1/4) 
 *                          CCP1,2キャプチャーが動かない???_>CLC入力がダメみたい
 * 2022.04.30   ver.8.06    TMRxGにて測る。TMR1-CLC5-PT12とTMR3-CLC6-PT34
 *                          センサ1-2距離=機械寸法
 * 2022.05.02   ver.8.07    MCC core.5.0.2->5.4.2へWIN10で変更。SDCard.cは前のからコピー
 *                          これでもWINで開く時3分以上かかる。MACでは開けない模様。
 *                          sd_spi.c 940行　READ_MULTI_BLOCK～　バグフィクス　CRCの4バイトを除去
 * 2022.05.03   ver.8.08    測定中無限ループ対策　TMR0OVFで検出3sec
 *                          センサ3-4距離=機械寸法
 * 2022.05.04   ver.8.09    データ受信scanfでOK。モーション不調
 * 2022.05.05               MCC core5.4.3　SDcardが無いので以前のバージョンのものを使用。MCCからは設定できない(あとでメモ書きしたので不確か)
 * 2022.05.08   ver.8.10    接続機器モード別に表示画面を整理。モーショングラフとターゲットの表示切り替え。(モードによる)
 * 2022.05.12   ver.8.11    バグフィクス
 * 2022.05.14   ver.8.12    バグフィクス。アングル表示がダメ->motion_clear()の配置がダメでmotion_gateがうまく働いていなかった。
 *              ver.8.13    モードを変えた時にもターゲット表示に着弾点を再描画(直近50発分まで)
 * 2022.05.15   ver.8.14    着弾=赤色、前の跡を橙～黄色にして現在着弾を見やすくした。
 * 2022.05.18               ターゲット側にビデオシンクロLED取り付け
 * 2022.05.21   ver.8.15    v0モードに初速一覧表示&平均値計算
 * 2022.05.22   ver.8.16    集弾センタtoセンタの計算表示       
 * 2022.06.01   ver.8.17    バグフィクスーCtoC表示とV0ave表示がダブり。V0_MODEの時CtoCクリアしない。
 * 2022.08.06   ver.8.18    初速計接続無しの時の傾斜角データはログしない。(タイミングが取れないため)
 * 2022.08.07   ver.8.19    モーションログのタイムオフセットがゼロのバグフィクス(lag_time計算値がsecだった -> x1000000してusecに)
 * 2022.08.08   ver.8.20    初速計接続でモーション表示を見たい時のモードを追加。見る時はLAN特別コネクタを使用。
 * 2022.08.15   ver.8.21    CtoCのサンプル数を表示
 * 2022.08.16   ver.8.22    CtoCをリセット(ターゲットをリセット)
 *                          未完ーーーーーーー
 * 2022.08.20   ver.8.23    v0チャートの表示行スクロールのバグフィクス
 * 2022.10.15   ver.8.30    SW2,SW3追加　MCC core v5.4.11
 * 2022.10.16   ver.8.31    SW2-ターゲットデータリセット、SW3-初速センサ1-2距離切り替え
 * 2022.10.16   ver.8.32    ストロボの初速と合わない1~2usec ??????原因不明　→ ストロボ装置側のミス(センサエッジが玉のいなくなる側riseに間違えていた)
 * 2022.10.17   ver.8.33    SW2-v0チャートデータをリセット
 * 2022.10.17   ver.8.34    パワーオン長押しのタイミングを変更(SYSTEM_INITの前に__delayはおかしい)タイトル画面追加
 * 
 * _V8_3
 * 2022.10.19   ver.8.50    ホップ回転数測定ストロボ用ログ項目を追加
 * 2022.10.20   ver.8.51    メインメニュー画面を制作開始。
 *                          tmp_string[140]はSDカードに書き出す時不足していた模様。[256]に増量。
 * 2022.10.22   ver.8.52    TOUCH_INTをRB4->RC5へ変更(LCD書き込み時にノイズが多い)。関係なかった。
 *                          TOUCH_INTを割込処理するとタッチしている間ONになりっぱなしなので変化割込が入らない。割り込み無しに変更
 *                          EEP ROM初期値書き込み
 *                          バックライト復帰
 * 2022.10.23   ver.8.55    メインメニュータッチ画面の原形完成。メモリ対策。カラーモード18bitのみ。dotRGB[960]->[580]へ減らしてデータメモリを確保
 * 2022.10.23   ver.8.56    FATfsのLongFileNameをやめてメモリーを2.6kあけた。ショートは8.3文字。ファイル名変更。dotRGB[1280]に。
 * 2022.10.27   ver.8.57    タッチ8回平均ーーーBUGBUG中
 * 2022.11.01   ver.8.58    SDカードへ保存のファイル名の拡張子を.txtから.csvに変更。　タッチXPT2046の温度係数を0.3141に変更->ダメ
 * 2022.11.01   ver.8.59    保存ファイルネームの接尾辞"A"→"Z"制御(EEPROMを使う) ヘッダだけ書込のときは更新しない。
 * 2022.11.05   ver.8.60    フッタ情報更新サブ。メニュー画面作成中。
 * 2022.11.05   ver.8.61    セットアップメニューとりあえずできた
 * 2022.11.06   ver.8.62    距離の数値入力をmとmmにわける。メニューボタン配置変更。
 * 2022.11.08   ver.8.63    ターゲットの計算エラーとRxエラーを分けて表示。
 * 2022.11.09   ver.8.64    v0チャートにジュール表示追加。
 * 2022.11.10   ver.8.65    関係のない受信UARTデータを消去する。->TARGET対応
 * 
 * _V9 edition
 * 2023.01.28   ver.9.00    ESP32WiFiを追加に伴い、ハード変更
 *                          ピン45 PT4_WiFi　CLCin
 *                          ピン39←45から変更　LAN7 VIDEO_LED　PWM31→PWM11
 *                          RS485 UART2->UART4に変更
 *                          SPI2廃止してSPI1に統合　　3ピンあく
 *                          ESP32通信　UART2
 *                          MCC苦戦　部分exportで1つずつ保存。ダメなものは読込failになる
 *                                  MC3ファイルを新しい場所に持って行って、MCCで開きgenerateするときれいなファイルができる->user分の修正が必要
 *                                  mcc_generaateed_filesフォルダごとそっくりコピーしてもgitが効いて差分がわかるので変更修正は楽
 * 2023.02.05   ver.9.01    ESPからのターゲットデバイスチェックと受信の設定
 * 2023.02.07   ver.9.02    target_mode変数をローカル化。(target_mode_get()関数を追加)
 * 2023.02.20   ver.9.03    ESP32とのデータやり取りUART2を9600bpsから115200bpsに変更
 * 2023.02.25   ver.9.04    UARTの整理。CtoCデータ数2つ未満のとき表示無し
 * 2023.02.26   ver.9.05    1発目の入力不良の調査。シリアルDEBUGgerへ受信データ表示。LAN接続するとシリアル表示はキャンセルされる。ーーーーーーー
 * 2023.03.11   ver.9.06    BBを6mm表示に。UARTバッファクリアを少し変更。DEBUGger表示。LANの時は表示しない。
 * 2023.03.12   ver.9.07    プログラム書き換え後のメニュ-データが不正になることがある。eeprom読み出しの時にチェック。
 * 2023.03.18   ver.9.10    ターゲット設定メニューを追加
 * 2023.03.22   ver.9.11    ターゲットメニューにキャンセル追加。リターン時はセットしてから->セット&リターンボタンに変更
 * 2023.03.25   ver.9.12    ターゲット有線接続時のprintf整理
 * 2023.04.15   ver.9.13    ターゲットCtoC集計n=49発まで　-> 80発　エラー表示。  memory Data 89->91%
 * 2023.04.22   ver.9.14    ターゲットクリア後のCtoCログ値が前回の値になっているのを修正
 * 2023.04.23   ver.9.15    ターゲット設定値をEEPROMへ保存し、起動時反映。ログに狙点高さを追加
 * 
 * 
 *  ////モーションセンサーの発射時のタイミングログ取得にSMT1のタイマーを使っているけれど、着弾してしまうとタイマーが止まってしまう可能性がある。別タイマーにするか???
 * 
 * 
 * 
 * 
 * 保留事項、不具合事項
 * ハードリセットがかからない?
 * デバッグの時、SDカードで止まる。SD抜けば大丈夫なよう???
 * モーション割り込み　IOC->INT0~2にしたい。IOC割り込みはピンの判別に一手間かかっているので、時間がかかる。
 * RTCC割り込み　IOC->INT0~2にしたい
 * VEオンリーモードでVEセンサで初速測定。動作自体はV0モードと同じ一覧表示としたい
 * デバッグの時、I2C通信中に停止するとバスエラーになるみたい。電源切らないと復帰しない。リセットも効かない。
 * 玉発射の時のモーションの計算。前データとの比率位置で求める。トリガーを引く前の静止状態の数値ではダメみたいなので。
 * ログデータにWiFiと有線の記載を追加したい
 *  WiFiペアリングされている時に、電子ターゲット側のESP32にプログラムを書き込むとタマモニがフリーズする?????
 * ターゲットデバイスに変化があった時に着玉データが残ることがある。着玉データのクリアのタイミング?
 * 
 * command_uart_send() target_set_up_command() この辺整理してtarget_graph.cへ
 * 
 * 
 * 
 * 
*/
#include    "header.h"

__EEPROM_DATA (0x00, 0x23, 0x01, 0x01, 0x41, 0x00, 0x07, 0xf4); //eeprom_address_t
//              v0    Y     M     D     suf   out   m     mmL
__EEPROM_DATA (0x01, 0x02, 0xfa, 0x00, 0x07, 0xc3, 0x00, 0xf1); 
//              mmU   gun   bbL   bbU   BB    nuL   nuU   osY
__EEPROM_DATA (0x4a, 0x5a, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff); 
//              aiY   brL   brU
//Picketの設定でpreserveにチェックするとプログラム書き込み時に書き換えられない


#define     BACKLIGHT_OFF_TIME  (5 * 60)    //5min                          
#define     SLEEP_TIME          (9 * 60)    //9min その後カウントダウン60sec


//global
const char  title[] = "Bullet Logger V9";
const char  version[] = "9.15"; 
char        tmp_string[256];    //sprintf文字列用
uint8_t     dotRGB[1280];        //可変できない 2倍角文字576バイト
bool        sw1_int_flag = 0;   //SW1割込フラグ

//FATfs関連 ここで定義しないとスペース確保できずにエラー
FATFS       drive;
FIL         file;
UINT        actualLength;



#ifdef  TIMING_LOG   //debug
uint8_t     time_log;
uint8_t     time_point[66];
uint24_t    time_stamp[66];
uint8_t     time_remain[66];
#endif  //TIMING_LOG

//local
uint8_t     part_num;           //メインループ中の小分け処理順番番号
bool        RTC_error;          //I2Cセンサーエラーチェック
bool        Atmos_error;
bool        Motion_error;
bool        bloff_flag;         //バックライトオフのフラグ
bool        sleep_flag;         //スリープのフラグ
bool        low_battery_int_flag;
uint16_t    touch[3];           //タッチ座標
uint8_t     i;

/*
                         Main application
 */

//debug timing log
#ifdef TIMING_LOG                   //debug
void DEBUG_timing_log(uint8_t point){
    //タイミング測定　デバッグ用
    if (measure_status == VMEASURE_READY) {
        return;
    }
    time_point[time_log] = point;
    time_stamp[time_log] = SMT1TMR;
    time_remain[time_log] = motion_message_remain;
    time_log++;
    if(time_log > 63){
        time_log = 63;
    }
}
#endif


void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    DMA1_StopTransfer();            //DMA1_Initializeで動作開始されているのでストップさせる　　DMA1 -> SPI1に設定している
    i2c_master_initialize();        //手製シンプルI2Cドライバ
    //バックブースト電源

    __delay_ms(500);                //やや長押しとする
    if(SW1_PORT == SW_ON){
        POWER_MODE_SetLow();        //L:ハイパワーモード, H:パワーセーブ
        POWER_EN2_SetHigh();        //バックブースト電源3.3Vオン
    }
    
    //******* パワーオン *************
    LED_RIGHT_SetHigh();     
    LED_LEFT_SetHigh();                          //傾斜LEDを両方とも点灯
    
    //RTCCモジュールクロック出力オン
    RTCC_FOE_SetHigh();
    
    IOCAF4_SetInterruptHandler(detect_SW1); //スイッチ1割込コールバック関数割り当て
    vmeasure_initialize();
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
    
    //割込フラグクリア
    sw1_int_flag = 0;
    RTC_int_flag = 0;                     
    low_battery_int_flag = 0;
    //low_battery_count = 0;

    LCD_initialize();
    PWM3_LoadDutyValue(100 * (0x03ff / 100));   //LCDバックライトオン　100% (10bit)

    //LCD タイトル
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();                          //傾斜LEDを両方とも消灯
    LCD_Title();
    __delay_ms(200);
    
    printf("\n");
    printf("********************\n");
    printf(" Bullet Logger V9   \n");
    printf("           ver.%s\n", version);
    printf(" AUTO Device mode   \n");/////////ver8.10-//////////////////////////
    printf(" WiFi ESP32         \n");/////////ver9.00-//////////////////////////
    printf("           Feb 2023 \n");
    printf("********************\n");
    
    touch_init();
    
    //I2C
    RTC_error = RTC_initialize();
    Atmos_error = BME280_initialize();
    Motion_error = MPU6050_initialize();
    motion_initialize();
    angle_level_tilt_disp_init();
   
    BatV_disp(1);                               //フル表示
    BME280_disp(1);                             //フル表示
    RTC_disp(1);                                //全体表示　クロックに電源断等あったときはセットタイム画面になる。
    
    generate_filename_date();                   //保存用ファイルネーム生成
    if (shotheader_sd_write() == 0){
        //ヘッダーが書き込めたのでSDカードは入っている
        ShotHeader_write_flag = 1;              //ヘッダ書き出し済み
    }else{
        ShotHeader_write_flag = 0;
    }
    
    while(SW1_PORT == SW_ON){
        //スイッチオンのキーを離すのを待つ
    }
    sw1_int_flag = 0;
    
    //EEP ROM読み込み
    read_rom_main();
    read_rom_setup();
 
    //タイトル表示中
    for(i = 0; i < 200; i++){
        __delay_ms(10);
        footer_rewrite(1);      //時計等の表示更新のみ
        motion_data_read();
        angle_level_disp();     //傾斜角度とLED
    }
    LCD_Title_Clear();              //タイトル終了
    
    sensor_connect_check();         //マト側機器の接続チェックー測定モードの表示切り替え
    //target_lcd_default_command();   //電子ターゲットにデフォルトセットコマンドを送る
    //target_set_up_command();
    vmeasure_ready();
    
    //***** MAIN LOOP **********************************************************
    //
    
    while (1){
        //弾速測定とモーション表示
        measure_status = vmeasure();
        footer_rewrite(0);
        if(measure_status == VMEASURE_READY){
            input_mmi();
        }
        
    }
}


void footer_rewrite(bool setup_flag){
    //RTC1秒更新周期で時刻気象バッテリ等の表示を更新
    //各サブルーチンは小分け処理。測定開始した時は、サブへ飛ばなくしている。
    //setup_flag:セットアップメニューからの呼び出し時。時計、天気、バッテリの表示のみ
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){//////////////RTC_int_flag==1を整理できそう//////////
        RTC_disp(0);    //違うとこだけ表示
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){
        BME280_disp(0); //数値だけ小分け表示
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){
        BatV_disp(0);   //数値だけ小分け表示
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY) && (setup_flag == 0)){
        sensor_connect_check();     //マト側機器の接続チェックー測定モードの表示切り替え
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY) && (setup_flag == 0)){
        switch (part_num){
            case 0:
                low_battery();
                part_num++;
                break;
            case 1:
                if (ShotHeader_write_flag == 0){ 
                    //SDカードが入っていない時は確認する
                    //抜いたのの確認はしない-->負荷が大きい
                    generate_filename_date();                   //保存用ファイルネーム生成
                    if (shotheader_sd_write() == 0){
                        //ヘッダーが書き込めたのでSDカードは入っている
                        ShotHeader_write_flag = 1;              //ヘッダ書き出し済み
                    }
                }
                part_num++;
                break;
#if 0
            case 2:
                MPU6050_disp_temp();
                part_num++;
                break;
#endif
            default:
                part_num = 0;
                break;

        }
    }
    RTC_int_flag = 0;                    //フラグをリセット
  
}


void input_mmi(void){
    //Man Machine Interface
    //メインループ中のキー操作入力&電源関連
    //操作中は撃つことはないはずなので、測定開始時のディレイ対策は無し
    //測定サイクルスタートしていない時のみ処理
    uint8_t     c = 0;
    uint8_t     tmp;                //UART捨て読み

    if (TOUCH_INT_PORT == 0){
        //タッチパネル入力あり
        touch_xyz(touch);
        sleep_count_reset();
    }
       
    if (sw1_int_flag == 1){
        //SW1が押された
        LED_RIGHT_SetLow();     
        LED_LEFT_SetLow();          //傾斜LEDを両方とも消灯
        sleep_count_reset();
        power_switch();             //ちょん押し:セットアップ、長押し:電源オフ
        sw1_int_flag = 0;
        setup_menu();
        sleep_count_reset();
        return;
    }

    if (SW2_PORT == SW_ON){
        //SW2が押された
        sleep_count_reset();
        target_data_reset();    //ターゲットデータをリセット
        v0_data_reset();        //v0チャートデータのリセット
    }

    if (SW3_PORT == SW_ON){
        //SW3が押された
        sleep_count_reset();
        set_v0sensor(1);    //1:change
    }
    
    //rx_buffer_clear();      //UARTバッファのゴミを捨て読み LANのほうはしょっちゅうクリアされてしまう???/////////////////////////////
    
    if (sleep_count >= BACKLIGHT_OFF_TIME){
        if (bloff_flag == 0){
            //バックライトオフ処理
            PWM3_LoadDutyValue(15 * (0x03ff / 100));           //LCDバックライトオフ　15% (10bit) 
            bloff_flag = 1;
        }
    }
    
    if (sleep_count >= SLEEP_TIME){
        shut_down();
    }

}


void detect_SW1(void){
    //押しボタン割り込み
    sw1_int_flag = 1;
}


void Stop_until_SW1(void){
    //SW1が押されるまでストップさせておく
    sprintf(tmp_string, "Push button to next ");
    LCD_Printf(COL_WARNING, ROW_WARNING1, tmp_string, 1, PINK, 1);
    sw1_int_flag = 0;
    
    do {
    //sw1が押されるまでループ
    } while((sw1_int_flag == 0) && (TOUCH_INT_PORT == 1));
    
    sprintf(tmp_string, "                    ");
    LCD_Printf(COL_WARNING, ROW_WARNING1, tmp_string, 1, BLACK, 1); 
    sw1_int_flag = 0;
    
    while(SW1_PORT == SW_ON){
        //スイッチオンのキーを離すのを待つ
    }
    __delay_ms(500);
    
}


uint8_t BatV_disp(bool full_disp){
    //バッテリー電圧を測定
    //full_disp フル表示
    
#define     ADFVR                   //ADコンバータリファレンス電圧+ VDD / FVR
#ifndef     ADFVR
    #define     VREF    3.263       //VDD = LTC3558 3.3Vバックブースト電圧　実測値
    #define     R1      4680        //分圧抵抗+側 Ω
    #define     R2      4680        //分圧抵抗-側
#else
    #define     VREF    (2.048 * 1.00988)   //ADFVR = FVR1:x2(+-4%)  実測値で補正
    #define     R1      4680                //分圧抵抗+側 Ω
    #define     R2      2348                //分圧抵抗-側
#endif
#define     LOW_BATTERY_VOLTAGE     3.20    //低電圧判定
    
    static uint8_t      disp_stage = 0;
    static uint16_t     batv;
    static float        voltage;
    
    do {
        switch (disp_stage){
            case 0:
            case 1:
                //電圧をAD変換
                DIVIDER_ON_SetHigh();               //分圧抵抗をオン
                __delay_us(2);
                ADCC_StartConversion(BAT_V_AN);
                while(!ADCC_IsConversionDone());
                batv = ADCC_GetConversionResult();
                DIVIDER_ON_SetLow();                //分圧抵抗をオフ
                disp_stage = 2;
                break;
            case 2:
                //実数計算
                voltage = (float)(batv * (((float)R1 + R2) / R2) * VREF / 4096);
                disp_stage = 3;
                break;
            case 3:
                //電圧判定
                if (voltage <= LOW_BATTERY_VOLTAGE){
                    low_battery_int_flag = 1;               
                    ///HLVDではマイコンの3.3Vを判定するけれど、昇降圧コンバータのため低電圧になりにくい
                }else{
                    low_battery_int_flag = 0;               
                }
                disp_stage = 4;
                break;
            case 4:
                //フレーム画面表示
                if (full_disp){
                    sprintf(tmp_string, "BAT %4.2fV", voltage);
                    LCD_Printf(COL_BATV, ROW_BATV, tmp_string, 1, YELLOW, 1);
                }
                disp_stage = 5;
                break;
            case 5:
                //電圧画面表示
                sprintf(tmp_string, "%4.2f", voltage);
                LCD_Printf((COL_BATV + 6 * 4), ROW_BATV, tmp_string, 1, YELLOW, 1); //4文字右から
                disp_stage = 6;
                break;
            case 6:
                //充電中の表示
                if (POWER_CHRG_PORT == 0){
                    LCD_Printf((COL_BATV + 6 * 10), ROW_BATV, "CHRG", 1, RED, 1); //10文字右から
                }else{
                    LCD_Printf((COL_BATV + 6 * 10), ROW_BATV, "    ", 1, BLACK, 1);
                }
                disp_stage = 0;
                break;

        }
    } while((full_disp == 1) & (disp_stage != 0)); 
        
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(18);       //BatV return
#endif
        return disp_stage;
}


void sleep_count_reset(void){
    //スリープカウントをリセットする
    //バックライト減光時は復帰
    sleep_count = 0;
    if (bloff_flag == 1){
        PWM3_LoadDutyValue(100 * (0x03ff / 100));           //LCDバックライトオフ　100% (10bit) 
        bloff_flag = 0;
    } 
}


void    power_switch(void){
    //メニュースイッチ長押し～パワーオフ
    //SW1割り込みフラグ検出からくる
    uint8_t i;
    
    //2秒長押し
    for (i = 0; i < 20; i++){
        __delay_ms(100);
        if (SW1_PORT == SW_OFF){
            sw1_int_flag = 0;                           //ちょん押しの時はフラグリセットしてリターン
            return;
        }
    }
    write_rom_setup();///////setupからくるときに必要
    power_off();
}

void    power_off(void){
    //電源オフ
    LED_RIGHT_SetHigh();     
    LED_LEFT_SetHigh();                                 //傾斜LEDを両方とも点灯
    LCD_SetColor(0,0,0);                                //黒
    LCD_DrawFillBox(10, 120, 229, 160);                 //塗りつぶし四角
    sprintf(tmp_string, "POWER OFF!!");
    LCD_Printf( 20, 130, tmp_string, 3, PINK, 0);       //3倍角はDMA描写不可
    __delay_ms(1000);
    
    //LCD clear
    PWM3_LoadDutyValue(0 * (0x03ff / 100));             //LCDバックライトオフ　0% (10bit) 
    LCD_ClearScreen(0);                                 // 画面を0=黒、1=白で消去する
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();                                  //傾斜LEDを両方とも消灯
    //power off
    POWER_EN2_SetLow();
    while(1);
}


void    shut_down(void){
    //LCD減光からの電源オフ
    uint16_t     i = 600;                               //100msec x 600 = 60secカウントダウン
    
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();         //傾斜LEDを両方とも消灯
    sleep_flag = 1;
    LCD_SetColor(0,0,0);                                //黒
    LCD_DrawFillBox(10, 128, 229, 207);                 //塗りつぶし四角
    sprintf(tmp_string, "SHUT DOWN!!");
    LCD_Printf( 20, 136, tmp_string, 3, PINK, 0);       //3倍角はDMAでの描写不可

    do {
        //カウントダウン
        sprintf(tmp_string, "%2d sec   ", i / 10);
        LCD_Printf( 70, 180, tmp_string, 2, YELLOW, 1);
        RTC_disp(0);    //違うとこだけ表示
        BME280_disp(0); //数値だけ小分け表示
        BatV_disp(0);   //数値だけ小分け表示
        __delay_ms(80);                                 //実測で調整
        if ((sw1_int_flag == 1) || (TOUCH_INT_PORT == 0)){
           //SW1,タッチで中止
           sleep_flag = 0;
           sw1_int_flag = 0;
           break;
        }
        i--;
    }while (i > 6);

    if (sleep_flag == 1){
        LCD_ClearScreen(0);                             // 画面を0=黒で消去
        //power off
        POWER_EN2_SetLow();
        while(1);
    }
    //シャットダウン中止
    LCD_SetColor(0,0,0);                                //黒
    LCD_DrawFillBox(0, 120, 239, 264);                  //塗りつぶし四角
    sleep_count_reset();
    print_targetmode(INDIGO);                          //画面再描画
    vmeasure_ready();
} 


void    low_battery(void){
    //バッテリー低電圧
#define     LOW_BAT_TIME    10      //x1sec(rtc_intで減算)
    
    enum {
        IDLE,
        SET,
        COUNTDOWN,
        BOUNCE_BACK,
        POWER_OFF
    };
    static uint8_t  low_bat_stat = 0;
    
    switch(low_bat_stat){
        case IDLE:
            if (low_battery_int_flag == 1){                
                low_bat_stat = SET;
                low_battery_int_flag = 0;
            }
            break;
            
        case SET:
            low_battery_count = LOW_BAT_TIME;   //カウンタをセット
            low_bat_stat = COUNTDOWN;
            break;
            
        case COUNTDOWN:
            if (low_battery_int_flag == 0){
                low_bat_stat = BOUNCE_BACK;
            }
            
            sprintf(tmp_string, "Low Battery%2d", low_battery_count);
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
            low_battery_count--;      //rtc_int()で1secごとに減算
            if (low_battery_count <= 0){
                low_bat_stat = POWER_OFF;
            }
            break;
            
        case BOUNCE_BACK:
            sprintf(tmp_string, "              ");
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
            low_bat_stat = IDLE;
            print_targetmode(INDIGO);                      //画面再描画
            break;
            
        case POWER_OFF:
            LCD_SetColor(0,0,0);                            //黒
            LCD_DrawFillBox(10, 128, 229, 207);             //塗りつぶし四角
            sprintf(tmp_string, "LOW BATTERY!");
            LCD_Printf( 20, 156, tmp_string, 3, PINK, 0);   //3倍角はDMAでの描写不可
            __delay_ms(3000);
            //LCD clear
            PWM3_LoadDutyValue(0 * (0x03ff / 100));         //LCDバックライトオフ　0% (10bit) 
            LCD_ClearScreen(0);                             // 画面を0=黒、1=白で消去する
            //power off
            POWER_EN2_SetLow();
            while(1);
            break;
    }
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(19);       //Low Bat return
#endif
}


void    read_rom_main(void){
    //EEP ROMからデータを代入
    sensor_type = DATAEE_ReadByte(V0SENS12_ADDRESS);    //初速計の種類
}



/**
 End of File
*/
