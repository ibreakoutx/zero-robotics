//Declare any variables shared between functions here

float spsPosn1[3];
float spsPosn2[3];
float spsPosn3[3];
float itemLarge1[3];
float itemLarge2[3];
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

void init(){
	//This function is called once when your code is first loaded.
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	step = 1;
	tolerance = 0.02;
	zoneCenterOffset = 0.15;

	spsPosn1[0] =  0.50;
	spsPosn1[1] = -0.55;
	spsPosn1[2] =  0.0;

	spsPosn2[0] =  0.50;
	spsPosn2[1] =  0.55;
	spsPosn2[2] =  0.0;

	spsPosn3[0] = -0.50;
	spsPosn3[1] = -0.55;
	spsPosn3[2] =  0.0;

	itemLarge1[0] = 0.23;
	itemLarge1[1] = 0.36;
	itemLarge1[2] = 0.0 ;

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

void goToPosition( float posn[] , float tolerance ) {
    float err = ABS(myState[0]-posn[0]) + ABS(myState[1]-posn[1]);
    DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1]));
    DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],err));
    if (err > tolerance)
        api.setPositionTarget(posn);
    else
        step++;
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
	api.getMyZRState(myState);
    if      (step == 1) {
        DEBUG(("step %d",step));
        game.dropSPS();
        step++;
    }
    else if (step == 2) {
        DEBUG(("step %d",step));
        goToPosition(spsPosn1,0.3);
    }
    else if (step == 3) {
        DEBUG(("step %d",step));
        game.dropSPS();
        step++;
    }
    else if (step == 4) {
        DEBUG(("step %d",step));
        goToPosition(spsPosn2,0.3);
    }
    else if (step == 5) {
        DEBUG(("step %d",step));
        game.dropSPS();
        step++;
    }
    else if (step == 6 ) {
        DEBUG(("step %d",step));
        goToPosition(itemLarge1,0.02);
        api.setAttitudeTarget(faceDown);
        if ( game.dockItem() ) {
            DEBUG(("Picked up item"));
            step++;
        }
    }
    else if (step == 7 ) {
        DEBUG(("step %d",step));
        DEBUG(("item 0 picked up by %d",game.hasItem(0)));
        if ( game.getZone(zoneInfo) ) {
            DEBUG(("ZoneInfo: %f,%f,%f,%f",zoneInfo[0],zoneInfo[1],zoneInfo[2],zoneInfo[3]));
        }
        zonePosn[0] = zoneInfo[0] - (myState[6] * zoneCenterOffset) ;
        zonePosn[1] = zoneInfo[1] - (myState[7] * zoneCenterOffset);
        step++;
    }
    else if (step == 8 ) {
        DEBUG(("step %d",step));
        goToPosition(zonePosn,0.01);
    }
    else if (step == 9 ) {
        DEBUG(("step %d",step));
        game.dropItem();
        DEBUG(("Dropped item"));
        step++;
    }
    else if (step == 10) {
        DEBUG(("Score = %f", game.getScore()));
    }

}
