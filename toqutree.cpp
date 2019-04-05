
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
	/* This constructor grabs the 2^k x 2^k sub-image centered */
	/* in imIn and uses it to build a quadtree. It may assume  */
	/* that imIn is large enough to contain an image of that size. */
	/* your code here */

	//Cropping the imIn to be a square
	//Remember that imIn is not neccessarily a perfect square	
	int ctrX = imIn.width()/2;
	int ctrY = imIn.height()/2;
	int ulx = ctrX - pow(2,k)/2;
	int uly = ctrY - pow(2,k)/2;
	pair<int, int> ul_middle(ulx,uly);
	
	printf("The height of Original Image is :%d", imIn.height());	
	//make the subPNG 
	PNG * parent = makePNG(&imIn, ul_middle, k);
	
	root = buildTree(parent, k);
}

int toqutree::size() {
	/* your code here */
}

//@param k : dimension of im
toqutree::Node * toqutree::buildTree(PNG * im, int k) {

	if(k==0){
	printf("BASE CASE OF BUILD TREE!!!\n");
		return new Node(make_pair(0,0), k, *(im->getPixel(0,0)));
	}	
	//the splitting point that has the least entropy
	pair<int, int> Split_Point;//AKA center
	
	//add index to ul to get lr
	int index = pow(2,k-1) - 1;
	int dim = pow(2,k);
	
	//the square to traverse through to find minimum entropy
	int ul = pow(2,k)/4;
	//int lr = ul + pow(2,k-1) - 1;
	pair<int, int> ul_middle(ul,ul);
	//pair<int, int> lr_middle(lr, lr);
	
	printf("The index is: %d\n", index);
	stats statsInfo(*im);
	printf("What is k for the stat? : %d\n AND What is the height of im? : %d\n", k, im->height());
	printf("stats object made\n");	
	//Find the min ENTROPY within ul_middle
	double minEntropy = get_Avg(statsInfo, ul_middle, k-1);  	
	printf("The index is: %f\n", minEntropy);
	double check_Entropy;
	pair<int,int> check_P = ul_middle;
	Split_Point = check_P;
	for(int x = ul_middle.first; x <= ul_middle.first + index; x++){
		for(int y = ul_middle.second; y <= ul_middle.second + index; y++){
			check_P.first = x;
			check_P.second = y;
			check_Entropy = get_Avg(statsInfo, check_P, k-1);
			if(check_Entropy < minEntropy){
				minEntropy = check_Entropy;
				Split_Point = check_P;
				printf("CHANGED MIN ENTROPY\n");	
			}
			printf("%d, %f\n", y, minEntropy);
		}
				printf("DO I GET HERE\n"); 
	}
	printf("I've found the optimal split_point\n");
	//Find average pixel for the root
	HSLAPixel a = statsInfo.getAvg(make_pair(0,0), make_pair(pow(2,k)-1, pow(2,k)-1));
	//Node to return (The root of the toqutree)
	
	Node * node = new Node(Split_Point, k, a); 
	
	//MAKE THE sub-PNGs
	pair<int,int> SW_ul, NE_ul, NW_ul;

	//mod is for if the point is off the picture
	SW_ul.first = (Split_Point.first + index + 1) % dim;
	SW_ul.second = Split_Point.second;

	NE_ul.first = Split_Point.first;
	NE_ul.second = (Split_Point.second + index + 1) % dim;

	NW_ul.first = SW_ul.first;
	NW_ul.second = NE_ul.second;
	
	PNG * SEs = makePNG(im, Split_Point, k-1);
	PNG * SWs = makePNG(im, SW_ul, k-1);
	PNG * NEs = makePNG(im, NE_ul, k-1);
	PNG * NWs = makePNG(im, NW_ul, k-1);

	printf("I've made all sub PNGS for the childern\n");

	node->NW = buildTree(SEs, k-1);
	node->SW = buildTree(SWs, k-1);
	node->NE = buildTree(NEs, k-1);
	node->NW = buildTree(NWs, k-1);

	delete(SEs);
	delete(SWs);
	delete(NEs);
	delete(NWs);
	
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
double toqutree::get_Avg(stats & Stats, pair<int,int> SE_ul, int k){
	//call getEntropy for each Quadrant 
	
	printf("I'm in get_Avg\n");	
	double Average = 0.;
	pair<int,int> SE_lr, SW_ul, SW_lr, NE_ul, NE_lr, NW_ul, NW_lr;
	int index = pow(2,k) - 1;
	int dim = pow(2,k+1);
	SE_lr.first = SE_ul.first + index;
	SE_lr.second = SE_ul.second + index;

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
	Average = getEntropy(Stats, SE_ul, SE_lr, k) + getEntropy(Stats, SW_ul, SW_lr, k)
		+ getEntropy(Stats, NE_ul, NE_lr, k) + getEntropy(Stats, NW_ul, NW_lr, k);
	return Average;
}
//get entropy of given quadrant *HANDLE WRAPPING HERE*
// @param s : Stats of original image
// @param ul, lr: ul, lr of the quadrant you are calculating entropy for
// @param k : the dimension of the quadrant where the quadrant is 2^k x 2^k
double toqutree::getEntropy(stats & s, pair<int,int> ul, pair<int,int> lr, int k){
	//printf("the dimension: %d, the ul is: (%d, %d), the lr is: (%d, %d) and k is: %d\n", s.lastIndex_x, ul.first, ul.second, lr.first, lr.second, k);
	printf("I'm in getEntropy (toqutree.cpp)\n");
	vector<int> Hist = s.buildHist(ul, lr);
	//printf("Hist vector is successfully found\n")
	printf("%f\n", s.entropy(Hist, pow(2,k+1)));
	return s.entropy(Hist, pow(2,k+1));
/*
	int last_index = pow(2,k+1) - 1;
	int index = pow(2,k) - 1;
	//NO WRAPPING
	if(ul.first < lr.first && ul.second < lr.second){
		return s->entropy(ul, lr);
	}
	//wrapping the corners
	else if(ul.first > lr.first && ul.second > lr.second){
		//bottomright + bottomleft + topright + topleft
		vector<int> BR = s->buildHist(ul, make_pair(last_index, last_index));
		vector<int> BL = s->buildHist(make_pair(0, ul.second), make_pair(lr.first, last_index));
		vector<int> TR = s->buildHist(make_pair(ul.first,0), make_pair(last_index, lr.second));
		vector<int> TL = s->buildHist(make_pair(0,0), lr);
		for(int a = 0; a < 36; a++){
			BR[a] += BL[a] + TR[a] + TL[a];
		}
		return s->entropy(BR, pow(2,k+1));
	} 
	//WRAPPING AROUND THE SIDE
	else if(ul.first > lr.first){
		//Left rect + right rect
		vector<int> Left_Rect = s->buildHist(ul, make_pair(last_index, lr.second));	
		vector<int> Right_Rect = s->buildHist(make_pair(0,ul.second), lr);
		for(int a = 0; a < 36; a++){
			Left_Rect[a] = Left_Rect[a] + Right_Rect[a];
		}	
		return s->entropy(Left_Rect, pow(2,k+1));
	}
	//WRAPPING AROUND THE TOP
	else if(lr.second < ul.second){
		//bottom + top
		vector<int> bottom = s->buildHist(ul, make_pair(lr.first, last_index));
		vector<int> top = s->buildHist(make_pair(ul.first, 0), lr);
		for(int a = 0; a < 36; a++){
			bottom[a] += top[a];
		}
		return s->entropy(bottom, pow(2,k+1));
	}
*/
	
//return s.entropy(make_pair(0,0), make_pair(0,0));
}
PNG toqutree::render(){
	return render(root);
}
PNG toqutree::render(Node * node){

	int Dim = node->dimension;
	//2^Dim x 2^Dim
	Dim = pow(2,Dim);
	PNG NWs, NEs, SEs, SWs;	
	PNG png(Dim, Dim);

	if(node->NW == NULL && node->NE == NULL && node->SE == NULL && node->SW == NULL){
		for(int x = 0; x < Dim; x++){
			for(int y = 0; y < Dim; y++){
				*(png.getPixel(x,y)) = node->avg;
			}
		}
	}
	else{
/*
	if(node->NW == NULL){
	    //fill it with average color 	
	}else{
	    //recruse render and get NW
	    NWs = render(node->NW);	
	}
	
	if(node->SW == NULL){
	    //fill it with average color
	}else{
	   //recurse render
	   NEs = render(node->NE);
	}
	
	if(node->NE == NULL){
	    //fill it with average
	}else{
	    //recurse
	    SEs = render(node->SE);
	}	
	
	if(node->SE == NULL){
	    //fill it with average
	}else{
	    //recurse
	    SWs = render(node->SW);
	}
*/
	NWs = render(node->NW);
	NEs = render(node->NE);
	SEs = render(node->SE);
	SWs = render(node->SW);
	// My algorithm for this problem included a helper function
	// that was analogous to Find in a BST, but it navigated the 
	// quadtree, instead.
	}
	putback(SEs, SWs, NEs, NWs, node->center, png, Dim-1);
	/* your code here */
	return png;
}
//Dim : the dimension of the subPNG pow(2,Dim) = the length of the subPNG
void toqutree::putback(PNG SE, PNG SW, PNG NE, PNG NW, pair<int,int> ctr, PNG &png, int Dim){

	int length = pow(2,Dim);
		
	pair<int,int> SE_ul, SW_ul, NW_ul, NE_ul;

	SE_ul.first = ctr.first;
	SE_ul.second = ctr.second;

	SW_ul.first = (ctr.first + length) % png.width();
	SW_ul.second = ctr.second;

	NE_ul.first = ctr.first;
	NE_ul.second = (ctr.second + length) % png.height();

	NW_ul.first = SW_ul.first;
	NW_ul.second = NE_ul.second;

	for(int x = 0; x < Dim; x++){
		for(int y = 0; y < Dim; y++){
			*(png.getPixel((SE_ul.first + x) % png.width(), (SE_ul.second + y) % png.height())) = *(SE.getPixel(x,y)); 
			*(png.getPixel((SW_ul.first + x) % png.width(), (SW_ul.second + y) % png.height())) = *(SW.getPixel(x,y)); 
			*(png.getPixel((NE_ul.first + x) % png.width(), (NE_ul.second + y) % png.height())) = *(NE.getPixel(x,y)); 
			*(png.getPixel((NW_ul.first + x) % png.width(), (NW_ul.second + y) % png.height())) = *(NW.getPixel(x,y)); 	
		}
	}
}
/*
HSLAPixel * toqutree::findPixel(Node * node, int x, int y){
	
	if(node->dimension == 0){
		return &(node->avg);
	}
	else{
	pair<int,int> SE_ul = node->center;
	//height or width
	int originalL = pow(2, node->dimension);
	//add this index to find the lr
	int index = pow(2,node->dimension - 1) - 1;
	pair<int,int> SW_ul, NE_ul, NW_ul;
	
	SW_ul.first = (SE_ul.first + index + 1) % originalL;
	SW_ul.second = SE_ul.second;

	NE_ul.first = SE_ul.first;
	NE_ul.second = (SE_ul.second + index + 1) % originalL;

	NW_ul.first = SW_ul.first;
	NW_ul.second = NE_ul.second;

	//get the lower points	
	pair<int,int> SE_lr, SW_lr, NE_lr, NW_lr;
	
	SE_lr.first = SE_lr.first + index;
	SE_lr.second = SE_lr.second + index;

	SW_lr.first = (SW_ul.first + index) % originalL;
	SW_lr.second = (SW_ul.second + index) % originalL;

	NE_lr.first = (NE_ul.first + index) % originalL;
	NE_lr.second = (NE_ul.second + index) % originalL;

	NW_lr.first = (NW_ul.first + index) % originalL;
	NW_lr.second = (NW_ul.second + index) % originalL;
	
	pair<int,int> Coord = make_pair(x, y);
	int newX, newY;
		if(checkinQuad(SE_ul, SE_lr, Coord, node->dimension)){
			newX = (x - SE_ul.first + originalL) % originalL;
			newY = (y - SE_ul.second + originalL) % originalL;
			return findPixel(node->SE, newX, newY);
		}
		else if(checkinQuad(SW_ul, SW_lr, Coord, node->dimension)){
			newX = (x - SW_ul.first + originalL) % originalL;
			newY = (y - SW_ul.second + originalL) % originalL;		
			return findPixel(node->SW, newX, newY);
		}
		else if(checkinQuad(NE_ul, NE_lr, Coord, node->dimension)){
			newX = (x - NE_ul.first + originalL) % originalL;
			newY = (y - NE_ul.second + originalL) % originalL;
			return findPixel(node->NE, newX, newY);
		}
		else if(checkinQuad(NW_ul, NW_lr, Coord, node->dimension)){
			newX = (x - NW_ul.first + originalL) % originalL;
			newY = (y - NW_ul.second + originalL) % originalL;
			return findPixel(node->NW, newX, newY);
		}
	}
}
*//*
//where k is the dimension of the whole square
bool toqutree::checkinQuad(pair<int,int> ul, pair<int,int> lr, pair<int,int> coord, int k){

	//NO WRAPPING
	if(ul.first < lr.first && ul.second < lr.second){
		if(coord.first >= ul.first && coord.second >= ul.second && coord.first <= lr.first && coord.second <= lr.second){
			return true;
		}else{
			return false;
		}
	}
	//wrapping the corners
	else if(ul.first > lr.first && ul.second > lr.second){
		//check if its the top corner
		if(coord.first >= 0 && coord.second >= 0 && coord.first <= lr.first && coord.second <= lr.second){
			return true;
		}
		//check if tis the top right corner
		else if(coord.first >= ul.first && coord.second >= 0 && coord.first <= pow(2,k) - 1 && coord.second <= lr.second){
			return true;
		}
		//check if its bottom left corner
		else if(coord.first >= 0 && coord.second >= ul.second && coord.first <= lr.first && coord.second <= pow(2,k) - 1){
			return true;
		}
		//check if its the bottom right corner
		else if(coord.first >= ul.first && coord.second >= ul.second && coord.first <= pow(2,k) - 1 && coord.second <= pow(2,k) - 1){
			return true;
		}
		else{
			return false;
		}
	}
	//WRAPPING AROUND THE SIDE
	else if(ul.first > lr.first){
		//check if its one the right
		if(coord.first >= ul.first && coord.second >= ul.second && coord.first <= pow(2,k)-1 && coord.second <= lr.second){
			return true;
		}
		//check if its on the left
		else if(coord.first >= 0 && coord.second >= ul.second && coord.first <= lr.first && coord.second <= lr.second){
			return true;
		}
		else{
			return false;
		}
	}	
	//WRAPPING AROUND THE TOP
	else if(lr.second < ul.second){
		//check the top
		if(coord.first >= ul.first && coord.second >= 0 && coord.first <= lr.first && coord.second <= lr.second){
			return true;
		}
		//check bottom
		else if(coord.first >= ul.first && coord.second >= ul.second && coord.first <= lr.first && coord.second <= pow(2,k)-1){
			return true;
		}
		else{
			return false;
		}
	}	
}*/
/*
PNG * toqutree::helpRender(PNG * Image, Node * node){
	
	if(node->NW == NULL){
		
	}
	unsigned int originalL = pow(2,node->dimension);
	//add the index to get lr
	unsigned int index = pow(2,root->dimension-1) - 1; 

		
	PNG * renderit = new PNG(originalL, originalL);
	
	pair<int,int> SE_ul = node->center;
	

/*	pair<int,int> SW_ul, NE_ul, NW_ul;
	
	SW_ul.first = (SE_ul.first + index + 1) % originalL;
	SW_ul.second = SE_ul.second;

	NE_ul.first = SE_ul.first;
	NE_ul.second = (SE_ul.second + index + 1) % originalL;

	NW_ul.first = SW_ul.first;
	NW_ul.second = NE_ul.second;

	//get the lower points	
	pair<int,int> SE_lr, SW_lr, NE_lr, NW_ul;
	
	SE_lr.first = SE_lr.first + index;
	SE_lr.second = SE_lr.second + index;

	SW_lr.first = (SW_ul.first + index) % originalL;
	SW_lr.second = (SW_ul.second + index) % originalL;

	NE_lr.first = (NE_ul.first + index) % originalL;
	NE_lr.second = (NE_ul.second + index) % originalL;

	NW_lr.first = (NW_ul.first + index) % originalL;
	NW_lr.second = (NW_ul.second + index) % originalL;
	
	fillPNG(*renderit, SE_ul, SE_lr, node->dimension - 1);
	fillPNG(*renderit, SW_ul, SW_lr, node->dimension - 1);
	fillPNG(*renderit, NE_ul, NE_lr, node->dimension - 1);
	fillPNG(*renderit, NW_ul, NW_lr, node->dimension - 1);
/*
}
*/
//Helper function for helpRender
//Call this function 4 times per Node using their ul, lr to fill in the Image 
// @param Image : Image that you will fill in
// @param ul, lr : points of the quadran you will fill in
// @param k : the dimension of the quadrant
/*
void toqutree::fillPNG(PNG * Image, pair<int,int> ul, pair<int,int> lr, int k){

	//NO WRAPPING
	if(ul.first < lr.first && ul.second < lr.second){
		for(unsigned int x = ul.first; x < ul.first + pow(2,k); x++){
			for(unsigned int y = ul.second; y < ul.second + pow(2,k); y++){
				*(Image.getPixel(x,y)) = 	
			}
		}
		 
	}
	//wrapping the corners
	else if(ul.first > lr.first && ul.second > lr.second){
	}
	//WRAPPING AROUND THE SIDE
	else if(ul.first > lr.first){
	}	
	//WRAPPING AROUND THE TOP
	else if(lr.second < ul.second){
	}	
}
*/
/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	
	//	prune(root,tol);

}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
	if(curr->dimension == 0){
		delete curr;
		return;
	}
	else{
		if(curr->NW != NULL){
			clear(curr->NW);
			delete curr;
			return;
		}
		if(curr->NE != NULL){
			clear(curr->NW);
			delete curr;
			return;
		}
		if(curr->SW != NULL){
			clear(curr->NW);
			delete curr;
			return;
		}
		if(curr->SE != NULL){
			clear(curr->NW);
			delete curr;
			return;
		}

	}
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

	/* your code here */
}

//@param  im  : the PNG of the parent
//@param  ul, lr  : ul and lr of the children PNG you will return
//@param  k   : the dimension of the PNG you want to return
PNG * toqutree::makePNG(PNG *im, pair<int,int> ul, int k){
printf("I'm in makePNG\n");
	PNG * Children = new PNG(pow(2,k), pow(2,k));
//double check
	int i = 0;
	int j = 0;
	for(int x = 0; x < pow(2,k); x++){
		for(int y = 0; y < pow(2,k); y++){
			i = (x + ul.first) % (int) im->width();
			j = (y + ul.second) % (int) im->height();
			*(Children->getPixel(x, y)) = *(im->getPixel(i,j));
		}
	}
printf("Done makePNG\n");
return Children;
}
