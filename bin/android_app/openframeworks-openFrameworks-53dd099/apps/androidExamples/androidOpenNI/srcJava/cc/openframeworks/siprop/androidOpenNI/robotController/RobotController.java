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
package cc.openframeworks.siprop.androidOpenNI.robotController;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;

import android.util.Log;

public class RobotController {
	
	static final private byte toward = 0x10;
	static final private byte turn_right = 0x15;
	static final private byte turn_left = 0x14;
	static final private byte calibration = 0x25;
	static final private byte finish_calibration = 0x26;
	static final private byte applause = 0x08;
	static final private byte home_position = 0x02;
	static final private byte laydown_recover = 0x24;
	
	private static final RobotController instance = new RobotController();
	private RobotController() {}
	static public RobotController getIinstance() {
		return instance;
	}
	
	
	public void toward() throws RobotControllerException {
        Log.d("RobotController", "Toward!");
		sendPacket(toward);
	}
	public void turnRight() throws RobotControllerException {
        Log.d("RobotController", "turnRight!");
		sendPacket(turn_right);
	}
	public void turnLeft() throws RobotControllerException {
        Log.d("RobotController", "turnLeft!");
		sendPacket(turn_left);
	}
	public void calibrate() throws RobotControllerException {
        Log.d("RobotController", "calibrate!");
		sendPacket(calibration);
	}
	public void homePosition() throws RobotControllerException {
        Log.d("RobotController", "homePosition!");
		sendPacket(home_position);
	}
	public void laydownRecover() throws RobotControllerException {
        Log.d("RobotController", "laydownRecover!");
		sendPacket(laydown_recover);
	}
	public void applause() throws RobotControllerException {
        Log.d("RobotController", "applause!");
		sendPacket(applause);
	}
	public void finishCalibrate() throws RobotControllerException {
        Log.d("RobotController", "finishCalibration!");
		sendPacket(finish_calibration);
		
	}
	
	private void sendPacket(byte commandNumber) throws RobotControllerException {
        InetSocketAddress remoteAddress =
                new InetSocketAddress("192.168.0.1", 8000);
        
		byte[] sendBuffer = {commandNumber};
		
		// Create & send UDP packet
        Log.d("RobotController", "Try to send packet:" + sendBuffer[0]);
		DatagramPacket sendPacket;
		try {
			sendPacket = new DatagramPacket(sendBuffer, sendBuffer.length, remoteAddress);
		    new DatagramSocket().send(sendPacket);
		} catch (SocketException e) {
	        Log.d("RobotController", "SocketException:" + e.getMessage());
			e.printStackTrace();
	        throw new RobotControllerException();
		} catch (IOException e) {
	        Log.d("RobotController", "IOException:" + e.getMessage());
			e.printStackTrace();
	        throw new RobotControllerException();
		}
	}
		
}
