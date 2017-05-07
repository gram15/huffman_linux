//#include "stdafx.h"
#include "packeddata.h"


CPackedData::CPackedData(void)
{
    acPackedBlockData = NULL;
    for (int i = 0; i < MAX_HUFFMAN_SYMBOLS; i++)
    {
        aiFrequencyArray[i] = 0;
    }
}


CPackedData::~CPackedData(void)
{
    if (acPackedBlockData)
    {
        delete acPackedBlockData;
        acPackedBlockData = NULL;
    }
}


int CPackedData::LoadDataFromFile(TCHAR *szFileName)
{
    int iRet = 0;

    // questa funzione deve caricare Header, allocare lo spazio per i dati e leggere dati compressi da file. vanno implementate funzioni di validazione
    if (szFileName)
    {
        ClearData();
        //CFile oInFile;
        FILE *oInFile = NULL;
        //if (oInFile.Open(szFileName, CFile::modeRead | CFile::shareDenyWrite, NULL))
        oInFile = fopen(szFileName, "rb");
        if(oInFile)
        {
            //oInFile.Read(&tPackedBlockHeader, sizeof(COMPRESSED_BLOCK_HEADER));
            fread(&tPackedBlockHeader, sizeof(char), sizeof(COMPRESSED_BLOCK_HEADER), oInFile);

            // verifichiamo versione
            char acBuffer[4] = "HF1";
            //if(!strcmp(tBlockHeader.szVersion, "HF1\0"))
            if (!memcmp(tPackedBlockHeader.szVersion, acBuffer, 3) != 0)
            {
                // caricamento array frequenze simboli
                //oInFile.Read(&aiFrequencyArray, MAX_HUFFMAN_SYMBOLS * sizeof(int));
                fread(&aiFrequencyArray, sizeof(int), MAX_HUFFMAN_SYMBOLS, oInFile);

                // caricamento blocco dati compresso
                acPackedBlockData = new unsigned char[tPackedBlockHeader.iByteOccupiedNumber];
                ZeroMemory(acPackedBlockData, tPackedBlockHeader.iByteOccupiedNumber); // eventualmente non serve

                //oInFile.Read(acPackedBlockData, tPackedBlockHeader.iByteOccupiedNumber);
                fread(acPackedBlockData, sizeof(char), tPackedBlockHeader.iByteOccupiedNumber, oInFile);

            }
            else
            {
                // formato file non riconosciuto
                iRet = -1;
            }

            fclose(oInFile);
        }
        else
        {
            // impossibile aprire il file
            iRet = -1;
        }
    }
    else
    {
        iRet = -1;
    }


    return iRet;
}

int CPackedData::WriteDataOnFile(TCHAR *szFileName)
{
    int iRet = 0;

    if (szFileName)
    {
        //CFile oOutFile;
        FILE *oOutFile = NULL;
        oOutFile = fopen(szFileName, "wb+");

        //if (oOutFile.Open(szFileName, CFile::modeCreate | CFile::modeWrite, NULL))
        if(oOutFile)
        {
            //oOutFile.Write(&tPackedBlockHeader, sizeof(COMPRESSED_BLOCK_HEADER));
            fwrite(&tPackedBlockHeader, sizeof(char), sizeof(COMPRESSED_BLOCK_HEADER), oOutFile);

            //oOutFile.Write(aiFrequencyArray, MAX_HUFFMAN_SYMBOLS * sizeof(int));
            fwrite(aiFrequencyArray, sizeof(int), MAX_HUFFMAN_SYMBOLS, oOutFile);

            //oOutFile.Write(acPackedBlockData, tPackedBlockHeader.iByteOccupiedNumber*sizeof(char));
            fwrite(acPackedBlockData, sizeof(char), tPackedBlockHeader.iByteOccupiedNumber, oOutFile);

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

int CPackedData::AllocatePackedDataBlock(long lByteNum)
{
    int iRet = 0;

    if (!acPackedBlockData)
    {
        acPackedBlockData = new unsigned char[lByteNum];
    }
    else
    {
        iRet = -1;
    }

    return iRet;
}


int CPackedData::ClearData()
{
    int iRet = 0;

    ZeroMemory(&tPackedBlockHeader, sizeof(COMPRESSED_BLOCK_HEADER));

    for (int i = 0; i < MAX_HUFFMAN_SYMBOLS; i++)
    {
        aiFrequencyArray[i] = 0;
    }

    if (acPackedBlockData)
    {
        delete acPackedBlockData;
        acPackedBlockData = NULL;
    }

    return iRet;
}
