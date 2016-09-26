//Declare any variables shared between functions here

float position[3];
float myState[12];

void init(){
 	position[0] = 0.5;
	position[1] = 0.0;
	position[2] = 0.0;
}

void loop(){
    api.getMyZRState(myState);

	//This function is called once per second.  Use it to control the satellite.

    DEBUG(("LOOP:%d, posn=%f\n",api.getTime(),myState[0]));
    api.setPositionTarget(position);
}
