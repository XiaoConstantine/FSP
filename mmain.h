/*************************************************************************
    > File Name: mmain.h
    > Author: Xiao Cui
    > Mail: xc432@nyu.edu 
    > Created Time: Tue Dec 11 17:34:35 2012
 ************************************************************************/

#include "XC_util.h"
#include "XC_socket.h"
#include "XC_calculator.h"
#include "XC_yieldCurve.h"
#include "json_spirit/json_spirit.h"
#include <map>
#include <math.h>
#include <vector>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#define MATURITY 100
using namespace json_spirit;
static XC_util util;
double t2[1000], t5[1000], t10[1000], t30[1000], t2_chg[999], t5_chg[999], t10_chg[999], t30_chg[999];
int * T_number_for_bond;
void get_T_yields(char* filename, double* arr, double* arr_chg);
void get_his_data(double[][999], int length, XC_tradeBook* output);
double yield_arr[1000];
double min = 0;
double min_ir = 0;
double min_cs = 0;
double pnl_arr[999];
double total_lgd_change = 0;
double total_amount_change = 0;
double yields_arr[1000];
double pnl_arr_cp[999];
double pnl_arr_cs_cp[999];
double pnl_arr_ir_cp[999];

double T_2_risk_v = 0;
double T_2_bond_v = 0;
double T_2_hedge_v = 0;
double T_5_risk_v = 0;
double T_5_bond_v = 0;
double T_5_hedge_v = 0;
double T_10_risk_v = 0;
double T_10_bond_v = 0;
double T_10_hedge_v = 0;
double T_30_risk_v = 0;
double T_30_bond_v = 0;
double T_30_hedge_v = 0;
std::vector<double> T_2_risk;
std::vector<double> T_5_risk;
std::vector<double> T_10_risk;
std::vector<double> T_30_risk;
std::vector<double> T_2_bond;
std::vector<double> T_5_bond;
std::vector<double> T_10_bond;
std::vector<double> T_30_bond;
std::vector<double> T_2_hedge;
std::vector<double> T_5_hedge;
std::vector<double> T_10_hedge;
std::vector<double> T_30_hedge;

typedef struct GUI_data{
	int amount;
	double risk;
	double lgd;
    
	GUI_data& operator+=(const GUI_data& c){
		amount += c.amount;
		risk += c.risk;
		lgd += c.lgd;
		return *this;
	}

	GUI_data& operator=(const GUI_data& c){
        amount = c.amount;
		risk = c.risk;
		lgd = c.lgd;
		return *this;
	}
  
};


std::map<std::string, std::vector<GUI_data> >Gui_out_opening;
std::map<std::string, std::vector<GUI_data> >Gui_out_closing;
std::map<std::string, std::vector<GUI_data> >Gui_quality_opening;
std::map<std::string, std::vector<GUI_data> >Gui_quality_closing;

int compare(const void*a, const void*b){
	return *(double*)a > *(double*)b ? 1 : -1;
}

/*double processT_2(std::vector<double> map){
   for(std::vector<double>::iterator itr = T_10_risk.begin(); itr!=T_10_risk.end(); itr++){
		T_10_risk_v += (*itr);
    }
   for(std::vector<double>::iterator itr = T_10_bond.begin(); itr!=T_10_bond.end(); itr++){
		T_10_bond_v += (*itr);
   }
}*/


mObject processForGUI(std::map<std::string, std::vector<GUI_data> > map){
    mObject obj;
	for(std::map<std::string, std::vector<GUI_data> >::iterator itr = map.begin(); itr!= map.end(); itr++){
       mArray arr;
	   GUI_data tmp;
	   tmp.amount = 0;
	   tmp.risk = 0;
	   tmp.lgd = 0;
	   std::string str = itr->first;
	   for(std::vector<GUI_data>::iterator itr2 = itr->second.begin(); itr2!=itr->second.end(); itr2++){
	       GUI_data tmp1 = *itr2;
		   tmp+= tmp1;
	   }
	   arr.push_back(mValue(tmp.amount));
	   arr.push_back(mValue(tmp.risk));
	   arr.push_back(mValue(tmp.lgd));
	   obj[str] = arr;
	}
	return obj;
}

mObject calChange(mObject o1, mObject o2){
    mObject::iterator itr;
	mObject::iterator itr2;
    
	mObject obj;
    
	for(itr2 = o2.begin(); itr2!=o2.end(); itr2++){
		if(o1.find(itr2->first) != o1.end()){
			continue;
		}else{
			mArray arr;
			arr.push_back(mValue(0));
			arr.push_back(mValue(0));
			arr.push_back(mValue(0));
			o1[itr2->first] = arr;
		}
	}
    

    for(itr = o1.begin(), itr2 = o2.begin(); itr!=o1.end()&&itr2!=o2.end(); itr++, itr2++){
		 mArray arrChange;
		 mArray arrOpen = (*itr).second.get_array();
		 mArray arrClose = (*itr2).second.get_array();
		 arrChange.push_back(mValue(arrClose.at(0).get_int() - arrOpen.at(0).get_int()));
		 arrChange.push_back(mValue(arrClose.at(1).get_real() - arrOpen.at(1).get_real()));
		 arrChange.push_back(mValue(arrClose.at(2).get_real() - arrOpen.at(2).get_real()));
		 obj[itr2->first] = arrChange;
	  }          

	return obj;
}

int calIndexForPnl(int amount){
	return floor(amount - amount*0.99);
}


int createSocket(){
	int sd;
	char hostname[128];
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("create socket error!");
		exit(1);
	}
    
	struct sockaddr_in sock_addr;
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
#ifdef XC_ANY
	sock_addr.sin_addr.s_addr = INADDR_ANY;
#else
	if(gethostname(hostname, sizeof(hostname))){
		fprintf(stderr, "Get hostname failed errno: %d\n", errno);
		exit(1);
	}
	printf("XC gethostname() local hostname: \"%s\"\n", hostname);
	struct hostent *hp;
	if((hp = gethostbyname(hostname))==0){
		fprintf(stderr, "XC gethostname(...) failed errno: %d exiting...\n", errno);
		exit(1);
	}
	sock_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
#endif
	sock_addr.sin_port = htons(PORT);
    
	if(bind(sd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
		perror("bind error");
		exit(1);
	}
    
	if(listen(sd, 5) == -1){
		perror("listen error");
		exit(1);
	}
	return sd;
    
}


void get_his_data(double arr[][999], int length, XC_tradeBook* output){
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
        for(int j = 0; j<999;j++){
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
	for(int i = 0; i<1000; i++){
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


