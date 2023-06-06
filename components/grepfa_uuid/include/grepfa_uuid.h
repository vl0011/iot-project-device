//
// Created by vl0011 on 23. 6. 2.
//

#ifndef P1_GREPFA_UUID_H
#define P1_GREPFA_UUID_H

#define UUID_STR_LEN 37

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void random_uuid(char* dest);
void random_u128(uint64_t* word);
void bin_to_uuid(uint64_t* word, char* desc);

#ifdef __cplusplus
};
#endif

#endif //P1_GREPFA_UUID_H
