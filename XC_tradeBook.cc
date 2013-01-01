#include "XC_tradeBook.h"
#include <stdio.h>
void XC_tradeBook::show(){
	printf("ID: %s Ticker: %s SettleDate: %d Coupon: %.2f MatDate: %d Frequency: %d RateType: %s Rate: %.3f Quality: %s Amount: %d\n",
			SecurityID,
			Ticker,
			SettlementDate,
			CouponRate,
			MaturityDate,
			Frequency,
			RateT,
			YieldRate,
			Quality,
			amount
			);
}

void XC_tradeBook::set_bond(double tmp){
    bond = tmp;
}

void XC_tradeBook::set_dv01(double tmp){
	dv01 = tmp;
}

void XC_tradeBook::set_risk(double tmp){
	risk = tmp;
}

void XC_tradeBook::set_lgd(double tmp){
	lgd = tmp;
}

double XC_tradeBook::get_bond(){
	return bond;
}

double XC_tradeBook::get_dv01(){
	return dv01;
}

double XC_tradeBook::get_risk(){
	return risk;
}

double XC_tradeBook::get_lgd(){
	return lgd;
}

char*
XC_tradeBook_file::writeToFile2(const char* filename, std::map<double, double>risk_map, std::map<int, double>amount_map){
     FILE* file = std::fopen(filename, "w");
    char result[128];
     std::map<double, double>::iterator iter = risk_map.begin();
	 std::map<double, double>::reverse_iterator riter = risk_map.rbegin();
     std::map<int, double>::iterator itr = amount_map.begin();
	 std::map<int, double>::reverse_iterator rit = amount_map.rbegin();
	 double total_risk = 0.0;
	 double most_risk = 0.0;
	 int largest_long_p = 0;
	 int largest_short_p = 0;
	 largest_long_p = rit->first;
	 largest_short_p = itr->first;
     
     most_risk = riter->first;
	 for(iter; iter!=risk_map.end(); ++iter){
         total_risk += iter->first;
	 }
     std::fprintf(_file,"%d\n%d\n%3.3f\n%3.3f\n", largest_long_p, largest_short_p, most_risk, total_risk);
    std::sprintf(result,"%d\n%d\n%3.3f\n%3.3f\n", largest_long_p, largest_short_p, most_risk, total_risk );
     std::fclose(file);
    return result;
}

void
XC_tradeBook_file::writeToFile(const char* filename, int line){  
	  FILE* file = std::fopen(filename, "w");
	  int i = 0;
	  for(i;i<line;++i){
      double bond = _fields_array[i].get_bond();
	  double dv01 = _fields_array[i].get_dv01();
	  double risk = _fields_array[i].get_risk();
	  double lgd = _fields_array[i].get_lgd();
      std::fprintf(_file, "%s %s %d %f %d %d %s %f %s %d %3.3f %3.3f %3.3 %3.3\n ", _fields_array[i].SecurityID, _fields_array[i].Ticker, _fields_array[i].SettlementDate, _fields_array[i].CouponRate, _fields_array[i].MaturityDate, _fields_array[i].Frequency, _fields_array[i].RateT, _fields_array[i].YieldRate, _fields_array[i].Quality, _fields_array[i].amount, bond, dv01, risk, lgd);	
	  }
	  std::fclose(file);

}


XC_tradeBook*
XC_tradeBook_file::get_fields(){
   return _fields_array; 
}

int XC_tradeBook_file::get_record_count(){
    int line_count_minus_comments = 0;
	int comment_count = 0;
	while(fgets(_line_buf, XC_LINE_BUFFER_LENGTH, _file)){
		if('#' == _line_buf[0]){
			comment_count++;
			continue;
		}
		++line_count_minus_comments;
	}
    rewind(_file);
	return line_count_minus_comments;
}

void 
XC_tradeBook_file::free_records(){
	delete[] _fields_array;
}

XC_tradeBook* XC_tradeBook_file::get_records(int& length){
    length = get_record_count();
    
	_fields_array = new XC_tradeBook[length];
    
	if(NULL == _fields_array){
		fprintf(stderr,"calloc failed - errno: %d\n", errno);
		return _fields_array;
	}
	int comment_count = 0;
    
	_line_buf[0] = ' ';
	int non_comments_line_count_read = 0;
	char *token;
	while(std::fgets(_line_buf,XC_LINE_BUFFER_LENGTH,_file)){
        
		//printf("XC CHECK file line: %s\n", _line_buf);
        
		if('#' == _line_buf[0]) {
			continue; // skip through comments
			comment_count++;
		}
        
		// the line should be:
		// SecurityID Ticker  SettlementDate CouponRate MaturityDate Frequency RateType YieldRate Quality Amount
        
		// SecurityID
		token = strtok(_line_buf," ");
		if(NULL == token) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("SecurityID: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].SecurityID, token);
        
		// Ticker
		token = strtok(NULL, " ");
		if(NULL == token) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("SecurityID: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Ticker, token);
        
        
		// SettlementDate
		token = strtok(NULL," ");
		if(NULL == token) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("SettlementDate: %s\n", token);
		_fields_array[non_comments_line_count_read].SettlementDate = atoi(token);
        
		// CouponRate
		token = strtok(NULL," ");
		if(NULL == token ) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("CouponRate: %s\n", token);
		_fields_array[non_comments_line_count_read].CouponRate = atof(token);
        
		// MaturityDate
		token = strtok(NULL," ");
        if(NULL == token){
            fprintf(stderr, "line parsing failed on buf: %s\n", _line_buf);
            exit(1);
        }
		//printf("MaturityDate: %s\n", token);
		_fields_array[non_comments_line_count_read].MaturityDate = atoi(token);
        
		// Frequency
		token = strtok(NULL," ");
        if(NULL == token){
            fprintf(stderr, "line parsing failed on buf: %s\n", _line_buf);
            exit(1);
        }
		//printf("Frequency: %s\n", token);
		_fields_array[non_comments_line_count_read].Frequency = atoi(token);


		token = strtok(NULL, " ");
		if(NULL == token){
            fprintf(stderr, "line parsing failed on buf: %s\n",_line_buf);
			exit(1);
		}
		strcpy(_fields_array[non_comments_line_count_read].RateT, token);

		// Yield (later we will load in spread and a string token of an OTR. Also will have a separate file
        
		token = strtok(NULL," ");
		if(NULL == token ) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("YieldRate is:  %s\n", token);
		_fields_array[non_comments_line_count_read].YieldRate = atof(token);
       
 
		// Quality
		token = strtok(NULL," ");
		if(NULL == token) {
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
		//printf("Quality: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Quality, token);
        
		token = strtok(NULL, " ");
		if(NULL == token){
			fprintf(stderr, "line parsing failed on buf: %s\n", _line_buf);
			exit(1);
		}
        _fields_array[non_comments_line_count_read].amount = atoi(token);
		non_comments_line_count_read++;
	}
    
	//printf("XC lines read: %d \n", non_comments_line_count_read);
    
	return _fields_array;
}

