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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCppWrapper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

#include "TcpServer.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
//#define SAMPLE_XML_PATH "../../Config/SamplesConfig.xml"
//#define SAMPLE_XML_PATH "/usr/etc/primesense/work/OpenNI/Samples/Config/SamplesConfig.xml"
#define SAMPLE_XML_PATH "/usr/etc/primesense/config/SamplesConfig.xml"
#define SAMPLE_XML_PATH_LOCAL "SamplesConfig.xml"
#define MAX_NUM_USERS 5

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::UserGenerator g_UserGenerator;
xn::ImageGenerator g_ImageGenerator;
xn::DepthGenerator g_DepthGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnBool fileExists(const char *fn) {
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d New User %d\n", epochTime, nId);

    if (g_bNeedPose) {
        g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
    } else {
        g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Lost user %d\n", epochTime, nId);	
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Calibration started for user %d\n", epochTime, nId);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    if (eStatus == XN_CALIBRATION_STATUS_OK) {
        // Calibration succeeded
        printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
        g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    } else {
        // Calibration failed
        printf("%d Calibration failed for user %d\n", epochTime, nId);
        if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT) {
            printf("Manual abort occured, stop attempting to calibrate!");
            return;
        }

        if (g_bNeedPose) {
            g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        } else {
            g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}


#define CHECK_RC(nRetVal, what)					    \
    if (nRetVal != XN_STATUS_OK)				    \
{								    \
    printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));    \
    return nRetVal;						    \
}

void *userThread(void *data) {
    XnUInt32 epochTime = 0;
    xn::SceneMetaData sceneMD;
    xn::ImageMetaData imageMD;

    XnUserID aUsers[MAX_NUM_USERS];
    XnUInt16 nUsers;
    XnSkeletonJointTransformation torsoJoint;

    TcpServer *imageServer = new TcpServer();
    TcpServer *depthServer = new TcpServer();
    imageServer->start(8013);
    depthServer->start(8014);

    while (1) {
        // g_Context.WaitOneUpdateAll(g_UserGenerator);
        g_Context.WaitAnyUpdateAll();

        nUsers = MAX_NUM_USERS;
        g_UserGenerator.GetUsers(aUsers, nUsers);
        int numTracked = 0;
        int userToPrint=-1;
	int buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        g_ImageGenerator.GetMetaData(imageMD);
        char *imageData = (char *)imageMD.WritableData();

        for (XnUInt16 i = 0; i < nUsers; i++) {
            memset(buff, 0, sizeof(buff));
            if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==FALSE) {
	        // clear 
                buff[0] = i;
                depthServer->writeToClient(sizeof(buff), buff);
                continue;
            }

            //
            // tracking user
            //
            XnPoint3D com;
            g_UserGenerator.GetCoM(aUsers[i], com);
            g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);
	    buff[0] = i;
            buff[1] = (int)com.X;
            buff[2] = (int)com.Y;
            buff[3] = (int)com.Z;


            //
            // check power meter(using head to torso point)
            //
            XnSkeletonJointPosition point1, point2, headpoint;
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD , headpoint);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, point1);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD , point2);


            float lengthX = point1.position.X - point2.position.X;
            float lengthY = point1.position.Y - point2.position.Y;
            float lengthZ = point1.position.Z - point2.position.Z;
            float powerLength = sqrtf(lengthX*lengthX + lengthY*lengthY + lengthZ*lengthZ);

            // get integera number
            float powerInt;
            modff(powerLength*1000, &powerInt);

            if (point1.fConfidence >= 0.5 && point2.fConfidence >= 0.5) {
                buff[4] = (int)headpoint.position.X;
                buff[5] = (int)headpoint.position.Y;
                buff[6] = (int)headpoint.position.Z;
                buff[7] = (int)powerInt;
            }
	    depthServer->writeToClient(sizeof(buff), buff);

            //
            // draw tracked user
            // (r,g,b)
            //
            g_UserGenerator.GetUserPixels(0, sceneMD);
            const XnLabel* pLabels = sceneMD.Data();
            for (int w = 0; w < 320; w++) {
                for (int h = 0; h < 240; h++) {
                    int index = (w * 240 + h) * 3;
                    if (pLabels[w*240 + h] == 1) {
                        imageData[index] = 255; imageData[index + 1] =   0; imageData[index + 2] = 0;
                    } else if (pLabels[w*240 + h] == 2) {
                        imageData[index] =   0; imageData[index + 1] = 255; imageData[index + 2] = 0;
                    } else if (pLabels[w*240 + h] == 3) {
                        imageData[index] =   0; imageData[index + 1] =   0; imageData[index + 2] = 255;
		    } else if (pLabels[w*240 + h] != 0) {
                        imageData[index] = 102; imageData[index + 1] =   0; imageData[index + 2] = 0;
                    }
                }
            }
        }
        imageServer->writeToClient(320*240*3, (void*)imageData);
    }
    printf("userThread end ...\n");
}


int main(int argc, char **argv) {
    XnStatus nRetVal = XN_STATUS_OK;
    xn::EnumerationErrors errors;

    const char *fn = NULL;
    if    (fileExists(SAMPLE_XML_PATH)) fn = SAMPLE_XML_PATH;
    else if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
    else {
        printf("Could not find '%s' nor '%s'. Aborting.\n" , SAMPLE_XML_PATH, SAMPLE_XML_PATH_LOCAL);
        return XN_STATUS_ERROR;
    }

    nRetVal = g_Context.InitFromXmlFile(fn, g_scriptNode, &errors);
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_ImageGenerator);
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if (nRetVal != XN_STATUS_OK) {
        nRetVal = g_UserGenerator.Create(g_Context);
        CHECK_RC(nRetVal, "Find user generator");
    }

    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
        printf("Supplied user generator doesn't support skeleton\n");
        return 1;
    }

    nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);

    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration()) {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
            printf("Pose required, but not supported\n");
            return 1;
        }
        nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
        CHECK_RC(nRetVal, "Register to Pose Detected");
        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    nRetVal = g_Context.StartGeneratingAll();

    pthread_t t_UserThread, t_ImageThread, t_DepthThread;
    pthread_create(&t_UserThread, NULL, userThread, (void *)NULL );
    pthread_join(t_UserThread, NULL);

    g_scriptNode.Release();
    g_DepthGenerator.Release();
    g_UserGenerator.Release();
    g_Context.Release();

}
