//#include "StdAfx.h"
#include <iostream>
#include "prioritylist.h"

using namespace std;

CPriorityList::CPriorityList(void)
{
    //metto a null entrambi i puntatori
    pInitList	= NULL;
    pEndList	= NULL;
    pListCursor = NULL;
}

CPriorityList::~CPriorityList(void)
{
    //se uno dei due puntatori non e' null, elimino la lista
    if (pInitList != NULL || pEndList != NULL)
    {
        CleanPriorityList(pInitList);
    }

}

// Primo elemento della lista
LIST_ELEMENT * CPriorityList::FirstElement(void)
{
    return pInitList;
}

// Ultimo elemento della lista
LIST_ELEMENT * CPriorityList::LastElement(void)
{
    return pEndList;
}

// Inserisci nuovo elemento
int CPriorityList::InsertNewElement(void *pNewElement, void *pNewLeaf, unsigned int NewElementPriority)
{
    int iRet=0;

    // gestione dell'inserimento di un nuovo elemento nella lista di priorita'
    // la priorita' massima si ha all'inizio, con valore piu' alto

    if (pNewElement!=NULL || pNewLeaf!=NULL)
    {
        // caso di pInitList == NULL
        if (pInitList == NULL)
        {
            // allora il nuovo elemento e' il primo della lista
            pInitList = new LIST_ELEMENT;
            pInitList->pElementPointer = pNewElement;
            pInitList->pLeaf = pNewLeaf;
            pInitList->value = NewElementPriority;
            pInitList->pFwdElem=NULL;
            pInitList->pBwdElem=NULL;

            pEndList=pInitList; // se e' il primo elemento ed e' l'unico, allora e' anche l'ultimo

            return iRet;
        }

        // caso di pInitList != NULL
        // cerco il primo elemento a priorita' inferiore o il primo elemento che abbia fwd =null
        if (pInitList!=NULL)
        {
            // creo nuovo elemento
            LIST_ELEMENT *pTempElem1;
            pTempElem1=new LIST_ELEMENT; // creo il nuovo elemento
            pTempElem1->value = NewElementPriority;
            pTempElem1->pElementPointer=pNewElement;
            pTempElem1->pLeaf = pNewLeaf;

            //inizio a scandire la lista
            LIST_ELEMENT *pCursorElement = pInitList;
            LIST_ELEMENT *pPrevElement = NULL;

            if (NewElementPriority>pCursorElement->value) // se maggiore del primo elemento
            {
                pInitList=pTempElem1;
                pTempElem1->pBwdElem=NULL;
                pTempElem1->pFwdElem=pCursorElement;
                pCursorElement->pBwdElem=pTempElem1;
            }
            else
            {
                while(pCursorElement!=NULL)
                {
                    if(NewElementPriority<=pCursorElement->value)
                    {
                        pPrevElement=pCursorElement;
                        pCursorElement=(LIST_ELEMENT *)pCursorElement->pFwdElem;
                    }
                    else
                    {
                        break;
                    }
                }
                //innesto dell'elemento
                pTempElem1->pFwdElem=pCursorElement;
                if (pCursorElement!=NULL) //ultimo elemento
                {
                    pCursorElement->pBwdElem=pTempElem1;
                }
                pTempElem1->pBwdElem=pPrevElement;
                pPrevElement->pFwdElem=pTempElem1;

            }


            pCursorElement = pInitList;	 // riparto dall'inizio
            // cerco la fine della lista
            while (pCursorElement->pFwdElem!=NULL)
            {
                // eseguo un avanzamento
                pCursorElement	= (LIST_ELEMENT *)pCursorElement->pFwdElem;
            }
            pEndList = pCursorElement; // raggiunta la fine della lista

        }
    }
    else
    {
        iRet=-1;
    }

    return iRet;
}

// Elimina elemento dalla lista
int CPriorityList::DeleteElement(LIST_ELEMENT *ElementToRemove)
{
    int iRet=0;
    // dato il puntatore ad un elemento, si controlla se fwd e bwd sono != NULL
    // si procede all'estirpazione
    if (ElementToRemove!=NULL)
    {
        // se bwd == NULL, allora e' il primo elemento
        if (ElementToRemove->pBwdElem!=NULL)
        {
            LIST_ELEMENT *PreviousElement = (LIST_ELEMENT *)ElementToRemove->pBwdElem;
            if(PreviousElement!=NULL)
            {
                PreviousElement->pFwdElem = (LIST_ELEMENT *)ElementToRemove->pFwdElem;
            }
        }
        else
        {
            // se invece e' il primo
            pInitList = (LIST_ELEMENT *)ElementToRemove->pFwdElem;
            if(pInitList!=NULL)
            {
                pInitList->pBwdElem=NULL;
            }
        }

        // se fwd == NULL, allora e' l'ultimo elemento
        if (ElementToRemove->pFwdElem!=NULL)
        {
            LIST_ELEMENT *NextElement = (LIST_ELEMENT *)ElementToRemove->pFwdElem;
            if(NextElement!=NULL)
            {
                NextElement->pBwdElem = (LIST_ELEMENT *)ElementToRemove->pBwdElem;
            }
        }
        else
        {
            // se invece e' l'ultimo
            pEndList = (LIST_ELEMENT *)ElementToRemove->pBwdElem;
            if(pEndList!=NULL)
            {
                pEndList->pFwdElem=NULL;
            }
        }
    delete ElementToRemove;
    ElementToRemove = NULL;
    }
    else
    {
        iRet=-1;
    }

    return iRet;
}

// Inizializza Lista
int CPriorityList::InitPriorityList(void)
{
    return 0;
}

// cancella tutto gli elementi della lista
int CPriorityList::CleanPriorityList(LIST_ELEMENT * pInit)
{
    //brutal mode
    int iRet=0;
    LIST_ELEMENT *Cursor = (LIST_ELEMENT *)pInit;
    LIST_ELEMENT *TempElem = NULL;
    if (pInit!=NULL)
    {
        while (Cursor->pFwdElem!=NULL)
        {
            TempElem = (LIST_ELEMENT *)Cursor;
            Cursor=(LIST_ELEMENT *)Cursor->pFwdElem;
            delete TempElem;
            TempElem = NULL;
        }
        delete Cursor;
        Cursor = NULL;
        pEndList = NULL;
        pInitList = NULL;
    }
    else
    {	// l'elemento iniziale non esiste
        iRet=-1;
    }
    return iRet;
}

// lettura della lista concatenata in senso diretto
int CPriorityList::ListReadFwd(LIST_ELEMENT * pInit)
{
    int iRet=0;
    LIST_ELEMENT *Cursor = (LIST_ELEMENT *)pInit;
    if (pInit!=NULL)
    {
        while (Cursor->pFwdElem!=NULL)
        {

            //cout<<"List Element : char "<<Cursor->value<<"\n";
            Cursor=(LIST_ELEMENT *)Cursor->pFwdElem;
        }
    }
    else
    {	// l'elemento iniziale non esiste
        iRet=-1;
    }
    return iRet;
}

// porta il cursore della lista all'inizio della stessa, a pInitList
int CPriorityList::ResetCursor(void)
{
    int iRet = 0;
    if (pInitList==NULL)
    {
        iRet = -1;
    }
    else
    {
        pListCursor=pInitList;
    }
    return iRet;
}

// avanti di un elemento
int CPriorityList::CursorGoFwd(void)
{
    int iRet=0;
    if (pListCursor->pFwdElem!=NULL)
    {
        pListCursor = (LIST_ELEMENT *)pListCursor->pFwdElem;
    }
    else
    {
        iRet=-1;
    }
    return iRet;
}

//indietro di un elemento
int CPriorityList::CursorGoBackward(void)
{
    int iRet=0;
    if (pListCursor->pBwdElem!=NULL)
    {
        pListCursor = (LIST_ELEMENT *)pListCursor->pBwdElem;
    }
    else
    {
        iRet=-1;
    }
    return iRet;
}

//permette di ottenere l'elemento puntato dal cursore
LIST_ELEMENT * CPriorityList::GetCursorElement(void)
{
    return pListCursor;
}
