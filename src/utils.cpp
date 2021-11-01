#include "utils.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

int getToken(char* dst, size_t dstSize, const char* src, char separator, int pos){

    DEBUG_PRINT("[getToken] str=%s sep=%c pos=%d\n",src,separator,pos);
    int toGo = pos;

    const char* ts = src;
    unsigned int tl = 0;

    int p = 0;
    // find n-th token in the source string
    DEBUG_PRINT("sep=");
    while(src[p] != 0){
        if (src[p] == separator){
            DEBUG_PRINT("%d ",p);
            toGo--;
            if (toGo>0) ts = src+p+1;
            else break;
        }
        p++;
    }
    DEBUG_PRINT("\n");

    if (src[p] != 0) {  // if token found
        tl = src + p - ts; // determine token length
        if (tl+1 > dstSize) return 0; // if too large for destination buffer
        strncpy(dst, ts, tl);
        DEBUG_PRINT("[getToken] token=%s length=%d\n",dst,tl);
        return tl;
    } 

    return 0;

}