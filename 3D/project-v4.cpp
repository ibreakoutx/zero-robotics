//Begin page main
//Declare any variables shared between functions here
/*
3-D game, Version 4
With opponent sphere also operational.
Highest score posted with opponent Item-bot : 46
Highest score posted with opponent Item-Thief : 15
Uses setTargetPosition to move sphere to specified coord.
Changed parameters of PD controller from default for
greater speed, comes at the expense of more fuel consumption
The New Strategy:
1. Drop first SPS at initial position.
2. Go and pick up large item closest to SPHERE
3. With the item, go and drop the 2nd and then the third SPS
4. Go to zone and drop large item there.
5. Now find the closest item and repeat the process of finding item and dropping it in zone til SPHERE runs out of fuel

ISSUES:
1. Code size is 111% (not eligible for submisson). Fix by using mathVec operations in
   computeDistance function. Code size with change is now:
2. Modify dockItem() call to include item_id, as in docItem(item_id). This should
   fix the issue when 2 items are close to the sphere and we incur docking penalties
   because the sphere is trying to dock with wrong item.
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
float targetRate[3];
float stop[3];
int counter;

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
float faceZDown[3];
float faceZUp[3];
float largeitem[3];
float zoneInfo[4];
float myState[12];
float otherState[12];
float zonePosn[3];
float zoneCenterOffset ;
float vectorBetween[3];
int step ;
float tolerance ;
float spsTolerance ;
float zonePosnTolerance ;
float posn[3];
float orient[3];
int item_id;
float dist ;
float diffVec[3];

void init(){
	//This function is called once when your code is first loaded.
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	step = 1;
	tolerance = 0.02;
	zoneCenterOffset = 0.15;
	zonePosnTolerance = 0.03;

	targetRate[0] = -40 * PI / 180;
	targetRate[1] = 0.00;
	targetRate[2] = 0.00;
	counter = 0;
	stop[0] = 0.0;
	stop[1] = 0.0;
    stop[2] = 0.0;
	itemApproachOffset[LARGE_1]=0.16;
	itemApproachOffset[LARGE_2]=0.16;
  itemApproachOffset[MEDIUM_1]=0.15;
	itemApproachOffset[MEDIUM_2]=0.15;
	itemApproachOffset[SMALL_1]=0.13;
	itemApproachOffset[SMALL_2]=0.13;
	api.getMyZRState(myState);

   if(myState[1] >= 0.140){
	spsPosn[0][0] =  0.30;
	spsPosn[0][1] = 0.50;
	spsPosn[0][2] =  0.30;

	spsPosn[1][0] =  -0.45;
	spsPosn[1][1] =  0.30;
	spsPosn[1][2] =  0.30;

	spsPosn[2][0] = -0.50;
	spsPosn[2][1] = -0.35;
	spsPosn[2][2] =  0.0;
	largeitem[0] = -0.23;
	largeitem[1] = -0.23;
	largeitem[2] = -0.23;
   }
   else {
    spsPosn[0][0] =  -0.40;
	spsPosn[0][1] = -0.30;
	spsPosn[0][2] =  -0.30;

	spsPosn[1][0] =  0.55;
	spsPosn[1][1] =  -0.30;
	spsPosn[1][2] =  -0.30;

	spsPosn[2][0] = -0.50;
	spsPosn[2][1] = -0.35;
	spsPosn[2][2] =  0.0;
	largeitem[0] = 0.23;
	largeitem[1] = 0.23;
	largeitem[2] = 0.23;
   }

	//Larger values will get sphere to drop SPS
	//before reaching target position
	spsTolerance = 0.30 ;

	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;

	//Initialize itemPosn array
	//Items don't move, so calculate just once.
	for(int i=0;i<NUMBER_OF_ITEMS;i++) {
	    game.getItemLoc( itemPosn[i] , i);
	}
    faceZDown[0] = 0.0;
    faceZDown[1] = 0.0;
    faceZDown[2] = -1.0;
    faceZUp[0] = 0.0;
    faceZUp[1] = 0.0;
    faceZUp[2] = 1.0;
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
    float c[3];
    //vector c = vector a - vector b
    mathVecSubtract(c,a,b,3);
    return mathVecNormalize(c,3);
    /*
    //Use mathVec operations to reduce size of code
    //with changes above: 89%
    float sum = 0;
    for (int i=0; i<3; i++)
        sum = sum + (a[i]-b[i])*(a[i]-b[i]) ;
    return sqrt(sum);
    */
}

//The opponent could have moved taken items
//including those in your assembly zone
//so update item position before calling
//routine to find closest available item
//To account for the fact that an
//item that was previously placed in your
//assembly zone could have been moved out
//calculate the item position distance from
//the assembly zone, if > sps error, mark
//that item as available.
void updateItemPositions() {
    for (int i=0;i<NUMBER_OF_ITEMS;i++) {
	    game.getItemLoc( itemPosn[i] , i);
	    //zoneInfo[4] has the spsTolerance
	    if ( computeDistance( itemPosn[i] , zoneInfo) > 0.3 )
	        itemAvailable[i] = 1;
    }
}

//Based on current position of sphere, return ID of
//closest available item
int getClosestAvailableItem() {
    float minDist = 10.0;//Initialize to large value
    int minDistItem = NUMBER_OF_ITEMS + 1 ; //Initialize to ID out of bounds
    for(int i=0;i<NUMBER_OF_ITEMS;i++) {
        if ( game.hasItem(i) == OPPONENT_ID ) {
            DEBUG(("Opponent has item %d",i));
        } else
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

int getLargeItem() {
    if(itemAvailable[0] == 1)
    {
        if(game.hasItem(0) == OPPONENT_ID)
        {
            DEBUG(("Opponent has item 0 "));
        }
        else {
            return 0;
        }
    }
    if(itemAvailable[1] == 1)
    {
        if(game.hasItem(1) == OPPONENT_ID)
        {
            DEBUG(("Opponent has item 1 "));
        }
        else {
            return 1;
        }
    }
    return 99;
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
  /*if (myState[Z_COORD] > itemPosn[itemId][Z_COORD])
    {
        setFloatArray(orient,faceZDown,3);
        posn[Z_COORD] = posn[Z_COORD] + itemApproachOffset[itemId];
    }
    else {
        setFloatArray(orient,faceZUp,3);
        posn[Z_COORD] = posn[Z_COORD] - itemApproachOffset[itemId];
    }  */
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
    float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]);
    DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1],myState[2]));
    DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],posn[2],err));
   // mathVecSubtract(vectorBetween,posn,myState,3);
    if (err > tolerance)
        api.setPositionTarget(posn);
    else if(inc)
            step++;

  }

void loop(){
	//This function is called once per second.  Use it to control the satellite.
	api.getMyZRState(myState);
	api.getOtherZRState(otherState);
    //By default Position controller used PD and
    //Attitude controller uses PID
    //not changing it right now
	//api.setControlMode(CTRL_PD,CTRL_PID);
     //float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]) ;
     api.setControlMode(CTRL_PD,CTRL_PID);
     if (game.getFuelRemaining() < 13)
     {
         api.setPosGains(0.4,0.01,3.0);
     }
     else if (game.getFuelRemaining() < 50)
     {
         api.setPosGains(0.5,0.01,3.0);
     }
     else {
         api.setPosGains(0.4,0.01,3.0);
     }

	//P, I and D values for position controller
	//High P value will increase speed of movement
	//resulting in overshoot, counteract with a higher
	//D value.
	//Experimenting with a few values, came up with this
	//combination, that posted a score of upto 40 in some
	//games.
   // api.setPosGains(0.45,0.01,3.0);

	switch(step) {

        case 1:
            //api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.40,0.01,3.0);
            DEBUG(("step %d",step));
            game.dropSPS();
            step++;
            break ;

        case 2:
            updateItemPositions();
            item_id = getClosestAvailableItem() ;
            DEBUG(("Closest item is: %d",item_id));
            if ( item_id < NUMBER_OF_ITEMS ) {
                getItemApproachInfo( item_id, posn, orient ) ;

								#ifdef VERBOSE
                DEBUG(("posn: %f, %f, %f",posn[0],posn[1],posn[2]));
                DEBUG(("orient: %f, %f, %f",orient[0],orient[1],orient[2]));
								#endif

                goToPosition(posn,0.02,STEP_NO_INC);
                float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]) ;
                api.setAttitudeTarget(orient);
                if (err < 0.02) {
                //Call dockItem() with item_id
                if ( game.dockItem(item_id) ) {
                     DEBUG(("Picked up item"));
                    step++;
                }
                }
            }
            break ;
        case 3:
           // api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.50,0.01,3.0);
            DEBUG(("step %d",step));
            //api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.6,0.01,3.0);
            goToPosition(spsPosn[0],spsTolerance, STEP_INC);
            break ;

        case 4:
            DEBUG(("step %d",step));
            game.dropSPS();
            step++;
            break;

        case 5:
            DEBUG(("step %d",step));
            goToPosition(spsPosn[1],spsTolerance, STEP_INC);
            break ;

        case 6:
            DEBUG(("step %d",step));
            game.dropSPS();
            if ( game.getZone(zoneInfo) ) {
                DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
                step++;
            }
            break ;

        case 7:
        //api.setControlMode(CTRL_PD,CTRL_PID);
        //api.setPosGains(0.5,0.01,3.0);
        DEBUG(("step %d",step));
            DEBUG(("item %d picked up by player %d",item_id,game.hasItem(item_id)));

						mathVecSubtract(orient, zoneInfo, myState,3);
            dist = mathVecNormalize( orient , 3);
            dist = dist - zoneCenterOffset ;
            for(int i=0;i<3;i++)
                diffVec[i] = orient[i] * dist ;
            mathVecAdd( zonePosn, myState, diffVec,3 );

						/*
					  zonePosn[0] = zoneInfo[0]  ;
            zonePosn[2] = zoneInfo[2];
            if (zoneInfo[1] < 0) {
                setFloatArray(orient,faceDown,3);
                zonePosn[1] = zoneInfo[1] + zoneCenterOffset;
            }
            else {
                zonePosn[1] = zoneInfo[1] - zoneCenterOffset;
                setFloatArray(orient,faceUp,3);
            }
						*/

            step++;
            break;


        case 8:
            DEBUG(("step %d",step));
            api.setAttitudeTarget(orient);
            goToPosition(zonePosn,zonePosnTolerance,STEP_INC);
            break;

        case 9:
            DEBUG(("step %d",step));
            game.dropItem();
            DEBUG(("Dropped item %d",item_id));
            //Indicate this item is no longer available
            itemAvailable[item_id] = 0;
            step++;//loop back to find next closest item
            break;

        case 10:
           // api.setPosGains(0.5,0.01,3.0);
            updateItemPositions();
            float opponentdistance = fabsf(otherState[0]-largeitem[0]) + fabsf(otherState[1]-largeitem[1]) + fabsf(otherState[2] - largeitem[2]);
            float mydistance = fabsf(myState[0]-largeitem[0]) + fabsf(myState[1]-largeitem[1]) + fabsf(myState[2] - largeitem[2]);
            if ((itemAvailable[0] == 1 || itemAvailable[1] == 1) && mydistance < opponentdistance)
            {
            item_id = getLargeItem();
            }
            else {
                item_id = getClosestAvailableItem();
            }
            //item_id = getClosestAvailableItem() ;
            //item_id = getLargeItem();
            DEBUG(("Closest item is: %d",item_id));
            if ( item_id < NUMBER_OF_ITEMS ) {
                getItemApproachInfo( item_id, posn, orient ) ;
                DEBUG(("posn: %f, %f, %f",posn[0],posn[1],posn[2]));
                DEBUG(("orient: %f, %f, %f",orient[0],orient[1],orient[2]));
                goToPosition(posn,0.02,STEP_NO_INC);
                float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]);
                api.setAttitudeTarget(orient);
                if (err < 0.02) {
                if ( game.dockItem(item_id) ) {
                     DEBUG(("Picked up item"));
                    step = 7;
                }
                }
            }
            break;


	}//switch
}
//End page main
