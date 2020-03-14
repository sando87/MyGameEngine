#pragma once
#include "junks.h"

class jGenericValue
{
public:
	jGenericValue() { memset(&mVal, 0, sizeof(mVal)); }
	virtual ~jGenericValue() {}

	void operator=(void* val) { mVal.pval = val; }
	void operator=(double val) { mVal.dval = val; }
	void operator=(float val) { mVal.fval = val; }
	void operator=(u64 val) { mVal.u64val = val; }
	void operator=(u32 val) { mVal.u32val = val; }
	void operator=(u8 val) { mVal.u8val = val; }
	void operator=(int val) { mVal.intval = val; }
	void operator=(char val) { mVal.chval = val; }
	void operator=(bool val) { mVal.bval = val; }
	void operator=(string val) { mStr = val; }

	template<typename Ty> Ty val() { return (Ty)mVal.pval; }
	template<> void* val() { return mVal.pval; }
	template<> double val() { return mVal.dval; }
	template<> float val() { return mVal.fval; }
	template<> u64 val() { return mVal.u64val; }
	template<> u32 val() { return mVal.u32val; }
	template<> u8 val() { return mVal.u8val; }
	template<> int val() { return mVal.intval; }
	template<> char val() { return mVal.chval; }
	template<> bool val() { return mVal.bval; }
	template<> string val() { return mStr; }

private:
	union gtype {
		void* pval;
		double dval;
		float fval;
		u64 u64val;
		u32 u32val;
		u8 u8val;
		int intval;
		char chval;
		bool bval;
	};
	gtype mVal;
	string mStr;
};

