#ifndef RUNLENGTHENGINE_H
#define RUNLENGTHENGINE_H

#include "rawdata.h"

class CRunLengthEngine
{
public:
    CRunLengthEngine();
    ~CRunLengthEngine();

    // nota: RLE prende in input un CRawData e genera un altro CRawData eventualmente più corto
    int Encode(CRawData * tInputRawData, CRawData * tOutputPackedData);
    int Decode(CRawData * tInputPackedData, CRawData * tOutputRawData);
};

#endif // RUNLENGTHENGINE_H
