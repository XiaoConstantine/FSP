/*************************************************************************
    > File Name: XC_caculator.h
    > Author: Xiao Cui
    > Mail: xc432@nyu.edu 
    > Created Time: Sun Sep 23 14:32:26 2012
 ************************************************************************/
#include "XC_date.h"
#include "XC_ratings.h"
class XC_calculator{
	public:
		XC_calculator();
		~XC_calculator(){
		}
	  	double XC_calculate(int fre,double c_rate, int maturity, double preva_rate, int time);
        double XC_dv01(int fre, double c_rate, int maturity, double preva_rate, int time);
        int get_number_of_periods_semiannual(XC_date from_dt, int para_F, XC_date to_dt);
	    double XC_risk(int fre, double c_rate, int maturity, double preva_rate, int time, int amount);
        double XC_lgd(char*, int amount);
        //
		//double XC_pnl(double bump);

		int para_F;
		double c_rate;
		int maturity;
		double preva_rate;
		int time;
        XC_date from_dt;
        XC_date to_dt;
		bool isZeroCoupon;
		XC_bond_ratings* rating;
        
};

