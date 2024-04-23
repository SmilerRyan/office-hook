#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <windows.h>
#include <shlwapi.h>
#include "sl.h"
BOOL IsGracePeriodProduct(HSLC hSLC, SLID *pProductSkuId) {
    PBYTE pBuffer = 0;
    UINT cbSize = 0;
    if(SLGetProductSkuInformation(hSLC, pProductSkuId, L"Name", NULL, &cbSize, &pBuffer) != S_OK) {LocalFree(pBuffer); return FALSE;}
    if(StrStrNIW((PWSTR)pBuffer, L"Grace", cbSize) != NULL) {LocalFree(pBuffer); return TRUE;}
    LocalFree(pBuffer);
    return FALSE;
}
HRESULT WINAPI SLGetLicensingStatusInformationHook(HSLC hSLC, SLID *pAppID, SLID *pProductSkuId, PWSTR pwszRightName, UINT *pnStatusCount, SL_LICENSING_STATUS **ppLicensingStatus) {
    HRESULT hResult = SLGetLicensingStatusInformation(hSLC, pAppID, pProductSkuId, pwszRightName, pnStatusCount, ppLicensingStatus);
    if(hResult != S_OK) return hResult;
    for(int i = 0; i < *pnStatusCount; i++) {
        if((*ppLicensingStatus+i)->eStatus == SL_LICENSING_STATUS_UNLICENSED) continue;
        if(IsGracePeriodProduct(hSLC, &((*ppLicensingStatus+i)->SkuId))) continue;
        (*ppLicensingStatus+i)->eStatus = SL_LICENSING_STATUS_LICENSED;
        (*ppLicensingStatus+i)->dwGraceTime = 0;
        (*ppLicensingStatus+i)->dwTotalGraceDays = 0;
        (*ppLicensingStatus+i)->hrReason = 0;
        (*ppLicensingStatus+i)->qwValidityExpiration = 0;
    }
    HKEY hKey = 0;
    LSTATUS lStatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\16.0", 0, KEY_ALL_ACCESS, &hKey);
    if(lStatus == ERROR_SUCCESS) {RegSetKeyValueW(hKey, L"Common\\Licensing\\Resiliency", L"TimeOfLastHeartbeatFailure", REG_SZ, L"2040-01-01T00:00:00Z", 42);};
    RegCloseKey(hKey);
    return hResult;
}
BOOL APIENTRY WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}
