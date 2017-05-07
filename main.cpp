// Huffman
// main.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//#include "ArrayAnalyzer.h"
#include "rawdata.h"
#include "huffmanengine.h"
#include "packeddata.h"

#define APPLICATION_NAME "Huffman Compressor"
#define APPLICATION_AUTHOR "Giacomo Romoli <giacomo.romoli@libero.it>"
#define APPLICATION_LICENSE "GPL 2.0"
#define APPLICATION_VERSION "0.1b"

#define APPLICATION_INFO APPLICATION_NAME" ver. "APPLICATION_VERSION"\nby "APPLICATION_AUTHOR"\nreleased under "APPLICATION_LICENSE" license"

int main(int argc, _TCHAR* argv[])
{
    int iRet=0;
    //costruzione
    //CArrayAnalyzer oArray; // create it as static
    //iRet = oArray.ArrayInit();

    // nomefile
    TCHAR szFileNameInput[MAX_PATH];
    ZeroMemory(szFileNameInput, MAX_PATH * sizeof(TCHAR));

    if (argc == 3) // gli argomenti sono disposti come 0: percorso dell'eseguibile, 1: primo parametro, 2: secondo parametro etc etc
    {
        if (!_tcscmp(argv[1], _T("-compress")))
        {
            /* COMPRESSIONE */
            _stprintf_s(szFileNameInput, MAX_PATH, _T("%s"), (TCHAR *)argv[2]);

            CRawData tFileToPack; // creo l'oggetto dati grezzi

            if (!tFileToPack.LoadFromFile(szFileNameInput)) // se il caricamento fallisce devo uscire
            {

                //cout<<tFileToPack.pInternalRawData->szNomeFile<<endl;
                _tprintf(_T("%s\n%d bytes\n"), tFileToPack.pInternalRawData->szNomeFile, tFileToPack.pInternalRawData->iFileDim);

                CHuffmanEngine oCompressionEngine;
                CPackedData tPackedResultData;

                if (!oCompressionEngine.Encode(&tFileToPack, &tPackedResultData))
                {
                    // posso scrivere su file
                    TCHAR szFileNameResult[MAX_PATH];
                    ZeroMemory(szFileNameResult, MAX_PATH * sizeof(TCHAR));

                    _stprintf_s(szFileNameResult, MAX_PATH, _T("%s.HFZ"), (TCHAR *)&szFileNameInput[0]);

                    tPackedResultData.WriteDataOnFile(szFileNameResult);

                    //CFile ofile;
                    //FILE *ofile = NULL;
                }


                // nb: trovare il modo di separare il nome dal path
            }
        }
        else if (!_tcscmp(argv[1], _T("-uncompress")))
        {
            /* DECOMPRESSIONE */
            _stprintf_s(szFileNameInput, MAX_PATH, _T("%s"), (TCHAR *)argv[2]);

            CPackedData tFileToUnPack; // creo l'oggetto dati compressi

            if (!tFileToUnPack.LoadDataFromFile(szFileNameInput)) // se il caricamento fallisce devo uscire
            {
                //cout<<tFileToPack.pInternalRawData->szNomeFile<<endl;
                TCHAR szTrimmedFileName[MAX_PATH];
                ZeroMemory(szTrimmedFileName, MAX_PATH * sizeof(TCHAR));

                int iStrLength = _tcslen(szFileNameInput);
                if (iStrLength > 4)
                {
                    if (!_tcscmp(&szFileNameInput[iStrLength-4], _T(".HFZ")))
                    {
                        // adesso che ho verificato il nome taglio l'estensione
                        memcpy(szTrimmedFileName, szFileNameInput, (iStrLength - 4)*sizeof(TCHAR));


                        _tprintf(_T("%s\n%d bytes\n"), szTrimmedFileName, tFileToUnPack.tPackedBlockHeader.iByteOccupiedNumber + sizeof(COMPRESSED_BLOCK_HEADER) + 1024);

                        CHuffmanEngine oCompressionEngine;
                        CRawData tUnpackedResultData;

                        // preparazione e allocazione
                        _stprintf_s(tUnpackedResultData.pInternalRawData->szNomeFile, MAX_PATH, _T("%s"), szTrimmedFileName);
                        tUnpackedResultData.pInternalRawData->iFileDim = tFileToUnPack.tPackedBlockHeader.iSymbolsNumber;
                        tUnpackedResultData.AllocateRawData(tFileToUnPack.tPackedBlockHeader.iSymbolsNumber);

                        if (!oCompressionEngine.Decode(&tFileToUnPack, &tUnpackedResultData))
                        {
                            // posso scrivere su file
                            TCHAR szFileNameResult[MAX_PATH];
                            ZeroMemory(szFileNameResult, MAX_PATH * sizeof(TCHAR));

                            _stprintf_s(szFileNameResult, MAX_PATH, _T("%s"), szTrimmedFileName);

                            tUnpackedResultData.WriteToFile(szFileNameResult);

                            //CFile ofile;
                            //FILE *ofile = NULL;
                        }
                        else
                        {
                            //decoding error
                            return -1;
                        }
                    }
                    else
                    {
                        // file name uncorrect
                        return -1;
                    }
                }
                else
                {
                    // filename too short
                    return -1;
                }

            }
            else
            {
                // impossible to load the file
                return -1;
            }
        }
        else
        {
            _tprintf(_T(APPLICATION_INFO"\nWrong Parameters!!!\nhuffman -help for info\n"));
        }
    }
    else if(argc == 2)
    {
        if (!_tcscmp(argv[1], _T("-help")))
        {
            // non sono state date opzioni per cui invio l'help
            _tprintf(_T(APPLICATION_INFO"\nUSAGE:\n\nhuffman -compress FILENAME for compressing a file\nhuffman -uncompress FILENAME.HFZ for uncompressing a file\n\nNote: the compressed filename will be the same filename as source one, plus .HFZ\nCopyright 2015 GRom\n"));
        }
        else
        {
            _tprintf(_T(APPLICATION_INFO"\n\wrong parameter\n"));
        }
    }
    else
    {
        _tprintf(_T(APPLICATION_INFO"\n\nhuffman -help for info\n"));
    }
    //system("pause");

    //getchar();
    return iRet;
}


