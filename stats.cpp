
#include "stats.h"

stats::stats(PNG & im){

    double valueX = 0.;
    double valueY = 0.;
    double Sat = 0.;
    double Lum = 0.;
    vector<int> histogram;
    histogram.resize(36);

    for(int i = 0; i < im->width_; i++){
        for(int j = 0; j < im->height_; j++){
 	    /* add the value at [i][j] */
	    valueX = cos(im.getPixel(i,j)->h * PI/180);
	    valueY = sin(im.getPixel(i,j)->h * PI/180);
	    Sat    = im.getPixel(i,j)->s;
	    Lum    = im.getPixel(i,j)->l;
            /* add the squares around and minus the overlap */
            if(i > 0){
	        valueX += sumHueX[i-1][j];
	        valueY += sumHueY[i-1][j];
	        Sat    += sumSat[i-1][j];
	        Lum    += sumLum[i-1][j];
	     }
            if(j > 0){
                valueX += sumHueX[i][j-1];
	        valueY += sumHueY[i][j-1];
	        Sat    += sumSat[i][j-1];
	        Lum    += sumLum[i][j-1];
	    }
            if(i > 0 && j > 0){
                valueX -= sumHueX[i-1][j-1];
	        valueY -= sumHueY[i-1][j-1];
	        Sat    -= sumSat[i-1][j-1];
	        Lum    -= sumLum[i-1][j-1];
   	    }
	    sumHueX[i][j] = valueX;
	    sumHueY[i][j] = valueY;
	    sumSat[i][j]  = Sat;
	    sumLum[i][j]  = Lum;
	    /* increment the histogram */
	    hist[i][j][im.getPixel(i,j)->h/10] += 1;
	    for(int k = 0; k < 36; k++){
		if(i > 0);
		    hist[i][j][k] += hist[i-1][j][k];
		if(j > 0);
		    hist[i][j][k] += hist[i][j-1][k];
		if(i > 0 && j > 0);
		    hist[i][j][k] -= hist[i-1][j-1][k];
 	    }
        }
     }

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
  long length = lr.first - ul.first;
  long height = lr.second - ul.second;

  return height*length;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
    HSLAPixel * p;
    
    double avgX = 0.;
    double avgY = 0.;
    double avgS = 0.;
    double avgL = 0.;
    avgX = sumHueX[lr.first][lr.second];
    avgY = sumHueY[lr.first][lr.second];
    avgS = sumSat[lr.first][lr.second];
    avgL = sumLum[lr.first][lr.second];
    if(ul.first > 0){
	avgX -= sumHueX[ul.first-1][lr.second];
	avgY -= sumHueY[ul.first-1][lr.second];
	avgS -= sumSat[ul.first-1][lr.second];
 	avgL -= sumLum[ul.first-1][lr.second];
    }
    if(ul.second > 0){
	avgX -= sumHueX[lr.first][ul.second-1];
	avgY -= sumHueY[lr.first][ul.second-1];
	avgS -= sumSat[lr.first][ul.second-1];
	avgL -= sumLum[lr.first][ul.second-1];

    }
    if(ul.first > 0 && ul.second > 0){
	avgX += sumHueX[ul.first-1][ul.second-1];
	avgY += sumHueY[ul.first-1][ul.second-1];
	avgS += sumSat[ul.first-1][ul.second-1];
	avgL += sumLum[ul.first-1][ul.second-1];
    }
    avgX = avgX / rectArea(ul, lr);
    avgY = avgY / rectArea(ul, lr);
    avgS = avgS / rectArea(ul, lr);
    avgL = avgL / rectArea(ul, lr);
    p->h = atan2(avgX, avgY);

}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

    vector<int> histogram;
    histogram.resize(36);
    for(int k = 0; k < 36; k++){
	histogram[k] = 0;
	histogram[k] += hist[lr.first][lr.second][k];
	// subtract the rectangle on its left
	histogram[k] -= hist[ul.first-1][lr.second][k];
	// subtract the rectangle on the top
	histogram[k] -= hist[lr.first][ul.second-1];
	// add the upper left corner rectangle
	histogram[k] += hist[ul.first-1][ul.second-1];
    }
    return histogram;
}
// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

double en = 0.;
/* your code here */
return en;
}
