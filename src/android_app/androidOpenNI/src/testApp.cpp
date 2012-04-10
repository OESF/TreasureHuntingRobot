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
#include "testApp.h"

//--------------------------------------------------------------
int testApp::connectServer(char *server, int port) {
        int sock;
        struct sockaddr_in dst;

        memset(&dst, 0, sizeof(dst));
        dst.sin_port = htons(port);
        dst.sin_family = AF_INET;
        dst.sin_addr.s_addr = inet_addr(CONNECT_SERVER);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        int ret = connect(sock, (struct sockaddr *) &dst, sizeof(dst));
        if (ret != 0) {
                __android_log_print(ANDROID_LOG_INFO, "OF", "fail: connect to server %s:%d, %d", server, port, ret);
                return 0;
        }

        __android_log_print(ANDROID_LOG_INFO, "OF", "success: connect to server %s:%d, %d", server, port, ret);

        return sock;
}

void testApp::updateImage(testApp *app, char *data) {
    // get image data and save recvImageBuffer loop (need mutex lock when write to recvImageBuffer)
    pthread_mutex_lock(&app->imageBufferLock);

	for (int i = 0; i < OPENNI_WIDTH; i++) {
		for (int j = 0; j < OPENNI_HEIGHT; j++) {
			int index = (j * OPENNI_WIDTH + i) * 3;
			ofColor ofc = ofColor(data[index], data[index+1], data[index+2]);
			app->image.setColor(i, j, ofc);
		}
	}
	app->image.update();
    pthread_mutex_unlock(&app->imageBufferLock);
}

void* testApp::imageWorker(void* data) {
        __android_log_print(ANDROID_LOG_INFO, "OF", "imageWorker thread start");

        // get own class
        thread_args* tf_args = static_cast<thread_args*>(data);
        testApp *app = tf_args->This;

        // parameter
        int sock = app->connectServer(CONNECT_SERVER, IMG_SERVER_PORT);
        if (!sock) return 0;
        char recvImageBuffer[OPENNI_HEIGHT*OPENNI_WIDTH*3];
        int max_len = OPENNI_WIDTH * OPENNI_HEIGHT * 3;
        char tmpbuff[max_len];
        int len, total_len = 0;
        fd_set fds, readfds;
        struct  timeval tv;

        // init parameters
        bzero(&readfds, sizeof(readfds));
        FD_SET(sock, &readfds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        memset(recvImageBuffer, 0, sizeof(recvImageBuffer));

        while (1) {
			memcpy(&fds, &readfds, sizeof(fd_set));
			select(sock + 1, &fds, NULL, NULL, &tv);

			if(FD_ISSET(sock, &fds)) {
				len = read(sock, tmpbuff, max_len);
				if (len == 0) break;

	        	if (total_len == max_len) {
	        		total_len = 0;
	        		// draw;
	        		app->updateImage(app, recvImageBuffer);

	        		memcpy(recvImageBuffer, tmpbuff, len);
	        		total_len += len;
	        	} else if (total_len + len > max_len) {
	        		memcpy(recvImageBuffer + total_len, tmpbuff, max_len - total_len);
	        		// draw;
	        		app->updateImage(app, recvImageBuffer);

	        		memcpy(recvImageBuffer, tmpbuff + (max_len - total_len), len - (max_len - total_len));
	        		total_len = len - (max_len - total_len);
	        	} else {
	        		memcpy(recvImageBuffer + total_len, tmpbuff, len);
	        		total_len += len;
	        	}
			}
        }

        close(sock);
        return 0;
}

void testApp::checkUserInfo(testApp *app, int *userInfo) {
    pthread_mutex_lock(&app->depthListLock);
    if (userInfo[0] < MAX_NI_USER) {

    	// check position. tolerance = 20.
    	if ((depthInfo[userInfo[0]].x != 0 && depthInfo[userInfo[0]].z != 0 && userInfo[1] != 0 && userInfo[3] != 0) &&
    			((depthInfo[userInfo[0]].z - userInfo[3] >= 20 || depthInfo[userInfo[0]].z - userInfo[3] <= -20
        				|| depthInfo[userInfo[0]].x - userInfo[1] >= 20 || depthInfo[userInfo[0]].x - userInfo[1] <= -20))) {
    		float integra;
    		float radian = atan2f((float)(depthInfo[userInfo[0]].z - userInfo[3]),
    							(float)(depthInfo[userInfo[0]].x - userInfo[1]));
    		modff(radian*180/PAI, &integra);
    		depthInfo[userInfo[0]].degree = (int)integra;

    		/*
            __android_log_print(ANDROID_LOG_INFO, "OF", "[%d][%d][%d][%d]:[%f]degree:[%d]",
            		depthInfo[userInfo[0]].z, userInfo[3],
            		depthInfo[userInfo[0]].x, userInfo[1],
            		integra,
            		depthInfo[userInfo[0]].degree);
			*/
    	}

    	depthInfo[userInfo[0]].x = userInfo[1];
        depthInfo[userInfo[0]].y = userInfo[2];
        depthInfo[userInfo[0]].z = userInfo[3];

        depthInfo[userInfo[0]].headX = userInfo[4];
        depthInfo[userInfo[0]].headY = userInfo[5];
        depthInfo[userInfo[0]].headZ = userInfo[6];

        depthInfo[userInfo[0]].power = userInfo[7];

        //__android_log_print(ANDROID_LOG_INFO, "OF", "recv info:[%d,%d,%d,%d,%d,%d,%d,%d]",
        //				userInfo[0],userInfo[1],userInfo[2],userInfo[3],userInfo[4],userInfo[5],userInfo[6],userInfo[7]);
    }
    pthread_mutex_unlock(&app->depthListLock);
}

void* testApp::depthWorker(void* data) {
        __android_log_print(ANDROID_LOG_INFO, "OF", "depthWorker thread start");

        // get own class
        thread_args* tf_args = static_cast<thread_args*>(data);
        testApp *app = tf_args->This;

        // parameters
        int sock = app->connectServer(CONNECT_SERVER, DEPTH_SERVER_PORT);
        if (!sock) return 0;
        int max_len = sizeof(int) * 8;
        char tmpbuff[max_len];
        char userbuff[max_len];
        int len, total_len = 0;
        struct  timeval tv;
        fd_set fds, readfds;

        // init parameters
		bzero(&readfds, sizeof(readfds));
        FD_SET(sock, &readfds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        while (1) {
			memcpy(&fds, &readfds, sizeof(fd_set));
			select(sock + 1, &fds, NULL, NULL, &tv);

			if(FD_ISSET(sock, &fds)) {
		        len = read(sock, tmpbuff, max_len);
		        if (len == 0) break;

				if (total_len == max_len) {
					total_len = 0;
					app->checkUserInfo(app, (int *)userbuff);
					memcpy(userbuff, tmpbuff, len);
					total_len += len;
				} else if (total_len + len > max_len) {
					memcpy(userbuff + total_len, tmpbuff, max_len - total_len);
					app->checkUserInfo(app, (int *)userbuff);
					memcpy(userbuff, tmpbuff + (max_len - total_len), len - (max_len - total_len));
					total_len = len - (max_len - total_len);
				} else {
					memcpy(userbuff + total_len, tmpbuff, len);
					total_len += len;
		        }
			}
        }

        __android_log_print(ANDROID_LOG_INFO, "OF", "depthWorker thread end");
        close(sock);
        return 0;
}

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);

	image.allocate(640, 480, OF_IMAGE_COLOR);
	ofBackground(255, 255, 255);
	ofSetColor(255, 255, 255);

    __android_log_print(ANDROID_LOG_INFO, "OF", "thread start");
    pthread_mutex_init(&imageBufferLock, NULL);
    pthread_create(&imageThread, NULL, imageWorker, new thread_args(this,0));

    pthread_mutex_init(&depthListLock, NULL);
    pthread_create(&depthThread, NULL, depthWorker, new thread_args(this,0));
//	ofBackground(255,255,255);
}


//--------------------------------------------------------------
void testApp::update(){
    pthread_mutex_lock(&imageBufferLock);
	image.update();
    pthread_mutex_unlock(&imageBufferLock);
}

//--------------------------------------------------------------
void testApp::draw(){
	/*
	ofFill();
	ofSetHexColor(0xe0be21);

	ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode

    pthread_mutex_lock(&depthListLock);
	DepthInfo tmpdepthInfo[MAX_NI_USER];
	memcpy(tmpdepthInfo, depthInfo, sizeof(depthInfo));
    pthread_mutex_unlock(&depthListLock);

	for (int i = 0; i < MAX_NI_USER; i++) {
		if (tmpdepthInfo[i].x <= 0 || tmpdepthInfo[i].y <= 0 || tmpdepthInfo[i].z <= 0) continue;

		float xPct = (float)(tmpdepthInfo[i].x) / (float)(ofGetWidth());
		float yPct = (float)(tmpdepthInfo[i].z/5) / (float)(ofGetHeight());
		int nTips = 5 + xPct * 60;
		int nStarPts = nTips * 2;
		float angleChangePerPt = TWO_PI / (float)nStarPts;
		float innerRadius = 0 + yPct*40;
		float outerRadius = 40;
		float origx = tmpdepthInfo[i].x;
		float origy = tmpdepthInfo[i].z/5;
		float angle = 0;

		ofSetHexColor(0xa16bca);
		ofBeginShape();
		for (int i = 0; i < nStarPts; i++){
			if (i % 2 == 0) {
				// inside point:
				float x = origx + innerRadius * cos(angle);
				float y = origy + innerRadius * sin(angle);
				ofVertex(x,y);
			} else {
				// outside point
				float x = origx + outerRadius * cos(angle);
				float y = origy + outerRadius * sin(angle);
				ofVertex(x,y);
			}
			angle += angleChangePerPt;
		}
		ofEndShape();
	}

	glPopMatrix();
	*/

    pthread_mutex_lock(&imageBufferLock);
	image.draw(0, 0);

    pthread_mutex_lock(&depthListLock);
	DepthInfo tmpdepthInfo[MAX_NI_USER];
	memcpy(tmpdepthInfo, depthInfo, sizeof(depthInfo));
    pthread_mutex_unlock(&depthListLock);

    for (int i = 0; i < MAX_NI_USER; i++) {
		if (tmpdepthInfo[i].x <= 0 || tmpdepthInfo[i].y <= 0 || tmpdepthInfo[i].z <= 0 || tmpdepthInfo[i].power <= 0) continue;
		ofDrawBitmapString(ofToString(tmpdepthInfo[i].power), tmpdepthInfo[i].x, tmpdepthInfo[i].y);
	}

    pthread_mutex_unlock(&imageBufferLock);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	ofLog(OF_LOG_WARNING,"%i,%i",x,y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
