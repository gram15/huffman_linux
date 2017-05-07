//#include "stdafx.h"

#include <memory.h>
#include <math.h>
#include "huffmanengine.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
*/

CHuffmanEngine::CHuffmanEngine(void)
{
    CharFreqList = NULL;
}


CHuffmanEngine::~CHuffmanEngine(void)
{
}


int CHuffmanEngine::Encode(CRawData * tInputRawData, CPackedData * tOutputPackedData)
{
    if (tInputRawData && tOutputPackedData) // non devono essere nulli
    {
        /*compressione*/

        CharFreqList = new CPriorityList;

        FrequencyCounter((unsigned char *)tInputRawData->pInternalRawData->acData, tInputRawData->pInternalRawData->iFileDim, CharFreqList);

        // costruzione dell'albero binario

        TREE_NODE *ptFreqTree = TreeGenerator(CharFreqList);
        //TreeShow(ptFreqTree);

        // costruzione del dizionario
        GenerateDictionary(ptFreqTree, NULL, NULL);

        // distruzione dell'albero binario
        TreeDestructor(ptFreqTree);

        //pulizia dell'header
        ZeroMemory(&tComprBlock, sizeof(COMPRESSED_BLOCK_HEADER));

        // scrittura versione algoritmo:
        snprintf(tComprBlock.szVersion, 4, "HF1\0");
        tComprBlock.iSymbolsNumber = tInputRawData->pInternalRawData->iFileDim;

        // creazione di array per il blocco di dati compresso (si assume che la lunghezza sia non superiore a quella non compressa)
        char * acNewTranslatedBlock = NULL;
        acNewTranslatedBlock = new char[tComprBlock.iSymbolsNumber];
        ZeroMemory(acNewTranslatedBlock, tComprBlock.iSymbolsNumber);

        // compressione sulla base del dizionario Huffmann
        TranslateBlock((unsigned char *)acNewTranslatedBlock, &tComprBlock, (unsigned char *) tInputRawData->pInternalRawData->acData, &tDictionary);

        // si copia la lista frequenze ordinate nell'oggetto dati compressi
        for (int i = 0; i < MAX_HUFFMAN_SYMBOLS; i++)
        {
            tOutputPackedData->aiFrequencyArray[i] = aFreqArray[i].Frequency;
        }

        // si copia l'header nell'oggetto dati compressi
        memcpy(&tOutputPackedData->tPackedBlockHeader, &tComprBlock, sizeof(COMPRESSED_BLOCK_HEADER));

        // si alloca lo spazio nell'oggetto dati compressi e si copiano i dati
        tOutputPackedData->AllocatePackedDataBlock(tComprBlock.iByteOccupiedNumber);
        memcpy(tOutputPackedData->acPackedBlockData, acNewTranslatedBlock, tComprBlock.iByteOccupiedNumber);



        if (CharFreqList)
        {
            delete CharFreqList;
        }

        if (acNewTranslatedBlock)
        {
            delete acNewTranslatedBlock;
            acNewTranslatedBlock = NULL;
        }

    }
    else
    {
        return -1;
    }

    return 0;
}


int CHuffmanEngine::Decode(CPackedData * tInputPackedData, CRawData * tOutputRawData)
{
    if (tInputPackedData && tOutputRawData)
    {
        // pulizia e lettura frequency array
        memset(&aFreqArray, 0, MAX_HUFFMAN_SYMBOLS * sizeof(FREQUENCY_ELEMENT));
        for (int i = 0; i < MAX_HUFFMAN_SYMBOLS; i++)
        {
            aFreqArray[i].Value = i;
            aFreqArray[i].Frequency = tInputPackedData->aiFrequencyArray[i];
        }

        // creazione del dizionario a partire dalle frequenze, costruzione albero decodifica
        CPriorityList *CharFreqListDecode = NULL;
        CharFreqListDecode = new CPriorityList;
        FrequencyCounter(NULL, 0, CharFreqListDecode);

        TREE_NODE *ptFreqTreeDecoding = TreeGenerator(CharFreqListDecode);

        // costruzione del dizionario
        GenerateDictionary(ptFreqTreeDecoding, NULL, NULL);

        UnTranslateBlock((unsigned char *)tOutputRawData->pInternalRawData->acData, &tInputPackedData->tPackedBlockHeader, tInputPackedData->acPackedBlockData, ptFreqTreeDecoding);

        TreeDestructor(ptFreqTreeDecoding);

        if (CharFreqListDecode)
        {
            delete CharFreqListDecode;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}


int CHuffmanEngine::FrequencyCounter(unsigned char *cInArray, unsigned int iInArrayDim, CPriorityList *CharList)
{
    if((cInArray != NULL) && (iInArrayDim != 0))
    {
        // conteggio delle frequenze di ogni simbolo all'interno dell'array
        memset(aFreqArray, NULL, MAX_HUFFMAN_SYMBOLS*sizeof(FREQUENCY_ELEMENT)); //inizializzazione
        for(unsigned int i=0; i<MAX_HUFFMAN_SYMBOLS;i++)
        {
            aFreqArray[i].Value=(unsigned char)i;
            aFreqArray[i].Frequency = 0; // initialization
        }
        for (unsigned int i=0; i<iInArrayDim; i++)
        {
            aFreqArray[(unsigned int)cInArray[i]].Frequency++;
            if(aFreqArray[(unsigned int)cInArray[i]].Frequency>iInArrayDim)
            {
                volatile int ErrorSforamento = -1;
            }
        }
    }

    // questo pezzo e' comune, se non passo parametri significa che ho aFreqArray gia' a posto
    for(unsigned int i=0; i<MAX_HUFFMAN_SYMBOLS;i++)
    {
        //cout<<"Freq char "<<i<<" = "<<aFreqArray[i].Frequency<<"\n";
        //TRACE("Freq char %d = %d", i, aFreqArray[i].Frequency);
        //printf("Freq char %d = %d", i, aFreqArray[i].Frequency);

        if (aFreqArray[i].Frequency!=0)
        {
            CharList->InsertNewElement(NULL, &(aFreqArray[i]), aFreqArray[i].Frequency);
        }
    }

    CharList->ListReadFwd(CharList->FirstElement());

    // 	cout<<"--------\nTree Generation\n--------\n";
    // 	//qui mi metto a costruire l'albero
    // 	TREE_NODE *ptFreqTree = TreeGenerator(CharList);
    // 	TreeShow(ptFreqTree);
    //
    // 	// costruzione del dizionario
    // 	GenerateDictionary(ptFreqTree, NULL, NULL);
    //
    // 	// distruzione dell'albero binario
    // 	TreeDestructor(ptFreqTree);

    return 0;
}

TREE_NODE *CHuffmanEngine::TreeGenerator(CPriorityList *List)
{
    TREE_NODE *TreeRoot=NULL;

    //Per l'albero della codifica Huffmann devo prendere i due ultimi elementi dell'array,
    // sommarne le priorit�, torglierli dall'array ed inserire un nodo che abbia la
    // priorit� somma delle precedenti

    if (List!=NULL) // il processo � vincolato all'avere una lista valida
    {
        // controllare che ilpuntatore di inizio e quello di fine siano sempre diversi da NULL
        if (List->FirstElement()!=NULL && List->LastElement()!=NULL)
        {
            //TRACE("Lista valida \n");

            LIST_ELEMENT *pTempElement=NULL;

            TREE_NODE *tNewNode;
            TREE_NODE *tNewNode2;
            TREE_NODE *tNewNodeUp;

            // finche sono diversi fra loro
            while(List->LastElement()!=List->FirstElement())
            {
                // primo elemento

                pTempElement = List->LastElement();
                if (pTempElement->pLeaf!=NULL)
                {
                    // allora creo la foglia
                    tNewNode = new TREE_NODE;

                    FREQUENCY_ELEMENT *tTempFreq=(FREQUENCY_ELEMENT *)pTempElement->pLeaf;
                    tNewNode->ptElement=(FREQUENCY_ELEMENT *)pTempElement->pLeaf;
                    tNewNode->iFreq=pTempElement->value;
                    tNewNode->iNodeFather=NULL;
                    tNewNode->iNodeLeft=NULL;
                    tNewNode->iNodeRight=NULL;
                }
                else
                {
                    // in questo caso non si tratta di una foglia
                    tNewNode = (TREE_NODE *)pTempElement->pElementPointer;
                    tNewNode->iFreq=pTempElement->value;
                    tNewNode->ptElement=NULL;
                    tNewNode->iNodeFather=NULL;
                    // i nodi destro e sinistro invece devono essere lasciati

                }

                List->DeleteElement(pTempElement); // elimino l'elemento di cui ho creato la foglia

                // secondo elemento
                pTempElement = List->LastElement();
                if (pTempElement->pLeaf!=NULL)
                {
                    // allora creo la foglia
                    tNewNode2 = new TREE_NODE;

                    FREQUENCY_ELEMENT *tTemp2Freq=(FREQUENCY_ELEMENT *)pTempElement->pLeaf;
                    tNewNode2->ptElement=(FREQUENCY_ELEMENT *)pTempElement->pLeaf;
                    tNewNode2->iFreq=pTempElement->value;
                    tNewNode2->iNodeFather=NULL;
                    tNewNode2->iNodeLeft=NULL;
                    tNewNode2->iNodeRight=NULL;
                }
                else
                {
                    // in questo caso non si tratta di una foglia
                    tNewNode2 = (TREE_NODE *)pTempElement->pElementPointer;
                    tNewNode2->iFreq=pTempElement->value;
                    tNewNode2->ptElement=NULL;
                    tNewNode2->iNodeFather=NULL;
                    // i nodi destro e sinistro invece devono essere lasciati

                }

                List->DeleteElement(pTempElement); // elimino l'elemento di cui ho creato la foglia

                // a questo punto si genera il nodo superiore
                tNewNodeUp = new TREE_NODE;
                tNewNodeUp->iNodeFather=NULL;
                tNewNodeUp->ptElement=NULL;
                tNewNodeUp->Value=NULL;
                tNewNodeUp->iFreq=(tNewNode->iFreq)+(tNewNode2->iFreq); // aggiorno la priorit� come somma
                tNewNodeUp->iNodeRight=tNewNode;
                tNewNodeUp->iNodeLeft=tNewNode2;
                tNewNode->iNodeFather=tNewNodeUp;
                tNewNode2->iNodeFather=tNewNodeUp;

                List->InsertNewElement(tNewNodeUp, NULL, tNewNodeUp->iFreq);

                //TO DO
                //cout<<"inserted priority : "<<tNewNodeUp->iFreq<<"\n";

            }
            // se lastelement e firstelement sono uguali, significa che � rimasto un solo elemento, ed � il nodo root dell'albero
            pTempElement = List->LastElement();
            TreeRoot = (TREE_NODE *)pTempElement->pElementPointer;

            List->CleanPriorityList(pTempElement); //elimino tutto
        }

    }

    return TreeRoot;
}

int CHuffmanEngine::TreeDestructor(TREE_NODE * TreeRootPoint)
{
    int iRet=0;
    TREE_NODE *tNodeTemp=NULL;
    if(TreeRootPoint != NULL)
    {
        //caso 1 ramo destro
        if(TreeRootPoint->iNodeRight!=NULL)
        {
            // vado a vedere
            tNodeTemp = TreeRootPoint->iNodeRight;
            //cout<<"0->";
            TreeDestructor(tNodeTemp);
            TreeRootPoint->iNodeRight = NULL;
        }
        //caso 2 ramo sinistro
        if (TreeRootPoint->iNodeLeft!=NULL)
        {
            // vado a vedere
            tNodeTemp = TreeRootPoint->iNodeLeft;
            //cout<<"1->";
            TreeDestructor(tNodeTemp);
            TreeRootPoint->iNodeLeft = NULL;

        }
        delete TreeRootPoint;
        TreeRootPoint = NULL;
    }
    else
    {
        iRet = -1;
    }

    return iRet;
}

int CHuffmanEngine::GenerateDictionary(TREE_NODE * tTreeRoot, FREQUENCY_ELEMENT * aFreqArray, int iFrequencyArrayDim)
{
    int iRet = 0;
    // questo metodo deve generare il dizionario a partire dall'albero di Huffman.
    // in questa versione ci sono solo 256 simboli, per cui la lunghezza massima
    // della codifica � di (N-1) con N il numero di simboli totali presenti
    // (ref teoremi sugli alberi binari)

    // inizializzazione del dizionario
    memset(&tDictionary, 0, sizeof(tDictionary));

    //initialize
    memset(acHuffCodeBuffer, 0, sizeof(acHuffCodeBuffer));
    iHuffCodeIndex = 0;

    //scan the tree to obtain the binary code
    TreeScan(tTreeRoot, acHuffCodeBuffer, iHuffCodeIndex);

    return iRet;
}


// scansione ricorsiva dell'albero per scrivere i codici di huffman
int CHuffmanEngine::TreeScan(TREE_NODE * tRoot, unsigned char * acHuffmanCode, unsigned char iIndex)
{
    int iRet=0;
    // algoritmo ricorsivo...
    TREE_NODE *tNodeTemp=NULL;

    if (tRoot->iNodeRight==NULL && tRoot->iNodeLeft==NULL)
    {
        // allora questa � una foglia
        FREQUENCY_ELEMENT *pTempFreqEl=NULL;
        pTempFreqEl=tRoot->ptElement;
        //cout<<" value: "<<(int)pTempFreqEl->Value<<" "<<(char)pTempFreqEl->Value<<"\n";
        // scrivo il codice di huffman
        //cout<<" value: "<<(int)pTempFreqEl->Value<<"\t "<<acHuffCodeBuffer<<"\n";

        // qui devo andare a comporre la voce di dizionario
        AddLemma(&tDictionary, (int)pTempFreqEl->Value, acHuffCodeBuffer);

    }
    else
    {

        //caso 1 ramo destro
        if(tRoot->iNodeRight!=NULL)
        {
            // vado a vedere
            tNodeTemp = tRoot->iNodeRight;
            //cout<<"0->";
            acHuffCodeBuffer[iHuffCodeIndex] = '0';
            iHuffCodeIndex++;
            TreeScan(tNodeTemp, acHuffCodeBuffer, iHuffCodeIndex);
            iHuffCodeIndex--;
            acHuffCodeBuffer[iHuffCodeIndex] = (char)0;
        }
        //caso 2 ramo sinistro
        if (tRoot->iNodeLeft!=NULL)
        {
            // vado a vedere
            tNodeTemp = tRoot->iNodeLeft;
            //cout<<"1->";
            acHuffCodeBuffer[iHuffCodeIndex] = '1';
            iHuffCodeIndex++;
            TreeScan(tNodeTemp, acHuffCodeBuffer, iHuffCodeIndex);
            iHuffCodeIndex--;
            acHuffCodeBuffer[iHuffCodeIndex] = (char)0;
        }
    }

    return iRet;
}


// analizza la stringa binaria e costruisce la voce di dizionario corripondente a cCharSymbol
int CHuffmanEngine::AddLemma(FULL_DICTIONARY *tLocalDictionary, unsigned char cCharSymbol, unsigned char * acBinaryString)
{
    int iRet = 0;
    if (tLocalDictionary!=NULL)
    {
        //scansione dell'array fino a trovare il carattere null
        int iIndex = 0;
        int iDWnumber = 0;
        int iBitNumber = 0;

        // in una futura revisione potrebbe essere eliminato
        tLocalDictionary->Lemma[(int)cCharSymbol].cSymbol = cCharSymbol;

        while(iIndex<256 && acBinaryString[iIndex]!=0)
        {
            iDWnumber = iIndex / 32;
            iBitNumber = iIndex % 32;
            switch(acBinaryString[iIndex])
            {
            case '0':
                {
                    //devo posizionare uno 0 binario
                    // nota: una DWORD � un unsigned long int a 32 bit

                    // questo � solo per esempio, � inutile
                    //tLocalDictionary->Lemma[(int)cCharSymbol].dwWord[iDWnumber]+= 0 * pow(2, iBitNumber);

                }
                break;

            case '1':
                {
                    // devo posizionare un 1 binario
                    // si parte da array di dword tutte a NULL
                    tLocalDictionary->Lemma[(unsigned int)cCharSymbol].dwWord[iDWnumber]+= 1 * pow(2.0, iBitNumber);

                }
                break;

            default:
                iRet = -1; // ho trovato un carattere non desiderato
                break;

            }
            iIndex++;
        }
        // includo il numero di bit che effettivamente utilizzo
        tLocalDictionary->Lemma[(int)cCharSymbol].cLenght=(unsigned char)iIndex;


        //test da fare solo in debug o in decodifica
        char * acTemp;
        acTemp = new char[256];
        memset(acTemp, 0, 256);
        int iTempDWnum = 0;
        int iTempBITnum = 0;

        for (int i = 0; i< (int)tLocalDictionary->Lemma[(int)cCharSymbol].cLenght; i++)
        {
            iTempDWnum = i / 32;
            iTempBITnum = i % 32;
            if (tLocalDictionary->Lemma[(int)cCharSymbol].dwWord[iTempDWnum] & (int)pow(2.0, iTempBITnum))
            {
                acTemp[i] = '1';
            }
            else
            {
                acTemp[i] = '0';
            }

        }
        //cout<<"Lemma "<<(int)cCharSymbol<<"\t"<<acTemp<<endl;
        delete[] acTemp;

    }
    else
    {
        iRet = -1;
    }


    return iRet;
}

// esegue la funzione di traduzione da simboli char a simboli Huffmann del dizionario
int CHuffmanEngine::TranslateBlock(unsigned char * acRawTranslatedBlock, COMPRESSED_BLOCK_HEADER * tZippedBlock, unsigned char * acInputBlock, FULL_DICTIONARY * tZipDictionary)
{
    int iRet = 0;
    if (tZippedBlock!=NULL && tZipDictionary!=NULL && acInputBlock!=NULL)
    {
        // funzione di traduzione
        int iMaxBytes = tZippedBlock->iSymbolsNumber;

        int iTotalBitNum=0;
        int iByteCounter=0;
        int iTempCodeLenght=0;
        int iTempByteCode=0;
        int iTempByteNum=0;
        int iTempBitNum=0;

        int iTmpCodeDWNum=0;
        int iTmpCodeBitNum=0;


        for (int iIndex = 0; iIndex<iMaxBytes; iIndex++) // indice dell'array di origine
        {
            iTempByteCode = (int)acInputBlock[iIndex];
            //cout<<(char)iTempByteCode;
            iTempCodeLenght = (int)tZipDictionary->Lemma[iTempByteCode].cLenght;
            for (int iJindex = 0; iJindex<iTempCodeLenght; iJindex++) //indice dei bit del simbolo
            {
                iTempByteNum = iTotalBitNum / 8;
                iTempBitNum = iTotalBitNum % 8;

                iTmpCodeDWNum = iJindex / 32;
                iTmpCodeBitNum = iJindex % 32;

                //if (tZipDictionary->Lemma[iTempByteCode].dwWord[iTmpCodeDWNum] & (int)pow(2.0, iTmpCodeBitNum))
                //{
                //    acRawTranslatedBlock[iTempByteNum] += (int)pow(2.0, iTempBitNum);
                //}

                if (tZipDictionary->Lemma[iTempByteCode].dwWord[iTmpCodeDWNum] & (1<<iTmpCodeBitNum))
                {
                    acRawTranslatedBlock[iTempByteNum] += (1<<iTempBitNum);
                }
                iTotalBitNum++;
            }
        }

        // vanno riempitigli altri campi dell'header di blocco
        tZippedBlock->iTotalBitNumber = iTotalBitNum;
        if (iTotalBitNum % 8 == 0)
        {
            tZippedBlock->iByteOccupiedNumber = iTotalBitNum /8;
        }
        else
        {
            tZippedBlock->iByteOccupiedNumber = (iTotalBitNum /8)+1;
        }
        //cout<<endl;
    }
    else
    {
        iRet = -1;
    }


    // mostro a video i tre dati

    //cout<<"\n";
    //cout<<"Original Byte Number:\t"<<tZippedBlock->iSymbolsNumber<<endl;
    //cout<<"Final Byte Number:\t"<<tZippedBlock->iByteOccupiedNumber<<endl;
    //cout<<"Compression Ratio:\t"<<100*tZippedBlock->iByteOccupiedNumber/tZippedBlock->iSymbolsNumber<<" %\n";


    return iRet;
}

// convert a huffman compressed block into the original format
int CHuffmanEngine::UnTranslateBlock(unsigned char * acRawTranslatedBlock , COMPRESSED_BLOCK_HEADER * tZippedBlock , unsigned char * acInputBlock , TREE_NODE * tTreeRoot)
{
    int iRet = 0;

    if (tZippedBlock!=NULL && tTreeRoot!=NULL && acInputBlock!=NULL)
    {
        // funzione di traduzione
        int iMaxBytes = tZippedBlock->iSymbolsNumber;

        int iTotalBitNum= tZippedBlock->iTotalBitNumber;
        int iByteCounter=0;

        int iTempCodeLenght=0;
        int iTempByteCode=0;
        int iTempByteNum=0;
        int iTempBitNum=0;

        BOOL bTempBitValue = FALSE;

        int iTmpCodeDWNum=0;
        int iTmpCodeBitNum=0;

        int iBitIndex = 0;
        int iInputByteIndex = 0;
        int iBitPosition;

        TREE_NODE * pTempNode = tTreeRoot;
        while (iBitIndex<iTotalBitNum
            && iInputByteIndex<iMaxBytes)
        {
            if (iBitIndex == 7)
            {
                volatile int pippo = 0;
            }

            iInputByteIndex = iBitIndex /8;
            iBitPosition = iBitIndex % 8;



            // la riga seguente va riscritta bene con gli shift
            //bTempBitValue = (acInputBlock[iInputByteIndex] & (int)pow(2.0, iBitPosition))>>iBitPosition;
            bTempBitValue = (acInputBlock[iInputByteIndex] & (1<<iBitPosition))>>iBitPosition;


            if (bTempBitValue==/*FALSE*/TRUE) // ramo sinistro
            {
                pTempNode=pTempNode->iNodeLeft;
                if (pTempNode->iNodeLeft==NULL && pTempNode->iNodeRight==NULL)
                {
                    // in questo caso c'è il simbolo
                    acRawTranslatedBlock[iByteCounter]=pTempNode->ptElement->Value;
                    //cout<<(unsigned char)pTempNode->ptElement->Value;
                    iByteCounter++;
                    pTempNode = tTreeRoot; // si torna da capo
                }
            }
            else	//ramo destro
            {
                pTempNode=pTempNode->iNodeRight;
                if (pTempNode->iNodeLeft==NULL && pTempNode->iNodeRight==NULL)
                {
                    // in questo caso c'è il simbolo
                    acRawTranslatedBlock[iByteCounter]=pTempNode->ptElement->Value;
                    //cout<<(unsigned char)pTempNode->ptElement->Value;
                    iByteCounter++;
                    pTempNode = tTreeRoot; // si torna da capo
                }
            }
            iBitIndex++;

        }

    }
    else
    {
        iRet = -1;
    }


    // mostro a video i tre dati
    //cout<<"\n";
    //cout<<"Original Byte Number:\t"<<tZippedBlock->iSymbolsNumber<<endl;
    //cout<<"Final Byte Number:\t"<<tZippedBlock->iByteOccupiedNumber<<endl;
    //cout<<"Compression Ratio:\t"<<100*tZippedBlock->iByteOccupiedNumber/tZippedBlock->iSymbolsNumber<<" %\n";



    return iRet;
}
