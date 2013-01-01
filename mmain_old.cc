#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <cmath>
#include <map>
#include <string>
#include "XC_util.h"
#include "XC_calculator.h"
#include "XC_yieldCurve.h"
#define MATURITY 100
static XC_util util;
double t2[100], t5[100], t10[100], t30[100], t2_chg[99], t5_chg[99], t10_chg[99], t30_chg[99];
int * T_number_for_bond;
void get_T_yields(char* filename, double* arr, double* arr_chg);
void get_his_data(double[][99], int length, XC_tradeBook* output);
double yield_arr[100];
double min = 0;
double min_ir = 0;
double min_cs = 0;
double pnl_arr[99];
double total_lgd_change = 0;
double total_amount_change = 0;
double yields_arr[100];


int compare(const void*a, const void*b){
	return ((*(double*)a - *(double*)b>0)?1:-1);
}

int main(int argc, char*argv[]){
 START_TIMER(util);

 int time_periods = 0;   
 char tradebook_opening[50];
 char tradebook_closing[50];
 char yield[20];

 strcpy(tradebook_opening, argv[1]);
 strcpy(tradebook_closing, argv[2]);
 strcpy(yield, argv[3]);

 XC_tradeBook_file *input_opening = new XC_tradeBook_file(tradebook_opening);
 XC_tradeBook_file *input_closing = new XC_tradeBook_file(tradebook_closing);
 XC_yieldCurve_file *input_yield = new XC_yieldCurve_file(yield);
 double *pnl_arr = new double[99];
 double pnl_arr_ir[99];
 double pnl_arr_cs[99];
 for(int i = 0; i<99; i++){
	 pnl_arr[i] = 0;
	 pnl_arr_ir[i] = 0;
	 pnl_arr_cs[i] = 0;
 }

 get_T_yields("hist_files/T2.txt", t2, t2_chg);
 get_T_yields("hist_files/T5.txt", t5, t5_chg);
 get_T_yields("hist_files/T10.txt", t10, t10_chg);
 get_T_yields("hist_files/T30.txt", t30, t30_chg);
 int line = input_opening->get_record_count();
 int line_yield = input_yield->get_record_count();
 /*for(int i = 0; i<sizeof(t2)/sizeof(double); i++){
	 printf("%3.3f ", t2[i]);
 }
 for(int i = 0; i<sizeof(t2_chg)/sizeof(double); i++){
	 printf("%3.3f ", t2_chg[i]);
 }*/
 
 XC_tradeBook *output_opening = input_opening->get_records(line);
 XC_tradeBook *output_closing = input_closing->get_records(line);
 XC_yieldCurve *output_yield = input_yield->get_records(line_yield); 
 double (*hist_data)[99] = new double[line][99];
 double (*all_pnl)[99] = new double[line][99];
 T_number_for_bond = new int[line];
 double closing_price;
 get_his_data(hist_data, line, output_closing);

/* for(int i = 0; i<line; i++){
//	 for(int j = 0; j<99; j++){
	 printf("%3.3f\n ", hist_data[i][1]);
//	 }
 }
 for(int i = 0; i<99; i++){
	 pnl_arr[j] = 0;
 }*/


  XC_calculator *cal = new XC_calculator();
  for(int i = 0; i<line_yield; ++i){
     int para_F = output_yield[i].Frequency;
	 cal->from_dt.set_from_yyyymmdd(output_yield[i].SettlementDate);
     cal->to_dt.set_from_yyyymmdd(output_yield[i].MaturityDate);
     time_periods = cal->get_number_of_periods_semiannual(cal->from_dt,para_F,cal->to_dt);
	 output_yield->yield_map.insert(std::make_pair(time_periods, output_yield[i].YieldRate));
  } 
  for(int i = 0; i<line; ++i){
     std::map<int, double>::iterator iter = output_yield->yield_map.begin();
	 int para_F = output_closing[i].Frequency;
	 double y_rate = 0.0;
     cal->from_dt.set_from_yyyymmdd(output_closing[i].SettlementDate);
     cal->to_dt.set_from_yyyymmdd(output_closing[i].MaturityDate);
     time_periods = cal->get_number_of_periods_semiannual(cal->from_dt,para_F,cal->to_dt);
	 output_opening[i].YieldRate = output_yield->getRate(time_periods, output_opening[i].RateT, output_opening[i].YieldRate);
	 output_closing[i].YieldRate = output_yield->getRate(time_periods, output_closing[i].RateT, output_closing[i].YieldRate);
	 
	 output_opening[i].set_bond(cal->XC_calculate(para_F, output_opening[i].CouponRate, MATURITY, output_opening[i].YieldRate, time_periods));
     output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate, time_periods));
	 
	 output_opening[i].set_dv01(cal->XC_dv01(para_F, output_opening[i].CouponRate, MATURITY, output_opening[i].YieldRate, time_periods));
	 output_closing[i].set_dv01(cal->XC_dv01(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate, time_periods));
	 
	 output_opening[i].set_risk(cal->XC_risk(para_F, output_opening[i].CouponRate, MATURITY, output_opening[i].YieldRate, time_periods,output_opening[i].amount)); 
	 output_closing[i].set_risk(cal->XC_risk(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate, time_periods,output_closing[i].amount));
	 
	 output_opening[i].set_lgd(cal->XC_lgd(output_opening[i].Quality, output_opening[i].amount));
     output_closing[i].set_lgd(cal->XC_lgd(output_closing[i].Quality, output_closing[i].amount));
     closing_price = output_closing[i].get_bond();

     for(int k = 0; k<99; k++){
		 double new_price, new_price_ir, new_price_cs;
		 if(T_number_for_bond[i] == 0){
			 output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100,time_periods));
			 new_price = output_closing[i].get_bond();
		     pnl_arr[k]+=(new_price-closing_price)*output_closing[i].amount/100;
			 pnl_arr_ir[k]+=(new_price-closing_price)*output_closing[i].amount/100;
		 }else{
        	 output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100,time_periods));
		     new_price = output_closing[i].get_bond();
			 if(T_number_for_bond[i]==2){
				output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100 - t2_chg[k],time_periods));
			    new_price_cs = output_closing[i].get_bond();
			    output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate + t2_chg[k],time_periods));
			    new_price_ir = output_closing[i].get_bond();
			 } else if(T_number_for_bond[i]==5){
				output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100 - t5_chg[k],time_periods));
			    new_price_cs = output_closing[i].get_bond();
			    output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate + t5_chg[k],time_periods));
			    new_price_ir = output_closing[i].get_bond();
			 } else if(T_number_for_bond[i]==10){
				output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100 - t10_chg[k],time_periods));
			    new_price_cs = output_closing[i].get_bond();
			    output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate + t10_chg[k],time_periods));
			    new_price_ir = output_closing[i].get_bond();
			 } else if(T_number_for_bond[i]==30){
				output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate+hist_data[i][k]/100 - t30_chg[k],time_periods));
			    new_price_cs = output_closing[i].get_bond();
			    output_closing[i].set_bond(cal->XC_calculate(para_F, output_closing[i].CouponRate, MATURITY, output_closing[i].YieldRate + t30_chg[k],time_periods));
			    new_price_ir = output_closing[i].get_bond();
			 }
		 }
		 pnl_arr[k]+=(new_price - closing_price)*output_closing[i].amount/100;
		 pnl_arr_ir[k]+=(new_price_ir-closing_price)*output_closing[i].amount/100;
		 pnl_arr_cs[k]+=(new_price_cs-closing_price)*output_closing[i].amount/100;
         
		 if(i == line - 1){
			 if(pnl_arr[k] < min)
				 min = pnl_arr[k];
			 if(pnl_arr_ir[k]<min_ir)
				 min_ir = pnl_arr_ir[k];
			 if(pnl_arr_cs[k]<min_cs)
				 min_cs = pnl_arr_cs[k];
		 }
         //qsort(pnl_arr, 99, sizeof(double), compare);
		 
		// all_pnl[i][k] = pnl_arr[k];
	 }

    qsort(pnl_arr, 99, sizeof(double), compare);
	 /*
	 output->amount_map.insert(std::make_pair(output[i].amount, output[i].get_bond()));
     output->risk_map.insert(std::make_pair(output[i].get_risk(), output[i].get_bond())); 	 
 */ 
	 for(int k = 0; k<99; k++){
       all_pnl[i][k] = pnl_arr[k];
	 }

	 total_lgd_change+=output_closing[i].get_lgd()*output_closing[i].amount-output_opening[i].get_lgd()*output_opening[i].amount;
     total_amount_change+=output_closing[i].amount-output_opening[i].amount;
 }
 /*input->writeToFile("tradingbook.txt", line);
 input->writeToFile2("results.txt", output->risk_map, output->amount_map);
 */
 //printf("%3.3f\n", total_lgd_change);
 for(int k = 0; k<line; k++){
  for(int i =0; i<99; i++){
   printf("%3.3f\n", all_pnl[k][i]);
  }
  printf("------------------------");
 }
 //printf("------------------");
// printf("%3.3f\n", min);
 input_opening->free_records();
 input_closing->free_records();
 delete[] hist_data;
 delete pnl_arr;
 delete T_number_for_bond;
 END_TIMER(util);
 return 0;
}


void get_his_data(double arr[][99], int length, XC_tradeBook* output){
	for(int i = 0; i<length; i++){
		char filename[100];
		strcpy(filename, "hist_files/");
		strncat(filename, output[i].SecurityID, strlen(output[i].SecurityID));
		strncat(filename, ".txt", 4);
	    char _line_buf[256];
		_line_buf[0] = '0';
		bool is_spread = false;
		FILE* _file = fopen(filename, "r");
		char* token;
		double x = 0;
		double pre_yield = 0;
		if(!_file){
			fprintf(stderr, "cannot open file: %s\n", filename);
			throw std::runtime_error("open file failure");
		}
		fgets(_line_buf, 256, _file);
		token = strtok(_line_buf," ");
		token = strtok(NULL, " ");
		if(is_spread==false&&strcmp(token, "SPREAD")==0)
			is_spread = true;
		token = strtok(NULL, " ");
		if(!is_spread){
			pre_yield = atof(token);
			T_number_for_bond[i] = 0;
		}else{
			x = atof(token)/100;
			token = strtok(NULL, " ");
			token[strlen(token)-1]='\0';
			if(!strcmp(token, "T2")){
				x+=t2[0];
				T_number_for_bond[i] = 2;
			}else if(!strcmp(token, "T5")){
				x+=t5[0];
				T_number_for_bond[i] = 5;
			}else if(!strcmp(token, "T30")){
				x+=t30[0];
				T_number_for_bond[i]=30;
			}else throw std::runtime_error("no match T file");
			pre_yield = x;
		}
	  for(int j = 0; j<99;j++){
		  fgets(_line_buf, 256, _file);
		  token = strtok(_line_buf, " ");
		  token = strtok(NULL, " ");
		  if(is_spread == false&&strcmp(token, "SPREAD")==0)
			  is_spread = true;
		  token = strtok(NULL, " ");
		  if(!is_spread){
			  arr[i][j] = (atof(token)-pre_yield)*100;
			  pre_yield = atof(token);
		  }else{
			  x = atof(token)/100;
			  token = strtok(NULL, " ");
			  token[strlen(token)-1]='\0';
			  if(!strcmp(token, "T2"))
				  x+=t2[j+1];
			  else if(!strcmp(token, "T5"))
				  x+=t5[j+1];
			  else if(!strcmp(token, "T10"))
				  x+=t10[j+1];
			  else if(!strcmp(token, "T30"))
				  x+=t30[j+1];
			  else throw std::runtime_error("no match T file");
			  arr[i][j] = (x-pre_yield)*100;
			  pre_yield = x;
		  }
	  }
		fclose(_file);
	}

}


void get_T_yields(char* filename, double* arr, double* arr_chg){
	FILE* _Tfile = fopen(filename, "r");
	if(!_Tfile){
		fprintf(stderr, "cannot open file:%s\n", filename);
		throw std::runtime_error("open file failure");
	}
	char *token;
	char _line_buf[256];
	_line_buf[0] = '\0';
	double pre_yield = 0;
	for(int i = 0; i<100; i++){
		fgets(_line_buf, 256, _Tfile);
		token=strtok(_line_buf," ");
		token=strtok(NULL, " ");
		token=strtok(NULL, " ");
		arr[i] = atof(token);
		if(i!=0)
            arr_chg[i-1]=arr[i]-pre_yield;
		pre_yield = arr[i];
	}
	fclose(_Tfile);
}

