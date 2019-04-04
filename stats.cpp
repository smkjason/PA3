
#include "stats.h"

stats::stats(PNG & im){
 //testing
	double valueX = 0.;
	double valueY = 0.;
	double Sat = 0.;
	double Lum = 0.;
	int binnumber = 0;
	lastIndex_x = im.width() - 1;
	lastIndex_y = im.height() - 1;
	for(unsigned int t = 0; t < im.width(); t++){
		sumHueX.push_back(vector<double>(im.height(), 0));
		sumHueY.push_back(vector<double>(im.height(), 0));
		sumSat.push_back(vector<double>(im.height(), 0));
		sumLum.push_back(vector<double>(im.height(), 0));
		hist.push_back(vector<vector<int>>(im.height(), vector<int>(36,0)));
	}
	for(unsigned int i = 0; i < im.width(); i++){
		for(unsigned int j = 0; j < im.height(); j++){
			// add the value at [i][j] 
			valueX = cos(im.getPixel(i,j)->h * PI/180);
			valueY = sin(im.getPixel(i,j)->h * PI/180);
			Sat    = im.getPixel(i,j)->s;
			Lum    = im.getPixel(i,j)->l;
			// add the squares around and minus the overlap 
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
			//printf("%d, %d, %d, %d", valueX, valueY, Sat, Lum);
			sumHueX[i][j] = valueX;
			sumHueY[i][j] = valueY;
			sumSat[i][j]  = Sat;
			sumLum[i][j]  = Lum;
			// increment the histogram 
			binnumber = (im.getPixel(i,j)->h) / 10;
			if(binnumber == 36)
			    binnumber = 0;
			hist[i][j][binnumber] += 1;
			for(int k = 0; k < 36; k++){
				if(i > 0)
					hist[i][j][k] += hist[i-1][j][k];
				if(j > 0)
					hist[i][j][k] += hist[i][j-1][k];
				if(i > 0 && j > 0)
					hist[i][j][k] -= hist[i-1][j-1][k];
			}
		}
	}

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
	long length = lr.first - ul.first + 1;
	long height = lr.second - ul.second + 1;
	
	return height * length;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
	double hue = 0.;
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
	avgX = avgX / (double)rectArea(ul, lr);
	avgY = avgY / (double)rectArea(ul, lr);
	avgS = avgS / (double)rectArea(ul, lr);
	avgL = avgL / (double)rectArea(ul, lr);
	hue  = atan2(avgY, avgX) * 180/PI;
	if(hue < 0)
		hue += 360;
return HSLAPixel(hue, avgS, avgL, 1.0);
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
//printf("I'm in buildHist\n");
	vector<int> histogram(36, 0);
    //NO WRAP
    if(ul.first < lr.first && ul.second < lr.second){
	for(int k = 0; k < 36; k++){
		histogram[k] = hist[lr.first][lr.second][k];
		// subtract the rectangle on its left
		if(ul.first > 0){
		histogram[k] -= hist[ul.first-1][lr.second][k];
		}
		// subtract the rectangle on the top
		if(ul.second > 0){
		histogram[k] -= hist[lr.first][ul.second-1][k];
		}
		// add the upper left corner rectangle
		if(ul.first > 0 && ul.second > 0){
		histogram[k] += hist[ul.first-1][ul.second-1][k];
		}
	}
//	printf("NO WRAP CASE IS FINE\n\n");
	return histogram;
    }
    //WRAP CORNERS
    else if(ul.first > lr.first && ul.second > lr.second){
	for(int k = 0; k < 36; k++){
//	    printf("ul.first: %d ul.second: %d\n", ul.first, ul.second);
	    //add the top left corner
	    histogram[k] += hist[lr.first][lr.second][k];
	    
	    //add the top right corner
	    histogram[k] += hist[lastIndex_x][lr.second][k];
	    if(ul.first > 0)
	    histogram[k] -= hist[ul.first-1][lr.second][k];//left

	    //add the bottom left
	    histogram[k] += hist[ul.first][lastIndex_y][k];
	    histogram[k] -= hist[lr.first][ul.second-1][k];

	    //add the bottom right
	    histogram[k] += hist[lastIndex_x][lastIndex_y][k];
	    histogram[k] -= hist[lastIndex_x][ul.second-1][k];
	    histogram[k] -= hist[ul.first-1][lastIndex_y][k];
	    histogram[k] += hist[ul.first-1][ul.second-1][k];
   	}
//	printf("WRAP CORNERS IS FINE\n");
	return histogram;
    }
    //WRAPPING THE SIDE
    else if(ul.first > lr.first){
	for(int k = 0; k < 36; k++){
	    //the right
	    histogram[k] += hist[lastIndex_x][lr.second][k];
	    //the left
	    histogram[k] += hist[lr.first][lr.second][k];
	    // this is subtract the left  part for the right
	    if(ul.first > 0){
	        //the right
                histogram[k] -= hist[ul.first-1][lr.second][k];
	    }
	    // for the right subtract the top
	    if(ul.second > 0){
 		histogram[k] -= hist[lastIndex_x][ul.second-1][k];
	       //subtract the top part for the wrap on the left
		 histogram[k] -= hist[lr.first][ul.second-1][k];
	    }
	    //add the corner
	    if(ul.first > 0 && ul.second > 0){
	        histogram[k] += hist[ul.first-1][ul.second-1][k];
            }
	}
//	printf("WRAP SIDE IS FINE\n");
	return histogram;
    }

    //WRAP THE TOP
    else if(lr.second < ul.second){
	for(int k = 0; k < 36; k++){
	    //bottom
    	    histogram[k] += hist[lr.first][lastIndex_y][k];
	    histogram[k] -= hist[lr.first][ul.second-1][k];
	    histogram[k] -= hist[ul.first-1][lastIndex_y][k];
	    histogram[k] += hist[ul.first-1][ul.second-1][k];

	    //top
	    histogram[k] += hist[lr.first][lr.second][k];
	    histogram[k] -= hist[ul.first-1][lr.second][k];          	
	}
//	printf("WRAP TOP IS FINE\n");
	return histogram;
    }
//	printf("WARNING: THIS SHOULD BE NEVER PRINTED\n");
	return histogram;
}
// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

	double entropy = 0.;

//	 your code here 

	for (int i = 0; i < 36; i++) {
		if (distn[i] > 0 ) 
			entropy += ((double) distn[i]/(double) area) 
				* log2((double) distn[i]/(double) area);
	}

	return  -1 * entropy;
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

	vector<int> histogram = buildHist(ul, lr);
	double en = entropy(histogram, rectArea(ul, lr)); 
	/* your code here */
	return en;
}
