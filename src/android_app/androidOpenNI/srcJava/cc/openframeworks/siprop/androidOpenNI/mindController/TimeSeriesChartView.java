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

import java.text.SimpleDateFormat;

import org.afree.chart.ChartFactory;
import org.afree.chart.AFreeChart;
import org.afree.chart.axis.DateAxis;
import org.afree.chart.axis.ValueAxis;
import org.afree.chart.plot.Marker;
import org.afree.chart.plot.ValueMarker;
import org.afree.chart.plot.XYPlot;
import org.afree.chart.renderer.xy.XYItemRenderer;
import org.afree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.afree.data.time.Month;
import org.afree.data.time.Second;
import org.afree.data.time.TimeSeries;
import org.afree.data.time.TimeSeriesCollection;
import org.afree.data.xy.XYDataset;
import org.afree.graphics.SolidColor;
import org.afree.ui.RectangleInsets;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;

/**
 * TimeSeriesChartView
 */
public class TimeSeriesChartView extends BasicChartView {

	
	public static TimeSeries ATTENTION_eSense = null;
    public static int COUNTUP_BASE = 5000;
    public static int MAX_AXIS = 5;
    public static int HIGH_FILETER = 75;
    public static int LOW_FILETER = 40;
	
    /**
     * constructor
     * @param context
     */
    public TimeSeriesChartView(Context context) {
        super(context);

        final AFreeChart chart = createChart(createDataset());

        setChart(chart);
    }
    public TimeSeriesChartView(Context context, AttributeSet attrs) {
        super(context, attrs);

        final AFreeChart chart = createChart(createDataset());

        setChart(chart);
    }


    /**
     * Creates a chart.
     *
     * @param dataset  a dataset.
     *
     * @return A chart.
     */
    private static AFreeChart createChart(XYDataset dataset) {

    	
    	//http://java6.blog117.fc2.com/blog-category-13.html
    	//http://www.jfree.org/jfreechart/api/javadoc/index.html
    	//
    	
        AFreeChart chart = ChartFactory.createTimeSeriesChart(
            "your brain wave",  // title
            "Time",             // x-axis label
            "Brain wave Hz",   // y-axis label
            dataset,            // data
            true,               // create legend?
            false,               // generate tooltips?
            false               // generate URLs?
        );

        chart.setBackgroundPaintType(new SolidColor(Color.BLACK));

        XYPlot plot = (XYPlot) chart.getPlot();
        plot.setBackgroundPaintType(new SolidColor(Color.BLACK));
        plot.setDomainGridlinesVisible(false);
        plot.setRangeGridlinePaintType(new SolidColor(Color.LTGRAY));
        plot.setAxisOffset(new RectangleInsets(5.0, 5.0, 5.0, 5.0));
        plot.setDomainCrosshairVisible(true);
        plot.setRangeCrosshairVisible(true);
        
        //30-80 4,5,6,7
        Marker markerLow = new ValueMarker(LOW_FILETER);
        markerLow.setPaintType(new SolidColor(Color.BLUE));
        markerLow.setStroke(6.0f);
        plot.addRangeMarker(markerLow);

        Marker markerHigh = new ValueMarker(HIGH_FILETER);
        markerHigh.setPaintType(new SolidColor(Color.BLUE));
        markerHigh.setStroke(6.0f);
        plot.addRangeMarker(markerHigh);
        

        
        //
        ValueAxis valueAxis = plot.getRangeAxis();
        valueAxis.setAutoRange(false);
        valueAxis.setRange(10.0, 100.0);

        XYItemRenderer r = plot.getRenderer();
        if (r instanceof XYLineAndShapeRenderer) {
            XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer) r;
            renderer.setBaseShapesVisible(true);
            renderer.setBaseShapesFilled(true);
            renderer.setDrawSeriesLineAsPath(true);
            renderer.setSeriesStroke(0, 4.0f);
        }
        
        DateAxis axis = (DateAxis) plot.getDomainAxis();
        axis.setDateFormatOverride(new SimpleDateFormat("mm:ss"));

        return chart;

    }

    /**
     * Creates a dataset, consisting of two series of monthly data.
     *
     * @return The dataset.
     */
    private static XYDataset createDataset() {

        ATTENTION_eSense = new TimeSeries("ATTENTION");
//        for(int i=0; i<MAX_AXIS; i++) {
//            ATTENTION_eSense.add(new Second(i*COUNTUP_BASE, 0, 0, 1, 1, 1900), 0);
//        }

        TimeSeriesCollection dataset = new TimeSeriesCollection();
        dataset.addSeries(ATTENTION_eSense);

        return dataset;
    }
    
    
}