#include "ofMain.h"
#include "testApp.h"
#include "mind-controller.h"
#ifdef TARGET_ANDROID
	#include "ofAppAndroidWindow.h"
#else
	#include "ofAppGlutWindow.h"
#endif


testApp *app;
int main(){

#ifdef TARGET_ANDROID
	ofAppAndroidWindow *window = new ofAppAndroidWindow;
#else
	ofAppGlutWindow *window = new ofAppGlutWindow;
#endif
	ofSetupOpenGL(window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	app = new testApp();
	ofRunApp(app);
	return 0;
}


#include <jni.h>

//========================================================================
extern "C"{

void Java_cc_openframeworks_OFAndroid_init( JNIEnv*  env, jobject  thiz ){
	main();
}

JNIEXPORT
jintArray
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_OFActivity_getUserData( JNIEnv* env,
												  jobject thiz ) {
	pthread_mutex_lock(&app->depthListLock);
	int data[MAX_NI_USER] = {0,0,0,0,0};
	for (int i = 0; i < MAX_NI_USER; i++) {
		if (app->depthInfo[i].x == 0 && app->depthInfo[i].y == 0 && app->depthInfo[i].z == 0)
			data[i] = 0;
		else
			data[i] = 1;
	}

	jintArray jdata = env->NewIntArray(MAX_NI_USER);
	env->SetIntArrayRegion(jdata, 0 , MAX_NI_USER, data);
	pthread_mutex_unlock(&app->depthListLock);

	return jdata;
}

JNIEXPORT
jintArray
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_OFActivity_getUserPos( JNIEnv* env,
												  jobject thiz,
												  jint user) {
	int data[3] = {0};

	pthread_mutex_lock(&app->depthListLock);
	data[0] = app->depthInfo[user].x;
	data[1] = app->depthInfo[user].y;
	data[2] = app->depthInfo[user].z;

	jintArray jdata = env->NewIntArray(3);
	env->SetIntArrayRegion(jdata, 0 , 3, data);
	pthread_mutex_unlock(&app->depthListLock);

	return jdata;
}

JNIEXPORT
jintArray
JNICALL
Java_cc_openframeworks_siprop_androidOpenNI_OFActivity_getUserInfo( JNIEnv* env,
												  jobject thiz,
												  jint user) {
	int data[5] = {0,0,0,0,0};

	pthread_mutex_lock(&app->depthListLock);
	data[0] = app->depthInfo[user].x;
	data[1] = app->depthInfo[user].y;
	data[2] = app->depthInfo[user].z;
	data[3] = app->depthInfo[user].power;
	data[4] = app->depthInfo[user].degree;

	jintArray jdata = env->NewIntArray(5);
	env->SetIntArrayRegion(jdata, 0 , 5, data);
	pthread_mutex_unlock(&app->depthListLock);

	return jdata;
}

}
