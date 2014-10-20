/*
 * bilinear.cpp
 *
 *  Created on: 2014/10/18
 *      Author: Chun-Hsien Lin (D03922030)
 * Description: Using bilinear interpolation to zoom and rotate the image file.
 *
 */
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "opencv.hpp"
#include "highgui.h"
#include "cvaux.hpp"

using namespace std;
using namespace cv;

int main(int argc, const char * argv[])
{
	IplImage * image;
    IplImage * newImg;
    IplImage * rotateImg;
    int height, width, step, channels, depth;
    int new_height, new_width, new_step;
    int rotate_height, rotate_width, rotate_step;
    uchar *data;
    uchar *new_data;
    uchar *rotate_data;
    int i, j;
    CvScalar s, new_s;
	double col, row;
	CvScalar q11, q12, q21, q22;

    image = cvLoadImage( argv[1], 1);

    if (argc != 2 || !image)
    {
        printf( "Could not load image file: %s\n" , argv[1]);
        exit(0);
    }
    // get the original image data
    height = image->height;
    width = image->width;
    step = image->widthStep;
    depth = image->depth;
    channels = image->nChannels;
    data = ( uchar *)image->imageData;

    printf( "Processing a %dx%d image with %d channels %d depth\n" , height, width, channels, depth);
    // create a new zoom image buffer
    newImg = cvCreateImage(cvSize(width*1.5,height*1.5), IPL_DEPTH_8U, channels);
    new_data = (uchar *)newImg->imageData;
    new_height = newImg->height;
    new_width = newImg->width;
    new_step = newImg->widthStep/sizeof(uchar);
    printf( "Creating a new %dx%d image with %d channels that using bilinear interpolation.\n" , new_height, new_width, channels);
    // Using bilinear interpolation
    for (i = 1; i < new_height - 2; i++)
    	for (j = 1; j < new_width - 2; j++)
    	{
    		col = (float)i * height /new_height;
    		row = (float)j * width / new_width;
			s = cvGet2D(image, round(col), round(row)); 
//			printf("(i, j)=(%d, %d), (col, row)=(%d, %d), B=%f, G=%f, R=%f\n", i, j, round(col), round(row), s.val[0], s.val[1], s.val[2]);
    		if ((col >= 1) && (row >= 1)) {
    			q11 = cvGet2D(image, round(col)-1, round(row)-1);
//				printf("Q11(col, row)=(%d, %d), B=%f, G=%f, R=%f\n", round(col), round(row), q11.val[0], q11.val[1], q11.val[2]);
    		};
			if ((col <= height - 1) && (row <= width - 1)) {
				q22 = cvGet2D(image, round(col) + 1, round(row) + 1);
//				printf("Q22(col, row)=(%d, %d), B=%f, G=%f, R=%f\n", round(col), round(row), q22.val[0], q22.val[1], q22.val[2]);
			};
			if ((col <= height) && (row <= width - 1)) {
				q21 = cvGet2D(image, round(col) + 1, round(row) - 1);
//				printf("Q21(col, row)=(%d, %d), B=%f, G=%f, R=%f\n", round(col), round(row), q21.val[0], q21.val[1], q21.val[2]);
			};
			if ((col <= height) && (row >= 1)) {
				q12 = cvGet2D(image, round(col) - 1, round(row) + 1);
//				printf("Q12(col, row)=(%d, %d), B=%f, G=%f, R=%f\n", round(col), round(row), q12.val[0], q12.val[1], q12.val[2]);
			};
			new_s.val[0] = (q11.val[0] * (row + 1 - i) * (col + 1 - j)) + (q21.val[0] * (i - (row - 1)*(col + 1 - j))) + (q12.val[0] * (row + 1 - i)*(j - (col - 1))) + (q22.val[0] * (i - (row - 1))*(j - (col - 1)));
			new_s.val[1] = (q11.val[1] * (row + 1 - i) * (col + 1 - j)) + (q21.val[1] * (i - (row - 1)*(col + 1 - j))) + (q12.val[1] * (row + 1 - i)*(j - (col - 1))) + (q22.val[1] * (i - (row - 1))*(j - (col - 1)));
			new_s.val[2] = (q11.val[2] * (row + 1 - i) * (col + 1 - j)) + (q21.val[2] * (i - (row - 1)*(col + 1 - j))) + (q12.val[2] * (row + 1 - i)*(j - (col - 1))) + (q22.val[2] * (i - (row - 1))*(j - (col - 1)));
//    		printf("(i, j)=(%d, %d), B=%f, G=%f, R=%f\n", myround(col) , myround(row), s.val[0],s.val[1],s.val[2]);
//    		new_s.val[0] = s.val[0]; // Blue
//    		new_s.val[1] = s.val[1]; // Green
//    		new_s.val[2] = s.val[2]; // Red
    		cvSet2D(newImg, i, j, new_s);
    	}
    // Create a image buffer for rotate 45 degree
    rotateImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, channels);
    rotate_data = (uchar *)rotateImg->imageData;
    rotate_height = rotateImg->height;
    rotate_width = rotateImg->width;
    rotate_step = rotateImg->widthStep/sizeof(uchar);
    printf( "Creating a rotate %dx%d image with %d channels\n" , rotate_height, rotate_width, channels);
    // Using nearest-neighbor interpolation to rotate the image
	col = 0.0;
	row = 0.0;
    for (i = 0; i < height; i++)
    	for (j = 0; j < width; j++)
    	{
    		col = (double)i*cos(M_PI/4) - (double)j*sin(M_PI/4) + height/2;
    		row = (double)i*sin(M_PI/4) + (double)j*cos(M_PI/4) - width/4;
    		if (round(row) >= width) {
    			row = (double)width - 1;
    		}
    		if (round(col) >= height) {
    			col = (double)height - 1;
    		}
    		if (row <= 0) {
    			row = 0;
    		}
    		if (col <= 0) {
    			col = 0;
    		}
    		s = cvGet2D(image, round(col), round(row));
    		new_s.val[0] = s.val[0]; // Blue
    		new_s.val[1] = s.val[1]; // Green
    		new_s.val[2] = s.val[2]; // Red
    		if (row >= width-1) {
        		new_s.val[0] = 111; // Blue
        		new_s.val[1] = 111; // Green
        		new_s.val[2] = 111; // Red
    		}
    		if (col >= height-1) {
           		new_s.val[0] = 111; // Blue
            	new_s.val[1] = 111; // Green
            	new_s.val[2] = 111; // Red
    		}
    		if (row <= 0) {
           		new_s.val[0] = 111; // Blue
            	new_s.val[1] = 111; // Green
            	new_s.val[2] = 111; // Red
    		}
    		if (col <= 0) {
           		new_s.val[0] = 111; // Blue
            	new_s.val[1] = 111; // Green
            	new_s.val[2] = 111; // Red
    		}
//    		printf("col=%lf, row=%lf\n", col, row);
    		cvSet2D(rotateImg, i, j, new_s);
    	}

    // create a window
    cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );
    cvMoveWindow( "mainWin", 100, 50);
    cvNamedWindow( "zoomWin", CV_WINDOW_AUTOSIZE );
    cvMoveWindow( "zoomWin", 300, 50);
    cvNamedWindow( "rotateWin", CV_WINDOW_AUTOSIZE );
    cvMoveWindow( "rotateWin", 500, 50);

    // show the image
    cvShowImage( "mainWin", image);
    cvShowImage( "zoomWin", newImg);
    cvShowImage( "rotateWin", rotateImg);
    // wait for a key
    cvWaitKey(0);

    // release the image
    cvReleaseImage(&image);
    cvReleaseImage(&newImg);
    cvReleaseImage(&rotateImg);
    return 0;
}

