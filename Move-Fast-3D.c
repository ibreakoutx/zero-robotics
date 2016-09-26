//Declare any variables shared between functions here
//Compute quickest time to move 0.5 m

float accel ;
float forces[3];
float fmTstep ;
float fmTotalTime ;
int step ;

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
}

void initFastMove(float dist) {
    fmTstep = 0;
    fmTotalTime = sqrtf( dist/accel );
    DEBUG(("totalTime = %f\n",fmTotalTime));
}

void goFastMove() {
   	if (fmTstep < fmTotalTime/2) {
	    DEBUG(("ACCEL"));
	   setForceAccel();
	   api.setForces(forces);
	}
	else if (fmTstep < fmTotalTime) {
	    DEBUG(("DE-ACCEL"));
       setForceDeAccel();
	   api.setForces(forces);
	}
	else {
	    setForceZero();
	    api.setForces(forces);
        step++;
	}
    fmTstep++;
}

void setForceAccel() {
	forces[0] = 1.0;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void setForceDeAccel() {
	forces[0] = -1.0;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void setForceZero() {
	forces[0] = 0.0;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
    switch(step) {
        case 1:
            initFastMove(0.7);
            step++;
            break;
        case 2:
            goFastMove();
            break;
        case 3:
            DEBUG(("Final position reached"));
            break ;
    }
}
