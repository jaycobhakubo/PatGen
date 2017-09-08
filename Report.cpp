//##########################################################################
//
// PatDump.cpp : Defines the entry point for the console application.
//
//##########################################################################

#include "stdafx.h"
#include "PatGen.h"
#include <list> 
#include <Afxcoll.h>

// Database stuff that looks important.
extern Code4 cb;
extern Data4 data;
extern Tag4 tag;
extern Tag4 tagStyle;


int m_nMaxBits = 0;
int m_nTotPat  = 0;

int m_nTotBDG  = 0;
int m_nTotFSM  = 0;
int m_nTot1ON  = 0;
int m_nTotDA   = 0;
int m_nTot3ON  = 0;
int m_nTot4ON  = 0;
int m_nTot6ON  = 0;
int m_nTot9ON  = 0;


#define        MAX_PAT_KEYS 256
int pnPatKeys[ MAX_PAT_KEYS ];

int nNumPatKeys = 0;


// Pattern flag/name lookup structure
struct SIndexKey
{
    DWORD  dwFlag;
    char  *szName;
    DWORD  dwMaskOverlap;
    DWORD  dwMaskNoOverlap;
};

// Pattern flag/name lookup structure
struct SIndexKeyStyle
{
    DWORD  dwFlag;
    char  *szStyleName;
};


#define MAX_TAGS_TYPE  18
#define MAX_TAGS_STYLE 6

SIndexKey tags[ MAX_TAGS_TYPE ]  =
{
    { 0x0000,  "NORMAL",        PDEA_NONE,         PDEA_NOOVERLAP                     },
    { 0x0002,  "ROTATING",      PDEA_ROTATING,     PDEA_ROTATING    | PDEA_NOOVERLAP  },
    { 0x0004,  "UNIQUEMASKS",   PDEA_UNIQUEMASKS,  PDEA_UNIQUEMASKS | PDEA_NOOVERLAP  },
    { 0x0008,  "MASKLIST",      PDEA_MASKLIST,     PDEA_MASKLIST    | PDEA_NOOVERLAP  },
    { 0x0010,  "EACHLIST",      PDEA_EACHLIST,     PDEA_EACHLIST    | PDEA_NOOVERLAP  },
    { 0x0020,  "COUNT",         PDEA_COUNT,        PDEA_COUNT       | PDEA_NOOVERLAP  },
    { 0x0040,  "ANDEACHLIST",   PDEA_ANDEACHLIST,  PDEA_ANDEACHLIST | PDEA_NOOVERLAP  },
    { 0x0080,  "PATLIST",       PDEA_PATLIST,      PDEA_PATLIST     | PDEA_NOOVERLAP  },
    { 0x0100,  "PLUSCOUNT",     PDEA_PLUSCOUNT,    PDEA_PLUSCOUNT   | PDEA_NOOVERLAP  },
    { 0x0200,  "OREACHLIST",    PDEA_OREACHLIST,   PDEA_OREACHLIST  | PDEA_NOOVERLAP  },
    { 0x0400,  "REQEACHLIST",   PDEA_REQEACHLIST,  PDEA_REQEACHLIST | PDEA_NOOVERLAP  },
    { 0x0800,  "BIGLIST",       PDEA_BIGLIST,      PDEA_BIGLIST     | PDEA_NOOVERLAP  },
    { 0x8000,  "SPECIALCASE",   PDEA_SPECIALCASE,  PDEA_SPECIALCASE | PDEA_NOOVERLAP  },
    { 0x4000,  "ORLIST",        PDEA_ORLIST,       PDEA_ORLIST      | PDEA_NOOVERLAP  },
    { 0x2000,  "ANDLIST",       PDEA_ANDLIST,      PDEA_ANDLIST     | PDEA_NOOVERLAP  },
    { 0x1000,  "PUREANDLIST",   PDEA_PUREANDLIST,  PDEA_PUREANDLIST | PDEA_NOOVERLAP  },
    { 0x10000, "COUNTORLIST",   PDEA_COUNTORLIST,  PDEA_COUNTORLIST | PDEA_NOOVERLAP  },
    { 0x20000, "AWAYOFF",       PDEA_AWAYOFF,      PDEA_AWAYOFF     | PDEA_NOOVERLAP  },
};

SIndexKey tagsStyle[ MAX_TAGS_STYLE ]  =
{
    { 0x0000,  "1-ON"},
    { 0x0002,  "3-ON"},
    { 0x0004,  "4-ON"},
    { 0x0008,  "6-ON"},
    { 0x0010,  "9-ON"},
    { 0x0020,  "DA"},
};


// Position of each flag combination in output table
struct SFlagTag
{
    DWORD dwMask;
    int   x;
    int   y;
};


SFlagTag FlagInd[]  =
{
    { PDEA_NONE,                           0,  0 },
    { PDEA_NOOVERLAP,                      0,  1 },

    { PDEA_ROTATING,                       1,  0 },
    { PDEA_ROTATING | PDEA_NOOVERLAP,      1,  1 },

    { PDEA_UNIQUEMASKS,                    2,  0 },
    { PDEA_UNIQUEMASKS | PDEA_NOOVERLAP,   2,  1 },

    { PDEA_MASKLIST,                       3,  0 },
    { PDEA_MASKLIST | PDEA_NOOVERLAP,      3,  1 },

    { PDEA_EACHLIST,                       4,  0 },
    { PDEA_EACHLIST | PDEA_NOOVERLAP,      4,  1 },

    { PDEA_COUNT,                          5,  0 },
    { PDEA_COUNT | PDEA_NOOVERLAP,         5,  1 },

    { PDEA_ANDEACHLIST,                    6,  0 },
    { PDEA_ANDEACHLIST | PDEA_NOOVERLAP,   6,  1 },

    { PDEA_PATLIST,                        7,  0 },
    { PDEA_PATLIST | PDEA_NOOVERLAP,       7,  1 },

    { PDEA_PLUSCOUNT,                      8,  0 },
    { PDEA_PLUSCOUNT | PDEA_NOOVERLAP,     8,  1 },

    { PDEA_OREACHLIST,                     9,  0 },
    { PDEA_OREACHLIST | PDEA_NOOVERLAP,    9,  1 },

    { PDEA_REQEACHLIST,                    10, 0 },
    { PDEA_REQEACHLIST | PDEA_NOOVERLAP,   10, 1 },

    { PDEA_BIGLIST,                        11, 0 },
    { PDEA_BIGLIST | PDEA_NOOVERLAP,       11, 1 },

    { PDEA_SPECIALCASE,                    12, 0 },
    { PDEA_SPECIALCASE | PDEA_NOOVERLAP,   12, 1 },

    { PDEA_ORLIST,                         13, 0 },
    { PDEA_ORLIST | PDEA_NOOVERLAP,        13, 1 },

    { PDEA_ANDLIST,                        14, 0 },
    { PDEA_ANDLIST | PDEA_NOOVERLAP,       14, 1 },

    { PDEA_PUREANDLIST,                    15, 0 },
    { PDEA_PUREANDLIST | PDEA_NOOVERLAP,   15, 1 },

    { PDEA_COUNTORLIST,                    16, 0 },
    { PDEA_COUNTORLIST | PDEA_NOOVERLAP,   16, 1 },

    { PDEA_AWAYOFF,                        17, 0 },
    { PDEA_AWAYOFF | PDEA_NOOVERLAP,       17, 1 },

    { 0xFFFFFFFF, -1, -1 },
};


typedef int TOT_ARRAY[ MAX_TAGS_TYPE ][ 2 ];

int ppnTotalsBDG [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotalsFSM [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotals1ON [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotalsDA  [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotals3ON [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotals4ON [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotals6ON [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };
int ppnTotals9ON [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };

// For filtering 
#define        MAX_PAT_LIST 10000
int pnPatList[ MAX_PAT_LIST ];
int nNumPatList = 0;

int ppnTotalBig  [ MAX_TAGS_TYPE ][ 2 ]  =  { 0 };

typedef BOOL (*IS_FN)( int nPattern );


static
BOOL IsBDG( int nPattern )
{
    if ( nPattern <= -1100  &&
         nPattern >= -1150 )
    {
        return TRUE;
    }

    return ( FALSE );
}


static
BOOL IsFSM( int nPattern )
{
    if ( nPattern >= 4000  &&
         nPattern <  4500 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL Is1ON( int nPattern )
{
    if ( IsBDG( nPattern )  ||
         IsFSM( nPattern ) )
    {
        return FALSE;
    }
        
    if ( nPattern < 400  ||
         nPattern > 1100 )
    {
        return TRUE;
    }
    else if ( nPattern >= 25000 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL IsDA( int nPattern )
{
    if ( nPattern > 800  &&
         nPattern < 900 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL Is3ON( int nPattern )
{
    if ( nPattern > 900  &&
         nPattern < 1000 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL Is4ON( int nPattern )
{
    if ( nPattern > 400  &&
         nPattern < 500 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL Is6ON( int nPattern )
{
    if ( nPattern > 600  &&
         nPattern < 700 )
    {
        return TRUE;
    }

    return FALSE;
}


static
BOOL Is9ON( int nPattern )
{
    if ( nPattern > 1000  &&
         nPattern < 1100 )
    {
        return TRUE;
    }

    return FALSE;
}


static
int CountPatterns( PATDEF *pPatTable )
{
    int nCount = 0;

    for ( int i = 0;  pPatTable[ i ].key  !=  0;  i++ )
    {
        nCount++;
    }

    return nCount;
}


static
int CountFlags( DWORD dwVal )
{
    int nCount = 0;

    for ( int i = 0;  i < 32;  i++ )
    {
        DWORD dwMask  =  0x01 << i;

        if ( dwVal & dwMask )
        {
            nCount++;
        }
    }

    return nCount;
}


static
int CountMaxBits( PATDEF *pPatTable )
{
    int nMaxBits = 0;

    for ( int i = 0;  i < m_nTotPat;  i++ )
    {
        int nCount  =  CountFlags( pPatTable[ i ].attributes );

        if ( nCount > nMaxBits )
        {
            nMaxBits = nCount;
        }
    }

    return nMaxBits;
}


static
void CountPatTypes( PATDEF *pPatTable )
{
    m_nTotBDG  = 0;
    m_nTotFSM  = 0;
    m_nTot1ON  = 0;
    m_nTot3ON  = 0;
    m_nTot4ON  = 0;
    m_nTot6ON  = 0;
    m_nTot9ON  = 0;
    m_nTotDA   = 0;

    for ( int i = 0;  i < m_nTotPat;  i++ )
    {
        int nPattern  =  pPatTable[ i ].key;

        if      ( IsBDG( nPattern ) )  m_nTotBDG++;
        else if ( IsFSM( nPattern ) )  m_nTotFSM++;
        else if ( Is1ON( nPattern ) )  m_nTot1ON++;
        else if ( Is3ON( nPattern ) )  m_nTot3ON++;
        else if ( Is4ON( nPattern ) )  m_nTot4ON++;
        else if ( Is6ON( nPattern ) )  m_nTot6ON++;
        else if ( Is9ON( nPattern ) )  m_nTot9ON++;
        else if ( IsDA(  nPattern ) )  m_nTotDA++;
    }
}


CCompressedPattern* FindPattern( int nKey )
{
    // Find a pattern record in the g_patterns array
    for ( int i = 0;  i < g_numpatterns;  i++ )
    {
        CCompressedPattern& Pat  =  g_patterns[ i ];

        if ( Pat.GetKey()  ==  ( (WORD) nKey ) )
        {
            return & Pat;
        }
    }

    return NULL;
}


void SwapPattern( int &nPat1,
                  int &nPat2 )
{
    int nTemp = nPat1;

    nPat1 = nPat2;
    nPat2 = nTemp;
}


void SortPatKeys( PATDEF *pPatTable )
{
    BOOL bSwapped;

    do
    {
        bSwapped = FALSE;

        for ( int nPat = 0;  nPat  <  nNumPatKeys - 1;  nPat++ )
        {
            PATDEF *pPat1  =  & pPatTable[ pnPatKeys[ nPat     ] ];
            PATDEF *pPat2  =  & pPatTable[ pnPatKeys[ nPat + 1 ] ];

            if ( pPat1->hits < pPat2->hits )
            {
                continue;
            }
            else if ( pPat1->hits > pPat2->hits )
            {
                SwapPattern( pnPatKeys[ nPat     ],
                             pnPatKeys[ nPat + 1 ] );

                bSwapped = TRUE;
            }
            else if ( pPat1->multibingos < pPat2->multibingos )
            {
                continue;
            }
            else if ( pPat1->multibingos > pPat2->multibingos )
            {
                SwapPattern( pnPatKeys[ nPat     ],
                             pnPatKeys[ nPat + 1 ] );

                bSwapped = TRUE;
            }
            else if ( pPat1->key < pPat2->key )
            {
                continue;
            }
            else if ( pPat1->key > pPat2->key )
            {
                SwapPattern( pnPatKeys[ nPat     ],
                             pnPatKeys[ nPat + 1 ] );

                bSwapped = TRUE;
            }
        }
    }
    while ( bSwapped );
}


// Is pattern key in the pattern key table?
static
BOOL PatKeyDefined( PATDEF *pTargetPat,
                    PATDEF *pPatTable )
{
    for ( int nPat = 0;  nPat < nNumPatKeys;  nPat++ )
    {
        PATDEF *pPat  =  & pPatTable[ pnPatKeys[ nPat ] ];

        if ( pPat->hits        == pTargetPat->hits  &&
             pPat->multibingos == pTargetPat->multibingos )
        {
            // Already in the table
            return TRUE;
        }
    }

    // Not found
    return FALSE;
}


// Create PatNo/Hits/MultiBingo summary
static
BOOL SummarizePatGroup( FILE   *pFile,
                        PATDEF *pPatTable,
                        IS_FN   fn,
                        int     nTag,
                        BOOL    bOverlap )
{
    // Create our pattern list for this category
    nNumPatList = 0;

    // Loop through all patterns and create our filtered pattern list
    for ( int nPat = 0;  nPat < m_nTotPat;  nPat++ )
    {
        // Is this the right pattern category?
        PATDEF *pPat      =  & pPatTable[ nPat ];
        int     nPattern  =  pPat->key;

        if ( ! fn( nPattern ) )
        {
            // Wrong category - skip it
            continue;
        }

        // Look only at the approriate patterns for this flag tag
        DWORD dwMask;

        if ( bOverlap )  dwMask  =  tags[ nTag ].dwMaskOverlap;
        else             dwMask  =  tags[ nTag ].dwMaskNoOverlap;  

        if ( pPat->attributes != dwMask )
        {
            // This is the flag tag we're interested in
            continue;
        }

        if ( nNumPatList >= MAX_PAT_LIST )
        {
            fprintf( stderr,
                     "\n*** ERROR: Too many patterns for report (MAX=%d)\n", 
                     MAX_PAT_LIST );
            return FALSE;
        }

        pnPatList[ nNumPatList++ ]  =  nPat;
    }


    //---------------------------------------------------------
    // We now have just the patterns for this tag/flag/category
    // in pnPatList[].  Create our pnPatKeys[] table with only
    // the unique combinations of Hits/MultiBingos.
    //---------------------------------------------------------

    nNumPatKeys = 0;

    for ( int nPat = 0;  nPat < nNumPatList;  nPat++ )
    {
        // Get shortcut to pattern definition
        int     nPattern  =  pnPatList[ nPat ];
        PATDEF *pPat      =  & pPatTable[ nPattern ];
        
        if ( ! PatKeyDefined( pPat, pPatTable ) )
        {
            // This pattern isn't in the pattern key list - add it
            if ( nNumPatKeys >= MAX_PAT_KEYS )
            {
                fprintf( stderr,
                         "\n*** ERROR: Too many unique pattern keys for pattern category/flag.  MAX=%d\n",
                         MAX_PAT_KEYS );

                return FALSE;
            }
        
            pnPatKeys[ nNumPatKeys++ ]  =  pnPatList[ nPat ];
        }
    }


    //----------------------------------------------------
    // The only patterns in pnPatKeys[] are the unique 
    // Hits/MultiBingo counts.  Output them to report file
    //----------------------------------------------------

    if ( nNumPatKeys > 0 )
    {
        SortPatKeys( pPatTable );

        fprintf( pFile, "\n" );
        fprintf( pFile, "            Unique Pattern Summary\n" );
        fprintf( pFile, "            ----------------------\n" );

        for ( int nPat = 0;  nPat < nNumPatKeys;  nPat++ )
        {
            PATDEF *pPat  =  & pPatTable[ pnPatKeys[ nPat ] ];

            fprintf( pFile,
                     "            Hits=%-3d MultiBingo=%-3d Pattern=%-5d Name=%s\n",
                     int( pPat->hits ),
                     int( pPat->multibingos ),
                     int( pPat->key ),
                     pPat->name );
        }

        fprintf( pFile, "\n" );
    }
        
    return TRUE;
}


static
void OutputBigHeader( FILE *pFile,
                      char *pszBuf )
{
    int nLen  =  strlen( pszBuf );

    fprintf( pFile, " ____________________________________________________________________________\n" );
    fprintf( pFile, "|                                                                            |\n" );
    
    fprintf( pFile, "| %s", pszBuf );

    for ( int i = 0;  i  <  78 - nLen - 3;  i++ )
    {
        fputc( ' ', pFile );
    }

    fprintf( pFile, "|\n" );
    fprintf( pFile, "|____________________________________________________________________________|\n" );

}


static
void OutputUnderlinedText( FILE *pFile,
                           char *pszBuf,
                           char  cUnderlineChar,
                           int   nIndent )
{
    char pszIndent[ 256 ];

    int i;
    for ( i = 0;  i < nIndent;  i++ )
    {
        pszIndent[ i ]  =  ' ';
    }

    pszIndent[ i ]  =  '\0';


    fprintf( pFile, "%s%s\n", pszIndent, pszBuf );
    fprintf( pFile, "%s",     pszIndent );

    int nLen  =  strlen( pszBuf );

    for ( i = 0;  i < nLen;  i++ )
    {
        fputc( cUnderlineChar, pFile );
    }

    fprintf( pFile, "\n" );
}


static
BOOL Iterate( FILE      *pFile,
              PATDEF    *pPatTable,
              IS_FN      fn,
              TOT_ARRAY &ppnTotals,
              char       *pszName,
              int        nGroupTotal )
{
    ZeroMemory( ppnTotals,
                sizeof( TOT_ARRAY ) );

    int nCalc = 0;

    //------------------------------------------------
    // Calculate flag totals for this pattern category
    //------------------------------------------------

    for ( int nPat = 0;  nPat < m_nTotPat;  nPat++ )
    {
        int nPattern  =  pPatTable[ nPat ].key;

        if ( ! fn( nPattern ) )
        {
            // This isn't the right pattern for this iteration
            continue;
        }

        DWORD dwAttr  =  pPatTable[ nPat ].attributes;
        BOOL  bFound  =  FALSE;

        for ( int nFlag = 0;  FlagInd[ nFlag ].dwMask  !=  0xFFFFFFFF;  nFlag++ )
        {
            if ( dwAttr  ==  FlagInd[ nFlag ].dwMask )
            {
                bFound = TRUE;

                int x  =  FlagInd[ nFlag ].x;
                int y  =  FlagInd[ nFlag ].y;

                ppnTotals[ x ] [ y ]++;

                break;
            }
        }

        if ( ! bFound )
        {
            // Found special case
            fprintf( stderr,
                     "\n*** ERROR: FOUND SPECIAL FLAG CASE: NAME=%s, FLAG=0x%08lX\n",
                     pszName,
                     dwAttr );

            return FALSE;
        }

        nCalc++;
    }

    // Sanity check
    if ( nCalc != nGroupTotal )
    {
        // Stuff doesn't match!  UGH!
        fprintf( stderr,
                 "\n*** ERROR: COUNT MISMATCH: NAME=%s (nCalc=%d, nGroupTotal=%d)\n",
                 pszName,
                 nCalc,
                 nGroupTotal );

        return FALSE;
    }

    // Dump flag count table
    char pszBuf[ 256 ];

    sprintf( pszBuf,
             "PATTERN TYPE: %s",
             pszName );

    fprintf( pFile, "\n" );
    OutputBigHeader( pFile, pszBuf );
    fprintf( pFile, "\n" );

    fprintf( pFile, "     NAME             OVERLAP  NON-OVERLAP\n" );
    fprintf( pFile, "     =====================================\n" );

    int nOverlap   = 0;
    int nNoOverlap = 0;

    for ( int nTag = 0;  nTag < MAX_TAGS_TYPE;  nTag++ )
    {
        fprintf( pFile,
                 "     %-15.15s %8d  %8d\n",
                 tags[ nTag ].szName,
                 ppnTotals[ nTag ][ 0 ],
                 ppnTotals[ nTag ][ 1 ] );

        nOverlap    +=  ppnTotals[ nTag ][ 0 ];
        nNoOverlap  +=  ppnTotals[ nTag ][ 1 ];
    }

    fprintf( pFile, "     ======================================\n" );
    fprintf( pFile, "                     %8d  %8d (TOTAL=%d)\n",
             nOverlap,
             nNoOverlap,
             nOverlap + nNoOverlap );

    // More sanity checking
    if ( nOverlap + nNoOverlap  !=  nGroupTotal )
    {
        fprintf( stderr, "\n*** ERROR: TOTALS DON'T MATCH! (%d)\n", nGroupTotal );

        return FALSE;
    }


    //-----------------------------------------------------------
    // Crank out detailed information about this pattern category
    //-----------------------------------------------------------

    fprintf( pFile, "\n" );

    BOOL bBlankNeeded = FALSE;

    // Output each flag combination individually
    for ( int nTag = 0;  nTag < MAX_TAGS_TYPE;  nTag++ )
    {
        // Do it for overlapping, and non-overlapping
        for ( int nOverlap = 0;  nOverlap <= 1;  nOverlap++ )   // 0=overlap
        {                                                       // 1=no overlap
            if ( ppnTotals[ nTag ] [ nOverlap ] )
            {
                if ( bBlankNeeded )
                {
                    fprintf( pFile, "\n" );
                }

                // Show overlapping patterns
                sprintf( pszBuf,
                         "%s %s (%s)",
                         pszName,
                         tags[ nTag ].szName,
                         ( nOverlap == 0 )
                            ? "OVERLAP OK"
                            : "NO OVERLAP" );

                OutputUnderlinedText( pFile,
                                      pszBuf,
                                      '=',
                                      5 );

                // Create PatNo/Hits/MultiBingo summary
                if ( ! SummarizePatGroup( pFile,
                                          pPatTable,
                                          fn,
                                          nTag,
                                          BOOL( nOverlap == 0 ) ) )
                {
                    return FALSE;
                }

                bBlankNeeded = TRUE;

                fprintf( pFile, "            Pattern Listing\n" );
                fprintf( pFile, "            ---------------\n" );

                for ( int nPat = 0;  nPat < m_nTotPat;  nPat++ )
                {
                    PATDEF *pPat  =  & pPatTable[ nPat ];

                    if ( ! fn( pPat->key ) )
                    {
                        continue;
                    }

                    DWORD dwMask;

                    if ( nOverlap == 0 )  dwMask  =  tags[ nTag ].dwMaskOverlap;
                    else                  dwMask  =  tags[ nTag ].dwMaskNoOverlap;  

                    if ( pPat->attributes == dwMask )
                    {
                        CCompressedPattern* pComPat  =  FindPattern( int( pPat->key ) );

                        if ( pComPat != NULL )
                        {                        
                            fprintf( pFile,
                                     "            %-5d %-25.25s  Hits=%-3d MultiBingo=%-3d NumMasks=%d\n",
                                     int( pPat->key ),
                                     pPat->name,
                                     int( pPat->hits ),
                                     int( pPat->multibingos ),
                                     pComPat->GetNumMasks() );
                        }    
                        else
                        {
                            fprintf( pFile,
                                     "            %-5d %-25.25s  Hits=%-3d MultiBingo=%-3d NumMasks=NULL\n",
                                     int( pPat->key ),
                                     pPat->name,
                                     int( pPat->hits ),
                                     int( pPat->multibingos ) );
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}


static
void SwapPats( PATDEF *pPat1,
               PATDEF *pPat2 )
{
    PATDEF temp;

    CopyMemory( & temp,
                pPat1,
                sizeof( PATDEF ) );

    CopyMemory( pPat1,
                pPat2,
                sizeof( PATDEF ) );

    CopyMemory( pPat2,
                & temp,
                sizeof( PATDEF ) );
}


static
void SortPatterns( PATDEF *pPatTable )
{
    while ( 1 )
    {
        BOOL bAnySwapped = FALSE;

        for ( int i = 0;  i < m_nTotPat - 1;  i++ )
        {
            PATDEF *pPat1  =  & pPatTable[ i     ];   
            PATDEF *pPat2  =  & pPatTable[ i + 1 ];   

            if ( pPat1->key < 0   &&
                 pPat2->key > 0 )
            {
                // Negative goes at end
                SwapPats( pPat1, pPat2 );

                bAnySwapped = TRUE;
            }                 
            else if ( pPat1->key > 0   &&
                      pPat2->key < 0 )
            {
                // Skip - already at end
                continue;
            }                 
            else if ( pPat1->key < 0  &&
                      pPat2->key < 0  &&
                      pPat1->key < pPat2->key )
            {
                SwapPats( pPat1, pPat2 );

                bAnySwapped = TRUE;
            }
            else if ( pPat1->key > 0  &&
                      pPat2->key > 0  &&
                      pPat1->key > pPat2->key )
            {
                SwapPats( pPat1, pPat2 );

                bAnySwapped = TRUE;
            }
        }

        if ( ! bAnySwapped ) 
        {
            break;
        }
    }
}


static
void ComputeTotals( void )
{
    for ( int i = 0;  i < MAX_TAGS_TYPE;  i++ )
    {
        ppnTotalBig[ i ][ 0 ]  =  ppnTotalsBDG[ i ][ 0 ]  +
                                  ppnTotalsFSM[ i ][ 0 ]  +
                                  ppnTotals1ON[ i ][ 0 ]  +
                                  ppnTotalsDA [ i ][ 0 ]  +
                                  ppnTotals3ON[ i ][ 0 ]  +
                                  ppnTotals4ON[ i ][ 0 ]  +
                                  ppnTotals6ON[ i ][ 0 ]  +
                                  ppnTotals9ON[ i ][ 0 ];  

        ppnTotalBig[ i ][ 1 ]  =  ppnTotalsBDG[ i ][ 1 ]  +
                                  ppnTotalsFSM[ i ][ 1 ]  +
                                  ppnTotals1ON[ i ][ 1 ]  +
                                  ppnTotalsDA [ i ][ 1 ]  +
                                  ppnTotals3ON[ i ][ 1 ]  +
                                  ppnTotals4ON[ i ][ 1 ]  +
                                  ppnTotals6ON[ i ][ 1 ]  +
                                  ppnTotals9ON[ i ][ 1 ];  
    }
}


// Pattern report generator
BOOL CreateReport( char   *pszFilename, 
                   PATDEF *pPatTable )
{
    FILE *pFile  =  fopen( pszFilename, "wt" );

    if ( pFile == NULL )
    {
        return FALSE;
    }

    m_nTotPat   =  CountPatterns( pPatTable );
    m_nMaxBits  =  CountMaxBits(  pPatTable );

    SortPatterns(  pPatTable );
    CountPatTypes( pPatTable );

    if ( ! Iterate( pFile, pPatTable, IsBDG, ppnTotalsBDG, "BDG",  m_nTotBDG )  ||
         ! Iterate( pFile, pPatTable, IsFSM, ppnTotalsFSM, "FSM",  m_nTotFSM )  ||
         ! Iterate( pFile, pPatTable, Is1ON, ppnTotals1ON, "1-ON", m_nTot1ON )  ||
         ! Iterate( pFile, pPatTable, Is3ON, ppnTotals3ON, "3-ON", m_nTot3ON )  ||
         ! Iterate( pFile, pPatTable, Is4ON, ppnTotals4ON, "4-ON", m_nTot4ON )  ||
         ! Iterate( pFile, pPatTable, Is6ON, ppnTotals6ON, "6-ON", m_nTot6ON )  ||
         ! Iterate( pFile, pPatTable, Is9ON, ppnTotals9ON, "9-ON", m_nTot9ON )  ||
         ! Iterate( pFile, pPatTable, IsDA,  ppnTotalsDA,  "DA",   m_nTotDA  ) )
    {
        fclose( pFile );

        return FALSE;
    }

    ComputeTotals();

    // Dump master table
    fprintf( pFile, "\n" );
    OutputBigHeader( pFile, "GRAND TOTALS" );
    fprintf( pFile, "\n" );

    // Show pattern type summary
    OutputUnderlinedText( pFile,
                          "PATTERN TYPE SUMMARY",
                          '=',
                          5 );

    fprintf( pFile, "\n" );
    fprintf( pFile, "                Total  BDG: %d\n", m_nTotBDG );
    fprintf( pFile, "                Total  FSM: %d\n", m_nTotFSM );
    fprintf( pFile, "                Total 1-ON: %d\n", m_nTot1ON );
    fprintf( pFile, "                Total 3-ON: %d\n", m_nTot3ON );
    fprintf( pFile, "                Total 4-ON: %d\n", m_nTot4ON );
    fprintf( pFile, "                Total 6-ON: %d\n", m_nTot6ON );
    fprintf( pFile, "                Total 9-ON: %d\n", m_nTot9ON );
    fprintf( pFile, "                Total   DA: %d\n", m_nTotDA  );
    fprintf( pFile, "          -----------------------\n"       );
    fprintf( pFile, "            Total patterns: %d\n", m_nTotPat );

    // Show pattern flag summary
    fprintf( pFile, "\n" );
    fprintf( pFile, "\n" );
    OutputUnderlinedText( pFile,
                          "PATTERN STYLE SUMMARY",
                          '=',
                          5 );

    fprintf( pFile, "\n" );
    fprintf( pFile, "          NAME             OVERLAP  NON-OVERLAP\n" );
    fprintf( pFile, "          --------------------------------------\n" );

    int nOverlap   = 0;
    int nNoOverlap = 0;

    for ( int i = 0;  i < MAX_TAGS_TYPE;  i++ )
    {
        fprintf( pFile, "          %-15.15s %8d  %8d\n",
                 tags[ i ].szName,
                 ppnTotalBig[ i ][ 0 ],
                 ppnTotalBig[ i ][ 1 ] );

        nOverlap    +=  ppnTotalBig[ i ][ 0 ];
        nNoOverlap  +=  ppnTotalBig[ i ][ 1 ];
    }

    fprintf( pFile, "          --------------------------------------\n" );
    fprintf( pFile, "                          %8d  %8d (TOTAL=%d)\n",
             nOverlap,
             nNoOverlap,
             nOverlap + nNoOverlap );

    fprintf( pFile, "          --------------------------------------\n" );

    fprintf( pFile, "\n" );
    fprintf( pFile, "\n" );
    fprintf( pFile, "==============================================================================\n" );
    fprintf( pFile, "                                     # # #\n" );
    fprintf( pFile, "==============================================================================\n" );

    fclose( pFile );

    BOOL bRez = CreateCompatFiles( pPatTable );
    bRez = CreateCompatMatrix( pPatTable );

    return TRUE;
}

// A new Function that does Stuff.
BOOL CreateCompatFiles( PATDEF *pPatTable )
{
    CMapStringToString* pmSpecialCases = new CMapStringToString; 

    CStringArray* pAryByNumRecords = new CStringArray; 
    
    CStringArray* pAry1OnRecords = new CStringArray; 
    CStringArray* pAry3OnRecords = new CStringArray; 
    CStringArray* pAry4OnRecords = new CStringArray;  
    CStringArray* pAry6OnRecords = new CStringArray; 
    CStringArray* pAry9OnRecords = new CStringArray; 
    CStringArray* pAryDARecords = new CStringArray; 

    pAry1OnRecords->SetSize( 4000 );
    pAry1OnRecords->SetSize( 500 );
    pAry3OnRecords->SetSize( 500 );
    pAry4OnRecords->SetSize( 500 );
    pAry6OnRecords->SetSize( 500 );
    pAry9OnRecords->SetSize( 500 );
    pAryDARecords->SetSize( 500 );

    CString csStyle       = "";
    CString csPatternType = "";
    CString csOverlap     = "";
    CString csPatNum      = "";
    CString csPatName     = "";

    CString csFlags       = "";

    CString csRecordByStyle = "";
    CString csRecordByNum   = "";

    int iPoint;
    BOOL bRez;

    PopulateSpecialCasesList( pmSpecialCases );

    char* pszCompatSummaryByStyleFN = "Pattern Compatibility Summary_By_Style_Doh.csv";
    char* pszCompatSummaryNumberFN  = "Pattern Compatibility Summary_By_Number_Doh.csv";
    char* pszProductCompatMatrixFN  = "Product Pattern Compatibility Matrix_Doh.csv";
    char  pszTempString[64]  = {0};
    char  pszTempString2[64] = {0};

    FILE *pCompatSummNumFile   =  fopen( pszCompatSummaryNumberFN,  "wt" );
    FILE *pCompatSummStyleFile =  fopen( pszCompatSummaryByStyleFN, "wt" );

    BOOL bObsoleteRecord = FALSE;

    if (( pCompatSummNumFile   == NULL ) || 
        ( pCompatSummStyleFile == NULL ))

    {
        return FALSE;
    }


    // Print summary by pattern number and by style header
    fprintf( pCompatSummNumFile,   ",,,,,,,,,,,,,,,,,,,,,,,\n" );
    fprintf( pCompatSummNumFile,   "Pat #,Pattern Name,Style,Pattern Type,Overlap?,Traveler,,TED,,TED-2-C,,Handheld,,Fixed Base,,,,,,,,,,\n" );

    fprintf( pCompatSummStyleFile, ",,,,,,,,,,,,,,,,,,,,,,,\n" );
    fprintf( pCompatSummStyleFile, "Style,Pattern Type,Overlap?,Pat #,Pattern Name,Traveler,,TED,,TED-2-C,,Handheld,,Fixed Base,,,,,,,,,,\n" );

    SortPatterns(  pPatTable );

    // Assume, hope and pray that nTotPat was initialized correctly previously.
    // Loop through the DB by pattern number, build the two sets of records.
    // One set is ordered by pattern number.
    // The other set is ordered by pattern style.
    for (int iIdx = 0; iIdx < m_nTotPat; iIdx++)
    {
        bObsoleteRecord = FALSE;

        // Get the Pattern Number.
        DWORD dwPatNum  =  pPatTable[ iIdx ].key;
        itoa( dwPatNum, pszTempString, 10);
        csPatNum = CString ( pszTempString );

        // Pattern Name
        pszTempString[0] = 0;
        strncpy( pszTempString, pPatTable[ iIdx ].name, (strlen( pPatTable[ iIdx ].name ) + 1));
        csPatName = CString( pszTempString );
        csPatName.Replace(',','_');

        // Pattern Style
        pszTempString[0] = 0;
        GetPatternStyle( dwPatNum, pszTempString );
        csStyle = CString ( pszTempString );

        // Pattern Type & Overlap
        pszTempString[0] = 0;
        GetPatternType( pPatTable[ iIdx ].attributes, pszTempString, pszTempString2 );
        csPatternType = CString( pszTempString );
        csOverlap     = CString( pszTempString2 );

        bRez = pmSpecialCases->Lookup( csPatNum, csFlags );

        if ( !bRez )
        {
            pmSpecialCases->Lookup( CString( "0" ), csFlags);
        }

        csFlags.Insert(0, "Trav:#");

        iPoint = csFlags.Find(",", 1);
        if ( iPoint >= 1 )
        {
            csFlags.Insert(iPoint+1, "TED:#");
        }

        iPoint = csFlags.Find(",", iPoint+1);
        if ( iPoint >= 1 )
        {
            csFlags.Insert(iPoint+1, "TED2C:#");
        }

        iPoint = csFlags.Find(",", iPoint+1);
        if ( iPoint >= 1 )
        {
            csFlags.Insert(iPoint+1, "HH:#");
        }

        iPoint = csFlags.Find(",", iPoint+1);
        if ( iPoint >= 1 )
        {
            csFlags.Insert(iPoint+1, "FB:#");
        }

        csFlags.Replace('#', ',');

        // Build output records.
        csRecordByStyle = csStyle   + "," + csPatternType + "," + csOverlap  + "," +  
                          csPatNum  + "," + csPatName     + "," + csFlags + "\n";

        csRecordByNum   = csPatNum      + "," + csPatName + "," + csStyle   + "," +
                          csPatternType + "," + csOverlap + "," + csFlags + "\n";

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        if ( csStyle == "1-ON" )
        {
            pAry1OnRecords->Add( csRecordByStyle );
        }
        else if ( csStyle == "3-ON" )
        {
            pAry3OnRecords->Add( csRecordByStyle );
        }
        else if ( csStyle == "4-ON" )
        {
            pAry4OnRecords->Add( csRecordByStyle );
        }
        else if ( csStyle == "6-ON" )
        {
            pAry6OnRecords->Add( csRecordByStyle );
        }
        else if ( csStyle == "9-ON" )
        {
            pAry9OnRecords->Add( csRecordByStyle );
        }
        else if ( csStyle == "DA" )
        {
            pAryDARecords->Add( csRecordByStyle );
        }
        else
            bObsoleteRecord = TRUE;

        // Stuff the by-pattern-number records into
        // a nice air conditioned storage facility.
        if ( !bObsoleteRecord )
            pAryByNumRecords->Add( csRecordByNum );

    }

    // Loop through again and spew out the arrays into a handy local 
    // .csv file.
    for ( int iIdx = 0; iIdx < pAryByNumRecords->GetSize(); iIdx++)
    {
        // Print index by Pattern Number      
        fprintf( pCompatSummNumFile, "%s", pAryByNumRecords->GetAt( iIdx ));
    }

    // 1-ON
    for ( int iIdx = 0; iIdx < pAry1OnRecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAry1OnRecords->GetAt( iIdx ));
    }

    // 3-ON
    for ( int iIdx = 0; iIdx < pAry3OnRecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAry3OnRecords->GetAt( iIdx ));
    }

    // 4-ON
    for ( int iIdx = 0; iIdx < pAry4OnRecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAry4OnRecords->GetAt( iIdx ));
    }

    // 6-ON
    for ( int iIdx = 0; iIdx < pAry6OnRecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAry6OnRecords->GetAt( iIdx ));
    }

    // 9-ON
    for ( int iIdx = 0; iIdx < pAry9OnRecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAry9OnRecords->GetAt( iIdx ));
    }

    // DA
    for ( int iIdx = 0; iIdx < pAryDARecords->GetSize(); iIdx++)
    {

        // Yet another Uber-hack to kind of sort the 'By Style' output.
        fprintf( pCompatSummStyleFile, "%s", pAryDARecords->GetAt( iIdx ));
    }


    pmSpecialCases->RemoveAll();

    delete pmSpecialCases;

    delete pAry1OnRecords;
    delete pAry3OnRecords;
    delete pAry4OnRecords;
    delete pAry6OnRecords;
    delete pAry9OnRecords;
    delete pAryDARecords;

    return 0;
}

// This function returns the style of this pattern.
int GetPatternStyle( int   dwPatNum,
                     char* pszStyle )
{
    char* pszOnString = "???";

    if ( Is1ON( dwPatNum )) pszOnString = "1-ON";

    if ( Is3ON( dwPatNum )) pszOnString = "3-ON";

    if ( Is4ON( dwPatNum )) pszOnString = "4-ON";

    if ( Is6ON( dwPatNum )) pszOnString = "6-ON";

    if ( Is9ON( dwPatNum )) pszOnString = "9-ON";

    if ( IsDA(  dwPatNum )) pszOnString = "DA";

    // The following two styles are considered obsolete and not
    // supported anymore.
    if ( IsBDG( dwPatNum )) pszOnString = "BDG";

    if ( IsFSM( dwPatNum )) pszOnString = "FSM";
    
    strncpy( pszStyle, pszOnString, strlen( pszOnString )+1);
    
    return 0;

}

// This function returns the style of this pattern.
int GetPatternType( int   iAttributes,
                    char* pszOverlap,
                    char* pszType)
{

    if ( iAttributes & PDEA_NOOVERLAP )
        strncpy(pszOverlap, "NoOverlap", strlen("NoOverlap\0")+1);
    else
        strncpy(pszOverlap, "Overlap", strlen("Overlap\0")+1);
    
    if ( iAttributes == PDEA_NONE )
        strncpy(pszType, "Normal", strlen("Normal")+1);

    if ( iAttributes & PDEA_EACHLIST )
        strncpy(pszType, "EachList", strlen("EachList")+1);

    if ( iAttributes & PDEA_ANDEACHLIST )      
        strncpy(pszType, "AndEachList", strlen("AndEachList")+1);

    if ( iAttributes & PDEA_OREACHLIST )
        strncpy(pszType, "OREachList", strlen("OREachList")+1);

    if ( iAttributes & PDEA_REQEACHLIST )
        strncpy(pszType, "ReqEachList", strlen("ReqEachList")+1);

    if ( iAttributes & PDEA_ORLIST )
        strncpy(pszType, "OrList", strlen("OrList")+1);

    if ( iAttributes & PDEA_ANDLIST )
        strncpy(pszType, "AndList", strlen("AndList")+1);

    if ( iAttributes & PDEA_PATLIST )
        strncpy(pszType, "PatList", strlen("PatList")+1);

    if ( iAttributes & PDEA_PUREANDLIST )
        strncpy(pszType, "PureAndList", strlen("PureAndList")+1);

    if ( iAttributes & PDEA_ROTATING )
        strncpy(pszType, "Rotating", strlen("Rotating")+1);

    if ( iAttributes & PDEA_UNIQUEMASKS )
        strncpy(pszType, "UniqueMasks", strlen("UniqueMasks")+1);

    if ( iAttributes & PDEA_COUNT )
        strncpy(pszType, "Count", strlen("Count")+1);

    if ( iAttributes & PDEA_PLUSCOUNT )
        strncpy(pszType, "PlusCount", strlen("PlusCount")+1);

    if ( iAttributes & PDEA_BIGLIST )
        strncpy(pszType, "BigList", strlen("BigList")+1);

    if ( iAttributes & PDEA_SPECIALCASE )
        strncpy(pszType, "SpecialCase", strlen("SpecialCase")+1);

    if ( iAttributes & PDEA_COUNTORLIST )
        strncpy(pszType, "CountOrList", strlen("CountOrList")+1);

    if ( iAttributes & PDEA_MASKLIST )
        strncpy(pszType, "MaskList", strlen("MaskList")+1);

    if ( iAttributes & PDEA_AWAYOFF )
        strncpy(pszType, "AwayOff", strlen("AwayOff")+1);
    
    return 1;
}


// Create the Cross-Compat spreadsheet.
BOOL CreateCompatMatrix( PATDEF *pPatTable )
{

    SortPatterns(  pPatTable );
    m_nTotPat   =  CountPatterns( pPatTable );
    m_nMaxBits  =  CountMaxBits(  pPatTable );
    int iLen = 0;
    int iNewLength = 0;
    char* pBufferInSpew;
    char pszTempString[7] = {0};

    CFile* pInSpewTemplate = new CFile();
    CFile* pOutSpewMatrix  = new CFile();

    CString csTotPat, csTot1ON, csTotDA, csTot3ON, csTot4ON, csTot6ON, csTot9ON;

    if (!pInSpewTemplate->Open( _T("matrix_template.txt"),
                                    CFile::modeRead))
        return FALSE;

    if (!pOutSpewMatrix->Open( _T("Pattern_Compatibility_ Summary.htm"),
                                 ( CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate )))
        return FALSE;

    iLen = (int)pInSpewTemplate->GetLength();

    if ( iLen < 1 )
        return FALSE;

    pBufferInSpew = (char*)::malloc( iLen + 1 );
    
    pInSpewTemplate->Read( pBufferInSpew, iLen );

    // OK this is hokey but it goes with the general feel of this whole
    // application.
    CString csBigAssStringFromFile = CString( pBufferInSpew );

    
    itoa( m_nTotPat, pszTempString, 10);
    csTotPat = CString ( pszTempString );

    itoa( m_nTot1ON, pszTempString, 10);
    csTot1ON = CString ( pszTempString );

    itoa( m_nTotDA, pszTempString, 10);
    csTotDA = CString ( pszTempString );

    itoa( m_nTot3ON, pszTempString, 10);
    csTot3ON = CString ( pszTempString );

    itoa( m_nTot4ON, pszTempString, 10);
    csTot4ON = CString ( pszTempString );

    itoa( m_nTot6ON, pszTempString, 10);
    csTot6ON = CString ( pszTempString );

    itoa( m_nTot9ON, pszTempString, 10);
    csTot9ON = CString ( pszTempString );
    
    csBigAssStringFromFile.Replace( "[m_1OnPatternTotal]",   csTot1ON );
    csBigAssStringFromFile.Replace( "[m_DAPatternTotal]" ,   csTotDA );
    csBigAssStringFromFile.Replace( "[m_3OnPatternTotal]",   csTot3ON );
    csBigAssStringFromFile.Replace( "[m_4OnPatternTotal]",   csTot4ON );
    csBigAssStringFromFile.Replace( "[m_6OnPatternTotal]",   csTot6ON );
    csBigAssStringFromFile.Replace( "[m_9OnPatternTotal]",   csTot9ON );
    csBigAssStringFromFile.Replace( "[m_GrandPatternTotal]", csTotPat );
    
    iNewLength = csBigAssStringFromFile.GetLength();

    pOutSpewMatrix->Write ( csBigAssStringFromFile.GetBuffer( iNewLength ), 
                            iNewLength );

    CountPatTypes( pPatTable );
    ComputeTotals();

    if ( pBufferInSpew )
        delete pBufferInSpew;


    pOutSpewMatrix->Close();
    pInSpewTemplate->Close();


    return 0;        
}



