//Declare any variables shared between functions here
//Compute quickest time to move 0.5 m

int step ;

float posn[3];
float myState[12];
float velocity[3];

float unitVec[3];
float dist;

void init(){
        api.getMyZRState(myState);
	//This function is called once when your code is first loaded.
    step = 1;
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!

	posn[0] = 0.6;
	posn[1] = 0.5;
	posn[2] = 0.0;

	dist = calcDestinationVec( unitVec, posn) ;
	scalarMultiplyVec( velocity, unitVec, 10);
	printVec(velocity);
}

void printVec( float vec[]) {
    DEBUG(("vec: %f, %f, %f\n",vec[0],vec[1],vec[2]));
}

void scalarMultiplyVec( float out[], float in[] , float magnitude ) {
    for(int i=0;i<3;i++)
        out[i] = in[i] * magnitude ;
}
//Returns magnitude of Destination vector
//vec is the unit vector pointing in the direction
//of the destination from the sphere's current position
float calcDestinationVec( float vec[], float posn[]) {
    for(int i=0;i<3;i++)
        vec[i] = posn[i] - myState[i];
    return mathVecNormalize(vec,3);
}

float calcVelocityVec( float vec[] ) {
    for(int i=3;i<6;i++)
        vec[i] = myState[i];
    return mathVecNormalize(vec,3);
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
    api.getMyZRState(myState);
    step = 0;
    api.setVelocityTarget(velocity);
}
