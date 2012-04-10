#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxAndroid.h"


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include <android/log.h>



#define CONNECT_SERVER "127.0.0.1"
#define IMG_SERVER_PORT 8013
#define DEPTH_SERVER_PORT 8014
#define OPENNI_WIDTH 320
#define OPENNI_HEIGHT 240
#define MAX_NI_USER 5
#define PAI 3.1415926535


class testApp : public ofxAndroidApp{
	private:
		static int connectServer(char *server, int port);
		static void* imageWorker( void* param );
		static void* depthWorker( void* param );
		void updateImage(testApp *app, char *data);
		void checkUserInfo(testApp *app, int *userInfo);
	public:

		pthread_t imageThread, depthThread, powerThread;
		pthread_mutex_t imageBufferLock, depthListLock;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        struct thread_args {
                testApp *This;
                void* actual_arg;
                thread_args(testApp* t, void *p) : This(t), actual_arg(p){};
        };


		struct DepthInfo {
			int x, y, z;
			int headX, headY, headZ;
			int power;
			int degree;
		};

		DepthInfo depthInfo[MAX_NI_USER];

		ofImage image;
};

#endif	

