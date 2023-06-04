//
// Created by vl0011 on 23. 6. 2.
//

#include <grepfa_uuid.h>

#include <cstdlib>
#include <cstring>
#include <ctime>

static const char *temp = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
static const char *samples = "0123456789abcdef";

union Rnd { unsigned char b[16]; uint64_t word[2]; };

int get_nonce() {
    return 1858725;
}

int random_int() {
    unsigned int nonce = (unsigned int)time(nullptr) & get_nonce();
    srand(nonce);
    return rand();
}

uint64_t random_uint64() {
    uint64_t num = random_int() & 0x1FFFFF;
    num = (num << 32) | random_int();
    return num;
}

void random_u128(uint64_t* word) {
    word[0] = random_uint64();
    word[1] = random_uint64();
}

void bin_to_uuid(uint64_t* word, char* desc) {
    Rnd* rnd = (Rnd*) word;
    char uuid[37], *dst = uuid;
    const char *p = temp;
    int i = 0, n = 0;
    while(*p) {
        n = rnd->b[i >> 1];
        n = (i & 1) ? (n >> 4) : (n & 0xf);
        switch (*p) {
            case 'x':
                *dst = samples[n];
                i++;
                break;
            case 'y':
                *dst = samples[(n & 0x3) + 8];
                i++;
                break;
            default:
                *dst = *p;
        }
        p++;
        dst++;
    }
    uuid[36] = '\0';
    strncpy(desc, uuid, 37);
}

void random_uuid(char* desc) {
    Rnd rnd{};
    random_u128(rnd.word);
    bin_to_uuid(rnd.word, desc);
}
