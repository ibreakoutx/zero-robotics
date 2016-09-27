//Declare any variables shared between functions here
//Compute quickest time to move 0.5 m

float accel ;
float forces[3];
int fmTstep ;
float fmTotalTime ;
float fmVec[3];
float fmDist ;
float fmDistMin;
int step ;

float posn[3];
float myState[12];
float velocity[3];
float velocityMagnitude;

void init(){
	//This function is called once when your code is first loaded.
    accel = 2.085e-3;
    step = 1;
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!

	//totalTime = 20;
	forces[0] = 0.1;
	forces[1] = 0.0;
	forces[2] = 0.0;

	posn[0] = 0.5;
	posn[1] = -0.5;
	posn[2] = 0.0;

	velocityMagnitude = 0.1;
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

void initFastMove(float posn[]) {
    fmTstep = 0;
    fmDist = calcDestinationVec( fmVec, posn );
    fmTotalTime = sqrtf( fmDist/accel );
    DEBUG(("totalTime = %f\n",fmTotalTime));
    DEBUG(("totalDist = %f\n",fmDist));
    DEBUG(("Vec Magnitude = %f\n",mathVecMagnitude(fmVec,3)));
    fmDistMin = fmDist;
}

void goFastMove(float posn[]) {
    float dist;
    float speed;
    float vec[3];
    float velocityVec[3];

    dist = calcDestinationVec( vec, posn );
    //speed = calcVelocityVec( velocity );

    if ( dist < fmDistMin )
      fmDistMin = dist ;

    switch(fmTstep) {
        case 0:
            if (dist < ((fmDist * 0.5))) {
              //DEBUG(("DE-ACCEL, dist = %f",dist));
              //setForceDeAccel(fmVec);
	          //api.setForces(forces);
	          fmTstep++;
            }
	        else {
    	        DEBUG(("ACCEL, dist = %f",dist));
    	        DEBUG(("ACCEL, fmDist/2 = %f",fmDist * 0.5));
    	       for(int i=0;i<3;i++)
                  velocityVec[i] = velocityMagnitude * vec[i];
	            //setForceAccel(fmVec);
	            //api.setForces(forces);
	            api.setVelocityTarget(velocityVec);
	        }
	       break ;
	   case 1:
	       DEBUG(("DE-ACCEL, dist = %f",dist));
           //setForceDeAccel(fmVec);
	       //api.setForces(forces);
	       for(int i=0;i<3;i++)
             velocityVec[i] = 0.0;
	       api.setVelocityTarget(velocityVec);
	       //inner product is the magnitude of the
	       //vectors multiplied by the cosine of the
	       //angle between them. If they point in opposite
	       //directions it will negative
	       //if ( mathVecInner(velocity,fmVec,3) < 0 )
	       if ( dist > fmDistMin )
	            fmTstep++;
	       break ;
	   case 2:
	       	setForceZero();
	        api.setForces(forces);
	        api.setPositionTarget(posn);
	        break ;
	}
}

void setForceAccel(float vec[]) {
	forces[0] = vec[0];
	forces[1] = vec[1];
	forces[2] = vec[2];
}

void setForceDeAccel(float vec[]) {
	forces[0] = -vec[0] ;
	forces[1] = -vec[1] ;
	forces[2] = -vec[2] ;
}

void setForceZero() {
	forces[0] = 0.0;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
    api.getMyZRState(myState);
    //step = 0;
    //api.setPositionTarget(posn);

    switch(step) {
        case 1:
            initFastMove(posn);
            step++;
            break;
        case 2:
            goFastMove(posn);
            break;
        case 3:
            DEBUG(("Final position reached"));
            break ;
    }
}
