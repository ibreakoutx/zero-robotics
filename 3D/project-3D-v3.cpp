//Begin page main
//Declare any variables shared between functions here
/*
3-D game, Version 2
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
*/

//#define VERBOSE 1

#define NUMBER_OF_ITEMS 6
#define NUMBER_OF_ITEM_TYPES 3
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

#define MAX_DOCK_VELOCITY  0.01
#define MAX_DOCK_ANGLE     0.05

//Target positions to drop off the SPS
//1st position is the initial starting position of the sphere
//2nd position is given by the array below, initialized
// based on whether we are the blue or green sphere
//3rd position is where we drop just before picking up the item
float spsPosn[3];
int counter;

//Coordinates of the center of each item
float itemPosn[NUMBER_OF_ITEMS][3];

//In order to pick up the item, the target
//coordinates of the sphere must be offset
//from the center of the item
float itemApproachOffset[NUMBER_OF_ITEM_TYPES];

//Initialized to all 1s, set to 0, when
//corresponding item no longer available
//already picked up you or opponent.
int itemAvailable[NUMBER_OF_ITEMS] ;

float diffVec[3];

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
float err;
float dist ;
float mydistance;
float opponentdistance;
float spsExpand ;

void init(){
	//This function is called once when your code is first loaded.
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	step = 1;
	tolerance = 0.02;
	zoneCenterOffset = 0.18;
	zonePosnTolerance = 0.03;

	counter = 0;

    //offset differ only by the type of items
    //These defines are part of the game api
	itemApproachOffset[ITEM_TYPE_LARGE]=0.16;
    itemApproachOffset[ITEM_TYPE_MEDIUM]=0.15;
	itemApproachOffset[ITEM_TYPE_SMALL]=0.14;

	api.getMyZRState(myState);

	spsExpand = 0.2;
   //We are the blue sphere
   if(myState[1] >= 0.140){
	spsPosn[0] =  0.23;
	spsPosn[1] =  0.23;
	spsPosn[2] =  0.70;
   }
   //We are the red sphere
   else {
    spsPosn[0] =  -0.23;
	spsPosn[1] =  -0.23;
	spsPosn[2] =  -0.70;
   }

	//Larger values will get sphere to drop SPS
	//before reaching target position
	spsTolerance = 0.20 ;

	//Initialize itemPosn array
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

    //Initially all items are available
	for (int i=0;i<NUMBER_OF_ITEMS;i++)
	    itemAvailable[i] = 1;
}

//Compute distance and orientation between 2 coordinates
//Returns distance and updates orient[] array
//Use mathVec library functions to reduce size
//of code, was 111% of max before this change,
//now is 89%
float computeDistance( float a[], float b[]) {
    float c[3];
    //vector c = vector a - vector b
    mathVecSubtract(c,a,b,3);
    return mathVecNormalize(c,3);
}

//Function to check my sphere's velocity below threshold to dock
//since negative points if you call dock
//when you are too fast
//Returns 1 when less 0 otherwise
int dockVelocityOk() {
    float vel[3];
    float vmag ;

    //Velocity is in indices 3,4,5
    vel[0] = myState[3];
    vel[1] = myState[4];
    vel[2] = myState[5];

    vmag = mathVecMagnitude(vel,3);

    #ifdef VERBOSE
    DEBUG(("SPHERE:Velocity = %f\n",vmag));
    #endif

    return (vmag < MAX_DOCK_VELOCITY) ;
}

//Function to check if dock orientation ok
//returns 1 if ok, 0 otherwise
int dockOrientOk( float targetorient[] ) {
    float myorient[3];
    float angle ;

    //Orient in indices 6,7,8
    myorient[0] = myState[6];
    myorient[1] = myState[7];
    myorient[2] = myState[8];

    //these should already be normalized ?
    mathVecNormalize(myorient,3);
    mathVecNormalize(targetorient,3);

    //Angle between 2 vectors that are normalized is
    //the acos of the inner(dot) product
    angle = acosf(mathVecInner( myorient, targetorient,3 ));

    #ifdef VERBOSE
    DEBUG(("SPHERE:Angle = %f\n",angle));
    #endif

    return (angle < MAX_DOCK_ANGLE);
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
//Argument: limitTypes if set to 1, will look for
//closest large items.
int getClosestAvailableItem(int limitTypes) {
    float minDist = 10.0;//Initialize to large value
    int minDistItem = limitTypes + 1 ; //Initialize to ID out of bounds
    for(int i=0;i<=limitTypes;i++) {
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

void setFloatArray(float target[], float source[], int size) {
    for(int i=0;i<size;i++){
        target[i] = source[i];
    }
}

//No longer use simplistic face up/face down , now that we
//are in 3D.
//Compute vector from current position to target.
//Orientation will be the vector above normalized.
//Compute distance to all 6 faces of item and choose the shortest
void getItemApproachInfo( int itemId, float posn[], float orient[] ) {
    float dist ;
    float tpos[3];
    float offset ;
    float minDist = 10;
    int minDim = 4;
    int minDir = 0;
    float minOffset = 0;

    orient[0]=0;
    orient[1]=0;
    orient[2]=0;

    offset = itemApproachOffset[ game.getItemType(itemId) ];

    setFloatArray(posn, itemPosn[itemId],3);

    api.getMyZRState(myState);

    //Which direction do we approach the item cube from ?
    //Cube has 6 faces, take points offset from the cube face
    //in the X,Y and Z direction. Choose the point closest to
    //the sphere.
    for(int dim=0;dim<3;dim++) {
        tpos[0] = posn[0];
        tpos[1] = posn[1];
        tpos[2] = posn[2];
        for(int i=-1; i < 2; i+=2 ) {
            tpos[dim] = tpos[dim] + offset * i ;
            dist = computeDistance( tpos ,myState ) ;
            if ( dist < minDist ){
                minDim = dim ;
                minDist = dist ;
                minOffset = offset * i;
                minDir = i;
            }
        }
    }
    posn[minDim] = posn[minDim] + minOffset ;
    orient[minDim] = -1*minDir;

    //Should work since position is in first 3 indices of myState
    //even though myState itself is a higher dimensional array
    //dist = computeDistanceAndOrient( posn ,myState, orient) ;

    //shorten the distance
    //dist = dist - itemApproachOffset[ game.getItemType(itemId) ];

    //Compute the new diffVec
    //for (int i=0; i<3;i++ )
       //diffVec[i] = orient[i] * dist;

    //Target position is my position + diffVec
    //mathVecAdd( posn, myState, diffVec, 3);
}

//Sum of absolute error differences between 2 vectors
float absError( float a[], float b[], int n){
		float err=0;
		for(int i=0;i<n;i++)
			err += fabsf(a[i]-b[i]);
		return err;
}

float goToPosition( float posn[] , float tolerance , int inc ) {
    float err;

		/*
    float vectorBetween[3];
    mathVecSubtract(vectorBetween,posn,myState,3);
    err = mathVecMagnitude(vectorBetween,3);
		*/

		err = absError(myState,posn,3);

    if (err > tolerance)
        api.setPositionTarget(posn);
    else if(inc)
            step++;
    #ifdef VERBOSE
    DEBUG(("SPHERE:Position Error = %f\n",err));
    #endif

    return err ;
  }

//Pick up item and go to step specified by argument next_step
void pickUpItem(int next_step) {
    err =  goToPosition(posn,0.02,STEP_NO_INC);

    #ifdef VERBOSE
    DEBUG(("SPHERE:Posn   %f, %f, %f\n",posn[0],posn[1],posn[2]));
    DEBUG(("SPHERE:Orient %f, %f, %f\n",orient[0],orient[1],orient[2]));
    #endif

        api.setAttitudeTarget(orient);

        //Check speed, orientation and position are ok
        //important to avoid penalties
        //Call dockItem with specific id, this takes care of the case
        //when 2 items are close to each other, so that the system
        //knows exactly which item you want to dock with.
        if ( dockVelocityOk() && dockOrientOk(orient) && err < 0.02 ) {

            if ( game.dockItem(item_id) ) {
                 DEBUG(("SPHERE:Picked up item %d",item_id));
                        step = next_step ;
                    }
            }
            else {
                ;
                #ifdef VERBOSE
                DEBUG(("Item-Pickup:Incorrect orientation"));
                #endif
            }

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
           // api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.50,0.01,3.0);
            #ifdef VERBOSE
            DEBUG(("step %d",step));
            #endif
            //api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.6,0.01,3.0);
            goToPosition(spsPosn,spsTolerance, STEP_INC);
            break ;

        case 3:
            #ifdef VERBOSE
            DEBUG(("step %d",step));
            #endif
            updateItemPositions();
						//Get closest available large item
            item_id = getClosestAvailableItem(LARGE_2) ;
            DEBUG(("Closest large item is: %d",item_id));
            getItemApproachInfo( item_id, posn, orient ) ;
            game.dropSPS();
            step++;
            break;

        case 4:
            // updateItemPositions();
            // item_id = getClosestAvailableItem() ;
            // DEBUG(("Closest item is: %d",item_id));
            if ( item_id < NUMBER_OF_ITEMS ) {
                pickUpItem(5);
            }
            break ;

				case 5:
					if (item_id == LARGE_1) {
							posn[0] = myState[0] + spsExpand ;
							posn[1] = myState[1] + spsExpand ;
							posn[2] = myState[2];
					}
					else {
						posn[0] = myState[0] - spsExpand ;
						posn[1] = myState[1] - spsExpand ;
						posn[2] = myState[2];
					}
					step++;
					break ;

				case 6:
						err = goToPosition(posn,0.1, STEP_NO_INC);
						if (err < 0.1)  {
	            #ifdef VERBOSE
	            DEBUG(("step %d",step));
	            #endif
	            game.dropSPS();
	            if ( game.getZone(zoneInfo) ) {
	                DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
	                step++;
	            }
						}
            break ;

        case 7:
            //api.setControlMode(CTRL_PD,CTRL_PID);
            //api.setPosGains(0.5,0.01,3.0);
            #ifdef VERBOSE
            DEBUG(("step %d",step));
            #endif
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
            #ifdef VERBOSE
            DEBUG(("step %d",step));
            #endif
            err = goToPosition(zonePosn,zonePosnTolerance,STEP_NO_INC);
            api.setAttitudeTarget(orient);

            if ( dockOrientOk(orient) && err < zonePosnTolerance)
                step++;

            break;

        case 9:
            #ifdef VERBOSE
            DEBUG(("step %d",step));
            #endif
            game.dropItem();
            DEBUG(("Dropped item %d",item_id));
            //Indicate this item is no longer available
            itemAvailable[item_id] = 0;
            step++;//loop back to find next closest item
            break;

        case 10:
           // api.setPosGains(0.5,0.01,3.0);
            updateItemPositions();

            //Item indexes are from 0(LARGE_1) to 5(SMALL_2)
            //argument to getClosestAvailableItem specifies
            //to which item the search must be limited to.
            //SMALL_2 (the last index),
            //implies we are searching for all items.
            item_id = getClosestAvailableItem(LARGE_2);
            if (item_id > LARGE_2)
                item_id = getClosestAvailableItem(SMALL_2);

            DEBUG(("Closest item is: %d",item_id));
            getItemApproachInfo( item_id, posn, orient ) ;
            step++;
            break;

        case 11:
            if ( item_id < NUMBER_OF_ITEMS ) {
                pickUpItem(7);
            }
            break;

	}//switch
}
//End page main
