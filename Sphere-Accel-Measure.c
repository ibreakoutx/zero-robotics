//Declare any variables shared between functions here

float position[3];
float myState[12];

float velocity[12];
float accel[12];
float dist[12];
float forces[3];

float maxVelocity;
float maxAccel ;
float maxDist ;

int tstep ;
int printonce ;
void init(){
    maxVelocity = 0;
    maxAccel = 0;
    maxDist = 0;
	//This function is called once when your code is first loaded.
	api.getMyZRState(myState);

	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	position[0] = myState[0] + 1.5;
	position[1] = myState[1];
	position[2] = myState[2];

	api.getMyZRState(myState);

	forces[0] = 1.0;
	forces[1] = 0.0;
	forces[2] = 0.0;

	tstep = 0;
	printonce = 0;
 }

void loop(){
	//This function is called once per second.  Use it to control the satellite.
    //api.setPositionTarget(position);
	api.getMyZRState(myState);

    if (tstep < 11) {
        dist[tstep] = myState[0];
        velocity[tstep++] = myState[3];
        if ( maxVelocity < myState[3] )
            maxVelocity  = myState[3];
        api.setForces(forces);
        DEBUG(("dist = %f, ",myState[0]));
        DEBUG(("vel = %f\n",myState[3]));
    }
    else if (printonce) {

        for(int i=0;i<11;i++) {
            accel[i] = velocity[i+1] - velocity[i];
            DEBUG(("dist[%d] = %f\n",i,dist[i]));
            DEBUG(("velocity[%d] = %f\n",i,velocity[i]));
            DEBUG(("accel[%d] = %f\n",i,accel[i]));
        }
        DEBUG(("Max dist = %f",myState[0]));
        DEBUG(("Max velocity = %f",maxVelocity));
        DEBUG(("Max accel = %f",maxAccel));
        printonce = 0;
    }
}
