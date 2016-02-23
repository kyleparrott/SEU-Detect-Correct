#ifndef _CRC_H
#define _CRC_H

#include "block_header.h"

#ifndef NULL
#define NULL (0)
#endif

#define K_BYTES_IN_WORDS(x) ((x) * 256)

typedef void (*CRCCheck_t)( void );

__attribute__((no_instrument_function, always_inline)) inline static void profile_function(block_header_t* header_ptr) {

	uint32_t idx;

	CRC->CR = 0x00000001; /* Reset the CRC register */
/*  $$$ fix this! */
	for(idx=0; idx< 0 /* word_count */ ; idx++)
	{
		CRC->DR = 0 /* wptr[idx] */ ;
	}

	/* return CRC->DR; */
}


/* ((volatile unsigned long *) 0xE0028000) */



void __attribute__((no_instrument_function, section ("flash00func"))) section00_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash01func"))) section01_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash02func"))) section02_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash03func"))) section03_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash04func"))) section04_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash05func"))) section05_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash06func"))) section06_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash07func"))) section07_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash08func"))) section08_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash09func"))) section09_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash10func"))) section10_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash11func"))) section11_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash12func"))) section12_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash13func"))) section13_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash14func"))) section14_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash15func"))) section15_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash16func"))) section16_profile_func_enter(void);
void __attribute__((no_instrument_function, section ("flash17func"))) section17_profile_func_enter(void);

#endif /* #define _CRC_H */