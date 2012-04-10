/*
 * Copyright (C) 2006-2012 SIProp Project http://www.siprop.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "mind-controller.h"

#include <errno.h>
#include <time.h>
#include <stdio.h>

#define SYNC 0xAA
#define EXCODE 0x55
#define HEADSET_FOUND_CONNECTED 0xD0
#define HEADSET_NOT_FOUND 0xD1
#define HEADSET_DISCONNECTED 0xD2
#define HEADSET_REQUEST_DENIED 0xD3
#define HEADSET_STANDBY 0xD4

#define MAX_DATA 32


#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_RATE B38400
//#define BAUD_RATE B57600
#define _POSIX_SOURCE 1         //POSIX compliant source
#define FALSE 0
#define TRUE 1

int GLOBAL_FD = 0;

int parsePayload( unsigned char *payload, int *data, unsigned char pLength );


long BAUD = 38400;              // derived baud rate
//long BAUD = 57600;              // derived baud rate
long DATABITS = CS8;
long STOPBITS = CSTOPB;
long PARITYON = 0;
long PARITY = 0;

char connect_code[1] = {0xC2};
char disconnect_code[1] = {0xC1};



void signal_handler_IO (int status) {
//   wait_flag = FALSE;
	;
}

JNIEXPORT
jint
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStart( JNIEnv* env,
                                                  jobject thiz )
{

	int i, ret;
	unsigned char c = 0;
	struct termios oldtio, newtio;
	struct termios oldkey, newkey;
	struct sigaction saio;

	if(GLOBAL_FD != 0) {
		Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStop(env, thiz);
	}

	newkey.c_cflag = BAUD_RATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newkey.c_iflag = IGNPAR;
	newkey.c_oflag = 0;
	newkey.c_lflag = 0;
	newkey.c_cc[VMIN]=1;
	newkey.c_cc[VTIME]=0;

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Try Open");
	GLOBAL_FD = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (GLOBAL_FD < 0)
	{
	   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Error Open");
	   return errno;
	}
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "OK Open");

	sigemptyset(&saio.sa_mask);
	saio.sa_handler = signal_handler_IO;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Set setting");
	fcntl(GLOBAL_FD, F_SETOWN, getpid());
	fcntl(GLOBAL_FD, F_SETFL, FASYNC);
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "End setting");

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Try tcgetattr");
	tcgetattr(GLOBAL_FD,&oldtio);
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "End tcgetattr");
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | PARITYON | PARITY | CLOCAL | CREAD;
//	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Set setting 2");
	tcflush(GLOBAL_FD, TCIFLUSH);
	tcsetattr(GLOBAL_FD,TCSANOW,&newtio);
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "End setting 2");

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,"write init command.");
	ret = write(GLOBAL_FD, connect_code, sizeof(connect_code));
	if(ret == 0) {
		Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStop(env, thiz);
		return -1;
	}
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,"End init command.");

	while(1) {
		sleep(1);

		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "check SYNC");
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;

		ret = read(GLOBAL_FD, &c, 1);
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "command:%d", c);
		if( c == 0x04) {
			ret = read(GLOBAL_FD, &c, 1);
			if( c == HEADSET_FOUND_CONNECTED ) {
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Connected! command:%d", c);
				break;
			} else if( c == HEADSET_NOT_FOUND || c == HEADSET_DISCONNECTED ) {
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "not found! command:%d", c);
				write(GLOBAL_FD, connect_code, sizeof(connect_code));
				continue;
			}
		} else if( c == 0x03) {
			ret = read(GLOBAL_FD, &c, 1);
			ret = read(GLOBAL_FD, &c, 1);
			ret = read(GLOBAL_FD, &c, 1);
			if( c == 0x01 ) {
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "scanning! command:%d", c);
				continue;
			} else if( c == 0x00 ) {
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "re-connect! command:%d", c);
				write(GLOBAL_FD, connect_code, sizeof(connect_code));
				continue;
			}
		} else if( c == 0x02) {
			__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "re-connect! command:%d", c);
			write(GLOBAL_FD, connect_code, sizeof(connect_code));
			continue;
		} else {
			__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Unknown command:%d", c);
			continue;
		}
	}

	return 0;
}


JNIEXPORT
jint
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStop( JNIEnv* env,
                                                  jobject thiz )
{
	int ret;

	if(GLOBAL_FD == 0) {
		   return 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,"write end command.");
	write(GLOBAL_FD, disconnect_code, sizeof(disconnect_code));
	write(GLOBAL_FD, disconnect_code, sizeof(disconnect_code));
	write(GLOBAL_FD, disconnect_code, sizeof(disconnect_code));

	ret = close(GLOBAL_FD);
	GLOBAL_FD = 0;

	return ret;

}


JNIEXPORT
jintArray
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_getMindDataFull( JNIEnv* env,
                                                  jobject thiz )
{
	int checksum, ret;
	int data[MAX_DATA] = {0};
	unsigned char payload[MAX_DATA] = {0};
	unsigned char pLength = 0;
	unsigned char c = 0;
	unsigned char i;

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Start getMindData");
	if(GLOBAL_FD == 0) return NULL;

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Start getMindData Loop");
	/* Loop forever, parsing one Packet per loop... */
	while( 1 ) {
		/* Synchronize on [SYNC] bytes */
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;
		/* Parse [PLENGTH] byte */
		while( TRUE ) {
			ret = read(GLOBAL_FD, &pLength, 1);
			if( pLength != 170 ) break;
		}
		if( pLength > 169 || pLength < 24) continue;
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Length is OK");
		/* Collect [PAYLOAD...] bytes */
		ret = read(GLOBAL_FD, payload, pLength);
		/* Calculate [PAYLOAD...] checksum */
		checksum = 0;
		for( i=0; i<pLength; i++ ) {
			checksum += payload[i];
		}
		checksum &= 0xFF;
		checksum = ~checksum & 0xFF;
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "made checksum:%d", checksum);
		/* Parse [CKSUM] byte */
		ret = read(GLOBAL_FD, &c, 1);
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "getten checksum:%d", c);
		/* Verify [CKSUM] byte against calculated [PAYLOAD...] checksum */
		if( c != checksum ) continue;
		/* Since [CKSUM] is OK, parse the Data Payload */
		if( !parsePayload( payload, data, pLength ) ) break;

	}
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "End parse");

	jintArray jdata = (*env)->NewIntArray(env, MAX_DATA);
	(*env)->SetIntArrayRegion(env, jdata, 0 , MAX_DATA, data);
	//(*env)->ReleaseIntArrayElements(env, jdata, data, 0);

	return jdata;
}

JNIEXPORT
jintArray
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_getMindData( JNIEnv* env,
                                                  jobject thiz )
{
	int checksum, ret, loop_counter;
	int data[MAX_DATA] = {0};
	unsigned char payload[MAX_DATA] = {0};
	unsigned char pLength = 0;
	unsigned char c = 0;
	unsigned char i;

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Start getMindData");
	if(GLOBAL_FD == 0) return NULL;

	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Start getMindData Loop");
	/* Loop forever, parsing one Packet per loop... */
	for( loop_counter = 0; loop_counter < 1000; loop_counter++ ) {
		/* Synchronize on [SYNC] bytes */
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;
		ret = read(GLOBAL_FD, &c, 1);
		if( c != SYNC ) continue;
		/* Parse [PLENGTH] byte */
		while( TRUE ) {
			ret = read(GLOBAL_FD, &pLength, 1);
			if( pLength != 170 ) break;
		}
		if( pLength > 169 ) continue;
		/* Collect [PAYLOAD...] bytes */
		ret = read(GLOBAL_FD, payload, pLength);
		/* Calculate [PAYLOAD...] checksum */
		checksum = 0;
		for( i=0; i<pLength; i++ ) {
			checksum += payload[i];
		}
		checksum &= 0xFF;
		checksum = ~checksum & 0xFF;
		/* Parse [CKSUM] byte */
		ret = read(GLOBAL_FD, &c, 1);
		/* Verify [CKSUM] byte against calculated [PAYLOAD...] checksum */
		if( c != checksum ) continue;
		/* Since [CKSUM] is OK, parse the Data Payload */
		if( !parsePayload( payload, data, pLength ) ) break;

	}
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "End parse");

	jintArray jdata = (*env)->NewIntArray(env, MAX_DATA);
	(*env)->SetIntArrayRegion(env, jdata, 0 , MAX_DATA, data);
	//(*env)->ReleaseIntArrayElements(env, jdata, data, 0);

	return jdata;
}


int parsePayload( unsigned char *payload, int *data, unsigned char pLength ) {

	unsigned char bytesParsed = 0;
	unsigned char code;
	unsigned char length;
	unsigned char extendedCodeLevel;
	int i;
	int temp_data_1 = 0, temp_data_2 = 0, temp_data_3 = 0;

	code = payload[2];
//	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "check code=%d", code);
	if(code == 0x04) {
		//ATTENTION
		{
			data[9] = (int)(payload[3]*0.392);
			__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "check ATTENTION(Only)=%d, CHANGED=%d",payload[3], data[9]);
		}
		return 0;
//	} else if(code == 0x05) {
//		//MEDITATION
//		{
//			data[10] = (int)payload[3];
//		}
//		return 0;
	} else if(code == 0x83) {
		length = payload[3];
	} else {
		return 1;
	}
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "EXCODE level: %d CODE: 0x%02X length: %d\n", extendedCodeLevel, code, length );
	data[0] = code;
	//Delta
	{
		temp_data_1 = (int)payload[6];
		temp_data_2 = (int)payload[5];
		temp_data_3 = (int)payload[4];
		data[1] = (temp_data_1<<16) | (temp_data_2<<8);
		data[1] |= temp_data_3;
	}
	//Theta
	{
		temp_data_1 = (int)payload[9];
		temp_data_2 = (int)payload[8];
		temp_data_3 = (int)payload[7];
		data[2] = (temp_data_1<<16) | (temp_data_2<<8);
		data[2] |= temp_data_3;
	}
	//Low-alpha
	{
		temp_data_1 = (int)payload[12];
		temp_data_2 = (int)payload[11];
		temp_data_3 = (int)payload[10];
		data[3] = (temp_data_1<<16) | (temp_data_2<<8);
		data[3] |= temp_data_3;
	}
	//High-alpha
	{
		temp_data_1 = (int)payload[15];
		temp_data_2 = (int)payload[14];
		temp_data_3 = (int)payload[13];
		data[4] = (temp_data_1<<16) | (temp_data_2<<8);
		data[4] |= temp_data_3;
	}
	//Low-beta
	{
		temp_data_1 = (int)payload[18];
		temp_data_2 = (int)payload[17];
		temp_data_3 = (int)payload[16];
		data[5] = (temp_data_1<<16) | (temp_data_2<<8);
		data[5] |= temp_data_3;
	}
	//High-beta
	{
		temp_data_1 = (int)payload[21];
		temp_data_2 = (int)payload[20];
		temp_data_3 = (int)payload[19];
		data[6] = (temp_data_1<<16) | (temp_data_2<<8);
		data[6] |= temp_data_3;
	}
	//Low-gamma
	{
		temp_data_1 = (int)payload[24];
		temp_data_2 = (int)payload[23];
		temp_data_3 = (int)payload[22];
		data[7] = (temp_data_1<<16) | (temp_data_2<<8);
		data[7] |= temp_data_3;
	}
	//Mid-gamma
	{
		temp_data_1 = (int)payload[27];
		temp_data_2 = (int)payload[26];
		temp_data_3 = (int)payload[25];
		data[8] = (temp_data_1<<16) | (temp_data_2<<8);
		data[8] |= temp_data_3;
	}
	//ATTENTION
	{
		data[9] = (int)payload[29];
//		data[9] = (int)payload[29]*0.392;
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "check ATTENTION(Full)=%d, CHANGED=%d",payload[29], data[9]);
	}
	//MEDITATION
	{
		data[10] = (int)payload[31];
//		data[10] = (int)payload[31]*0.392;
	}
	return 0;
}


void
handleDataValueFunc( unsigned char extendedCodeLevel,
					unsigned char code,
					unsigned char valueLength,
					const unsigned char *value,
					void *customData ) {

	int i;

	if( extendedCodeLevel == 0 ) {
		switch( code ) {
			/* [CODE]: ATTENTION eSense */
			case( 0x04 ):
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Attention Level: %d\n", value[0] & 0xFF );
				break;
			/* [CODE]: MEDITATION eSense */
			case( 0x05 ):
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Meditation Level: %d\n", value[0] & 0xFF );
				break;
			/* Other [CODE]s */
			default:
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,  "EXCODE level: %d CODE: 0x%02X vLength: %d\n",
				extendedCodeLevel, code, valueLength );
				for( i=0; i<valueLength; i++ ) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Data value(s): %02X", value[i] & 0xFF );
		}
	}
}


