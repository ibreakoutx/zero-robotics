//Declare any variables shared between functions here
/*
2-D Intro game, with opponent sphere not operational.
Uses setTargetPosition to move sphere to specified coord.
Strategy:
1. Map out triangle by dropping SPS. To save time, first
   SPS dropped at start location of sphere. 2nd (and last)
   location is close to item LARGE_1.
2. Pick up item LARGE_1. Approaching from the top.
3. Move to Assembly zone, drop off LARGE_1.
4. Find next item that is closest.
5. Compute coordinates and orientation to approach.
6. Pick and drop off at assembly zone.
7. Repeat steps 4 through 6 until time runs out or no more
   items.
*/

#define NUMBER_OF_ITEMS 6
#define NUMBER_OF_SPS   3
#define OPPONENT_ID     2

#define X_COORD 0
#define Y_COORD 1
#define Z_COORD 2
#define X_VELOCITY 3
#define Y_VELOCITY 4
#define Z_VELOCITY 5
#define X_ORIENT 6
#define Y_ORIENT 7
#define Z_ORIENT 8

//Item IDs
#define LARGE_1  0
#define LARGE_2  1
#define MEDIUM_1 2
#define MEDIUM_2 3
#define SMALL_1  4
#define SMALL_2  5

//Option passed to functions
//indicating if they can increment
//the step counter
#define STEP_INC    1
#define STEP_NO_INC 0

//Target positions to drop off the SPS
float spsPosn[NUMBER_OF_SPS][3];

//Coordinates of the center of each item
float itemPosn[NUMBER_OF_ITEMS][3];

//In order to pick up the item, the target
//coordinates of the sphere must be offset
//from the center of the item
float itemApproachOffset[NUMBER_OF_ITEMS];

//Initialized to all 1s, set to 0, when
//corresponding item no longer available
//already picked up you or opponent.
int itemAvailable[NUMBER_OF_ITEMS] ;

float origin[3];

//Sphere orientations
float faceDown[3];
float faceUp[3];
float faceLeft[3];
float faceRight[3];

float zoneInfo[4];
float myState[12];
float zonePosn[3];
float zoneCenterOffset ;

int step ;
float tolerance ;
float spsTolerance ;
float zonePosnTolerance ;
float posn[3];
float orient[3];
int item_id;

void init(){
	//This function is called once when your code is first loaded.
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	step = 1;
	tolerance = 0.02;
	zoneCenterOffset = 0.15;
	zonePosnTolerance = 0.03;

	itemApproachOffset[LARGE_1]=0.16;
	itemApproachOffset[LARGE_2]=0.16;
    itemApproachOffset[MEDIUM_1]=0.15;
	itemApproachOffset[MEDIUM_2]=0.15;
	itemApproachOffset[SMALL_1]=0.13;
	itemApproachOffset[SMALL_2]=0.13;

	spsPosn[0][0] =  0.50;
	spsPosn[0][1] = -0.55;
	spsPosn[0][2] =  0.0;

	spsPosn[1][0] =  0.50;
	spsPosn[1][1] =  0.35;
	spsPosn[1][2] =  0.0;

	spsPosn[2][0] = -0.50;
	spsPosn[2][1] = -0.55;
	spsPosn[2][2] =  0.0;

	//Larger values will get sphere to drop SPS
	//before reaching target position
	spsTolerance = 0.3 ;

	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;

	//Initialize itemPosn array
	//Items don't move, so calculate just once.
	for(int i=0;i<NUMBER_OF_ITEMS;i++) {
	    game.getItemLoc( itemPosn[i] , i);
	}
/*	itemPosn[LARGE_1][X_COORD] = 0.23;
	itemPosn[LARGE_1][Y_COORD] = 0.23  ;
	itemPosn[LARGE_1][Z_COORD] = 0.0 ;

	itemPosn[LARGE_2][X_COORD] = -0.23;
	itemPosn[LARGE_2][Y_COORD] = -0.23  ;
	itemPosn[LARGE_2][Z_COORD] = 0.0 ;*/

	faceDown[0] = 0.0;
	faceDown[1] = -1.0;
	faceDown[2] = 0.0;
	faceUp[0] = 0.0;
	faceUp[1] = 1.0;
	faceUp[2] = 0.0;
	faceLeft[0] = -1.0;
	faceLeft[1] =  0.0;
	faceLeft[2] = 0.0;
	faceRight[0] = 1.0;
	faceRight[1] = 0.0;
	faceRight[2] = 0.0;

	zonePosn[0]=0.0;
	zonePosn[1]=0.0;
	zonePosn[2]=0.0;

	for (int i=0;i<NUMBER_OF_ITEMS;i++)
	    itemAvailable[i] = 1;
}

//Compute distance between 2 coordinates
float computeDistance( float a[], float b[]) {
    float sum = 0;
    for (int i=0; i<3; i++)
        sum = sum + (a[i]-b[i])*(a[i]-b[i]) ;
    return sqrt(sum);
}

float ABS(float a) {
    if (a < 0)
        return -a ;
    else
        return a ;
}

//Based on current position of sphere, return ID of
//closest available item
int getClosestAvailableItem() {
    float minDist = 10.0;//Initialize to large value
    int minDistItem = NUMBER_OF_ITEMS + 1 ; //Initialize to ID out of bounds
    for(int i=0;i<NUMBER_OF_ITEMS;i++) {
        if ( game.hasItem(i) == OPPONENT_ID )
            itemAvailable[i] = 0;
        if ( itemAvailable[i] ) {
             float dist = computeDistance(myState,itemPosn[i]);
             if ( minDist > dist) {
                 minDist = dist ;
                 minDistItem = i;
             }
        }
    }//for
    return minDistItem ;
}

void setFloatArray(float target[], float source[], int size) {
    for(int i=0;i<size;i++){
        target[i] = source[i];
    }
}

//Based on sphere's current position compute target coordinates
//of item to be picked up and orientation to pick up item.
//For example if the sphere is above the item, the target
//coordinate will be offset along the +Y direction.
//The output orientation will be available in the orient[]
//array. If the sphere is below the item and is oriented
//in the -Y direction, it will have to reorient to the +Y
//direction.
void getItemApproachInfo( int itemId, float posn[], float orient[] ) {
    setFloatArray(posn, itemPosn[itemId],3);
    //Is sphere above, if so approach from above with faceDown orientation
    if ( myState[Y_COORD] > itemPosn[itemId][Y_COORD] ) {
        setFloatArray(orient,faceDown,3);
        posn[Y_COORD] = posn[Y_COORD] + itemApproachOffset[itemId];
    }
    else { //sphere is below, approach with faceUp orientation
        setFloatArray(orient,faceUp,3);
        posn[Y_COORD] = posn[Y_COORD] - itemApproachOffset[itemId];
    }
}

void goToPosition( float posn[] , float tolerance , int inc ) {
    float err = ABS(myState[0]-posn[0]) + ABS(myState[1]-posn[1]);
    DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1]));
    DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],err));
    if (err > tolerance)
        api.setPositionTarget(posn);
    else if (inc)
            step++;
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
	api.getMyZRState(myState);

	switch(step) {

        case 1:
            DEBUG(("step %d",step));
            game.dropSPS();
            step++;
            break ;

        case 2:
            DEBUG(("step %d",step));
            goToPosition(spsPosn[0],spsTolerance, STEP_INC);
            break ;

        case 3:
            DEBUG(("step %d",step));
            game.dropSPS();
            step++;
            break;

        case 4:
            DEBUG(("step %d",step));
            goToPosition(spsPosn[1],spsTolerance, STEP_INC);
            break ;

        case 5:
            DEBUG(("step %d",step));
            game.dropSPS();
            step++;
            break ;

        case 6:
            item_id = getClosestAvailableItem() ;
            DEBUG(("Closest item is: %d",item_id));
            if ( item_id < NUMBER_OF_ITEMS ) {
                getItemApproachInfo( item_id, posn, orient ) ;
                DEBUG(("posn: %f, %f, %f",posn[0],posn[1],posn[2]));
                DEBUG(("orient: %f, %f, %f",orient[0],orient[1],orient[2]));
                step++;
            }
            break ;

        case 7:
            DEBUG(("step %d",step));
            goToPosition(posn,0.02,STEP_NO_INC);
            api.setAttitudeTarget(orient);
            if ( game.dockItem() ) {
                DEBUG(("Picked up item"));
                step++;
            }
            break;

        case 8:
            DEBUG(("step %d",step));
            DEBUG(("item %d picked up by player %d",item_id,game.hasItem(item_id)));
            if ( game.getZone(zoneInfo) ) {
                DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
            }
            // zonePosn[0] = zoneInfo[0] - (myState[6] * zoneCenterOffset) ;
            // zonePosn[1] = zoneInfo[1] - (myState[7] * zoneCenterOffset);
            zonePosn[0] = zoneInfo[0]  ;
            if (zoneInfo[1] < 0) {
                setFloatArray(orient,faceDown,3);
                zonePosn[1] = zoneInfo[1] + zoneCenterOffset;
            }
            else {
                zonePosn[1] = zoneInfo[1] - zoneCenterOffset;
                setFloatArray(orient,faceUp,3);
            }
            step++;
            break;

        case 9:
            DEBUG(("step %d",step));
            api.setAttitudeTarget(orient);
            goToPosition(zonePosn,zonePosnTolerance,STEP_INC);
            break;

        case 10:
            DEBUG(("step %d",step));
            game.dropItem();
            DEBUG(("Dropped item %d",item_id));
            //Indicate this item is no longer available
            itemAvailable[item_id] = 0;
            step = 6;//loop back to find next closest item
            break;

	}//switch
}
