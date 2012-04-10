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
package cc.openframeworks.siprop.androidOpenNI.mindController;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;

import cc.openframeworks.siprop.androidOpenNI.Sleep;

import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;

public class MindController {
	
	private static MindController instance;
	
	private MindController() {
	}
	
	public static MindController getInstance() {
		if(instance == null) {
			instance = new MindController();
		}
		return instance;
	}
	
    private int delta;
    private int theta;
    private int low_alpha;
    private int high_alpha;
    private int low_beta;
    private int high_beta;
    private int low_gamma;
    private int mid_gamma;
    private int ASIC_EEG_POWER;
    private int ATTENTION_eSense;
    private int MEDITATION_eSense;

    
    
    public int getDelta() {
		return delta;
	}

	public int getTheta() {
		return theta;
	}

	public int getLow_alpha() {
		return low_alpha;
	}

	public int getHigh_alpha() {
		return high_alpha;
	}

	public int getLow_beta() {
		return low_beta;
	}

	public int getHigh_beta() {
		return high_beta;
	}

	public int getLow_gamma() {
		return low_gamma;
	}

	public int getMid_gamma() {
		return mid_gamma;
	}
	
	public int getASIC_EEG_POWER() {
		return ASIC_EEG_POWER;
	}
	
	public int getMEDITATION_eSense() {
		return MEDITATION_eSense;
	}
	
	public int getATTENTION_eSense() {
		return ATTENTION_eSense;
	}
	
	
    
    public void refresh() throws MindControllerException {
		Log.d("refreash", "getData.");
		
    	int[] data = getMindData();
    	if(data == null) {
    		Log.d("refreash", "data is NULL.");
    		return;
    	}

    	
//    	data[0] = code;
//    	//Delta
//    		data[1] != temp_data_3;
//    	//Theta
//    		data[2] != temp_data_3;
//    	//Low-alpha
//    		data[3] != temp_data_3;
//    	//High-alpha
//    		data[4] != temp_data_3;
//    	//Low-beta
//    		data[5] != temp_data_3;
//    	//High-beta
//    		data[6] != temp_data_3;
//    	//Low-gamma
//    		data[7] != temp_data_3;
//    	//Mid-gamma
//    		data[8] != temp_data_3;
//    	//ATTENTION
//    		data[9] = (int)payload[28];
//    	//MEDITATION
//    		data[10] = (int)payload[30];
    	
    	ASIC_EEG_POWER = data[0];
    	delta = data[1];
    	theta = data[2];
    	low_alpha = data[3];
    	high_alpha = data[4];
    	low_beta = data[5];
    	high_beta = data[6];
    	low_gamma = data[7];
    	mid_gamma = data[8];
    	ATTENTION_eSense = data[9];
    	MEDITATION_eSense = data[10];
    	
    }

    
    

	public void start() throws MindControllerException {
		
    	if (mindStart() != 0) {
    		throw new MindControllerException();
    	}
    	
    	Sleep.sleep(12000);
    	
    }
    public void stop()  throws MindControllerException {
    	if (mindStop() != 0) {
    		throw new MindControllerException();
    	}
    }
    
    
    private native int  mindStop();
    private native int  mindStart();
    private native int[]	getMindData();
    
    static {
    	System.loadLibrary("OFAndroidApp");
//    	System.loadLibrary("mind-controller");
    }
    
}
