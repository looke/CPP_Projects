/*
 * BasicKalman.cpp
 *
 *  Created on: 2016��11��17��
 *      Author: looke
 */

#include "BasicKalman.h"

Kalman::Kalman() {
    /* We will set the variables like so, these can also be tuned by the user */
    Q_angle = 0.001f;  //Original
    //Q_angle = 0.0001f;
    Q_bias = 0.003f;
    R_measure = 0.03f; //original value
		//R_measure = 10.7f;
    angle = 0.0f; // Reset the angle
    bias = -0.14f; // Reset bias

    basicBias = -0.14f; //reset basicBias
    basicAngle = 0.0f; //reset basicAngle

    float K[2]; // Kalman gain - This is a 2x1 vector

    P[0][0] = 0.0f; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
    P[0][1] = 0.0f;
    P[1][0] = 0.0f;
    P[1][1] = 0.0f;
};

// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
//newAngle: ACC sensor angle
//newRate:  Gyro Sensor rate
float Kalman::getAngle(float newAngle, float newRate, float dt) {
    // KasBot V2  -  Kalman filter module - http://www.x-firm.com/?page_id=145
    // Modified by Kristian Lauszus
    // See my blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it

    // Discrete Kalman filter time update equations - Time Update ("Predict")
    // Update xhat - Project the state ahead

    //Multi-State Kalman
    //float currentState = STATIC_STATE;
    //float y = newAngle - angle;;

    /*
    //First: check boundary condition
    if(newAngle >= 27 || newAngle<=-27)
    {
    	//Normal street slope is smaller than 8%, Baldwin Street has 35% slope.
    	//If ACC sensor get the angel greater than 30 degree (60%), it means the result must be influenced greatly by other acceleration.
    	//In this case, ACC sensor does not take into the basic Kalman filter.
    	currentState = HUGE_ACCELERATE_STATE;
    	y = 0; //set y=0 means basic kalman filter will completely use Gyro sensor to calc the pitch
    }
    else
    {
    	//Calc the diff between newAngel(From ACC sensor) and the old pitch
    	float diff = newAngle - angle;

    	if(diff<=-10 || diff >= 10)
    	{
    		//Dt 0.01ms during this time, normal bicycle or motorcycle hardly can gain a slope changing which greater than 20 degree.
    		//The distance between two wheels is 109cm. tan(20) = 0.36. It means during 0.01ms the hight of wheel must change 40cm which is quite impossible in normal road.
    		//The distance between two wheels is 109cm. tan(10) = 0.17. It means during 0.01ms the hight of wheel must change 19cm which is quite impossible in normal road.
    		currentState = HUGE_ACCELERATE_STATE;
    		y = 0; //set y=0 means basic kalman filter will completely use Gyro sensor to calc the pitch
    	}
    }

    //check rate value if |rate| > 2 dps state set to ROTATE_STATE
    if(newRate<-2 || newRate >2)
    {
    	currentState = ROTATE_STATE;
    }
    */
    /* Step 1 */
    rate = newRate - bias;
    angle += dt * rate;

    //float rate_afterBias = newRate - bias;
    //float angle_temp = angle + dt * rate_afterBias;

    // Update estimation error covariance - Project the error covariance ahead
    /* Step 2 */
    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_bias * dt;

    // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
    // Calculate Kalman gain - Compute the Kalman gain
    /* Step 4 */
    float S = P[0][0] + R_measure; // Estimate error
    /* Step 5 */
    //float K[2]; // Kalman gain - This is a 2x1 vector
    K[0] = P[0][0] / S;
    K[1] = P[1][0] / S;

    // Calculate angle and bias - Update estimate with measurement zk (newAngle)
    /* Step 3 */
    float y = newAngle - angle; // Angle difference

    ///if(y!=0) //This means the boundary condition has passed.
    //{
    //	y = newAngle - angle_temp;
    //}

    //different y has different ratio for new bias and new angle
    /*
    if(y<=0.5 && y >= -0.5)  //y in [-0.5, 0.5] use the default Kalman algorithm to calc bias and angle
    {
    	//angle += K[0] * y;
    	angle = angle_temp + K[0] * y;
      bias += K[1] * y;
      rate = rate_afterBias;
    }
    else // y in (-xxx, -0.5) and (0.5, +xxx) use the Multi-State Kalman algorithm
    {
    	rate = newRate - basicBias;
    	angle = angle + dt * rate;

    }
    */


    if(y>=10 || y<=-10) //
    {
    	 angle += K[0] * y/100;
       bias += K[1] * y/100;
    }
    else if((y>=5 && y<10) || (y<=-5 && y>-10))
    {
    	 angle += K[0] * y/70;
    	 bias += K[1] * y/70;
    }
    else if((y>=2 && y<5) || (y<=-2 && y>-5))
    {
    	 angle += K[0] * y/45;
    	 bias += K[1] * y/45;
    }
    else if((y>=1 && y<2) || (y<=-1 && y>-2))
    {
    	 angle += K[0] * y/18;
    	 bias += K[1] * y/18;
    }
    else if((y>=0 && y<1) || (y<=0 && y>-1))
    {
    	 angle += K[0] * y;
    	 bias += K[1] * y;
    }

    /* Step 6 */
    //angle += K[0] * y;
    //bias += K[1] * y;

    // Calculate estimation error covariance - Update the error covariance
    /* Step 7 */
    float P00_temp = P[0][0];
    float P01_temp = P[0][1];

    P[0][0] -= K[0] * P00_temp;
    P[0][1] -= K[0] * P01_temp;
    P[1][0] -= K[1] * P00_temp;
    P[1][1] -= K[1] * P01_temp;

    return angle;
};

void Kalman::setAngle(float angle) { this->angle = angle; }; // Used to set angle, this should be set as the starting angle
float Kalman::getRate() { return this->rate; }; // Return the unbiased rate
float Kalman::getBias() { return this->bias; };
float Kalman::getK0() { return this->K[0]; };
float Kalman::getK1() { return this->K[1]; };

/* These are used to tune the Kalman filter */
void Kalman::setQangle(float Q_angle) { this->Q_angle = Q_angle; };
void Kalman::setQbias(float Q_bias) { this->Q_bias = Q_bias; };
void Kalman::setRmeasure(float R_measure) { this->R_measure = R_measure; };

float Kalman::getQangle() { return this->Q_angle; };
float Kalman::getQbias() { return this->Q_bias; };
float Kalman::getRmeasure() { return this->R_measure; };

void Kalman::setBasicBias(float bBias) { this->basicBias = bBias; };


