#include <stdio.h>
#include <iostream>
#include <string>

#define XC_FIRST_VALID_YEAR 1900
#define XC_ERROR -1

enum MonthName {
	NULL_MONTHNAME = 0,
	JANUARY,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

class XC_date {
	public:
		XC_date(){
			_month='0';
			_day='0';
			_year=0;
		}
		~XC_date() {}

		// copy constructor
		XC_date(const XC_date& rhs){
			_month = rhs._month;
			_day = rhs._day;
			_year= rhs._year;
		}
		
		// assignment
		XC_date& operator=(const XC_date& rhs){
			_month = rhs._month;
			_day = rhs._day;
			_year= rhs._year;
			return *this;
		}

		// equality
		bool operator==(const XC_date& rhs){
			return _month == rhs._month &&
				_day == rhs._day &&
				_year == rhs._year;	
		}

		// inequality
		bool operator!=(const XC_date& rhs){
			return _month != rhs._month ||
				_day != rhs._day ||
				_year != rhs._year;	
		}

		void set_from_yyyymmdd(long yyyymmdd);

		long get_yyyymmdd(){ return _year * 10000 + _month * 100 + _day; }

		void show() {
			printf("XC_date: day: %d month: %d year: %d readable: %d\n",
				_day, _month, _year, get_yyyymmdd() );
		}

		void add_months(int number); // -6 passed to go backwards a semi-annual period
		int is_valid();

	private:
		char _month;
		char _day;
		short _year;
};

//#ifdef EXAMPLE_CALL

/*int
get_number_of_periods_semiannual( XC_date from_dt, XC_date to_dt)
{
	// go backwards until we hit from_dt and count
	// we are assuming no partial periods (settlement date falls exactly on a coupon cycle)
	// once we assume partial periods are possible, while loop below changes to comparison <= instead of !=
	int semiannual_period_count = 0;
	while (from_dt != to_dt) {
		to_dt.add_months(-6);
		semiannual_period_count++;
	}
	return semiannual_period_count;
}
*/
//#endif

