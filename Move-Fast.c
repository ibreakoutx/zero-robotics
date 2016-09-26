//Declare any variables shared between functions here
//Compute quickest time to move 0.5 m

float accel ;
float dist ;
float forces[3];
float tstep ;
float totalTime ;

void init(){
	//This function is called once when your code is first loaded.
    accel = 2.085e-3;
    dist  = 0.5;
    tstep = 0;
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!

	totalTime = sqrtf( dist/accel );
	//totalTime = 20;
	DEBUG(("totalTime = %f\n",totalTime));

	forces[0] = 0.1;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void setForceAccel() {
	forces[0] = 0.1;
	forces[1] = 0.0;
	forces[2] = 0.0;
}

void setForceDeAccel() {
	forces[0] = -0.1;
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
	if (tstep < totalTime/2) {
	    DEBUG(("ACCEL"));
	   setForceAccel();
	   api.setForces(forces);
	}
	else if (tstep < totalTime) {
	    DEBUG(("DE-ACCEL"));
       setForceDeAccel();
	   api.setForces(forces);
	}
	else {
	    DEBUG(("FORCE ZERO"));
	    setForceZero();
	    api.setForces(forces);
	}
    tstep++;
}
