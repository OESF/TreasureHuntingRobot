//
// RCB-4にコントロールコードを送ってモーションを再生する
//
// このプログラムは
//

//#define _KCB3_DEBUG // デバッグモードが必要ない場合はこの行をコメントアウト

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <getopt.h>

// KCB-3WL SDK
#include <rcb4.h>
#include <gpio.h>

#define _POSIX_SOURCE 1 

#define MAX_BUFF_MASK		0x3F
#define BUFF_SIZE			64			//バッファーのサイズ(MAX_BUFF_MASK+1)
#define UDP_BUFF_SIZE		128			// UDP用バッファーのサイズ

//関数のプロトタイプ宣言
void usage (char *);

//
//グローバル変数
pthread_t threadRX; // スレッド
byte gDisp = 0;     // verboseモードセットフラグ

//
// uart受信スレッド
void * rx_thread (void *arg)
{
	// 必要ならここにスレッドプログラムを書く
}

//
// 使い方の表示
void usage (char *name)
{
	printf ("Usage: %s [-p <UDP PORT No.>] [-d <dev name>] [-v]\n", name);
}

//
// ここからmain関数
//
int main (int argc, char** argv)
{
	int sock;
	struct sockaddr_in	udpServerAddr; // サーバーアドレス
	struct sockaddr_in	udpClientAddr; // クライアントアドレス
	unsigned int cliAddrLen;
	char udpBuffer[UDP_BUFF_SIZE];	   // UDP用バッファ
	unsigned short udpServerPort = 0;
	int recvMsgSize, sendMsgSize;
	int val = 1; // ブロッキング切り替え
	unsigned short command , rcb4_res;
	
	int i = 0 , res , index;
	char *p_name = NULL;
	
	gDisp = 0;

	
	//
	// コマンドラインオプションを読み取る
	while ((res = getopt (argc,argv,"p:d:hv")) != -1)
	{
		switch(res)
		{
			case 'p': // ポートを読み取る
				udpServerPort = atoi (optarg);
				break;
			case 'd': // デバイスを読み取る
				p_name = (char *)malloc (sizeof(char) * (strlen (optarg) + 1));
				strncpy (p_name, optarg, strlen (optarg) + 1);
				break;
			case 'v': // プログラムをverboseモードにする
				gDisp = 1;
				break;
			case '?': // 簡単なヘルプを表示する
			case 'h':
			default:
				usage (argv[0]);
				exit (EXIT_SUCCESS);
		}
	}
	
	// udpサーバーのボートをセットする
	if (udpServerPort==0)
	{
		udpServerPort = 8000;
	}
	
	// シリアルポートのデバイスファイル名をセットする
	if (p_name == NULL)
	{
		p_name = (char *)malloc( sizeof(char) * 15);
		sprintf (p_name , "/dev/ttyS0");		//  SX-560の時のデフォルトポート
	}
	
	// ポートとデバイス名を表示する
	if (gDisp) 
	{
		printf ("UDP Port = %d , uart dev = %s\n", udpServerPort, p_name);
	}

	// RCB-4HVとの通信を開く
	if (rcb4_open (COM1, 0) == false) // ポートオープンに失敗したら終了
	{
		exit (1);
	}

	// スレッド作成	
	res = pthread_create (&threadRX, NULL, rx_thread, (void *) NULL);
	if (res != 0) // スレッド作成失敗
	{
		printf ("thread error\n");
		exit (1);
	} 
	else 
	{
		if (gDisp) 
		{
			printf ("RX thread start.\n");
		}
	}
	
	// gpioを開く
	if (gpio_open () < 0) 
	{
		perror ("gpio_open(): cannot open device /dev/gpio");
		exit (0);
	}

	// UDPソケットを開く
	if((sock = socket (PF_INET , SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror ("socket() failed");
	}

	// UDP通信設定をする
	memset (&udpServerAddr, 0, sizeof (udpServerAddr)); // アドレスデータを格納する構造体を初期化
	udpServerAddr.sin_family = AF_INET; // IPv4のアドレスを10進数形式ddd.ddd.ddd.dddで指定する
	// IPアドレスをホスト・バイトオーダーをネットワーク・バイトオーダーに変換 (4バイト)
	udpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	// ポートをホストバイトオーダーからネットワークバイトオーダーに変換
	udpServerAddr.sin_port = htons (udpServerPort);

	// セットしたネットワーク情報をソケットとバインドする
	if (bind (sock, (struct sockaddr *) &udpServerAddr, sizeof (udpServerAddr)) < 0)
	{
		perror("bind() failed");
	}

	ioctl (sock, FIONBIO, &val); // socketをノンブロッキングモードに設定する

	// コマンドデータ受信
	while(1)
	{
		// ソケットからメッセージを受け取る
		cliAddrLen = sizeof (udpClientAddr);
		recvMsgSize = recvfrom (sock, udpBuffer, UDP_BUFF_SIZE, 0, 
		                       (struct sockaddr *) &udpClientAddr, &cliAddrLen);

		// 受信に問題があった
		if (recvMsgSize < 1) 
		{
			if (errno == EAGAIN) // 単にデータがありませんでした
			{
				led_blue_on ();
			} 
			else // 受信エラーが起きました
			{
				perror("recvfrom() failed");
				break;
			}
		} 
		else // データを受けとりました
		{
			if (gDisp) // verboseモード
			{
				printf("\nHandling client %s\n", inet_ntoa (udpClientAddr.sin_addr));
			}
		
			command = udpBuffer[0];	
			// モーション再生
			rcb4_res = rcb4_motion_play (command);

			if (gDisp)
			{
				printf("Received: %d , RCB4 res = %d \n", command , rcb4_res);
			}
		}
	}

	close (sock);  // udpソケットクローズ
	rcb4_close (); // RCB-4との接続を閉じる
	gpio_close (); // GPIOとの接続を閉じる

	printf("Server Terminated\n");
	
	return 0;
}

