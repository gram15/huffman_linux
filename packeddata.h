#ifndef PACKEDDATA_H
#define PACKEDDATA_H

#pragma once

#include "datatypedef.h"
#include "tchar.h"

// struct COMPRESSED_BLOCK_HEADER
// {
// 	char szVersion[4]; // indica versione algoritmo
// 	int iSymbolsNumber;
// 	int iTotalBitNumber;
// 	int iByteOccupiedNumber;
// };

#define MAX_HUFFMAN_SYMBOLS (256)

class CPackedData
{
public:
    CPackedData(void);
    ~CPackedData(void);

    COMPRESSED_BLOCK_HEADER tPackedBlockHeader;
    int aiFrequencyArray[MAX_HUFFMAN_SYMBOLS];
    unsigned char *acPackedBlockData;

    int LoadDataFromFile(TCHAR *szFileName);
    int WriteDataOnFile(TCHAR *szFileName);
    int AllocatePackedDataBlock(long lByteNum);
    int ClearData();

};



#endif // PACKEDDATA_H
