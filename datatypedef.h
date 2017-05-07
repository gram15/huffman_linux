#ifndef DATATYPEDEF_H
#define DATATYPEDEF_H

#pragma once

#ifndef BOOL
#define BOOL int
#define FALSE 0
#define TRUE 1
#endif

struct FREQUENCY_ELEMENT
{
    unsigned char Value;
    unsigned int Frequency;
};

struct TREE_NODE
{
    unsigned char Value;
    unsigned int iFreq;
    TREE_NODE *iNodeFather;
    TREE_NODE *iNodeLeft;
    TREE_NODE *iNodeRight;
    FREQUENCY_ELEMENT *ptElement;
};

// struttura per il dizionario
struct DICT_WORD
{
    unsigned char cSymbol;
    unsigned char cLenght;
    unsigned long dwWord[8];
};

struct FULL_DICTIONARY
{
    DICT_WORD Lemma[256];
};

struct COMPRESSED_BLOCK_HEADER
{
    char szVersion[4]; // indica versione algoritmo
    int iSymbolsNumber;
    int iTotalBitNumber;
    int iByteOccupiedNumber;
};

#endif // DATATYPEDEF_H
