#include "mmain.h"

int main(int argc, char*argv[]){
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
    double *pnl_arr = new double[999];
    double pnl_arr_ir[999];
    double pnl_arr_cs[999];
    for(int i = 0; i<999; i++){
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
    
	XC_tradeBook *output_opening = input_opening->get_records(line);
    XC_tradeBook *output_closing = input_closing->get_records(line);
    XC_yieldCurve *output_yield = input_yield->get_records(line_yield);
    double (*hist_data)[999] = new double[line][999];
    double (*all_pnl)[999] = new double[line][999];
    T_number_for_bond = new int[line];
    double closing_price;
    get_his_data(hist_data, line, output_closing);
    double* result = new double[999];
    int shift_times = 0;
    
    struct sockaddr_in  client_sock_addr;
	socklen_t addrlen = sizeof(client_sock_addr);
	int sd_c;
	int ret;
	char test_msg[MSGSIZE];
    
//    while(1){
        int sd = createSocket();
        if((sd_c = accept(sd, (struct sockaddr*)&client_sock_addr,&addrlen)) == -1){
            fprintf(stderr, "XC accept(..) failed errno: %d exiting..\n", errno);
            exit(1);
        }
        printf("XC client ip address: %s port: %x\n", inet_ntoa(client_sock_addr.sin_addr), PORT);
    
        while(ret = recv(sd_c, test_msg, sizeof(test_msg), 0)>0){
            printf("XC server received msg: [%s] from client\n",test_msg);
            util.start_clock();
           
			if(test_msg[0] = 'Y'){

			}else if(test_msg[0] = 'S'){

			}else if(test_msg[0] = 'O'){

			}
			
			
			XC_calculator *cal = new XC_calculator();
           
			double hedge= 0;
			
			for(int i = 0; i<line_yield; ++i){
               int para_F = output_yield[i].Frequency;
               cal->from_dt.set_from_yyyymmdd(output_yield[i].SettlementDate);
               cal->to_dt.set_from_yyyymmdd(output_yield[i].MaturityDate);
               time_periods = cal->get_number_of_periods_semiannual(cal->from_dt,para_F,cal->to_dt);
               output_yield->yield_map.insert(std::make_pair(time_periods, output_yield[i].YieldRate));
               if(i == 1){
                   hedge = cal->XC_dv01(para_F, output_yield[i].CouponRate, MATURITY, output_yield[i].YieldRate, time_periods);
			   }
            }
           for(int i = 0; i<line; ++i){
               std::map<int, double>::iterator iter = output_yield->yield_map.begin();
               GUI_data open_data , close_data;

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
			  
			  if(time_periods/2 <= 2){
				  T_2_risk.push_back(output_closing[i].get_risk());
				  T_2_bond.push_back(output_closing[i].get_bond()*output_closing[i].amount/100);
			  }else if(time_periods/2 > 2 && time_periods/2 <=5){
                  T_5_risk.push_back(output_closing[i].get_risk());
				  T_5_bond.push_back(output_closing[i].get_bond()*output_closing[i].amount/100);
			  }else if(time_periods/2 >5 && time_periods/2 <=10){
                  T_10_risk.push_back(output_closing[i].get_risk());
				  T_10_bond.push_back(output_closing[i].get_bond()*output_closing[i].amount/100);
			  }else if(time_periods/2 >10 && time_periods/2 <=30){
                  T_30_risk.push_back(output_closing[i].get_risk());
				  T_30_bond.push_back(output_closing[i].get_bond()*output_closing[i].amount/100);
			  }

			  total_lgd_change+= output_closing[i].get_lgd() - output_opening[i].get_lgd();
			
			  open_data.amount = output_opening[i].amount;  
		      open_data.risk = output_opening[i].get_risk();  
		      open_data.lgd = output_opening[i].get_lgd();
              
			  close_data.amount = output_closing[i].amount;
		      close_data.risk = output_closing[i].get_risk();
	      	  close_data.lgd = output_closing[i].get_lgd();
	          
			  if(Gui_out_opening[output_opening[i].Ticker].size()==0){
				  std::vector<GUI_data> opening;
				  opening.push_back(close_data);
				  Gui_out_opening[output_opening[i].Ticker] = opening;
			  }else{
				  Gui_out_opening[output_opening[i].Ticker].push_back(open_data);
			  }
			  
              if(Gui_quality_opening[output_opening[i].Quality].size()==0){
				  std::vector<GUI_data> opening;
				  opening.push_back(open_data);
				  Gui_quality_opening[output_opening[i].Quality] = opening;
			  }else{
				  Gui_quality_opening[output_opening[i].Quality].push_back(open_data);
			  }
			  
			  if(Gui_out_closing[output_closing[i].Ticker].size()==0){
				  std::vector<GUI_data> closing;
				  closing.push_back(close_data);
				  Gui_out_closing[output_closing[i].Ticker] = closing;
			  }else{
				  Gui_out_closing[output_closing[i].Ticker].push_back(close_data);
			  }
              

              if(Gui_quality_closing[output_closing[i].Quality].size()==0){
				  std::vector<GUI_data> closing;
				  closing.push_back(close_data);
				  Gui_quality_closing[output_closing[i].Quality] = closing;
			  }else{
				  Gui_quality_closing[output_closing[i].Quality].push_back(close_data);
			  }

             closing_price = output_closing[i].get_bond();
              for(int k = 0; k<999; k++){
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
            
            pnl_arr[k]+=(new_price - closing_price)*output_closing[i].amount/100;
            pnl_arr_ir[k]+=(new_price_ir - closing_price)*output_closing[i].amount/100;
            pnl_arr_cs[k]+=(new_price_cs - closing_price)*output_closing[i].amount/100;
		  }

			pnl_arr_cp[k] = pnl_arr[k];
            /*if(i == line - 1){
                if(pnl_arr[k] < min)
                    min = pnl_arr[k];
                if(pnl_arr_ir[k]<min_ir)
                    min_ir = pnl_arr_ir[k];
                if(pnl_arr_cs[k]<min_cs)
                    min_cs = pnl_arr_cs[k];
            }*/
              
       }
    }
    qsort(pnl_arr, 999, sizeof(double), compare);
//    qsort(pnl_arr_cs, 999, sizeof(double), compare);

	int index = calIndexForPnl(999);
    double checkresult = pnl_arr[index];
    double pnl_ir_result;
    double pnl_cs_result;
	for(int i = 0 ; i < 999; i++){
		 if(pnl_arr_cp[i] == checkresult){
               pnl_cs_result = pnl_arr_cs[i];
			   pnl_ir_result = pnl_arr_ir[i];
		 }
	}

	printf("%f\n", pnl_arr_cs[index]);
	printf("%f\n", pnl_arr_ir[index]);
	
	std::stringstream ss;
	ss<<"tradingbook_opening\n";
    ss<<"Ticker:" <<"\t"<< "Amount:" <<"\t "<< "Risk:" <<"\t" << "Lgd: \n";
	

    std::stringstream ss1(std::stringstream::in|std::stringstream::out);
   
    mObject obj_open;
	mObject obj_close;
	mObject obj_quality_open;
	mObject obj_quality_close;
    mObject obj_change_book;
	mObject obj_change_quality;

	mObject obj_T_2;
    
	mObject obj_page_1;
    mObject obj_page_2;
    for(std::vector<double>::iterator itr = T_2_risk.begin(); itr!=T_2_risk.end(); itr++){
		T_2_risk_v += (*itr);
	}
    for(std::vector<double>::iterator itr = T_2_bond.begin(); itr!=T_2_bond.end(); itr++){
		T_2_bond_v += (*itr);
	}
	T_2_hedge_v = T_2_risk_v/hedge;


    for(std::vector<double>::iterator itr = T_5_risk.begin(); itr!=T_5_risk.end(); itr++){
		T_5_risk_v += (*itr);
	}
    for(std::vector<double>::iterator itr = T_5_bond.begin(); itr!=T_5_bond.end(); itr++){
		T_5_bond_v += (*itr);
	}
	T_5_hedge_v = T_5_risk_v/hedge;

    for(std::vector<double>::iterator itr = T_10_risk.begin(); itr!=T_10_risk.end(); itr++){
		T_10_risk_v += (*itr);
	}
    for(std::vector<double>::iterator itr = T_10_bond.begin(); itr!=T_10_bond.end(); itr++){
		T_10_bond_v += (*itr);
	}
	T_10_hedge_v = T_10_risk_v/hedge;

    for(std::vector<double>::iterator itr = T_30_risk.begin(); itr!=T_30_risk.end(); itr++){
		T_30_risk_v += (*itr);
	}
    for(std::vector<double>::iterator itr = T_30_bond.begin(); itr!=T_30_bond.end(); itr++){
		T_30_bond_v += (*itr);
	}
	T_30_hedge_v = T_30_risk_v/hedge;

    mArray arr_T_2;
	arr_T_2.push_back(mValue(T_2_risk_v));
    arr_T_2.push_back(mValue(T_2_bond_v));
	arr_T_2.push_back(mValue(T_2_hedge_v));
	obj_T_2["2YR"] = arr_T_2;
    
    mArray arr_T_5;
	arr_T_5.push_back(mValue(T_5_risk_v));
    arr_T_5.push_back(mValue(T_5_bond_v));
	arr_T_5.push_back(mValue(T_5_hedge_v));
	obj_T_2["5YR"] = arr_T_5;
    
	mArray arr_T_10;
	arr_T_10.push_back(mValue(T_10_risk_v));
    arr_T_10.push_back(mValue(T_10_bond_v));
	arr_T_10.push_back(mValue(T_10_hedge_v));
	obj_T_2["10YR"] = arr_T_10;

    mArray arr_T_30;
	arr_T_30.push_back(mValue(T_30_risk_v));
    arr_T_30.push_back(mValue(T_30_bond_v));
	arr_T_30.push_back(mValue(T_30_hedge_v));
	obj_T_2["30YR"] = arr_T_30;

	obj_open = processForGUI(Gui_out_opening);
	obj_close = processForGUI(Gui_out_closing);
    obj_quality_open = processForGUI(Gui_quality_opening);
	obj_quality_close = processForGUI(Gui_quality_closing);
    
	for(mObject::iterator itr = obj_quality_close.begin(); itr!=obj_quality_close.end(); itr++){
		mObject::iterator objIter = obj_quality_open.find(itr->first);
		if(objIter == obj_quality_open.end()){
            mArray arr;
			arr.push_back(0);
			arr.push_back(0);
			arr.push_back(0);
			obj_quality_open[itr->first] = arr;
		}
	}

    obj_change_book = calChange(obj_open, obj_close);
    obj_change_quality = calChange(obj_quality_open, obj_quality_close);
    
	mArray arr_T, arr_Q, arr_V;
	arr_T.push_back(obj_open);
	arr_T.push_back(obj_close);
	arr_T.push_back(obj_change_book);

	arr_Q.push_back(obj_quality_open);
	arr_Q.push_back(obj_quality_close);
	arr_Q.push_back(obj_change_quality);
	
    arr_V.push_back(mValue(pnl_cs_result));
	arr_V.push_back(mValue(pnl_ir_result));
	arr_V.push_back(mValue(pnl_arr[index]));

   // mArray page_array;
	for(mObject::iterator itr = obj_close.begin(); itr!=obj_close.end(); itr++ ){
		mObject::iterator objIter = obj_open.find(itr->first);
		if(objIter!=obj_open.end()){
			mArray *arr = &(*objIter).second.get_array();
			mArray arr_to = (*itr).second.get_array();
			arr->push_back(mValue(arr_to.at(0).get_int()));
			arr->push_back(mValue(arr_to.at(1).get_real()));
			arr->push_back(mValue(arr_to.at(2).get_real()));
		}
	}
    for(mObject::iterator itr = obj_change_book.begin(); itr!=obj_change_book.end(); itr++ ){
		mObject::iterator objIter = obj_open.find(itr->first);
		if(objIter!=obj_open.end()){
			mArray *arr = &(*objIter).second.get_array();
			mArray arr_to = (*itr).second.get_array();
			arr->push_back(mValue(arr_to.at(0).get_int()));
			arr->push_back(mValue(arr_to.at(1).get_real()));
			arr->push_back(mValue(arr_to.at(2).get_real()));
		}
	}
    mObject passObject;
	passObject["Ticker"] = obj_open;
	
	for(mObject::iterator itr = obj_quality_close.begin(); itr!=obj_quality_close.end(); itr++ ){
		mObject::iterator objIter = obj_quality_open.find(itr->first);
		if(objIter!=obj_open.end()){
			mArray *arr = &(*objIter).second.get_array();
			mArray arr_to = (*itr).second.get_array();
			arr->push_back(mValue(arr_to.at(0).get_int()));
			arr->push_back(mValue(arr_to.at(1).get_real()));
			arr->push_back(mValue(arr_to.at(2).get_real()));
		}
	}
			
    for(mObject::iterator itr = obj_change_quality.begin(); itr!=obj_change_quality.end(); itr++ ){
		mObject::iterator objIter = obj_quality_open.find(itr->first);
		if(objIter!=obj_open.end()){
			mArray *arr = &(*objIter).second.get_array();
			mArray arr_to = (*itr).second.get_array();
			arr->push_back(mValue(arr_to.at(0).get_int()));
			arr->push_back(mValue(arr_to.at(1).get_real()));
			arr->push_back(mValue(arr_to.at(2).get_real()));
		}
	}
	
	passObject["Quality"] = obj_quality_open;
    
	mObject total;
    mArray arr;
	arr.push_back(pnl_cs_result);
	arr.push_back(pnl_ir_result);
	arr.push_back(pnl_arr[index]);
	total["total"] = arr;

	
	passObject["Var"] = total;

	 double real_time = 0;
	 double user_time = 0;
	 double system_time = 0;
     util.end_clock(real_time, user_time, system_time);
     passObject["real time"] = real_time;
	 passObject["user_time"] = user_time;
	 passObject["system_time"] = system_time;

	 obj_T_2["real time"] = real_time;
	 obj_T_2["user_time"] = user_time;
	 obj_T_2["system_time"] = system_time;
	 write(passObject, ss1, single_line_arrays);		
    //write(obj_open, ss1, pretty_print);
	//write(obj_quality_open,ss1, pretty_print);
	//write(obj_quality_close, ss1, pretty_print);
	//write(obj_change_book, ss1, pretty_print);
	//write(obj_change_quality, ss1, pretty_print);
	//write(obj_T_2, ss1, pretty_print);
//	write(obj_page_1, ss1, pretty_print);
	write(obj_T_2, ss1, single_line_arrays);
	std::cout<<ss1.str()<<std::endl;
	
	ss<<"Total lgd change :\n";
	ss<<total_lgd_change;
	ss<<"|\n";
	ss<<"pnl:\n";
	ss<<pnl_arr[9];
	ss<<"\n";
	ss<<"pnl_cs:\n";
	ss<<pnl_arr_cs[9];
	ss<<"\n";
	ss<<"pnl_ir:\n";
	ss<<pnl_arr_ir[9];
	ss<<"\n";
	ss<<"pnl index:\n";
	ss<<index;
	ss<<"|\n";
    END_TIMER(util);

     if(send(sd_c, ss1.str().c_str(), strlen(ss1.str().c_str()),0)==-1){
        fprintf(stderr, "Send failed!", errno);
        exit(1);
    }
    close(sd_c);
	close(sd);
   }
    input_opening->free_records();
    input_closing->free_records();
    delete[] hist_data;
    delete pnl_arr;
    delete T_number_for_bond;
   
	if(0 == ret){
			printf("XC client exited...\n");
	}else if(-1 == ret){
		fprintf(stderr, "XC recv(....) returned failed - errno: %d exiting....\n", errno);
		exit(1);
	}
	END_TIMER(util);
	return 0;
}
