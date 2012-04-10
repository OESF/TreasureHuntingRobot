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
package cc.openframeworks.siprop.androidOpenNI;

import android.os.SystemClock;

public class Sleep {
	
    public static void sleep(long ms) {
    	long start = SystemClock.uptimeMillis();
    	long duration = ms;
    	boolean interrupted = false;
    	do {
    	    try {
    	        Thread.sleep(duration);
    	    }
    	    catch (InterruptedException e) {
    	        interrupted = true;
    	    }
    	    duration = start + ms - SystemClock.uptimeMillis();
    	} while (duration > 0);
    	if (interrupted) {
    	    // Important: we don't want to quietly eat an interrupt() event,
    	    // so we make sure to re-interrupt the thread so that the next
    	    // call to Thread.sleep() or Object.wait() will be interrupted.
    	    Thread.currentThread().interrupt();
    	}  	
    }
}
