//#include "../videoSender/stdafx.h"
#include "List.h"

//---------------------------------------------------------------------------------------
KList::KList(void)
:m_ulSize(0)
{
}

//---------------------------------------------------------------------------------------
KList::~KList(void)
{
	clear();
}

//---------------------------------------------------------------------------------------
KList::iterator KList::begin(void)
{
	return *m_iterator.m_pNext;
}

//---------------------------------------------------------------------------------------
KList::iterator KList::end(void)
{
	return m_iterator;
}

//---------------------------------------------------------------------------------------
void*KList::front(void)
{
	return m_iterator.m_pNext->m_pMember;
}

//---------------------------------------------------------------------------------------
void KList::pop_front(void)
{
	iterator*piterator=m_iterator.m_pNext;
	if (piterator!=&m_iterator)
	{
		m_iterator.m_pNext=piterator->m_pNext;
		piterator->m_pNext->m_pPrev=&m_iterator;
		delete piterator;
		piterator=NULL;

		m_ulSize--;
	}
}

//---------------------------------------------------------------------------------------
void KList::push_front(void* ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pBeginiterator=m_iterator.m_pNext;
		m_iterator.m_pNext=piterator;
		pBeginiterator->m_pPrev=piterator;
		piterator->m_pPrev=&m_iterator;
		piterator->m_pNext=pBeginiterator;
		m_ulSize++;
	}
}

//---------------------------------------------------------------------------------------
void* KList::back(void)
{
	return m_iterator.m_pPrev->m_pMember;
}

//---------------------------------------------------------------------------------------
void KList::pop_back(void)
{
	iterator*piterator=m_iterator.m_pPrev;
	if (piterator!=&m_iterator)
	{
		m_iterator.m_pPrev=piterator->m_pPrev;
		piterator->m_pPrev->m_pNext=&m_iterator;
		delete piterator;
		piterator=NULL;

		m_ulSize--;
	}
}

//---------------------------------------------------------------------------------------
void KList::push_back(void* ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pEnditerator=m_iterator.m_pPrev;

		m_iterator.m_pPrev=piterator;
		pEnditerator->m_pNext=piterator;
		piterator->m_pPrev=pEnditerator;
		piterator->m_pNext=&m_iterator;
		m_ulSize++;
	}
}

//---------------------------------------------------------------------------------------
void KList::insert(KList::iterator titerator,void* ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pInsertAfteriterator=titerator.m_pPrev;;
		iterator*pInsertBeforeiterator=pInsertAfteriterator->m_pNext;
		pInsertAfteriterator->m_pNext=piterator;
		piterator->m_pPrev=pInsertAfteriterator;
		piterator->m_pNext=pInsertBeforeiterator;
		pInsertBeforeiterator->m_pPrev=piterator;
		m_ulSize++;
	}
}

//---------------------------------------------------------------------------------------
void KList::erase(KList::iterator titerator)
{
	iterator*pEraseAfteriterator=titerator.m_pPrev;;
	iterator*pEraseiterator=pEraseAfteriterator->m_pNext;
	iterator*pEraseBeforeiterator=pEraseiterator->m_pNext;
	if (pEraseiterator!=&m_iterator)
	{
		pEraseAfteriterator->m_pNext=pEraseBeforeiterator;
		pEraseBeforeiterator->m_pPrev=pEraseAfteriterator;
		delete pEraseiterator;
		pEraseiterator=NULL;

		m_ulSize--;
	}
}

//---------------------------------------------------------------------------------------
unsigned long KList::size(void)
{
	return m_ulSize;
}

//---------------------------------------------------------------------------------------
bool KList::empty(void)
{
	if(m_ulSize==0)
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------
void KList::clear(void)
{
	while (size())
	{
		pop_front();
	}
}

//---------------------------------------------------------------------------------------
void KList::remove(void* ptr)
{
	KList::iterator item=find(ptr);
	erase(item);
}

//---------------------------------------------------------------------------------------
KList::iterator KList::find(void* ptr)
{
	KList::iterator item=begin();
	while (item!=end())
	{
		if (item.m_pMember==ptr)
		{
			break;
		}
		++item;
	}
	return item;
}

