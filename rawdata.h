#ifndef RAWDATA_H
#define RAWDATA_H

/*

this class represent the raw data to encode/compress

*/

#pragma once
#include <stdlib.h>
#include "tchar.h"

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

using namespace std;

struct RAW_DATA_ST
{
    TCHAR szNomeFile[MAX_PATH];
    long int iFileDim;
    char * acData;

    RAW_DATA_ST()
    {
        szNomeFile[0]= _T('0');
        iFileDim = 0;
        acData = NULL;
    }

    ~RAW_DATA_ST()
    {
        if (acData)
        {
            delete[] acData;
            acData = NULL;
        }
    }
};

class CRawData
{
public:
    CRawData(void);
    ~CRawData(void);

    RAW_DATA_ST * pInternalRawData;



    int LoadFromFile(TCHAR * szFileName);
    int WriteToFile(TCHAR * szFileName);
    int AllocateRawData(long lByteNum);
    int ClearAllData();

};

#endif // RAWDATA_H
