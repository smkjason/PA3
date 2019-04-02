
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 
	/*
	   pair<int,int> ctr;
	   int dim;
	   HSLAPixel a;

	   ctr.first = imIn.width()/2 - 1;
	   ctr.second = imIn.height()/2 - 1;

	   Node * centeredImage = new Node(ctr, dim, a); 
	 *//* This constructor grabs the 2^k x 2^k sub-image centered */
	/* in imIn and uses it to build a quadtree. It may assume  */
	/* that imIn is large enough to contain an image of that size. */
	/* your code here */
	
	//CHECK k for base case
	if(k == 1){
		Node * nw = new Node(make_pair(0,0), 0, 	




	}
	int ctrX = imIn->width()/2;
	int ul = ctrX - pow(2,k)/2;
	int lr = ul + pow(2,k-1) - 1;
	pair<int, int> ul_middle(ul,ul);
	pair<int, int> lr_middle(lr, lr);
	
	//make the subPNG 
	PNG * parent = makePNG(im, ul_middle, k-1);
	
	//Pointer??
	stats statsInfo = new stats(*root);
	
	*root = buildTree(parent, k);
}

int toqutree::size() {
	/* your code here */
}

//@param k : dimension of im
toqutree::Node * toqutree::buildTree(PNG * im, int k) {

	//the splitting point that has the least entropy
	pair<int, int> Split_Point;//AKA center

	//the square to traverse through to find minimum entropy
	int ul = pow(2,k)/4;
	int lr = ul + pow(2,k-1) - 1;
	pair<int, int> ul_middle(ul,ul);
	pair<int, int> lr_middle(lr, lr);
	
	//Pointer??
	stats statsInfo = new stats(*im);
	
	//Find the min ENTROPY within ul_middle
	double minEntropy = getAvg(statsInfo, ul_middle, k-1);  
	double check_Entropy;
	pair<int,int> check_P = ul_middle;
	Split_Point = check_P;

	for(int x = ul_middle; x < pow(2,k-1); x++){
		for(int y = ul; y < pow(2,k-1); y++){
			check_P.first = x;
			check_P.second = y;
			check_Entropy = getAvg(statsInfo, check_P, k-1);
			if(check_Entropy < minEntropy){
				minEntropy = check_Entropy;
				Split_Point = check_P;
			} 
		}
	}

	//MAKE THE PNG FOR THE CHILDREN
	
	HSLAPixel a = statsInfo.getAvg(make_pair(0,0), make_pair(pow(2,k)-1, pow(2,k)-1));
	//Split_Point is done calculating
	Node * node = new Node(Split_Point, k, a); 

	return node;
	// Note that you will want to practice careful memory use
	// In this function. We pass the dynamically allocated image
	// via pointer so that it may be released after it is used .
	// similarly, at each level of the tree you will want to 
	// declare a dynamically allocated stats object, and free it
	// once you've used it to choose a split point, and calculate
	// an average.

}
//having the average entropy of quadrants
//find the ul and lr of each quadrant is done here
// @param im  : the original image
// @param SE_ul : possible splitting point
// @param k   : dimention of the middle square you are traversing through AKA the dim of quadrant
double getAvg(stats Stats, pair<int,int> SE_ul, int k){
	//call getEntropy for each Quadrant 
	pair<int,int> SE_lr, SW_ul, SW_lr, NE_ul, NE_lr, NW_ul, NW_lr;
	int index = pow(2,k) - 1;
	int dim = pow(2,k+1);
	SE_lr.first = SE_ul.first + index;
	SE_lr.second = SE_lr.second + index;

	//mod is for if the point is off the picture
	SW_ul.first = (SE_ul.first + index + 1) % dim;
	SW_ul.second = SE_ul.second;

	NE_ul.first = SE_ul.first;
	NE_ul.second = (SE_ul.second + index + 1) % dim;

	NW_ul.first = SW_ul.first;
	NW_ul.second = NE_ul.second;

	//assuming the ul is calculated correctly
	SW_lr.first = (SW_ul.first + index) % dim;
	SW_lr.second = (SW_ul.second + index) % dim;

	NE_lr.first = (NE_ul.first + index) % dim;
	NE_lr.second = (NE_ul.second + index) % dim;

	NW_lr.first = (NW_ul.first + index) % dim;
	NW_lr.second = (NW_ul.second + index) % dim;

	//check the notation passing by reference etc for Stats
	double Average = getEntropy(Stats, SE_ul, SE_lr, k) + getEntropy(Stats, SW_ul, SW_lr, k)
		+ getEntropy(Stats, NE_ul, NE_lr, k) + getEntropy(Stats, NW_ul, NW_lr, k);
	return Average/4;
}
//get entropy of given quadrant *HANDLE WRAPPING HERE*
// @param s : Stats of original image
// @param ul, lr: ul, lr of the quadrant you are calculating entropy for
// @param k : the dimension of the quadrant where the quadrant is 2^k x 2^k
double getEntropy(stats s, pair<int,int> ul, pair<int,int> lr, int k){

	int last_index = pow(2,k+1) - 1;
	int index = pow(2,k) - 1;
	//NO WRAPPING
	if(ul.first < lr.first && ul.second < lr.second){
		return s.entropy(ul, lr);
	}
	//wrapping the corners
	else if(ul.first > lr.first && ul.second > lr.second){
		//bottomright + bottomleft + topright + topleft
		vector<int> BR = s.buildHist(ul, make_pair(last_index, last_index));
		vector<int> BL = s.buildHist(make_pair(0, ul.second), make_pair(lr.first, last_index));
		vector<int> TR = s.buildHist(make_pair(ul.first,0), make_pair(last_index, lr.second));
		vector<int> TL = s.buildHist(make_pair(0,0), lr);
		for(int a = 0; a < 36; a++){
			BR[a] += BL[a] + TR[a] + TL[a];
		}
		return s.entropy(BR, pow(2,k+1));
	} 
	//WRAPPING AROUND THE SIDE
	else if(ul.first > lr.first){
		//Left rect + right rect
		vector<int> Left_Rect = s.buildHist(ul, make_pair(last_index, ul.second+index));
		//long Left_Area = s.rectArea(ul, make_pair(last_index, ul.second + index));	
		vector<int> Right_Rect = s.buildHist(make_pair(0,ul.second), lr);
		//long Right_Area = s.rectArea(make_pair(0, ul.second), lr);
		for(int a = 0; a < 36; a++){
			Left_Rect[a] = Left_Rect[a] + Right_Rect[a];
		}	
		return s.entropy(Left_Rect, pow(2,k+1));
	}
	//WRAPPING AROUND THE TOP
	else if(lr.second < ul.second){
		//bottom + top
		vector<int> bottom = s.buildHist(ul, make_pair(lr.first, last_index));
		//long bottom_area = s.rectArea(ul, make_pair(lr.first, last_index));
		vector<int> top = s.buildHist(make_pair(ul.first, 0), lr);
		//long top_area = s.rectArea(make_pair(ul.first, 0), lr);
		for(int a = 0; a < 36; a++){
			bottom[a] += top[a];
		}
		return s.entropy(bottom, pow(2,k+1));
	}

}

PNG toqutree::render(){

	// My algorithm for this problem included a helper function
	// that was analogous to Find in a BST, but it navigated the 
	// quadtree, instead.

	/* your code here */

}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){

	//	prune(root,tol);

}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
	/* your code here */
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

	/* your code here */
}

//@param  im  : the PNG of the parent
//@param  ul, lr  : ul and lr of the children PNG you will return
//@param  k   : the dimension of children
PNG * makePNG(PNG *im, pair<int,int> ul, int k){
	
	PNG * Children(pow(2,k), pow(2,k));
//double check
	int i = 0;
	int j = 0;
	for(int x = 0; x < pow(2,k); x++){
		for(int y = 0; y < pow(2,k); y++){
			i = (x + ul.first) % pow(2,k+1);
			j = (y + ul.second) % pow(2,k+1);
			*(Children->getPixel(x, y)) = im->getPixel(i,j);
		}
	}
/*	
	int ogX, ogY;
	//NOWRAPPING
	if(ul.first < lr.first && ul.second < lr.second){
		for(int x = 0; x < pow(2,k); x++){
			for(int y = 0; y < pow(2,k); y++){
				ogX = x + ul.first;
				ogY = y + ul.second;
				*(Children->getPixel(x,y)) = im->getPixel(ogX, ogY);				
			}
		} 		
	}
	//WRAP CORNER
	else if(ul.first > lr.first && ul.second > lr.second){
		//GRAB the bottom right corner
		int i = 0;
		int j = 0;
		for(int x = 0; x < pow(2,k); x++){
			for(int y = 0; y < pow(2,k); y++){
				i = (x + ul.first) % pow(2,k+1);
				j = (y + ul.second) % pow(2,k+1);
				*(Children->getPixel(x, y)) = im->getPixel(i,j);
			}
		}
	}
	//WRAP AROUND THE SIDE
	else if(ul.first > lr.first){
		int i = 0;
		int j = 0;
		for(int x = 0; x < pow(2,k); x++){
			for(int y = 0; y < pow(2,k); y++){
				i = (x + ul.first) % pow(2,k+1);
				j = (y + ul.second) % pow(2,k+1);
				*(Children->getPixel(x, y)) = im->getPixel(i,j);
			}
		}
	}
	//WRAP TOP	
	else if(lr.second < ul.second){
	}

*/
}
