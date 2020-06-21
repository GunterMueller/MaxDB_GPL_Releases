/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/

#include <windows.h>
#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>
#include <wchar.h>

#define TASKS_TO_RETRIEVE          5

int main(int argc, char **argv)
{
    HRESULT hr = ERROR_SUCCESS;
    ITaskScheduler *pITS;


    /////////////////////////////////////////////////////////////////
    // Call CoInitialize to initialize the COM library and
    // then CoCreateInstance to get the Task Scheduler object.
    /////////////////////////////////////////////////////////////////
    hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_CTaskScheduler,
                 NULL,
                 CLSCTX_INPROC_SERVER,
                 IID_ITaskScheduler,
                 (void **) &pITS);
        if (FAILED(hr))
        {
            CoUninitialize();
            return hr;
        }
    }
    else
    {
        return hr;
    }

    /*
     * connect to remote machine
     */
    if (argc > 1) {
        const char * machineName = argv [1];
        WCHAR machineNameW [100];
        mbstowcs (machineNameW, machineName, strlen (machineName) + 1);
        hr = pITS->SetTargetComputer (machineNameW);
        if (FAILED (hr)) {
            CoUninitialize();
            return hr;
        }
    }

    /////////////////////////////////////////////////////////////////
    // Call ITaskScheduler::Enum to get an enumeration object.
    /////////////////////////////////////////////////////////////////
    IEnumWorkItems *pIEnum;
    hr = pITS->Enum(&pIEnum);
    if (FAILED(hr))
    {
        CoUninitialize();
        return hr;
    }

    /////////////////////////////////////////////////////////////////
    // Call IEnumWorkItems::Next to retrieve tasks. Note that
    // this example tries to retrieve five tasks for each call.
    /////////////////////////////////////////////////////////////////
    LPWSTR *lpwszNames;
    DWORD dwFetchedTasks = 0;
    while (SUCCEEDED(pIEnum->Next(TASKS_TO_RETRIEVE,
        &lpwszNames,
        &dwFetchedTasks))
        && (dwFetchedTasks != 0))
    {
        ///////////////////////////////////////////////////////////////
        // Process each task. Note that this example prints the
        // name of each task to the screen.
        //////////////////////////////////////////////////////////////
        while (dwFetchedTasks)
        {
            LPWSTR currentJobName = lpwszNames[--dwFetchedTasks];
            /*
             * get activated info
             */
            ITask *pITask;
            bool timeinfoOK = false;

            hr = pITS->Activate(lpwszNames[dwFetchedTasks],
                                IID_ITask,
                                (IUnknown**) &pITask);
            if (SUCCEEDED (hr)) {
                SYSTEMTIME pstNextRun;

                hr = pITask->GetNextRunTime(&pstNextRun);
                if (SUCCEEDED (hr)) {
                    bool isDummy = (pstNextRun.wMonth == 0)
                        && (pstNextRun.wDay == 0)
                        && (pstNextRun.wDay == 0);
                    LPWSTR applicationName = NULL;
                    LPWSTR parameters = NULL;
                    if (!isDummy) {
                        pITask->GetApplicationName(&applicationName);
                        pITask->GetParameters(&parameters);
                        wprintf(L"%s\t%u:%02u\t%s %s\n",
                            currentJobName,
                            pstNextRun.wHour,
                            pstNextRun.wMinute,
                            applicationName,
                            parameters);
                        if (applicationName != NULL) {
                            CoTaskMemFree (applicationName);
                        }
                        if (parameters != NULL) {
                            CoTaskMemFree (parameters);
                        }
                        timeinfoOK = true;
                    }
                }
                pITask->Release();
            }
            if (!timeinfoOK) {
                // wprintf(L"%s: unknown schedule\n", currentJobName);
            }
            CoTaskMemFree(lpwszNames[dwFetchedTasks]);
        }
        CoTaskMemFree(lpwszNames);
    }
    pITS->Release ();
    pIEnum->Release();
    CoUninitialize();
    return ERROR_SUCCESS;
}


