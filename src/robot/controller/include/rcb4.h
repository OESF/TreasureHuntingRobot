//
// RCB-4HVから任意のデータを取得するコマンドを作成します
//
// シリアルポートを初期化して、適当なデータを出力します。
// 
// Copyright (C) 2010 Kondo Kagaku Co.,Ltd. all rights reserved.
//
#ifndef _KCB3_RCB4
#define _KCB3_RCB4

#include <stdarg.h>
#include <kcb3wl.h>

#define RCB4_Ack                 0x06 // Acknowledge
#define RCB4_Nack                0x15 // Not Acknowledge
#define RCB4_OPT_ADDR            0x00 // オプションを読み書きするアドレス
#define RCB4_OPT_BYTE            0x02 // オプションデータのバイト数

// RCB4の命令一覧
#define RCB4_CMD_ACK             0xFE // ACK命令
#define RCB4_CMD_MOV             0x00 // MOV命令
#define RCB4_CMD_CALL            0x0C // CALL命令
#define RCB4_MAINLOOP_LOW        0xFD // メインループのアドレス（EEPROM）を分割したもの
#define RCB4_MAINLOOP_MID        0x03 // EEPROMアドレス＝0x0003FD
#define RCB4_MAINLOOP_HIGH       0x00

//
// RAMの割り当て位置定義
#define RCB4_CONFIG            0x0000 // RCB-4設定データアドレス
#define PROG_COUNT             0x0002 // プログラムカウンターアドレス
#define AD_CONFIG              0x000C // アナログ基準値アドレス
#define AD_DATA                0x0022 // アナログデータアドレス
#define PIO_CONFIG             0x0038 // PIO入出力設定アドレス
#define PIO_DATA               0x003A // PIOデータアドレス

#define RCB4_CONFIG_CNT             2 // RCB-4設定データバイト数
#define PROG_COUNT_CNT              3 // プログラムカウンターバイト数
#define AD_CONFIG_CNT               2 // アナログ基準値バイト数
#define AD_DATA_CNT                 2 // アナログデータバイト数
#define PIO_CONFIG_CNT              2 // PIO入出力設定バイト数
#define PIO_DATA_CNT                2 // PIOデータバイト数

#define SERVO_ADDRESS           0x090 // サーボモーターデータの先頭アドレス
#define SERVO_DATA_CNT             20 // サーボモーター１個当たりのデータ数

// サーボモーターの各データに対するアドレスオフセット
#define SERVO_OFST_ID               1 // ID
#define SERVO_OFST_TRIM             2 // トリム
#define SERVO_OFST_CURPOS           4 // 現在位置
#define SERVO_OFST_POS              6 // 目標位置
#define SERVO_OFST_CURFRM           8 // 補間中のフレーム数
#define SERVO_OFST_FRAME           10 // 補間フレーム数
#define SERVO_OFST_M1ADDR          14 // ミキシング１のアドレス（ADのアドレスを指定）
#define SERVO_OFST_M1VAL           16 // ミキシング１の値
#define SERVO_OFST_M2ADDR          17 // ミキシング２のアドレス（ADのアドレスを指定）
#define SERVO_OFST_M2VAL           19 // ミキシング２の値
// その他
#define SERVO_NEUTRAL            7500 // サーボモーターのニュートラルポジション
#define SERVO_FREE               8000 // フリーモード（教示モード）
#define SERVO_HOLD               7FFF // ホールドモード（ポジション維持）

// サーボモーター用ポート
#define SIO1                   0x0000 // RCB-4のSIO1〜SIO4はSIO1と指定する
#define SIO2                   0x0100 // SIO5〜8はSIO2と指定する
#define RCB4_MAX_ICS_COUNT         36 // ICSデバイスの使用可能最大数

//
// RCB-4HVコマンドヘッダー
#define RCB4_CMD_Move            0x00 // デバイス間のデータ移動
#define RCB4_CMD_And             0x01 // ビット演算（論理積）
#define RCB4_CMD_Or              0x02 // ビット演算（論理和）
#define RCB4_CMD_Xor             0x03 // ビット演算（排他的論理和）
#define RCB4_CMD_Not             0x04 // ビット反転
#define RCB4_CMD_Shift           0x05 // ビットシフト
#define RCB4_CMD_Add             0x06 // 足し算（バイト単位）
#define RCB4_CMD_Sub             0x07 // 引き算（バイト単位）
#define RCB4_CMD_Multiply        0x08 // 掛け算（バイト単位）
#define RCB4_CMD_Division        0x09 // 割り算
#define RCB4_CMD_Mod             0x0A // 剰余
#define RCB4_CMD_Jump            0x0B // 条件ジャンプ
#define RCB4_CMD_Call            0x0C // 条件サブルーチン呼び出し
#define RCB4_CMD_Return          0x0D // サブルーチンの呼び出しからの復帰
#define RCB4_CMD_ComToRam        0x0E // RAMの値を利用して接続機器と通信する
#define RCB4_CMD_SingleServo     0x0F // 単独でサーボモーターを動かす命令
#define RCB4_CMD_ConstFrameServo 0x10 // フレーム数固定でサーボ位置を変化させる
#define RCB4_CMD_SeriesServo     0x11 // スピードと異動先を個別に設定して動かす
#define RCB4_CMD_ServoParam      0x12 // サーボのパラメータを一括で送信する
#define RCB4_CMD_Version         0xFD // バージョンの確認
#define RCB4_CMD_AckCheck        0xFE // Acknowledge

// PIOポート指定
#define PIO1                   0x0001
#define PIO2                   0x0002
#define PIO3                   0x0004
#define PIO4                   0x0008
#define PIO5                   0x0010
#define PIO6                   0x0020
#define PIO7                   0x0040
#define PIO8                   0x0080
#define PIO9                   0x0100
#define PIO10                  0x0200
#define PIO_ALL_OUT            0x03FF
#define PIO_ALL_IN             0x0000

//
// データの受付方向
// 00: RAM, 01: Device, 02: COM or Literal, 03: ROM
// 0xXY <= X: どこへ Y: どこから
#define RCB4_RAM_To_RAM          0x00
#define RCB4_RAM_To_DEV          0x10
#define RCB4_RAM_To_COM          0x20
#define RCB4_RAM_To_ROM          0x30
#define RCB4_DEV_To_RAM          0x01
#define RCB4_DEV_To_DEV          0x11
#define RCB4_DEV_To_COM          0x21
#define RCB4_DEV_To_ROM          0x31
#define RCB4_COM_To_RAM          0x02
#define RCB4_COM_To_DEV          0x12
#define RCB4_COM_To_COM          0x22
#define RCB4_COM_To_ROM          0x32
#define RCB4_ROM_To_RAM          0x03
#define RCB4_ROM_To_DEV          0x13
#define RCB4_ROM_To_COM          0x23
#define RCB4_ROM_To_ROM          0x33

//---------------------------
// サーボモーターのポジション設定をする構造体
struct rcb4_reg_servo
{
	byte id;      // ID番号
	byte port;    // 接続ポート
	byte devno;   // デバイス番号
	int position; // ポジション
//	byte frame;   // フレーム数 // これ以降は今後のため
//	byte speed;   // スピード
//	byte stretch; // ストレッチ
};

struct rcb4_reg_servo* rcb4_reg_servo_data;  // サーボモーターの登録用の構造体ポインタ
byte selbit[5] = {0, 0, 0, 0, 0};             // サーボ選択ビット命令
int rcb4_reg_servo_count = -1;               // 登録されたサーボの数
byte *rcb4_reg_servo_index;                   // 登録されたサーボ構造体のデバイス番号
signed char rcb4_reg_servo_devno[RCB4_MAX_ICS_COUNT] = {0};

// デバイスファイルディスクリプタ
int rcb4_fd = -1;

// 関数定義
_Bool rcb4_motion_play (byte);

// RCB4のオプションスイッチの状態を保存しておきます
union {
	struct {
		byte b0:1;  // ICSスイッチ
		byte b1:1;  // EEPROMプログラム実行スイッチ
		byte b2:1;  // 補間動作終了メッセージスイッチ
		byte b3:1;  // ベクタジャンプスイッチ
		byte b4:1;  // 出力周期レジスタ[4:5]
		byte b5:1;  // 
		byte b6:1;  // COMボーレートレジスタ[6:7]
		byte b7:1;  // 
		byte b8:1;  // ゼロフラグ
		byte b9:1;  // キャリーフラグ
		byte b10:1; // プログラムエラーフラグ
		byte b11:1; // 未使用
		byte b12:1; // 未使用
		byte b13:1; // ICSスイッチボーレートレジスタ[13:14]
		byte b14:1; // 
		byte b15:1; // LEDレジスタ
	} bit;
	struct {
		byte  low;  // low  8 bit
		byte  high; // high 8 bit
	} byte;
	unsigned short word;
} rcb4_opt;

//---------------------------
//
// RCB-4HV 
//
//---------------------------
//
// データの送受信を行います
//
int rcb4_cmd_trx (
	byte *tx, // 送信すべきデータ
	byte txc, // 送信データバイト数
	byte *rx, // 受信データ格納用配列
	byte rxc  // 受信データ数
)
{
	int i;
	
	write (rcb4_fd, tx, txc);
	i = com_gets (rcb4_fd, rx, rxc);
	
	return i; // 受診データ数を返す
}
//
// RCB4に応答を願います(ユーザーは特に使う理由はありません)
//
_Bool rcb4_confirm ()
{
	// 生存確認コマンドを発行
	// 0x04:命令バイト数、0xFE:生存確認コマンド、0x06:生存確認データ、0x08:SUM
	byte cmd[4] = {0x04, RCB4_CMD_AckCheck, RCB4_Ack, 0x08};
	byte rx[4] = {0, 0, 0, 0}, i;
	
	if (rcb4_fd < 0)
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_confirm(): port is not open.\n");
#endif
		return false;
	}

	i = rcb4_cmd_trx (cmd, 4, rx, 4); // コマンドの送受信
	
#ifdef _KCB3_DEBUG
	if (i < 4)
	{
		printf ("rcb4_confirm(): recv %02x %02x %02x %02x\n", 
			rx[0], rx[1], rx[2], rx[3]);
		printf ("rcb4_confirm(): failed data receiving.\n", i);
		return false;
	}
#endif

#ifdef _KCB3_DEBUG
		printf ("rcb4_confirm(): recv %02x %02x %02x %02x\n", 
			rx[0], rx[1], rx[2], rx[3]);
#endif

	// rxとackがすべて一致しなくてはならないが、ここでは簡略的に１バイトだけチェックする
	if (rx[2] == RCB4_Ack) 
	{
		return true;  // 通信チェックOKのとき
	}
	else 
	{
		return false; // 通信チェックNGのとき
	}
}

//
// ソフトウェアスイッチ(Option)の状態を読み込みます(ユーザーは特に使う理由はありません)
//
//--------------------------------------------------
_Bool rcb4_get_opt ()
{
	// オプションデータを読み込むコマンド(詳しくはリファレンスを参照)
	// SUMはSIZEからSUM手前までのバイト列の総和
	//            SIZE  CMD           TYPE  ADDR_L, M,   H    RAM_L, M    NUM   SUM
	byte cmd[] = {0x0A, RCB4_CMD_MOV, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x2C};
	// 受信データは SIZE CMD DAT1 DAT2 SUM = 5
	byte rx[5] = {0, 0, 0, 0, 0}, i;
	
	if (rcb4_fd < 0)
	{
		return false;
	}
	
	// commandをRCB4に送ってデータを受け取る
	i = rcb4_cmd_trx (cmd, 10, rx, 5);

	if (i != 5)
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_get_opt(): insufficient data.\n");
#endif
		return false;		
	}
	
	if (rx[0] == 0x05 && rx[1] == cmd[1]) 
	{
		// RCB-4のシステムオプションを保存する
		rcb4_opt.byte.low = rx[2];
		rcb4_opt.byte.high = rx[3];
			
		return true;
	}
	else 
	{
		return false;
	}
}

//
// オプションデータをRAMに書き込みます
// RAM書き込みのため、電源を切るとROMに保存された初期状態に戻ります
_Bool rcb4_set_opt (int opt)
{
	// 送信データと受信データ配列
	byte cmd[9] = {0x09, RCB4_CMD_Move, RCB4_COM_To_RAM, 0, 0, 0, 0, 0, 0};
	byte rx[4];
	int i;
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_set_opt(): port is not open.\n");
		return -1;
	}
#endif	
	// 書き込み先RAMアドレスは0なので特に配列に代入はしない
	// 書き込みデータをセット
	cmd[6] = rcb4_opt.byte.low;
	cmd[7] = rcb4_opt.byte.high;
	
	// データを送信して返事を待つ
	i = rcb4_cmd_trx (cmd, 9, rx, 4);
	
	if (i != 4) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_set_opt (): insufficient data.\n");
#endif
		return false;
	}
	else if (rx[2] != RCB4_Ack)
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_set_opt (): cannot receive acknowledgement.\n");
#endif
		return false;
	}
}

//
// rcb4とのポートを開きます
//
// 1) 通信設定を行う（115200bps専用）
// 2) 状態が安定して、システムオプションが正しく読み取れることを確認する
// 3) スタートアップモーションを再生する
//
_Bool rcb4_open (
	char *port_name, // ポート名（デバイスファイル名を指定）
	byte mn  // モーション番号を指定する
)
{
	byte ret = 0; // 応答の有無を確認する
	int count = 0;
	byte tx[4] = {0x05, RCB4_CMD_AckCheck, RCB4_Ack, 0x08};
	byte rx[4] = {0, 0, 0, 0};
	
	// 通信設定
	rcb4_fd = com_open (port_name, B115200, PARITY_EVEN);
	
	if (rcb4_fd < 0)
	{
		printf ("rcb4_open(): cannot connect to %s\n", port_name);
		return false;
	}
	
	// 接続を確認します
	if (rcb4_confirm () == false)
	{
		return false; // 接続設定が失敗した場合
	}
	
	// システムの状態を取得する
	if (rcb4_get_opt () == false)
	{
		rcb4_opt.word = 0; // 接続設定が失敗した場合はシステムオプション変数をリセット
		return false;     // 失敗を返す
	}
	
	// モーション番号が０以外の場合はモーションを再生する（スタートアップ時は）
	if (mn != 0)
	{
		return rcb4_motion_play (mn);
	}
	
	return true;
}
//
// Comポートをクローズする
//
void rcb4_close ()
{
	if (rcb4_fd >= 0)
	{
		close (rcb4_fd);
	}
}

//
// rcb4に記録されているモーションを再生します
//
// EEPROMに保存されているモーションのみ再生します
// rcb4がコマンドを受信できた場合、TRUEを返す
// 関数ポインタが指定されると、再生途中で計算などの処理を実行できます
//
_Bool rcb4_motion_play (
	byte mn           // 再生するモーション番号1 ~ 50
)
{
	// RCB-4に送信するコマンド（最初に必ず初期化しておく）
	byte cmd[19] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	byte i, sum, rx[5];
	_Bool res;
	unsigned long mot_addr; // KCB-3WLではintも同じ32bit

	if (rcb4_fd < 0)
	{
		printf ("rcb4_motion_play(): RCB-4 is not connected.\n");
		return false;
	}

	// モーション再生には３段階の命令が必要
	// 1) EEPROM停止 & プログラムカウンター(現在のプロクラム実行アドレス)の初期化
	// 2) モーション再生コマンドを送る
	// 3) EEPROMから命令の読み込み・実行を再開
	
	// 1) EEPROMからの再生をいったん停止します（rcb4_open関数でシステムオプションは取得済み）
	rcb4_opt.bit.b1 = 0; // EEPROMからプログラムを実行するオプションを停止
	
	// EEPROMからの再生を停止する命令を作成する
	cmd[0]  = 0x13;         // SIZE（コマンド全体のサイズ）
	cmd[1]  = RCB4_CMD_MOV; // CMD（コマンドの種類：MOVE）
	cmd[2]  = 0x02;         // TYPE（ここではある数をRAMへ転送する）
	                        // システムオプションアドレスが0x000000なので、cmd[3] ~ cmd[5] = 0
	cmd[6]  = rcb4_opt.byte.low;  // オプション下位８ビットを代入
	cmd[7]  = rcb4_opt.byte.high; // オプション上位８ビットを代入
	cmd[8]  = RCB4_MAINLOOP_LOW;  // メインループのアドレスLOW
	cmd[9]  = RCB4_MAINLOOP_MID;  // メインループのアドレスMID
	cmd[10] = RCB4_MAINLOOP_HIGH; // メインループのアドレスHIGH
	                              // cmd[11] ~ cmd[17] = 0;
	
	// チェックサムcmd[0] ~ cmd[17]を計算する
	for (i = 0, sum = 0; i < 18; i++) 
	{
		sum += cmd[i];
	}
	
	// チェックサムをコマンドへ格納する
	cmd[18] = sum;
	
	// コマンドを送受信する
	i = rcb4_cmd_trx (cmd, 19, rx, 4);
	
	if (rx[2] != RCB4_Ack) 
	{
		return false; // コマンド送信結果が失敗した場合はFALSEを返して処理をやめる
	}
	
	// 2) モーション再生コマンドを命令を使って送信する
	// モーションデータのアドレスを作成
	// モーションデータ格納場所は、先頭アドレスが3000(0x0BB)、一区画4864バイトです
	mot_addr = 4864;
	mot_addr *= (mn - 1);
	mot_addr += 3000;
	//mot_addr = 4864 * (mn - 1) + 3000; // 左間違い（Ver.1.2.0 Rev20091005）
	
	// モーション再生コマンド（７バイト）を作成する
	cmd[0] = 0x07;           // SIZE（コマンド全体のサイズ）
	cmd[1] = RCB4_CMD_CALL;  // CALL命令
	cmd[2] = mot_addr;       // モーションアドレスの下位８ビット
	cmd[3] = mot_addr >> 8;  // モーションアドレスの中位８ビット
	cmd[4] = mot_addr >> 16; // モーションアドレスの上位８ビット
	cmd[5] = 0x00;
	cmd[6] = cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5]; // チェックサム
	
	// 作成したモーション再生コマンドを送信する（まだ動作は始まらない）
	i = rcb4_cmd_trx (cmd, 7, rx, 4);
	
	if (rx[2] != RCB4_Ack) 
	{
		return false; // コマンド送信結果が失敗した場合はFALSEを返して処理をやめる
	}
	
	// 3) EEPROMからの再生を開始（実際の動作開始コマンド）
	// システムオプションのEEPROMプログラム実行スイッチをONにする
	rcb4_opt.bit.b1 = 1;
	// ベクタジャンプフラグを０にする
	rcb4_opt.bit.b3 = 0;
	
	// EEPROMからのプログラム実行コマンドを作成する
	cmd[0] = 0x09;         // SIZE（コマンド全体のサイズ）
	cmd[1] = RCB4_CMD_MOV; // CMD（コマンドの種類：MOVE）
	cmd[2] = 0x02;         // TYPE（ここではある数をRAMへ転送する）
	cmd[3] = 0x00;         // システムオプションのアドレスは0x000000
	cmd[4] = 0x00;
	cmd[5] = 0x00;
	cmd[6] = rcb4_opt.byte.low;  // オプション下位８ビットを代入
	cmd[7] = rcb4_opt.byte.high; // オプション上位８ビットを代入
	cmd[8] = cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6] + cmd[7]; // チェックサム

	// 作成したモーション再生コマンドを送信する（まだ動作は始まらない）
	// 作成したモーション再生コマンドを送信する（まだ動作は始まらない）
	i = rcb4_cmd_trx (cmd, 9, rx, 4);
	
	if (rx[2] != RCB4_Ack) 
	{
		return false; // コマンド送信結果が失敗した場合はFALSEを返して処理をやめる
	}

	// ここからモーション再生終了を待ちます
	// モーション再生完了時には、システムオプションのベクタジャンプスイッチが１になるので、
	// 何度か読み込んで、１になるのを待つ
	
	for (i=0; i<5; i++) // モーション再生が完了するまで50ms間隔で、システムオプションを読み出す
	{
		// RCB-4からシステムオプションを取得、データはrcb4_optに格納される
		if (rcb4_get_opt () == true) 
		{
			if (rcb4_opt.bit.b3 == 1) // ベクタジャンプが１になった→モーション再生完了
			{
				break;
			}
		}

		usleep (50000); // 50ms程度待つ（約50msごとに、データを読み込む）
	}
	
	return true;
}

//
// コントロール入力値を送信する
// 引数は６バイト固定（data[] = {BTN_H, BTN_L, PA1, PA2, PA3, PA4}）
// 
_Bool rcb4_putkey (
	byte *data // コントロール入力値配列（６バイト固定）
)
{
	//              SIZE  CMD           TYPE  RAM_ADDR L, M, H  BTN1  BTN2   PA1   PA2   PA3   PA4   SUM
	byte cmd[13] = {0x0D, RCB4_CMD_MOV, 0x02, 0x50, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	byte rx[4];
	byte sum = 0, i;
	
	// 引数のデータを送信データ（cmd[6] ~ cmd[11]）に格納する
	for (i = 0; i < 6; i++) 
	{
		cmd[6 + i] = data[i];  
	}
	
	// チェックサムの計算をする	
	cmd[12] = check_sum (cmd, 12);

	// 作成したコマンドを送信
	rcb4_cmd_trx (cmd, 13, rx, 4);
	
	if (rx[2] == RCB4_Ack) // 送信コマンドに対する返事を取得
	{
		return TRUE;
	}
	
	return FALSE;		
}

//
// ７バイトのボタンデータを送信します(RCB-4)
//
int rcb4_put_7Bcode (
	unsigned short cc, // コントロールコード
	byte pa1, // アナログ１
	byte pa2, // アナログ２
	byte pa3, // アナログ３
	byte pa4  // アナログ４
)
{
	byte btn1, btn2; // ボタンデータ
	byte data[6];
	
	btn1 = cc >> 8;
	btn2 = cc;
	
	data[0] = btn1;  // cmd[6], [7]にボタンデータを格納します
	data[1] = btn2; 
	data[2] = pa1;   // cmd[8]~cmd[9]にアナログデータを格納します
	data[3] = pa2;
	data[4] = pa3;
	data[5] = pa4;

	return rcb4_putkey (data); // 作成したデータを送る（返値は成功または不成功）
}

//---------------------------
//
// RCB-4HVの内部データにアクセスする関数
//
//---------------------------
//
// アナログデータを読み込む
// 読み取り失敗時には-1を返す
// ！注意！
// ・ミキシングをかけているアナログポートのデータは基準値の分だけずれています
// 　アナログセンサーを読み込む場合はミキシングを外すこと
// ・１回の読み込みには数msかかります
//
int rcb4_ad_read (int port)
{
	// 送信データ
	byte cmd[] = {0x0A, RCB4_CMD_Move, RCB4_RAM_To_COM, 0, 0, 0, 0, 0, 0, 0};
	// 受信データ
	// ADデータは１ポート毎２バイト（10bit）＋ヘッダ＋コマンド＋チェックサム＝５
	byte rx[] = {0, 0, 0, 0, 0}; 
	byte i;
	int addr, res;
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_ad_read (): port is not open.\n");
		return -1;
	}
#endif

	// 読み取りアドレスの計算（ポートx２だけアドレスをずらす）
	addr = AD_DATA + (AD_DATA_CNT * port); 
	
	// cmd[3] - cmd[5] = 0固定
	// cmd[6] - cmd[7] = RAM_ADDRESS_LOW, RAM_ADDRESS_HIGH
	// cmd[8] = 受け取るデータ数
	cmd[6] = addr;
	cmd[7] = addr >> 8; // １バイト分下位にずらす
	cmd[8] = AD_DATA_CNT;

	// チェックサムの計算
	cmd[9] = check_sum (cmd, 9); // チェックサムデータを含めない
	
	i = rcb4_cmd_trx (cmd, 10, rx, 5);
	
	if (i != 5) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_ad_read (): insufficient data.\n");
#endif
		return -1;
	}
	
	// rx[0]: データバイト数
	// rx[1]: コマンド
	// rx[2]: 16ビットデータ下位バイト
	// rx[3]: 16ビットデータ上位バイト
	// rx[4]: チェックサム（面倒なのでチェックしない）
	res = rx[3];
	res = (res << 8) + rx[2]; 
	
	return res;
}

//---------------------------
//
// PIOデータ
//
// PIO入出力設定
// port = 0b0000_00XX_XXXX_XXXX (X: PIO1 - PIO10出力設定)
// X = 0(入力) or 1(出力)
// 設定失敗時はfalseを返す
//
_Bool rcb4_pio_dir (int port)
{
	// 送信データ
	byte cmd[] = {0x09, RCB4_CMD_Move, RCB4_COM_To_RAM, 0, 0, 0, 0, 0, 0, 0};
	// 受信データはヘッダ＋コマンド＋ACK(NACK)＋チェックサム＝４
	byte rx[] = {0, 0, 0, 0}; 
	byte i;
	int addr, res;
	_Bool bit;
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_pio_dir (): port is not open.\n");
		return false;
	}
#endif

	// 読み取りアドレスの設定
	addr = PIO_CONFIG;
	
	// cmd[3] - cmd[5] = RAM_ADDRESS_LOW, RAM_ADDRESS_HIGH
	// cmd[6] - cmd[7] = PIO出力データ
	// アドレスの設定
	cmd[3] = addr;
	cmd[4] = addr >> 8; // １バイト分下位にずらす
	cmd[5] = 0;
	// データの設定
	cmd[6] = port;
	cmd[7] = port >> 8; // １バイト分下位にずらす
	
	// チェックサムの計算
	cmd[8] = check_sum (cmd, 8); // チェックサムデータを含めない
	
	i = rcb4_cmd_trx (cmd, 9, rx, 4);
	
	if (i != 4) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_pio_dir (): insufficient data\n");
#endif
		return false;
	}
	else if (rx[2] != RCB4_Ack)
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_pio_dir (): cannot receive acknowledgement.\n");
#endif
		return false;
	}
	
	return true;
}

//
// PIOデータの読み込み
//
int rcb4_pio_read ()
{
	// 送信データ
	byte cmd[] = {0x0A, RCB4_CMD_Move, RCB4_RAM_To_COM, 0, 0, 0, 0, 0, 0};
	// 受信データは２バイト（10bit）＋ヘッダ＋コマンド＋チェックサム＝５
	byte rx[] = {0, 0, 0, 0, 0}; 
	byte i;
	int addr, res;
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_pio_in (): port is not open.\n");
		return -1;
	}
#endif

	// 読み取りアドレスの設定
	addr = PIO_DATA;
	
	// cmd[3] - cmd[5] = 0固定
	// cmd[6] - cmd[7] = RAM_ADDRESS_LOW, RAM_ADDRESS_HIGH
	// cmd[8] = 受け取るデータ数
	cmd[6] = addr;
	cmd[7] = addr >> 8; // １バイト分下位にずらす
	cmd[8] = PIO_DATA_CNT;

	// チェックサムの計算
	cmd[9] = check_sum (cmd, 9); // チェックサムデータを含めない
	
	i = rcb4_cmd_trx (cmd, 10, rx, 5);
	
	if (i != 5) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_pio_read (): port is not open.\n");
#endif
		return -1;
	}
	
	// rx[0]: データバイト数
	// rx[1]: コマンド
	// rx[2]: 16ビットデータ下位バイト
	// rx[3]: 16ビットデータ上位バイト
	// rx[4]: チェックサム（面倒なのでチェックしない）
	res = rx[3];
	res = (res << 8) + rx[2]; 

	return res & 0x3FF; // 10bitマスク
}

//
// 特定のPIOポートの状態を読み込む 
// 読み取り失敗時には-1を返す
// ！注意！
// ・rcb4_pio_dir()で入出力の設定を先に行うこと
// ・出力設定時の入力データは不定
//
int rcb4_pio_in (int port)
{
	int values, bit;
	
	values = rcb4_pio_read (); // PIOの状態を読み込む

#ifdef _KCB3_DEBUG
	printf ("rcb4_pio_in(): read values 0x%04x\n", values);
#endif

	if (values < 0)
	{
		return -1;
	}
	
	switch (port)
	{
		case PIO1:  bit = 0; break;
		case PIO2:  bit = 1; break;
		case PIO3:  bit = 2; break;
		case PIO4:  bit = 3; break;
		case PIO5:  bit = 4; break;
		case PIO6:  bit = 5; break;
		case PIO7:  bit = 6; break;
		case PIO8:  bit = 7; break;
		case PIO9:  bit = 8; break;
		case PIO10: bit = 9; break;
		default: bit = -1;
	}

	if (bit < 0)
	{
		return -1;
	}
	else
	{
		return ((values >> bit) & 0x01);
	}
	
	return -1;
}

//
// PIOデータを書き出す（PIOポートから出力する）
// 読み取り失敗時には-1を返す
// ！注意！
// ・rcb4_pio_dir()で入出力の設定を先に行うこと
// ・入力設定時の出力データは不定
//
_Bool rcb4_pio_out (int values)
{
	// 送信データ
	byte cmd[] = {0x09, RCB4_CMD_Move, RCB4_COM_To_RAM, 0, 0, 0, 0, 0, 0};
	// 受信データ（SIZE CMD ACK SUM）
	byte rx[] = {0, 0, 0, 0}; 
	byte i;
	int addr, res;
//	_Bool bit;
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_pio_out (): port is not open.\n");
		return false;
	}
#endif

	// 書き出しアドレスの設定
	addr = PIO_DATA;
	
	// cmd[3] - cmd[4] = RAMアドレス
	// cmd[5] = 0固定
	// cmd[6] - cmd[7] = リテラル値
	// cmd[8] = SUM
	cmd[3] = addr;
	cmd[4] = addr >> 8; // １バイト分下位にずらす

	// 書き出しデータの設定
	cmd[6] = values;
	cmd[7] = values >> 8;

	// チェックサムの計算
	cmd[8] = check_sum (cmd, 8); // チェックサムデータを含めない
	
	i = rcb4_cmd_trx (cmd, 9, rx, 4);
	
	if (i != 4) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_pio_out (): insufficient data.\n");
#endif
		return false;
	}
	
	return true;
}


//---------------------------
//
// サーボモーター関連
//
//---------------------------
//
// 任意のサーボモーターのポジションを設定します
//
int rcb4_single_servo (
	int port,     // ポートはSIO1またはSIO2から選択
	byte id,       // サーボモーターのID番号
	byte frame,    // フレーム数（フレーム周期×frameだけ時間がかかる）
	int position  // 目標位置
)
{
	int devno = id * 2 + (port >> 8), i;
	byte cmd[] = {0x07, RCB4_CMD_SingleServo, devno, frame, 0, 0, 0};
	byte rx[4];
	
	cmd[4] = position;      // 下位８ビット
	cmd[5] = position >> 8; // ポジションデータの上位８ビット
	
	cmd[6] = check_sum (cmd, 6); // チェックサムを入力
	
	i = rcb4_cmd_trx (cmd, 7, rx, 4);
	
	if (i != 4) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_single_servo (): insufficient data.\n");
#endif
		return -1;
	}
	
	return position; // 成功したらポジションをそのまま返す(読み込まない)
}

//
// SELBITを作成する
_Bool rcb4_set_selbit (byte *devno, byte num)
{
	int i, bit, idx;
	
	// selbitの初期化
	memset (selbit, 0, sizeof (byte) * 5);
	
	for (i = 0; i < num; i++)
	{
		bit = (devno[i] - 1) % 8; // byte単位のビット番号を計算する
	
		if (devno[i] <= 8)
		{
			idx = 0;
		}
		else if (devno[i] <= 16)
		{
			idx = 1;
		}
		else if (devno[i] <= 24)
		{
			idx = 2;
		}
		else if (devno[i] <= 32)
		{
			idx = 3;
		}
		else
		{
			idx = 4;
		}
		selbit[idx] |= (0x01 << bit);
	}

#ifdef _KCB3_DEBUG
	printf ("selbit = %02x %02x %02x %02x %02x\n", selbit[0], selbit[1], selbit[2], selbit[3], selbit[4]);
#endif
}

//
// 使用するサーボモーターの登録(可変長引数)
int rcb4_regist_servo (int num, ...)
{
	va_list list;
	int i, j, value, devno, id, port;
	byte *selbitlist;
	struct rcb4_reg_servo tmp; // 入換用
			
	// listをnumでスタックする
	va_start (list, num);
	
	// 構造体のサイズを確保して0クリア
	rcb4_reg_servo_data = (struct rcb4_reg_servo*)calloc (sizeof (struct rcb4_reg_servo), num);
	// メモリーを確保できませんでした
	if (rcb4_reg_servo_data == NULL)
	{
		printf ("rcb4_regist_servo(): cannot allocate memory (rcb4_reg_servo_data).\n");
		return -1;
	}
	
	rcb4_reg_servo_count = 0; // データ個数を初期化しておく
	// selbitのリストを作成する
	selbitlist = (byte *)calloc (sizeof (char), num);
	// メモリーを確保できませんでした
	if (selbitlist == NULL)
	{
		printf ("rcb4_regist_servo(): cannot allocate memory (selbitlist).\n");
		return -1;
	}

	// データ登録
	for (i = 0; i < num; i++)
	{
		// サーボモーターのIDやニュートラルの登録およびデバイス番号登録
		value = va_arg (list, int);
		id = value & 0x001F;
		port = value >> 8;
		devno = id * 2 + port;     // デバイス番号を取得
		
		// エラーチェック
		if (id > 18) // RCB-4ではSIOは１ポート18個制限
		{
			printf ("rcb4_regist_servo(): wroing id %d (must be 0 - 18)\n", id);
			return -1;
		}
		if (devno > 35) // SIOポート使用可能数0 - 35
		{
			printf ("rcb4_regist_servo(): wrong device number %d (must be 0 - 35)\n", devno);
			return -1;
		}

		// 重複登録チェック
		for (j = 0; j < i; j++) 
		{
			if (rcb4_reg_servo_data[j].devno == devno)
			{
				printf ("rcb4_regist_servo(): device number %d (id = %d, port = %s) is duplicated.\n", 
					devno, rcb4_reg_servo_data[j].id, (rcb4_reg_servo_data[j].port << 8 == SIO1 ? "SIO1" : "SIO2"));
				return -1;
			}
		}

		rcb4_reg_servo_data[i].id = id;       // IDは下位５ビットのみ取り出す
		rcb4_reg_servo_data[i].port = port;   // portは8ビット下位にシフト
		rcb4_reg_servo_data[i].devno = devno; // Device番号登録
		rcb4_reg_servo_data[i].position = SERVO_NEUTRAL; // ニュートラルをセット
//		rcb4_reg_servo_data[i].frame = 10;    // ここは適当にセットしておく（これ以降は今後のため）
//		rcb4_reg_servo_data[i].speed = 127;   // 最大値
//		rcb4_reg_servo_data[i].stretch = 127; // 最大値
		
		// 後でselbitを作成する
		selbitlist[i] = devno;
	}
	
	// サーボの数を保管しておく
	rcb4_reg_servo_count = num;
	
	// selbitの作成を済ませておく
	rcb4_set_selbit (selbitlist, num);
	

	// あとから処理が簡単になるように、rcb4_reg_servo_data配列をdevno順で並び替える(バブルソート)
	for (i = 0; i < rcb4_reg_servo_count - 1; i++)
	{
		for (j = i + 1; j < rcb4_reg_servo_count; j++)
		{
			// もしデバイス番号を元に順番が昇順になっていなかったら入れ替えをする
			if (rcb4_reg_servo_data[i].devno > rcb4_reg_servo_data[j].devno)
			{
				memcpy (&tmp, &rcb4_reg_servo_data[i], sizeof (struct rcb4_reg_servo)); // 一時データ保存
				memcpy (&rcb4_reg_servo_data[i], &rcb4_reg_servo_data[j], sizeof (struct rcb4_reg_servo));
				memcpy (&rcb4_reg_servo_data[j], &tmp, sizeof (struct rcb4_reg_servo));
			}
		}
	}

	// デバイス番号配列を-1(未登録)で初期化
	memset (rcb4_reg_servo_devno, -1, RCB4_MAX_ICS_COUNT);

	for (i = 0; i < rcb4_reg_servo_count; i++)
	{
		devno = rcb4_reg_servo_data[i].devno;
		// デバイス番号リストを登録する
		rcb4_reg_servo_devno[devno] = i; 
	}

#ifdef _KCB3_DEBUG
	// 送信コマンドを表示する
	for (i = 0; i < 36; i++)
	{
		printf ("%d ", rcb4_reg_servo_devno[i]);
	}
	printf ("\n");
#endif
	
	return rcb4_reg_servo_count; // 登録済みのサーボの個数を返す
}

//
// フレームが一定でサーボモーターを動かす
// モーター毎のポジションデータはあらかじめ登録してから呼び出すこと
int rcb4_set_pos (byte frame)
{
	int i, j;
	byte *cmd, cnt, rx[6];
	
#ifdef _KCB3_DEBUG
	if (rcb4_fd < 0)
	{
		printf ("rcb4_set_pos(): port is not open.\n");
		return -1;
	}
#endif	
	// まだサーボモーターが登録されていない場合はエラー
	if (rcb4_reg_servo_count < 0)
	{
		printf ("rcb4_set_position(): no servo motor is regstered.\n");
		return -1;	
	}
	
	// コマンド配列の大きさを確保する
	//   0   1   2 - 6     7    8     9        n-2   n-1   n
	// SIZE CMD SELBIT(5) FRM POS_L POS_H ... POS_L POS_H SUM
	cnt = 9 + rcb4_reg_servo_count * 2; // 固定バイト数9 + POS_L, POS_Hの組(rcb4_reg_servo_count) * 2
	// 送信コマンドのバイト列でーたの大きさを確保する
	cmd = (byte *)calloc (sizeof (char), cnt);
	// メモリーを確保できませんでした
	if (cmd == NULL)
	{
		printf ("rcb4_set_pos(): cannot allocate memory (command).\n");
		return -1;
	}
	
	// イニシャルデータ
	cmd[0] = cnt;
	cmd[1] = RCB4_CMD_ConstFrameServo;
	cmd[2] = selbit[0];
	cmd[3] = selbit[1];
	cmd[4] = selbit[2];
	cmd[5] = selbit[3];
	cmd[6] = selbit[4];
	cmd[7] = frame;

	// ポジションデータを追加しておく
	for (i = 0, j = 8; i < rcb4_reg_servo_count; i++)
	{
		cmd[j++] = rcb4_reg_servo_data[i].position;      // 下位8bitのみ
		cmd[j++] = rcb4_reg_servo_data[i].position >> 8; // 上位8bit 
	}
	
	// チェックサムの計算
	cmd[cnt - 1] = check_sum (cmd, cnt - 1);

#ifdef _KCB3_DEBUG
	printf ("cmd count = %d\n", cnt);
	// 送信コマンドを表示する
	for (i = 0; i < cnt; i++)
	{
		printf ("%02x ", cmd[i]);
	}
	printf ("\n");
#endif

	// --------------------------------------------
	// コマンドの送信
	i = rcb4_cmd_trx (cmd, cnt, rx, 4);
	
	if (i != 4) // 正しく受信できなかった
	{
#ifdef _KCB3_DEBUG
		printf ("rcb4_set_pos(): insufficient data.\n");
#endif
		return -1;
	}
#ifdef _KCB3_DEBUG
	else if (rx[2] == RCB4_Nack)
	{
		printf ("rcb4_set_pos(): cannot receive acknowledgement.\n");
	}
#endif

	return 0;
}

//
// サーボモーターのデータを登録します
int rcb4_set_servo_pos (int servo, int position)
{
	int devno = (servo & 0x1F) * 2 + (servo >> 8);

	if (rcb4_reg_servo_count < 0)
	{
		printf ("rcb4_set_servo_pos(): no servo is registered.\n");
		return -1;
	}

	rcb4_reg_servo_data[rcb4_reg_servo_devno[devno]].position = position;
	
	return devno;
}

#endif //_KCB3_RCB4
