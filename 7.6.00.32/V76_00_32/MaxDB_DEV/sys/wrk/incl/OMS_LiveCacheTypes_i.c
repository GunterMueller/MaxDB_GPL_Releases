#ifdef __cplusplus
extern "C"{
#endif

#ifndef __IID_DEFINED__
#define __IID_DEFINED__
typedef struct _IID
{
    unsigned long  x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;
#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#ifdef __cplusplus
}
#endif

