//Declare any variables shared between functions here

#define LARGE_1  0
#define LARGE_2  1
#define MEDIUM_1 2
#define MEDIUM_2 3
#define SMALL_1  4
#define SMALL_2  5

#define STEP_INC    1
#define STEP_NO_INC 0

float spsPosn[3][3];
float itemPosn[6][3];
float itemApproachOffset[6];

float origin[3];

float faceDown[3];
float faceUp[3];
float faceLeft[3];
float faceRight[3];

float zoneInfo[4];
float myState[12];
float zonePosn[3];
float zoneCenterOffset ;

float itemLargeApproachOffset ;
float itemMediumApproachOffset ;
float itemSmallApproachOffset ;

int step ;
float tolerance ;
float spsTolerance ;

void init(){
	//This function is called once when your code is first loaded.
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	step = 1;
	tolerance = 0.02;
	zoneCenterOffset = 0.15;

	itemApproachOffset[LARGE_1]=0.16;
	itemApproachOffset[LARGE_2]=0.16;
	itemMediumApproachOffset = 0.10 ;
	itemSmallApproachOffset = 0.07 ;

	spsPosn[0][0] =  0.50;
	spsPosn[0][1] = -0.55;
	spsPosn[0][2] =  0.0;

	spsPosn[1][0] =  0.50;
	spsPosn[1][1] =  0.55;
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

	itemPosn[LARGE_1][0] = 0.23;
	itemPosn[LARGE_1][1] = 0.23 + itemApproachOffset[LARGE_1] ;
	itemPosn[LARGE_1][3] = 0.0 ;

	itemPosn[LARGE_2][0] = -0.23;
	itemPosn[LARGE_2][1] = -0.23 + itemApproachOffset[LARGE_2] ;
	itemPosn[LARGE_2][3] = 0.0 ;

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

}

float ABS(float a) {
    if (a < 0)
        return -a ;
    else
        return a ;
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
            DEBUG(("step %d",step));
            goToPosition(itemPosn[LARGE_1],0.02,STEP_NO_INC);
            api.setAttitudeTarget(faceDown);
            if ( game.dockItem() ) {
                DEBUG(("Picked up item"));
                step++;
            }
            break;

        case 7:
            DEBUG(("step %d",step));
            DEBUG(("item 0 picked up by %d",game.hasItem(0)));
            if ( game.getZone(zoneInfo) ) {
                DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
            }
            zonePosn[0] = zoneInfo[0] - (myState[6] * zoneCenterOffset) ;
            zonePosn[1] = zoneInfo[1] - (myState[7] * zoneCenterOffset);
            step++;
            break;

        case 8:
            DEBUG(("step %d",step));
            goToPosition(zonePosn,0.01,STEP_INC);
            break;

        case 9:
            DEBUG(("step %d",step));
            game.dropItem();
            DEBUG(("Dropped item"));
            step++;
            break;

        case 10:
            DEBUG(("step %d",step));
            goToPosition(itemPosn[LARGE_2],0.02,STEP_NO_INC);
            api.setAttitudeTarget(faceDown);
            if ( game.dockItem() ) {
                DEBUG(("Picked up item"));
                step++;
            }
            break;

        case 11:
            DEBUG(("step %d",step));
            DEBUG(("item 1 picked up by %d",game.hasItem(1)));
            if ( game.getZone(zoneInfo) ) {
                DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
            }
            zonePosn[0] = zoneInfo[0] - (myState[6] * zoneCenterOffset) ;
            zonePosn[1] = zoneInfo[1] - (myState[7] * zoneCenterOffset);
            step++;
            break ;

        case 12:
            DEBUG(("step %d",step));
            goToPosition(zonePosn,0.01,STEP_INC);
            break ;

        case 13:
            DEBUG(("step %d",step));
            game.dropItem();
            DEBUG(("Dropped item"));
            step++;
            break;

        case 14:
            DEBUG(("Score = %f", game.getScore()));
            goToPosition(origin,0.01,STEP_NO_INC);
            break;

	}//switch
}
