/*
 * Copyright (C) 2006-2011 SIProp Project http://www.siprop.org/
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
#ifndef _MIND_CONTROLLER
#define _MIND_CONTROLLER

#include <assert.h>
#include <jni.h>
#include <string.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

// for __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "formatted message");
#include <android/log.h>

// for native asset manager
#include <sys/types.h>
//#include <android/asset_manager.h>
//#include <android/asset_manager_jni.h>


#define ANDROID_LOG_VERBOSE ANDROID_LOG_DEBUG
#define LOG_TAG "MINDJNI"
#define INVALID_ARGUMENT -18456








#define		SAFE_DELETE(p)			{ if(p){ delete (p); (p)=0; } }
#define		SAFE_DELETE_ARRAY(p)	{ if(p){ delete [](p); (p)=0; } }




#ifdef __cplusplus
extern "C" {
#endif

jint
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStart( JNIEnv* env,
                                                  jobject thiz );

jint
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_mindStop( JNIEnv* env,
                                                  jobject thiz );

jintArray
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_getMindData( JNIEnv* env,
                                                  jobject thiz );

jintArray
Java_cc_openframeworks_siprop_androidOpenNI_mindController_MindController_getMindDataFull( JNIEnv* env,
                                                  jobject thiz );

#ifdef __cplusplus
}
#endif

#endif
