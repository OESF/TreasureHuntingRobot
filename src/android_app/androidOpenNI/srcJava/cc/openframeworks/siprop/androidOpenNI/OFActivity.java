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

import java.util.Date;
import java.util.Random;

import org.afree.data.time.Second;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.RotateAnimation;
import android.view.animation.ScaleAnimation;
import android.view.animation.TranslateAnimation;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import cc.openframeworks.OFAndroid;
import cc.openframeworks.siprop.androidOpenNI.mindController.MindController;
import cc.openframeworks.siprop.androidOpenNI.mindController.MindControllerException;
import cc.openframeworks.siprop.androidOpenNI.mindController.TimeSeriesChartView;
import cc.openframeworks.siprop.androidOpenNI.radar.TreasureRadarView;
import cc.openframeworks.siprop.androidOpenNI.robotController.RobotController;
import cc.openframeworks.siprop.androidOpenNI.robotController.RobotControllerException;


public class OFActivity extends Activity{
	
	private TimeSeriesChartView chartView = null;
	private TreasureRadarView treasureRadar = null;	
	private TextView messageView = null;

	
    private Handler chartDrawHandler = new Handler();

    private final int REPEAT_INTERVAL = TimeSeriesChartView.COUNTUP_BASE; 
    private Runnable chartDrawRunnable;
    
    private long now_counter = 0;

    private int check_index = 0;
    private final int MAX_CHECK_LENGTH = 5;
	private int[] ring_buffer = new int[MAX_CHECK_LENGTH];
	private boolean isSkip = false;
	
	private boolean isDetected = false;
	private int detectedUserId = 0;
	
	
	private OFActivity thisActivity = null;

	
	private int ctrl_counter = 0;
	private Random rnd = new Random();


	private int getTreasureCounter = 0;
	private AlphaAnimation alpha = new AlphaAnimation(1, 0.2f);
    private TranslateAnimation translate = new TranslateAnimation(Animation.ABSOLUTE, 200.0f, Animation.ABSOLUTE, 200.0f, Animation.ABSOLUTE, -620.0f, Animation.ABSOLUTE, -350.0f);
	

	@Override
    public void onCreate(Bundle savedInstanceState)
    { 
        super.onCreate(savedInstanceState);
        String packageName = getPackageName();
        getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

		try {
			MindController.getInstance().start();
		} catch (MindControllerException e) {
			e.printStackTrace();
		}
        
        chartView = (TimeSeriesChartView) findViewById(R.id.chart_view);
		treasureRadar = (TreasureRadarView) findViewById(R.id.dragon_radar_view);
		if(treasureRadar != null) {
			treasureRadar.startPingRadar();
    		treasureRadar.setCallbackActivity(thisActivity);	        			
		}
		messageView = (TextView) findViewById(R.id.treasure_message_view);        
		alpha.setDuration(1000);
		alpha.setRepeatCount(Animation.INFINITE);
		alpha.setRepeatMode(Animation.RESTART);
		
		thisActivity = this;
        
        chartDrawRunnable = new Runnable() {

			public void run() {
            	
            	Log.d("Graph", "in chartDrawRunnable:");
                if(treasureRadar == null) {
	        		treasureRadar = (TreasureRadarView) findViewById(R.id.dragon_radar_view);
	        		if(treasureRadar == null) {
	        			chartDrawHandler.postDelayed(this, REPEAT_INTERVAL);
	        			return;
	        		} else {
	        			treasureRadar.startPingRadar();
		        		treasureRadar.setCallbackActivity(thisActivity);	        			
	        			chartDrawHandler.postDelayed(this, REPEAT_INTERVAL);
	        			return;
	        		}
                }
                if(messageView == null) {
	        		messageView = (TextView) findViewById(R.id.treasure_message_view); 
	        		if(messageView == null) {
	        			chartDrawHandler.postDelayed(this, REPEAT_INTERVAL);
	        			return;
	        		}
                }

            	Log.d("Graph", "in chartDrawRunnable:");
                if(treasureRadar == null) {
	        		treasureRadar = (TreasureRadarView) findViewById(R.id.dragon_radar_view);
	        		treasureRadar.setCallbackActivity(thisActivity);
                }
        		try {
					MindController.getInstance().refresh();
				} catch (MindControllerException e) {
					e.printStackTrace();
				}
            	Log.d("Graph", "out MindController.getInstance().refresh();");
            	
            	// set Graph data
            	int eSenes = MindController.getInstance().getATTENTION_eSense();
            	chartView.ATTENTION_eSense.add(new Second(new Date(now_counter*TimeSeriesChartView.COUNTUP_BASE)), eSenes);
            	Log.d("Graph", "getATTENTION_eSense:" + MindController.getInstance().getATTENTION_eSense());
            	// delete old Graph data
            	if(now_counter >= TimeSeriesChartView.MAX_AXIS) {
            		chartView.ATTENTION_eSense.delete(new Second(new Date(
            				(now_counter - TimeSeriesChartView.MAX_AXIS)*TimeSeriesChartView.COUNTUP_BASE)));
            	}
            	now_counter++;

                chartDrawHandler.postDelayed(this, REPEAT_INTERVAL);
            	
            	// check calibrate.
                if(!isDetected || now_counter%12 == 0) {
                	Log.d("detect", "in detect check");
	            	for (int i : getUserData()) {
	                	Log.d("detect", "getUserData() i:" + i);
						int[] usrInfo = getUserInfo(i);
						if (usrInfo[0] != 0) {
		                	Log.d("detect", "usrInfo[0]:" + usrInfo[0]);
			            	try {
								RobotController.getIinstance().finishCalibrate();
			                	Log.d("detect", "getUserInfo(i)[4]:" + getUserInfo(i)[4]);
				        		treasureRadar.updateRobotDirection((float)getUserInfo(i)[4]);
								isDetected = true;
								detectedUserId = i;
			                	Log.d("detect", "is detected");
				            	break;
							} catch (RobotControllerException e) {
								e.printStackTrace();
							}
						}
					}
	            	if(!isDetected) {
		            	try {
							RobotController.getIinstance().calibrate();
			        		treasureRadar.updateRobotDirection(0.0f);
							isDetected = false;
		                	Log.d("detect", "NOT detected");
						} catch (RobotControllerException e) {
							e.printStackTrace();
						}
		            	return;
	            	}
                }
            	
                // check get treasure
                if(isGetTreasure) {
                	if(getTreasureConter >= 2) {
		            	try {
							RobotController.getIinstance().applause();
						} catch (RobotControllerException e) {
							e.printStackTrace();
						}
		        		messageView.setText("");
		            	
		            	isGetTreasure = false;
		            	getTreasureConter = 0;
                	} else {
                		getTreasureConter++;                		
                	}
                	return;
                } else {
//            		messageView.setTextSize(12.0f);
            		messageView.setTextColor(Color.RED);
            		messageView.setGravity(Gravity.CENTER);
            		messageView.setText("Please get to the treasure!!!");
            		messageView.startAnimation(alpha);
                }
                
                if(getUserInfo(detectedUserId)[4] > 0)
	        		treasureRadar.updateRobotDirection((float)getUserInfo(detectedUserId)[4]);
                
                // skip moving direction. because robot is doing action.
                if(isSkip) {
                	isSkip = false;
                	return;
                }

            	// check to move direction
            	if(10 < eSenes && eSenes < TimeSeriesChartView.LOW_FILETER) {
                	ring_buffer[check_index++] = 1;
            	} else if(TimeSeriesChartView.LOW_FILETER <= eSenes && eSenes < TimeSeriesChartView.HIGH_FILETER) {
                	ring_buffer[check_index++] = 2;
            	} else if(TimeSeriesChartView.HIGH_FILETER <= eSenes && eSenes < 100) {
                	ring_buffer[check_index++] = 3;
            	} else {
                	ring_buffer[check_index++] = 0;
            	}
				check_index %= MAX_CHECK_LENGTH;
				
				int high_num = 0, low_num = 0, mid_num = 0;
				for(int i=0; i<MAX_CHECK_LENGTH; i++) {
                	Log.d("continue", "ring_buffer[" + i + "]=" + ring_buffer[i]);
            		if(ring_buffer[i] == 1) {
            			low_num++;
            		} else if(ring_buffer[i] == 2) {
            			mid_num++;
            		} else if(ring_buffer[i] == 3) {
            			high_num++;
            		}
				}
				// try to move.
            	try {
    				if(low_num >= 2) {
            			RobotController.getIinstance().turnRight();
            			isSkip = true;
						for(int i=0; i<MAX_CHECK_LENGTH; i++) ring_buffer[i] = 0;
    				} else if(mid_num >= 2) {
            			RobotController.getIinstance().toward();
                		treasureRadar.updateGoToward(true);
            			isSkip = true;
						for(int i=0; i<MAX_CHECK_LENGTH; i++) ring_buffer[i] = 0;
    				} else if(high_num >= 2) {
            			RobotController.getIinstance().turnLeft();
            			isSkip = true;
						for(int i=0; i<MAX_CHECK_LENGTH; i++) ring_buffer[i] = 0;
            		} else {
	                	Log.d("move", "don't move");
						RobotController.getIinstance().laydownRecover();
            		}
				} catch (RobotControllerException e) {
					e.printStackTrace();
				}
                
            }
        };
        chartDrawHandler.postDelayed(chartDrawRunnable, REPEAT_INTERVAL);        
        
        ofApp = new OFAndroid(packageName,this);
    }
	
	@Override
	public void onDetachedFromWindow() {
	}
	
    @Override
    protected void onPause() {
        super.onPause();
        ofApp.pause();
    }

    @Override
	protected void onStart() {
		super.onStart();
	}

	@Override
    protected void onResume() {
        super.onResume();
        ofApp.resume();
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        OFAndroid.onKeyDown(keyCode);
        return super.onKeyDown(keyCode, event);
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0)) {
            if( OFAndroid.onBackPressed() ) return true;
            else return super.onKeyUp(keyCode, event);
        }
        
        OFAndroid.onKeyUp(keyCode);
        return super.onKeyUp(keyCode, event);
    }


	OFAndroid ofApp;
    
	
	
    // Menus
    // http://developer.android.com/guide/topics/ui/menus.html
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	// Create settings menu options from here, one by one or infalting an xml
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	// This passes the menu option string to OF
    	// you can add additional behavior from java modifying this method
    	// but keep the call to OFAndroid so OF is notified of menu events
    	if(OFAndroid.menuItemSelected(item.getItemId())){
    		
    		return true;
    	}
    	return super.onOptionsItemSelected(item);
    }
    

    @Override
    public boolean onPrepareOptionsMenu (Menu menu){
    	// This method is called every time the menu is opened
    	//  you can add or remove menu options from here
    	return  super.onPrepareOptionsMenu(menu);
    }
    
    private boolean isGetTreasure = false;
    private int getTreasureConter = 0;
	public void getTreasureCallBack() {
		Log.d("Activity", "in getTreasure");
		isGetTreasure  = true;
		getTreasureConter = 0;
	}
    
	
    // Detected human skelton
    private native int[]	getUserData();
    private native int[]	getUserInfo(int user);
 
    static {
    	System.loadLibrary("OFAndroidApp");
    }
    
}



