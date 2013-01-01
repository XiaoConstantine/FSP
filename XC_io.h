#include <sys/errno.h>
#include <stdio.h>
#include <stdexcept>

#define XC_LINE_BUFFER_LENGTH 256
#define XC_INSTRUMENT_ID_LENGTH 32

// wide open class for loading data per instrument
class XC_instrument_fields {
	public:
		XC_instrument_fields(){
			SecurityID[0]=0;
			SettlementDate=0;
			CouponRate=0.0;
			MaturityDate=0;
			Frequency=0;
			YieldRate=0.0;
		} 
		~XC_instrument_fields() {};

	virtual	void show(){
			printf("ID: %s SettleDate: %d Coupon: %.2f MatDate: %d Frequency: %d Yield: %.3f\n",
				SecurityID,
				SettlementDate,
				CouponRate,
				MaturityDate,
				Frequency,
				YieldRate);
		}

		char SecurityID[XC_INSTRUMENT_ID_LENGTH];
		int SettlementDate;
		double CouponRate;
		int MaturityDate;
		short Frequency;
		double YieldRate;
};

// simple class for interfacing with data file that contains instrument desriptive data
// (and yield)
class XC_instrument_input_file {

	// Resource (file handle) acquisition is initialization version - "RAII"
	// destructor closes file...
	public:
		XC_instrument_input_file(const char* filename);
	    ~XC_instrument_input_file();

		//
		// once his object is contructed (and the file is open), returns number of items in it
		//
		virtual int get_record_count();
	
		//
		// returns a heap allocated array of file contents
		//
		virtual XC_instrument_fields* get_records(int& length);
			
		//
		// As a convenience to the caller memory is allocated inside this class
		// Call this to free it
		//
		virtual void free_records();

	protected:
		std::FILE* _file;
		char _line_buf[XC_LINE_BUFFER_LENGTH];
		XC_instrument_fields *_fields_array;
	 
		//
		// prevent copying
		// 
		XC_instrument_input_file (const XC_instrument_input_file &);
		
		//
		// prevent assignment
		//
		XC_instrument_input_file & operator= (const XC_instrument_input_file &);

		//
		// disable the default constructor
		// force construction with filename param only...
		//
		XC_instrument_input_file();
};
