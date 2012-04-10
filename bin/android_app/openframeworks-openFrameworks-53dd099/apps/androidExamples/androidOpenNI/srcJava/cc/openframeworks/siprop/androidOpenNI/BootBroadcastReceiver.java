package cc.openframeworks.siprop.androidOpenNI;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class BootBroadcastReceiver extends BroadcastReceiver {

    @Override  
    public void onReceive(Context context, Intent intent) {  
        Intent i = new Intent(context, OFActivity.class); 
        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(i);
    }
    
}
