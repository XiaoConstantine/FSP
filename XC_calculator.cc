#include "XC_calculator.h"
#include <stdio.h>
#include <cmath>


XC_calculator::XC_calculator():para_F(0), c_rate(0),maturity(0),preva_rate(0),time(0), isZeroCoupon(false), rating(new XC_bond_ratings){
}
int XC_calculator::get_number_of_periods_semiannual(XC_date from_dt, int para_F, XC_date to_dt){
    int semiannual_period_count = 0;
	while (from_dt != to_dt) {
		to_dt.add_months((-12)/para_F);
		semiannual_period_count++;
	}
	return semiannual_period_count;
}

double XC_calculator::XC_calculate(int fre,double c_rate,int maturity,double preva_rate,int time){
    double coupon =(c_rate/100)*maturity;
    double YTM = preva_rate/100;
	short periods = time;
	if(0.0 != coupon){
       double value = pow((1+YTM/fre), periods);
	   return coupon*(1 - 1/value)/YTM + maturity/value;
	}else{
	   double value = pow((1+YTM/fre), periods);
       isZeroCoupon = true;
	   return maturity/value;
	}
	
}	

double XC_calculator::XC_dv01(int fre, double c_rate, int maturity, double preva_rate, int time){
    double num_1 = XC_calculate(fre,c_rate, maturity, preva_rate- 0.01, time);
	double num_2 = XC_calculate(fre,c_rate, maturity, preva_rate+ 0.01, time);
	double price_minus = num_1 - num_2;
    double sum = price_minus;

	return sum/2; 
}

double XC_calculator::XC_risk(int fre, double c_rate, int maturity, double preva_rate, int time, int amount){
	double dv1 =  XC_dv01(fre, c_rate, maturity, preva_rate, time)/100;
	return dv1*amount;
}

double XC_calculator::XC_lgd(char* quality, int amount){
	return rating->LGD_given_SnP_Fitch(quality)*amount;
}
