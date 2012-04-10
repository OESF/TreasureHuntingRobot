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
