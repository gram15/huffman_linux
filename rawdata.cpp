//#include "stdafx.h"

#include <fstream>
#include <tchar.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rawdata.h"

#ifdef __unix__

#define _stat       stat
#define _stati64    stat64
#define _wstati64   wstat64

#endif


using namespace std;

CRawData::CRawData(void)
{
    pInternalRawData = new RAW_DATA_ST;
}


CRawData::~CRawData(void)
{
    if (pInternalRawData)
    {
        delete pInternalRawData;
        pInternalRawData = NULL;
    }
}


int CRawData::LoadFromFile(TCHAR * szFileName)
{
    int iRet=0;

    if(szFileName!=NULL && pInternalRawData)
    {
        long lFileLength=0;

        unsigned char buffer[1];
        //caricamento da file
        fstream file;


#ifdef _UNICODE
        struct _wstati64 tFileInfo;
        if (!_wstati64((const TCHAR *)szFileName, &tFileInfo))
#else // _UNICODE
        struct _stati64 tFileInfo;
        if (!_stati64((const TCHAR *)szFileName, &tFileInfo))
#endif // _UNICODE

        //struct _tstat tFileInfo;
        //if (!_tstat((const TCHAR *)szFileName, &tFileInfo))
        {
            lFileLength=tFileInfo.st_size;

            pInternalRawData->acData = new char[lFileLength];
            // quando scrivo la stringa a video mi serve che sia nullo l'ultimo carattere
            // per cui ne aggiungo uno in piu

            memset(pInternalRawData->acData, 0, lFileLength);//sizeof(cInputArray));

            file.open(szFileName, fstream::in | fstream::out | fstream::binary);
            if (file)
            {
                //un carattere alla volta o tutto intero?
                /*
                int iCharIndex=0;
                while (file.read(buffer, 1))
                {
                cInputArray[iCharIndex]=buffer[0];
                iCharIndex++;
                }
                */

                file.read(pInternalRawData->acData, lFileLength);

                pInternalRawData->iFileDim = lFileLength;

                _stprintf_s(pInternalRawData->szNomeFile, MAX_PATH, _T("%s"), szFileName);

            }
        }

        else
        {
            iRet = -1;
        }

        file.close();
        //sprintf(cInputArray, "");
        for (int i=0;i<lFileLength;i++)
        {
            //cout<<cInputArray[i];
        }
        //cout<<"\nFile Lenght: "<<lFileLength<<"bytes\n";
    }


    return iRet;
}


int CRawData::WriteToFile(TCHAR * szFileName)
{
    int iRet = 0;

    if (szFileName && pInternalRawData->acData)
    {
        //CFile oOutFile;
        FILE *oOutFile = NULL;
        //if (oOutFile.Open(szFileName, CFile::modeCreate | CFile::modeWrite, NULL))
        oOutFile = fopen(szFileName, "wb+");
        if (oOutFile)
        {
            //oOutFile.Write(pInternalRawData->acData, pInternalRawData->iFileDim);
            fwrite(pInternalRawData->acData, sizeof(char), pInternalRawData->iFileDim, oOutFile); //fwrite (buffer , sizeof(char), sizeof(buffer), pFile);

            //oOutFile.Close();
            fclose(oOutFile);
        }
        else
        {
            // impossibile aprire il file in scrittura
            iRet = -1;
        }

    }
    else
    {
        iRet = -1;
    }

    return iRet;
}

int CRawData::AllocateRawData(long lByteNum)
{
    int iRet = 0;

    if (!pInternalRawData->acData)
    {
        pInternalRawData->acData = new char[lByteNum];
        ZeroMemory(pInternalRawData->acData, lByteNum);
    }


    return iRet;
}

int CRawData::ClearAllData()
{
    int iRet = 0;

    if (pInternalRawData->acData)
    {
        delete pInternalRawData->acData;
        pInternalRawData->acData = NULL;
    }
    ZeroMemory(pInternalRawData->szNomeFile, MAX_PATH * sizeof(TCHAR));
    pInternalRawData->iFileDim = 0;

    return iRet;
}
