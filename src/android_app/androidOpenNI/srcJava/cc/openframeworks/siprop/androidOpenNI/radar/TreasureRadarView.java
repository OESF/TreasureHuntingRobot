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
package cc.openframeworks.siprop.androidOpenNI.radar;

import java.util.Random;

import cc.openframeworks.siprop.androidOpenNI.OFActivity;
import cc.openframeworks.siprop.androidOpenNI.R;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Point;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageView;

public class TreasureRadarView extends ImageView {
	
	private Point treasurePoint = null;
    private UpdateHandler radarDrawHandler = new UpdateHandler(this);
    private final int REPEAT_INTERVAL = 200; 
	

    public TreasureRadarView(Context context) {
		super(context);
//		startPingRader();
	}
    public TreasureRadarView(Context context, AttributeSet attrs) {
		super(context, attrs);
//		startPingRader();
	}
    public TreasureRadarView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
//		startPingRader();
	}
    
    private Bitmap hand = null;
    private Bitmap robot_marker = null;
    private Bitmap treasure = null;
    private Bitmap congra = null;
    private Bitmap[] handList = new Bitmap[20];
    private int handPosition = 0;
    
    private int offsetHight = 60;
    private int offsetWidth = 60;

    private static int radius = 90;
    
	private Random rnd = new Random();
    private Point robotPoint = new Point(100, 100);
	private float robotDirection = 0.0f;
	private boolean isChengedRobotDirection = false;
	private Bitmap robotBitmap = null;
	private boolean isMoveRobot = false;
	private Point[] walkingAnimation = new Point[walkingAnimationSize];
	private int walkingAnimationCurrentIndex = 0;
	private static int walkingAnimationSize = 80;
    
	private OFActivity callbackActivity = null;
	
    
	
	public void setCallbackActivity(OFActivity activity) {
		callbackActivity = activity;
	}
    
    public void startPingRadar() {
    	Resources r = getResources();
    	hand = BitmapFactory.decodeResource(r, R.drawable.hand);
    	robot_marker = BitmapFactory.decodeResource(r, R.drawable.robot_marker);
    	treasure = BitmapFactory.decodeResource(r, R.drawable.treasure);
    	congra = BitmapFactory.decodeResource(r, R.drawable.treasure_chest);
    	
    	{
			Matrix matrix = new Matrix();
	        matrix.postRotate(robotDirection);
	        robotBitmap = Bitmap.createBitmap(robot_marker, 0, 0, robot_marker.getWidth(), robot_marker.getHeight(), matrix, true);
	        isChengedRobotDirection = false;
    	}
    	
    	for(int i = 0; i < handList.length; i++) {
    		Log.d("startPingRadar", "handList i:" + i);
    		Log.d("startPingRadar", "handList Rotate:" + 18 * i);
	        Matrix matrix = new Matrix();
	        matrix.postRotate(18 * i);
	        Bitmap bitmap = Bitmap.createBitmap(hand, 0, 0, hand.getWidth(), hand.getHeight(), matrix, true);
	        handList[i] = bitmap;
    	}
    	
    	resetTreasure();
    	update();
        
    }
    public void update() {
    	handPosition++;
    	if(handPosition >= handList.length) {
    		handPosition = 0;
    	}
        // Re-call onDraw();
        invalidate();
        radarDrawHandler.sleep(REPEAT_INTERVAL);
    }
    public void updateRobotDirection(float direction) {
    	robotDirection = direction;
        isChengedRobotDirection = true;
    }
    public void updateGoToward(boolean isMove) {
    	isMoveRobot = isMove;
    }
    
    
    @Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
//		Log.d("onDraw", "handPosition:" + handPosition);
		if(handList[handList.length-1] == null)
			return;
		
        canvas.drawBitmap(handList[handPosition], -(handList[handPosition].getWidth()-hand.getWidth())/2 + offsetWidth, -(handList[handPosition].getHeight()-hand.getHeight())/2 + offsetHight, null);
        
        if(isMoveRobot) {
    		Log.d("onDraw", "in isMoveRobot");
	        isMoveRobot = false;
	        // Calc treasurePoint by walking distance. 
    		Log.d("onDraw", "old treasurePoint.x:" + treasurePoint.x + " old treasurePoint.y:" + treasurePoint.y);
	        int oldX = treasurePoint.x;
	        int oldY = treasurePoint.y;
	        treasurePoint.x = treasurePoint.x - (int)(Math.cos(Math.toRadians(robotDirection)) * 30);
	        treasurePoint.y = treasurePoint.y - (int)(Math.sin(Math.toRadians(robotDirection)) * 30);
    		Log.d("onDraw", "new treasurePoint.x:" + treasurePoint.x + " new treasurePoint.y:" + treasurePoint.y);

    		// Make animation bitmap
	        int distX = treasurePoint.x - oldX;
	        int distY = treasurePoint.y - oldY;
	        double rateX = (double)distX / (double)walkingAnimationSize;
	        double rateY = (double)distY / (double)walkingAnimationSize;
	        walkingAnimation[0] = new Point(oldX, oldY);
        	Log.d("Make anime", "distX:" + distX + " distY:" + distY);
        	Log.d("Make anime", "rateX:" + rateX + " rateY:" + rateY);
	        for(int i=1; i<walkingAnimationSize; i++) {
	        	Point p = new Point(oldX + (int)(rateX*i), oldY + (int)(rateY*i));
	        	if(checkInRadar(p)) {
		        	walkingAnimation[i] = p;
	        	} else {
		        	walkingAnimation[i] = new Point(walkingAnimation[i-1].x, walkingAnimation[i-1].y);
	        	}
	        }
        	walkingAnimationCurrentIndex = 1;
        	
	        // Out of radar range.
	        if(checkInRadar(treasurePoint)) {
	        	checkGetTreasure();
	        } else {
	        	resetTreasure();
	        }
        }
        if(walkingAnimationCurrentIndex > 0) {
        	boolean getTre = checkGetTreasure(walkingAnimation[walkingAnimationCurrentIndex]);
        	if(getTre) {
                canvas.drawBitmap(congra, 0, 0, null);
        	} else {
                canvas.drawBitmap(treasure, walkingAnimation[walkingAnimationCurrentIndex].x + offsetWidth, walkingAnimation[walkingAnimationCurrentIndex].y  + offsetHight, null);
        	}
            walkingAnimationCurrentIndex++;
            if(walkingAnimationCurrentIndex+1 >= walkingAnimationSize) {
            	walkingAnimationCurrentIndex = 0;
            }
            if(getTre)
            	return;
        } else {
            canvas.drawBitmap(treasure, treasurePoint.x + offsetWidth, treasurePoint.y + offsetHight, null);
        }
        
        // draw robot marker.
        if(isChengedRobotDirection) {
    		Matrix matrix = new Matrix();
            matrix.postRotate(robotDirection);
            robotBitmap = Bitmap.createBitmap(robot_marker, 0, 0, robot_marker.getWidth(), robot_marker.getHeight(), matrix, true);
            isChengedRobotDirection = false;
        }
        canvas.drawBitmap(robotBitmap, -(robotBitmap.getWidth()-robot_marker.getWidth())/2 + offsetWidth, -(robotBitmap.getHeight()-robot_marker.getHeight())/2 + offsetHight, null);

	}
    
    
    private boolean checkInRadar(Point point) {
    	if(calcRadius(point) >= radius) {
    		return false;
    	} else {
    		return true;
    	}
    }
    private double calcRadius(Point point) {
    	Point p = new Point(point.x, point.y);
    	p.x = p.x - radius;
    	p.y = p.y - radius;
		Log.d("checkInRadar", "sqrt:" + Math.sqrt(p.x*p.x + p.y*p.y));
    	return Math.sqrt(p.x*p.x + p.y*p.y);
    }
    
    private boolean checkGetTreasure(Point p) {
		Log.d("checkGetTreasure", "start");
    	if(calcRadius(p) < 40) {
    		Log.d("checkGetTreasure", "get Treasure!!!");
    		callbackActivity.getTreasureCallBack();
        	resetTreasure();
        	return true;
    	}
    	return false;
    }
    private boolean checkGetTreasure() {
    	return checkGetTreasure(treasurePoint);
    }
    
    private void resetTreasure() {
		Log.d("resetTreasure", "start");
    	int x = 0;
    	int y = 0;
    	double r = 0.0;
    	boolean loopOn = false;
    	do {
    		loopOn = false;
	    	x = rnd.nextInt((radius+10)*2);
	    	y = rnd.nextInt((radius+10)*2);
	    	Point p = new Point(x, y);
	        if(!checkInRadar(p)) {
	    		loopOn = true;
	    	}
	    	if(calcRadius(p) < 40) {
	    		loopOn = true;
	    	}
    	} while(loopOn);
    	
		Log.d("resetTreasure", "new x:" + x + " y:" + y);
    	setTreasure(new Point(x, y));
    }

    public void setTreasure(Point point) {
    	treasurePoint = point;
    }
    
    public Point getCurrentTreasurePoint() {
    	return treasurePoint;
    }
    public float getCurrentRobotDirection() {
    	return robotDirection;
    }
    public Point getRobotPoint() {
    	return robotPoint;
    }
    
    

    public class UpdateHandler extends Handler {
            private TreasureRadarView updateTarget = null;

            public UpdateHandler(TreasureRadarView updateTarget) {
                    this.updateTarget = updateTarget;
            }

            @Override
            public void handleMessage(Message msg) {
            	updateTarget.update();
            }

            public void sleep(long sleepTime) {
                    removeMessages(0);
                    sendMessageDelayed(obtainMessage(0), sleepTime);
            }
    }
}
