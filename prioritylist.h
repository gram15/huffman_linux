#ifndef PRIORITYLIST_H
#define PRIORITYLIST_H

#pragma once

struct LIST_ELEMENT
{
    // valore, in realt� un puntatore void
    unsigned int value;
    void *pElementPointer;
    void *pLeaf;
    void *pFwdElem;
    void *pBwdElem;
};


class CPriorityList
{
private:
    LIST_ELEMENT *pInitList;
    LIST_ELEMENT *pEndList;
    LIST_ELEMENT *pListCursor;

public:
    CPriorityList(void);
    ~CPriorityList(void);
    // Primo elemento della lista
    LIST_ELEMENT * FirstElement(void);
    // Ultimo elemento della lista
    LIST_ELEMENT * LastElement(void);
    // Inserisci nuovo elemento
    int InsertNewElement(void *pNewElement, void *pNewLeaf,  unsigned int NewElementPriority);
    // Elimina elemento dalla lista
    int DeleteElement(LIST_ELEMENT *ElementToRemove);
    // Inizializza Lista
    int InitPriorityList(void);
    //pulisce lista esistente eliminando tutti gli elementi
    int CleanPriorityList(LIST_ELEMENT * pInit);
    // lettura della lista concatenata in senso diretto
    int ListReadFwd(LIST_ELEMENT * pInit);
    // porta il cursore della lista all'inizio della stessa, a pInitList
    int ResetCursor(void);
    int CursorGoFwd(void);
    int CursorGoBackward(void);
    LIST_ELEMENT * GetCursorElement(void);
};


#endif // PRIORITYLIST_H
