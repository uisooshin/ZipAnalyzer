//
//  ZipAnalyzer.h
//  zipAnalyzer
//
//  Created by 신의수 on 2016. 10. 27..
//
//

#ifndef ZipAnalyzer_h
#define ZipAnalyzer_h

#define EO_CDIR_SIG_BIG		0x06054B50
#define EO_CDIR_SIG_LITTLE	0x504B0506
#define EO_CDIR_SIZE        22

#define CDIR_SIG_BIG		0x02014B50
#define CDIR_SIG_LITTLE		0x504B0102
#define CDIR_SIZE           52

#define LOCAL_HEADER_SIG_BIG	0x04034B50
#define LOCAL_HEADER_SIG_LITTLE 0x504B0304


#endif /* ZipAnalyzer_h */

typedef struct LocalFileHeader{
    int    sig;                 //local file header signature(4 bytes, 0x04034B50)
    short  needVersion;         //version needed to extract(2 bytes)
    short  bitFlag;             //general purpose bit flag(2 bytes)
    short  compMethod;          //compression method(2 bytes)
    short  lastTime;	    	//last mod file time(2 bytes)
    short  lastDate;	    	//last mod file date(2 bytes)
    int    crc32;		    	//crc-32(4 bytes)
    int    compSize;	    	//compressed size(4 bytes)
    int    uncompSize;	    	//uncompressed size(4 bytes)
    short  fileNameLen;	    	//file name length(2 bytes)
    short  extFieldLen;	    	//extra field length(2 bytes)
    
    char * fileName;	    	//variable size
    char * extraField;	    	//variable size
}LOCAL_HEADER;

typedef struct CentralDirectory{
    int    sig; 		    	//central file header signature(4 bytes, 0x02014B50)
    short  madeVersion;	    	//version made by(2 bytes)
    short  needVersion;	    	//version needed to extract(2 bytes)
    short  bitFlag;		    	//general purpose bit flag(2 bytes)
    short  compMethod;	    	//compression method(2 bytes)
    short  lastTime;	    	//last mod file time(2 bytes)
    short  lastDate;	    	//last mod file date(2 bytes)
    int    crc32;		    	//crc-32(4 bytes)
    int    compSize;	    	//compressed size(4 bytes)
    int    uncompSize;	    	//uncompressed size(4 bytes)
    short  fileNameLen;	    	//file name length(2 bytes)
    short  extFieldLen;	    	//extra field length(2 bytes)
    short  fileCmtLen;	    	//file comment length(2 bytes)
    short  diskNumStart;		//disk number start(2 bytes)
    short  internalFileAttr;	//internal file attributes(2 bytes)
    int    externalFileAttr;	//external file attributes(4 bytes)
    int    localHeaderOffset;	//relative offset of local header(4 bytes)
    
    int    nextCDOffset;        //next Central Directory offset(4bytes, made by usshin)
    
    char * fileName;	    	//variable size
    char * extraField;	    	//variable size
    char * fileComment;	    	//variable size
}CENTRAL_DIRECTORY;

typedef struct EoCentralDirectory{
    int    sig; 		     	 //end of central dir signature(4 bytes, 0x06054B50)
    short  diskNum;		    	 //number of this disk(2 bytes)
    short  cdStartDiskNum;		 //number of the disk with the start of the central dir(2 bytes)
    short  totalCdirNumDisk;     //total number of entries in the central directory on this disk(2 bytes)
    short  totalCdirNum;		 //total number of entries in the central directory(2 bytes)
    int    sizeOfCdir;	    	 //size of the central directory(4 bytes)
    int    cDirStartOffset;		 //offset of start of central directory with respect to the starting disk number(4 bytes)
    short  zipFileCmtLen;		 //.zip file comment length(2 bytes)
    
    char * zipFileCmt;	    	 //variable size
}EO_CENTRAL_DIRECTORY;


int find_eoCdir_sig(FILE *fp);
int eoCdir_init(FILE *fp, EO_CENTRAL_DIRECTORY *_eoCdir);
int cDir_init(FILE *fp, CENTRAL_DIRECTORY * _cDir, int offset);
int localHeader_init(FILE *fp, LOCAL_HEADER *lHeader, int offset);
void printHexString(char *s, int len);



