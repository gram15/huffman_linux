#ifndef HUFFMANENGINE_H
#define HUFFMANENGINE_H

#pragma once

#include "rawdata.h"
#include "packeddata.h"
#include "prioritylist.h"
#include "datatypedef.h"

class CHuffmanEngine
{
public:
    CHuffmanEngine(void);
    ~CHuffmanEngine(void);

    // DATA
    CPriorityList *CharFreqList;

    COMPRESSED_BLOCK_HEADER tComprBlock;
    FREQUENCY_ELEMENT aFreqArray[MAX_HUFFMAN_SYMBOLS];
    FULL_DICTIONARY tDictionary; //allocato staticamente, eventualmente rivedere

    unsigned char acHuffCodeBuffer[MAX_HUFFMAN_SYMBOLS];
    unsigned char iHuffCodeIndex; // da 0 a 255 e basta




    int FrequencyCounter(unsigned char *cInArray, unsigned int iInArrayDim, CPriorityList *CharList);

    TREE_NODE *TreeGenerator(CPriorityList *List);
    int TreeDestructor(TREE_NODE * TreeRootPoint);

    int GenerateDictionary(TREE_NODE * tTreeRoot, FREQUENCY_ELEMENT * aFreqArray, int iFrequencyArrayDim);
    // scansione ricorsiva dell'albero per scrivere i codici di huffman
    int TreeScan(TREE_NODE * tRoot, unsigned char * acHuffmanCode, unsigned char iIndex);
    // analizza la stringa binaria e costruisce la voce di dizionario orrizpondente a cCharSymbol
    int AddLemma(FULL_DICTIONARY *tLocalDictionary, unsigned char cCharSymbol, unsigned char * acBinaryString);
    // esegue la funzione di traduzione da simboli char a simboli Huffmann del dizionario
    int TranslateBlock(unsigned char * acRawTranslatedBlock, COMPRESSED_BLOCK_HEADER * tZippedBlock, unsigned char * acInputBlock, FULL_DICTIONARY * tZipDictionary);
    // convert a huffman compressed block into the original format
    int UnTranslateBlock(unsigned char * acRawTranslatedBlock , COMPRESSED_BLOCK_HEADER * tZippedBlock , unsigned char * acInputBlock , TREE_NODE * tTreeRoot);

    int Encode(CRawData * tInputRawData, CPackedData * tOutputPackedData);
    int Decode(CPackedData * tInputPackedData, CRawData * tOutputRawData);
};



#endif // HUFFMANENGINE_H
