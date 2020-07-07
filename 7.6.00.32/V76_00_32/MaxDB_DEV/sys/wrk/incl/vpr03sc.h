
#define CPR_INFO_UDT         (CPR_INFO_STRING+1)
#define CPR_INFO_ABAP_HANDLE (CPR_INFO_STRING+2)
#define CPR_VABAPHANDLE      (CPR_VODATE+1)

#ifdef __cplusplus
extern "C" {
#endif
void p03sABAPError(struct SQLERROR *sqlemp, char *szStr, char cbErrorCode);
#ifdef __cplusplus
}
#endif

