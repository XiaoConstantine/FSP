/*************************************************************************
    > File Name: XC_yieldCurve.h
    > Author: Xiao Cui
    > Mail: xc432@nyu.edu 
    > Created Time: Mon Oct 22 14:52:27 2012
 ************************************************************************/
#include "XC_tradeBook.h"

class XC_yieldCurve: public XC_tradeBook{
   public:
	   std::map<int, double>yield_map;
	   XC_yieldCurve(): XC_tradeBook(){

	   }
	   double getRate(int, const char*, double);
};

class XC_yieldCurve_file: public XC_tradeBook_file{
   public:
      XC_yieldCurve_file(const char* filename):XC_tradeBook_file(filename){

	  }
	  ~XC_yieldCurve_file();
	  int get_record_count();
	  XC_yieldCurve* get_records(int&);
//	  XC_yieldCurve* get_fields();
   protected:
	 XC_yieldCurve * _fields_array;
};
