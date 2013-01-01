#include "XC_yieldCurve.h"
#include <string>
double XC_yieldCurve::getRate(int periods, const char* name, double rate){
    std::string str = "SPREAD";
	std::map<int,double>::iterator itr = yield_map.begin();
	if(str.compare(name)==0){
      for(itr; itr!=yield_map.end(); itr++){
		  if(periods <= itr->first){
			  rate = rate/100 + itr->second;
			  break;
		  }else{
			  itr++;
		  }
	  }
	}
	return rate;
}

int XC_yieldCurve_file::get_record_count(){
  return XC_tradeBook_file::get_record_count();
}

/*XC_yieldCurve*
XC_yieldCurve_file::get_fields(){
   return _fields_array;
}*/

XC_yieldCurve* XC_yieldCurve_file::get_records(int& length){
    length = get_record_count();
    
	_fields_array = new XC_yieldCurve[length];
    
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
