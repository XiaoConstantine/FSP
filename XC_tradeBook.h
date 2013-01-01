/*************************************************************************
    > File Name: XC_tradeBook.h
    > Author: Xiao Cui
    > Mail: xc432@nyu.edu 
    > Created Time: Mon Oct  1 16:00:58 2012
 ************************************************************************/
#include "XC_io.h"
#include <map>
class XC_tradeBook: public XC_instrument_fields{
   public:
	   XC_tradeBook():XC_instrument_fields(){
		   Ticker[0] = 0;
		   RateT[0] = 0;
		   Quality[0] = 0;
		   amount = 0;
	   }
	   void show(); 
	   void set_bond(double);
	   void set_dv01(double);
	   void set_risk(double);
	   void set_lgd(double);

	   double get_bond();
	   double get_dv01();
	   double get_risk();
	   double get_lgd();

	   char Ticker[XC_INSTRUMENT_ID_LENGTH];
	   char RateT[XC_INSTRUMENT_ID_LENGTH];
	   char Quality[XC_INSTRUMENT_ID_LENGTH];
	   int amount;

	   std::map<int, double>amount_map;
	   std::map<double, double>risk_map;

   private:
	    double bond;
		double dv01;
		double risk;
		double lgd;
};

class XC_tradeBook_file: public XC_instrument_input_file{
	public:
		XC_tradeBook_file(const char* filename):XC_instrument_input_file(filename){

		}
		~XC_tradeBook_file();
		int get_record_count();
		XC_tradeBook* get_records(int&);
		void free_records();
		void writeToFile(const char*, int);
        XC_tradeBook* get_fields();
		char* writeToFile2(const char*, std::map<double,double>risk_map, std::map<int, double>);
	protected:
        XC_tradeBook *_fields_array; 
};
