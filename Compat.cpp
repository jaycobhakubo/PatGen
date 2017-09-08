
#include "stdafx.h"
#include "patgen.h"

#if 0
#define MEMCHECK_H_

/*MCCONFIG{{  Do not modify these lines - use MCCONFIG! */
   static char szMCFile[] = __FILE__;
   #define _MCSF_ szMCFile 
   #include <memcheck.h>
/*}}MCCONFIG*/
#endif

extern "C" {
int _fastcall ElimSubSets(DWORD* mlist, DWORD num);
extern DWORD RLE_EncodeBuffer(BYTE* inbuf, BYTE* outbuf, DWORD len);
extern DWORD RLE_DecodeBuffer(BYTE* inbuf, BYTE* outbuf);
extern DWORD RLE_GetExpandedSize(BYTE* inbuf);
};

#undef TRACE
void TRACE(LPCTSTR lpszFormat, ...);

// index a cell's bit position (0-24) int this to get bit mask for new position
// of the cell for a 90 degree rotation
DWORD rot90masks[] = {
	0x10,0x200,0x4000,0x80000,0x1000000,
	0x8,0x100,0x2000,0x40000,0x800000,
	0x4,0x80,0x1000,0x20000,0x400000,
	0x2,0x40,0x800,0x10000,0x200000,
	0x1,0x20,0x400,0x8000,0x100000,
};

///////////////////////////////////////////////////////////////////////////////
// CCompressedPattern
///////////////////////////////////////////////////////////////////////////////

CCompressedPattern::CCompressedPattern()
{
	m_pMasks = NULL;
	strcpy(m_pName,"<NONE>");
	m_nNumMasks = 0;
	m_nNumHits = 0;
	m_nAttributes = 0;
	m_nMultiBingos = 0;
	m_nHandHeldVersion = FALSE;
}

CCompressedPattern::~CCompressedPattern()
{
	if (m_pMasks != NULL)
		free(m_pMasks);
}

void CCompressedPattern::SetKey(WORD key)
{
	m_nKey = key;
}
WORD CCompressedPattern::GetKey()
{
	return(m_nKey);
}

void CCompressedPattern::SetName(char* name)
{
	strcpy(m_pName,name);
}
char* CCompressedPattern::GetName()
{
	return(&m_pName[0]);
}

void CCompressedPattern::SetMaxMasks(WORD max)
{
	m_nMaxMasks = max;
	m_pMasks = (DWORD *)malloc(m_nMaxMasks*sizeof(DWORD));
}

WORD CCompressedPattern::GetNumMasks()
{
	return(m_nNumMasks);
}
WORD CCompressedPattern::GetNumHits()
{
	return(m_nNumHits);
}
void CCompressedPattern::SetNumHits(WORD hits)
{
	m_nNumHits = hits;
}

void CCompressedPattern::SetAttributes(DWORD butes)
{
	m_nAttributes = butes;
}

BOOL CCompressedPattern::GetNoOverlap()
{
	if (m_nAttributes & PDEA_NOOVERLAP)
		return(TRUE);
	else
		return(FALSE);
}

void CCompressedPattern::SetNoOverlap(BOOL flag)
{
	if (flag)
		m_nAttributes |= PDEA_NOOVERLAP;
	else
		m_nAttributes &= ~PDEA_NOOVERLAP;
}

BOOL CCompressedPattern::GetRotating()
{
	if (m_nAttributes & PDEA_ROTATING)
		return(TRUE);
	else
		return(FALSE);
}

void CCompressedPattern::SetRotating(BOOL flag)
{
	if (flag)
		m_nAttributes |= PDEA_ROTATING;
	else
		m_nAttributes &= ~PDEA_ROTATING;
}

void CCompressedPattern::SetSingleFrame(DWORD mask)
{
	m_nRepresentativeSingleFrame = mask;
}

void CCompressedPattern::SetMultiBingos(WORD num)
{
	m_nMultiBingos = num;
}

DWORD CCompressedPattern::GetMask(WORD index)
{
	if (index < m_nNumMasks)
	{
		return(m_pMasks[index]);
	}

	return(0);
}

BOOL CCompressedPattern::Add(DWORD mask)
{
	if (m_nNumMasks < m_nMaxMasks)
	{
		m_pMasks[m_nNumMasks] = mask;
		m_nNumMasks++;
		return(TRUE);
	}
		return(FALSE);
}

void CCompressedPattern::SetMasks(DWORD* mlist)
{
	m_nNumMasks = m_nMaxMasks;
	for(int ii=0;ii<m_nMaxMasks;ii++)
		m_pMasks[ii] = mlist[ii];
}

// returns length of data need to store this pattern
DWORD CCompressedPattern::GetBinaryLength()
{
	DWORD len = m_nNumMasks * sizeof(DWORD);
	len += strlen(m_pName)+1;
	len += sizeof(m_nNumMasks);
	len += sizeof(m_nNumHits);
	len += sizeof(m_nAttributes);
	if (!m_nHandHeldVersion)
		len += sizeof(m_nRepresentativeSingleFrame);
	len += sizeof(m_nMultiBingos);

//	TRACE(_T("len = %d\n",len);

	return(len);
}

BOOL CCompressedPattern::WriteToFile(FILE* fp)
{

	// don't write the m_nKey or m_nMaxMasks
#if 1
	fwrite(&m_nNumMasks,sizeof(m_nNumMasks),1,fp);
	fwrite(m_pMasks,sizeof(DWORD),m_nNumMasks,fp);	// write masks
	fwrite(&m_nNumHits,sizeof(m_nNumHits),1,fp);
	fwrite(&m_nAttributes,sizeof(m_nAttributes),1,fp);
	if (!m_nHandHeldVersion)
		fwrite(&m_nRepresentativeSingleFrame,sizeof(m_nRepresentativeSingleFrame),1,fp);
	fwrite(&m_nMultiBingos,sizeof(m_nMultiBingos),1,fp);
	fwrite(&m_pName[0],sizeof(char),strlen(m_pName)+1,fp);
#else
	// see if we can RLE compress the masks and gain anything
	static BYTE* obuf = NULL;
	if (obuf == NULL)
		obuf = (BYTE *)malloc(20000*sizeof(DWORD));

	DWORD olen  = m_nNumMasks*sizeof(DWORD);
	DWORD clen = RLE_EncodeBuffer((BYTE *)m_pMasks,obuf,olen);
	TRACE(_T("olen=%d,clen=%d\n",olen,clen);
	if (clen < olen)
	{ // did save some space
		fwrite(&m_nNumMasks,sizeof(m_nNumMasks),1,fp);
		fwrite(obuf,1,clen,fp);	// write masks
		TRACE(_T("DID SAVE\n");
	}
	else
	{ // won't save any space
		fwrite(&m_nNumMasks,sizeof(m_nNumMasks),1,fp);
		fwrite(m_pMasks,sizeof(DWORD),m_nNumMasks,fp);	// write masks
	}

	fwrite(&m_nNumHits,sizeof(m_nNumHits),1,fp);
	fwrite(&m_nAttributes,sizeof(m_nAttributes),1,fp);
	if (!m_nHandHeldVersion)
		fwrite(&m_nRepresentativeSingleFrame,sizeof(m_nRepresentativeSingleFrame),1,fp);
	fwrite(&m_nMultiBingos,sizeof(m_nMultiBingos),1,fp);
	fwrite(&m_pName[0],sizeof(char),strlen(m_pName)+1,fp);
#endif

	return(TRUE);
}

DWORD* CCompressedPattern::GetFromBuffer(BYTE* b, DWORD len, WORD& nummasks, 
			WORD& numhits, DWORD& butes, WORD& multibingos, char* name, DWORD& singlemask)
{
	// get nummasks
	WORD* wp = (WORD *)b;
	nummasks = *wp;wp++;

	// get the masks
	DWORD* maskptr = (DWORD *)wp;

	DWORD* dp = maskptr + nummasks;

	// get numhits
	wp = (WORD *)dp;
	numhits = *wp;wp++;
	dp = (DWORD *)wp;

	// get attributes
	butes = *dp;dp++;

	if (!m_nHandHeldVersion)
		singlemask = *dp;dp++;

	// get multibingos
	wp = (WORD *)dp;
	multibingos = *wp;wp++;

	// get name
	char* bp = (char *)wp;

//	TRACE(_T("name = %s\n",bp);

	strcpy(name,bp);
	
	return(maskptr);
}

WORD CCompressedPattern::Expand(DWORD* mlist, WORD nummasks, WORD num_hits, DWORD butes, DWORD* outlist)
{
	WORD outmasks = 0;

	if (nummasks <= 0) return(0);
	if (num_hits <= 0) return(0);

	int ii,jj,kk,ll,mm;

	if (num_hits == 1)
	{
		if (butes & PDEA_ROTATING)
		{
	//
	// handle rotating mask here
	//
			for(ii=0;ii<nummasks;ii++)
			{
				DWORD mask = mlist[ii];
				outlist[outmasks++] = mask;	// 0 degree mask
				// now rotate 90 degrees, 3 times
				for(jj=0;jj<3;jj++)
				{
					DWORD mask1 = 0;
					for(kk=0;kk<25;kk++,mask = mask>>1)
					{
						if (mask & 1) mask1 |= rot90masks[kk];
					}
					outlist[outmasks++] = mask1;
					mask = mask1;		// save the 90 degree rot, and and rot it again
				}
			}
		}
		else
		{
	//
	// non rotating single masks here
	//
			memcpy(outlist,mlist,nummasks*sizeof(DWORD));
			outmasks = nummasks;
		}
		return(outmasks);
	}

	//
	// handle multi hit masks here
	//

	// check for group bits
	BOOL hasgroups = FALSE;
	for(ii=0;ii<nummasks;ii++)
	{
		if (mlist[ii] & MASKGROUP_BIT)
			hasgroups = TRUE;
	}

	if (hasgroups)
	{
		if (num_hits == 2)
		{
			DWORD mask,mask2;
			DWORD mm,mm2;

			for(ii=0;ii<nummasks-1;ii++)
			{
				for(jj=ii+1;jj<nummasks;jj++)
				{
					if (butes & PDEA_NOOVERLAP)
					{
						mask  = mlist[jj] | mlist[ii];
						mask2 = mlist[jj] ^ mlist[ii];
						if (mask == mask2)
							outlist[outmasks++] = (mask & ~MASKGROUP_BIT);
					}
					else
					{
						mask =  mlist[ii];
						mask2 = mlist[jj];
						if ((mask&mask2)&MASKGROUP_BIT)
						{ // same group, reject overlaps
							mm  = mlist[ii] & ~MASKGROUP_BIT;
							mm2 = mlist[jj] & ~MASKGROUP_BIT;
							mask  = mm | mm2;
							mask2 = mm ^ mm2;
							if (mask == mask2)
								outlist[outmasks++] = (mask & ~MASKGROUP_BIT);
						}
						else
						{
							mask  &= ~MASKGROUP_BIT;
							mask2 &= ~MASKGROUP_BIT;
							if ((mask | mask2) != mask)
								outlist[outmasks++] = mask | mask2;
						}
					} // end else
				} // end for jj
			} // end for ii
		} // end for 2 hits


	if (num_hits == 3)
	{
		DWORD mask,mask1,mask2;
		for(ii=0;ii<nummasks-2;ii++)
		{
			for(jj=ii+1;jj<nummasks-1;jj++)
			{
				for(kk=jj+1;kk<nummasks;kk++)
				{
					mask = mlist[ii];
					mask1 = mlist[jj];
					mask2 = mlist[kk];
					if (GroupCheck(mask,mask1) &&
						 GroupCheck(mask,mask2) &&
						 GroupCheck(mask1,mask2))
					{
#if 1
						outlist[outmasks++] = ((mask | mask1 | mask2)&~MASKGROUP_BIT);
#else
						outmasks++;
#endif
					}
				}
			} // end for jj
		} // end for ii
		TRACE(_T("3 HITS Group Before masks = %d\n"),outmasks);

//		int tot_masks = ElimSubSets(&outlist[0],outmasks);
//		TRACE(_T("3 HITS Group After masks = %d\n",tot_masks);

#if 0
		// delete any masks which are subsets of other masks
		int tot_masks = outmasks;
		for(ii=0;ii<outmasks-1;ii++)
		{
			for(jj=ii+1;jj<outmasks;jj++)
			{
				DWORD m1 = outlist[ii];
				DWORD m2 = outlist[jj];
				if (m1 != MLIST_DELIMITER && m2 != MLIST_DELIMITER)
				{
					if ((m1&m2) == m1)
					{ // m1 is subset of m2, delete m2
						outlist[jj] = MLIST_DELIMITER;
						tot_masks--;
					}
					else
					{
						if ((m1&m2) == m2)
						{ // m2 is a subset of m1, delete m1
							outlist[ii] = MLIST_DELIMITER;
							tot_masks--;
						}
					}
				} // end if
			} // end for jj
		} // end for ii

		TRACE(_T("3 HITS Group After masks = %d\n",tot_masks);
#endif

	}

	if (num_hits == 4)
	{
		DWORD mask,mask1,mask2,mask3;
		for(ll=0;ll<nummasks-3;ll++)
		{
			for(ii=ll+1;ii<nummasks-2;ii++)
			{
				for(jj=ii+1;jj<nummasks-1;jj++)
				{
					for(kk=jj+1;kk<nummasks;kk++)
					{
						mask3 = mlist[ll];
						mask  = mlist[ii];
						mask1 = mlist[jj];
						mask2 = mlist[kk];
						if (GroupCheck(mask,mask1) &&
							 GroupCheck(mask,mask2) &&
							 GroupCheck(mask,mask3) &&
							 GroupCheck(mask1,mask2) &&
							 GroupCheck(mask1,mask3) &&
							 GroupCheck(mask2,mask3))
						{
#if 1
							outlist[outmasks++] = ((mask | mask1 | mask2 | mask3)&~MASKGROUP_BIT);
#else
							outmasks++;
#endif
						} // end if
					} // end for kk
				} // end for jj
			} // end for ii
		} // end for ll
		TRACE(_T("4 HITS Group Before masks = %d\n"),outmasks);
#if 0
	   outlist[0] = 0x1FFFFFF;
		outmasks = 1;
#endif
	} // end if num_hits == 4

#if 0
		// delete any masks which are subsets of other masks
		int tot_masks = outmasks;
		for(ii=0;ii<outmasks-1;ii++)
		{
			for(jj=ii+1;jj<outmasks;jj++)
			{
				DWORD m1 = outlist[ii];
				DWORD m2 = outlist[jj];
				if (m1 != MLIST_DELIMITER && m2 != MLIST_DELIMITER)
				{
					if ((m1&m2) == m1)
					{ // m1 is subset of m2, delete m2
						outlist[jj] = MLIST_DELIMITER;
						tot_masks--;
					}
					else
					{
						if ((m1&m2) == m2)
						{ // m2 is a subset of m1, delete m1
							outlist[ii] = MLIST_DELIMITER;
							tot_masks--;
						}
					}
				} // end if
			} // end for jj
		} // end for ii

		TRACE(_T("After Subset Elim masks = %d\n",tot_masks);
#else
#if 0
		int tot_masks = ElimSubSets(&outlist[0],outmasks);
		TRACE(_T("After Subset Elim masks = %d\n",tot_masks);
#else
#endif
		outmasks = ElimSubSets(&outlist[0],outmasks);
#endif

		return(outmasks);
	}

	// in an effort to save memory, gonna assume a max of 4 hits for now
	if (num_hits == 2)
	{
		DWORD mask,mask2;
		for(ii=0;ii<nummasks-1;ii++)
		{
			for(jj=ii+1;jj<nummasks;jj++)
			{
				if (butes & PDEA_NOOVERLAP)
				{
					mask  = mlist[jj] | mlist[ii];
					mask2 = mlist[jj] ^ mlist[ii];
					if (mask == mask2)
						outlist[outmasks++] = mask;
				}
				else
				{
					mask =  mlist[ii];
					mask2 = mlist[jj];
					if ((mask | mask2) != mask)
						outlist[outmasks++] = mask | mask2;
				} // end else
			} // end for jj
		} // end for ii
	} // end for 2 hits
	
	if (num_hits == 3)
	{
		DWORD mask,mask1,mask2;
		for(ii=0;ii<nummasks-2;ii++)
		{
			for(jj=ii+1;jj<nummasks-1;jj++)
			{
				for(kk=jj+1;kk<nummasks;kk++)
				{
					if (butes & PDEA_NOOVERLAP)
					{
						mask = mlist[ii];
						mask1 = mlist[jj];
						mask2 = mlist[kk];
						if ((mask & mask1) ||
							 (mask & mask2) ||
							 (mask1 & mask2))
						{
						}
						else
						{
#if 1
							outlist[outmasks++] = (mask | mask1 | mask2);
#else
							outmasks++;
#endif
						}
					}
					else
					{
						mask = mlist[ii];
						mask1 = mlist[jj];
						mask2 = mlist[kk];
						if ((mask != mask1) &&
							 (mask != mask2) &&
							 (mask1 != mask2))
						{
#if 1
							outlist[outmasks++] = (mask | mask1 | mask2);
#else
							outmasks++;
#endif
						}
					}
				}
			} // end for jj
		} // end for ii
#if 0
		// temp
		outlist[outmasks++] = 0x1FFFFFF;
		TRACE(_T("3 HITS - masks = %d\n",outmasks);
#endif

	}

	if (num_hits == 4)
	{
		DWORD mask,mask1,mask2,mask3;
		for(ll=0;ll<nummasks-3;ll++)
		{
			for(ii=ll+1;ii<nummasks-2;ii++)
			{
				for(jj=ii+1;jj<nummasks-1;jj++)
				{
					for(kk=jj+1;kk<nummasks;kk++)
					{
						{
							mask3 = mlist[ll];
							mask  = mlist[ii];
							mask1 = mlist[jj];
							mask2 = mlist[kk];
#if 0
							if ((mask != mask1) &&
							 	(mask != mask2) &&
							 	(mask1 != mask2))
#endif
							{
								outlist[outmasks++] = (mask | mask1 | mask2 | mask3);
							}
						}
					}
				} // end for jj
			} // end for ii
		} // end for ll

	} // end if num_hits == 4

	if (num_hits == 5)
	{
		DWORD mask,mask1,mask2,mask3,mask4;
		for(mm=0;mm<nummasks-4;mm++)
		{
			for(ll=mm+1;ll<nummasks-3;ll++)
			{
				for(ii=ll+1;ii<nummasks-2;ii++)
				{
					for(jj=ii+1;jj<nummasks-1;jj++)
					{
						for(kk=jj+1;kk<nummasks;kk++)
						{
							{
								mask4 = mlist[mm];
								mask3 = mlist[ll];
								mask  = mlist[ii];
								mask1 = mlist[jj];
								mask2 = mlist[kk];
								{
									outlist[outmasks++] = (mask | mask1 | mask2 | mask3 | mask4);
								}
							}
						}
					} // end for jj
				} // end for ii
			} // end for ll
		} // end for mm

	} // end if num_hits == 5

#if 0
	// how's about always reducing subsets
	outmasks = ElimSubSets(&outlist[0],outmasks);
#endif

	return(outmasks);
}

DWORD CCompressedPattern::CalcFilter(DWORD* mlist, WORD nummasks)
{
	DWORD filter = 0;

	for(int ii=0;ii<nummasks;ii++)
	{
		if (mlist[ii] != MLIST_DELIMITER)
			filter |= mlist[ii];
	}

	return(filter);
}

BOOL CCompressedPattern::GroupCheck(DWORD m1, DWORD m2)
{
	DWORD um1 = m1 & ~MASKGROUP_BIT;
	DWORD um2 = m2 & ~MASKGROUP_BIT;

	if ((m1&m2)&MASKGROUP_BIT)
	{ // same group, so no overlap
		if (um1 & um2) return(FALSE);	// overlap
		return(TRUE);
	}
	else
	{
		if (um1 == um2) return(FALSE);
		return(TRUE);
	}
}

void CCompressedPattern::Print()
{
	TRACE(_T("--- %d: %s\n"),(signed short)m_nKey, m_pName);
	TRACE(_T("Num = %d, Hits = %d, Attribs = %d, MultiBingo = %d - "),m_nNumMasks,m_nNumHits,m_nAttributes,m_nMultiBingos);

	if ( m_nAttributes & 0x0001  ) TRACE(_T("PDEA_NOOVERLAP "));
	if ( m_nAttributes & 0x0002  ) TRACE(_T("PDEA_ROTATING "));
	if ( m_nAttributes & 0x0004  ) TRACE(_T("PDEA_UNIQUEMASKS "));
	if ( m_nAttributes & 0x0008  ) TRACE(_T("PDEA_MASKLIST "));
	if ( m_nAttributes & 0x0010  ) TRACE(_T("PDEA_EACHLIST "));
	if ( m_nAttributes & 0x0020  ) TRACE(_T("PDEA_COUNT "));
	if ( m_nAttributes & 0x0040  ) TRACE(_T("PDEA_ANDEACHLIST "));
	if ( m_nAttributes & 0x0080  ) TRACE(_T("PDEA_PATLIST "));
	if ( m_nAttributes & 0x0100  ) TRACE(_T("PDEA_PLUSCOUNT "));
	if ( m_nAttributes & 0x0200  ) TRACE(_T("PDEA_OREACHLIST "));
	if ( m_nAttributes & 0x0400  ) TRACE(_T("PDEA_REQEACHLIST "));
	if ( m_nAttributes & 0x0800  ) TRACE(_T("PDEA_BIGLIST "));
	if ( m_nAttributes & 0x8000  ) TRACE(_T("PDEA_SPECIALCASE "));
	if ( m_nAttributes & 0x4000  ) TRACE(_T("PDEA_ORLIST "));
	if ( m_nAttributes & 0x2000  ) TRACE(_T("PDEA_ANDLIST "));
	if ( m_nAttributes & 0x1000  ) TRACE(_T("PDEA_PUREANDLIST "));
	if ( m_nAttributes & 0x10000 ) TRACE(_T("PDEA_COUNTORLIST "));
	if ( m_nAttributes & 0x20000 ) TRACE(_T("PDEA_AWAYOFF "));
    //	if (GetRotating()) TRACE(_T("Rotating "));
    //	if (GetNoOverlap()) TRACE(_T("No Overlap"));
	TRACE(_T("\n Masks - "));
	int ii;
#if 0
	for(ii=0;ii<m_nNumMasks;ii++)
		TRACE(_T("%08X ",m_pMasks[ii]);
#else
#if 1
	TRACE(_T("\n"));
	for(ii=0;ii<m_nNumMasks;ii++)
	{
		PrintMask(m_pMasks[ii]);
		TRACE(_T(".\n"));
	}
#endif
#endif

	TRACE(_T("\n\n"));
}

void CCompressedPattern::PrintMask(DWORD mask)
{
	for(int ii=0;ii<25;ii++,mask = mask>>1)
	{
		if (mask & 1) TRACE(_T("1"));
			else
				TRACE(_T("+"));
		if ((ii+1)%5 == 0) TRACE(_T("\n"));
		//if ((ii+1)%5 == 0) TRACE(_T(","));
	}
	TRACE(_T("\n"));
}

