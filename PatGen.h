
//
#ifndef TRUE
#define TRUE  1
#define FALSE 0

#define BOOL int
#define WORD short
#define DWORD unsigned long
#define BYTE unsigned char
#endif
/////

// Database stuff that looks important.
#ifdef _DB_STUFF
#undef _DB_STUFF
Code4 cb;
Data4 data;
Tag4 tag;
Tag4 tagStyle;
#endif

//#pragma pack(1)

//typedef const char * LPCTSTR;

extern "C" {
int FindBestMask(DWORD hitmask, DWORD* masklist, DWORD nummasks, DWORD* bestindex);
int CountHits(DWORD hitmask);
int CheckMask(DWORD hitmask, DWORD targetmask);
int SetIgnoreMask(DWORD mask);
};


#define PAT_NAME_LEN 32

#define PDEA_NONE		 0x0000
#define PDEA_NOOVERLAP	 0x0001
#define PDEA_USEFREE	 0x0001
#define PDEA_ROTATING	 0x0002

#define PDEA_UNIQUEMASKS 0x0004
#define PDEA_MASKLIST    0x0008
#define PDEA_EACHLIST    0x0010
#define PDEA_COUNT       0x0020
#define PDEA_ANDEACHLIST 0x0040
#define PDEA_PATLIST 	 0x0080
#define PDEA_PLUSCOUNT   0x0100
#define PDEA_OREACHLIST  0x0200     // For 1-ON games, add PDEA_USEFREE to
                                    // allow free space on postage stamp.
#define PDEA_REQEACHLIST 0x0400
#define PDEA_BIGLIST     0x0800
#define PDEA_SPECIALCASE 0x8000

#define PDEA_ORLIST		 0x4000
#define PDEA_ANDLIST	 0x2000
#define PDEA_PUREANDLIST 0x1000
#define PDEA_COUNTORLIST 0x10000
#define PDEA_AWAYOFF     0x20000

#define NOOV				 1

#define MASKGROUP_BIT    0x80000000

#define COMPATIBLE              0x00000000
#define ONE_AWAY_COUNT_NOT_RED  0x00000001

///////////////////////////////////////////////////////////////////////////////
// CCompressedPattern
///////////////////////////////////////////////////////////////////////////////

class CCompressedPattern
{
public:
	CCompressedPattern();
	~CCompressedPattern();

	void SetKey(WORD key);
	WORD GetKey();

	void SetName(char* name);
	char* GetName();

	void SetMaxMasks(WORD max);
	WORD GetNumMasks();
	WORD GetNumHits();
	void SetNumHits(WORD hits);

	void SetAttributes(DWORD butes);
	BOOL GetNoOverlap();
	void SetNoOverlap(BOOL flag);
	BOOL GetRotating();
	void SetRotating(BOOL flag);

	void SetHandHeld()
	{
		m_nHandHeldVersion = TRUE;
	}

	void SetSingleFrame(DWORD mask);

	void SetMultiBingos(WORD num);

	DWORD GetMask(WORD index);
	void SetMasks(DWORD* mlist);
	BOOL Add(DWORD mask);

	DWORD GetBinaryLength();
	BOOL WriteToFile(FILE* fp);

	DWORD* GetFromBuffer(BYTE* b, DWORD len, WORD& nummasks, 
			WORD& numhits, DWORD& butes, WORD& multibingos, char* name, DWORD& singlemask);

	WORD Expand(DWORD* mlist, WORD nummasks, WORD num_hits, DWORD butes, DWORD* outlist);
	DWORD CalcFilter(DWORD* mlist, WORD nummasks);
	BOOL GroupCheck(DWORD m1, DWORD m2);

	void Print();
	void PrintMask(DWORD mask);

public:
	WORD		m_nKey;
	DWORD*	    m_pMasks;
	char		m_pName[PAT_NAME_LEN];
	WORD		m_nMaxMasks;
	WORD		m_nNumMasks;
	WORD		m_nNumHits;
	DWORD		m_nAttributes;
	DWORD		m_nRepresentativeSingleFrame;
	WORD		m_nMultiBingos;
	BOOL		m_nHandHeldVersion;
};

// raw input pattern
typedef struct {
	short key;
	char name[PAT_NAME_LEN];
	WORD hits;
	DWORD attributes;
	BYTE *masks;
	WORD multibingos;
	BYTE *singleframe;
} PATDEF;

typedef struct {
	BYTE* mlist;
	int   hits;
} MULTILIST;

typedef struct {
	int flags;
	BYTE* list0;
	BYTE* list1;
	BYTE* list2;
	BYTE* list3;
} ORLIST;

typedef struct {
	int flags;
	BYTE* list0;
	BYTE* list1;
	BYTE* list2;
} ANDLIST;

#define MLIST_DELIMITER 0xFFFFFFFF

typedef struct {
	short  key;
	WORD  length;
	DWORD offset;
} PATLIB_DIR;


struct structCompatFlags
{
    CString csTravFlags;
    CString csTEDFlags;
    CString csTED2CFlags;
    CString csHHFlags;
    CString csFBFlags;

};

// Pattern report generator
BOOL CreateReport( char   *pszFilename, 
                   PATDEF *pPatTable );

// Create Compat Files
BOOL CreateCompatFiles( PATDEF *pPatTable );

// Create Compat Matrix
BOOL CreateCompatMatrix( PATDEF *pPatTable );

// Get style of pattern.
int GetPatternType( int iAttributes, char* pszTempString, char* pszOverlap);

// Get type of pattern.
int GetPatternStyle( int dwPatNum, char* pszTempString );

// Stuff all the special guys into an Uber list.
int PopulateSpecialCasesList( CMapStringToString* pmSpecialCases );

// Uber hack
void SwitchAroo ( char* pszString, char cFindChar, char cReplaceChar );


//__( EXTERNAL GLOBAL VARIABLES )______________________________________________
extern int                 g_numpatterns;
extern CCompressedPattern* g_patterns;

