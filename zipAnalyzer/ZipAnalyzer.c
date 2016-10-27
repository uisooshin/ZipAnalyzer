//
//  ZipAnalyzer.c
//  zipAnalyzer
//
//  Created by 신의수 on 2016. 10. 27..
//
//

#include <stdio.h>
#include <stdlib.h>
#include "ZipAnalyzer.h"

void printHexString(char *s, int len) {
    
    int arrSize = 0;
    int tmp_hexString = 0;
    int i = 0;
    
//    printf("printHexString !\n");
//    printf("len = %d\n", len);
    
    arrSize = len / 4;
    if((len % 4) != 0 ) arrSize++;
    
//    printf("arrSize = %d\n", arrSize);
    
    for(i = 0; i < arrSize; i++){
        
        memcpy(&tmp_hexString, s, sizeof(int));
        
        if(i == 0) printf("%#.4x", tmp_hexString);
        else printf(" %4x", tmp_hexString);
        
        tmp_hexString = 0;
    }
    
    printf("\n");
}

//find Start point of End of Central Directory
int find_eoCdir_sig(FILE *fp){
    
    int read_sig  = 0;
    size_t read_size = 0;
    int total_read_size = 0;
    
    
    //because size of "end of central directory"(except comment) is 22
    fseek(fp, 0 - EO_CDIR_SIZE, SEEK_END);
    
    read_size = fread(&read_sig, sizeof(read_sig), sizeof(char), fp);
    
    
    //    for debug
    //    printf("read_sig = %#.4x\n", read_sig);
    //    read_sig == EO_CDIR_SIG_BIG ? printf("Same\n") : printf("Different\n");
    
    if( (read_sig != EO_CDIR_SIG_BIG) && (read_sig != EO_CDIR_SIG_LITTLE)){
        
        while(fp == SEEK_SET){
            fseek(fp, -1, SEEK_CUR);
            read_size = fread(&read_sig, sizeof(read_sig), sizeof(char), fp);
            total_read_size += read_size;
            
            if( (read_sig == EO_CDIR_SIG_BIG) || (read_sig == EO_CDIR_SIG_LITTLE)) break;
        }
        
        if( (read_sig != EO_CDIR_SIG_BIG) || (read_sig != EO_CDIR_SIG_LITTLE)){
            printf("Could not find End of Central Directory Signature\n");
            return -3;
        }
    }
    
    fseek(fp, -4, SEEK_CUR);
    
    return 0;
}

//End of Central Directory initialization function
int eoCdir_init(FILE *fp, EO_CENTRAL_DIRECTORY *_eoCdir){
    
    int result = 0;
    //    int read_sig  = 0;
    
    
    EO_CENTRAL_DIRECTORY *eoCdir = _eoCdir;
    
    
    if(fp == NULL) return -2;
    
    result = find_eoCdir_sig(fp);
    
    if(result != 0) return result;
    
    //    check signature comment
    //    fread(&read_sig, sizeof(read_sig), sizeof(char), fp);
    //    printf("read_sig in eoCdir_init = %#.4x\n", read_sig);
    //    fseek(fp, -4, SEEK_CUR);
    
    //End of Central Directory initialization
    
    eoCdir->cDirStartOffset  = 0;
    
    eoCdir->sig              = 0;
    eoCdir->diskNum          = 0;
    eoCdir->cdStartDiskNum   = 0;
    eoCdir->totalCdirNumDisk = 0;
    eoCdir->totalCdirNum     = 0;
    eoCdir->sizeOfCdir       = 0;
    eoCdir->cDirStartOffset  = 0;
    eoCdir->zipFileCmtLen    = 0;
    eoCdir->zipFileCmt       = NULL;
    
    //variable setting
    fread(&eoCdir->sig,              sizeof(eoCdir->sig),              sizeof(char), fp);
    fread(&eoCdir->diskNum,          sizeof(eoCdir->diskNum),          sizeof(char), fp);
    fread(&eoCdir->cdStartDiskNum,   sizeof(eoCdir->cdStartDiskNum),   sizeof(char), fp);
    fread(&eoCdir->totalCdirNumDisk, sizeof(eoCdir->totalCdirNumDisk), sizeof(char), fp);
    fread(&eoCdir->totalCdirNum,     sizeof(eoCdir->totalCdirNum),     sizeof(char), fp);
    fread(&eoCdir->sizeOfCdir,       sizeof(eoCdir->sizeOfCdir),       sizeof(char), fp);
    fread(&eoCdir->cDirStartOffset,  sizeof(eoCdir->cDirStartOffset),  sizeof(char), fp);
    fread(&eoCdir->zipFileCmtLen,    sizeof(eoCdir->zipFileCmtLen),    sizeof(char), fp);
    
    if(eoCdir->zipFileCmtLen != 0){
        printf("*zipFileComment is exist*\n");
        printf("*zipFileComment size is %d*\n\n", eoCdir->zipFileCmtLen);
        
        eoCdir->zipFileCmt = (char *)malloc(eoCdir->zipFileCmtLen + 1);
        memset(eoCdir->zipFileCmt, '\0', (eoCdir->zipFileCmtLen) + 1);
        fread(&eoCdir->zipFileCmt, eoCdir->zipFileCmtLen, sizeof(char), fp);
    }
    
    
    printf("-----------------End Of Centaral Directory is setting...-----------------\n\n");
    printf("end of Central Directory signature = %#.8x\n", eoCdir->sig);
    
    printf("number of this disk = %d\n", eoCdir->diskNum);
    printf("number of the disk withe the start of the Central Directory = %d\n", eoCdir->cdStartDiskNum);
    printf("total number of entries in the Centreal Directory on this disk = %d\n", eoCdir->totalCdirNumDisk);
    printf("total number of entries in the Centreal Directory = %d\n", eoCdir->totalCdirNum);
    printf("size of the central directory = %d\n", eoCdir->sizeOfCdir);
    printf("offset of start of Central Directory with respect to the starting disk number = %#.4x\n", eoCdir->cDirStartOffset);
    printf(".zipfile comment length = %d\n", eoCdir->zipFileCmtLen);
    
    //for debug
    /*
     printf("\n\n\n\nnumber of this disk = %#.4x\n", eoCdir->diskNum);
     printf("number of the disk withe the start of the Central Directory = %#.4x\n", eoCdir->cdStartDiskNum);
     printf("total number of entries in the Centreal Directory on this disk = %#.4x\n", eoCdir->totalCdirNumDisk);
     printf("total number of entries in the Centreal Directory = %#.4x\n", eoCdir->totalCdirNum);
     printf("size of the central directory = %#.4x\n", eoCdir->sizeOfCdir);
     printf("offset of start of Central Directory with respect to the starting disk number = %#.4x\n", eoCdir->cDirStartOffset);
     printf(".zipfile comment length = %#.4x\n", eoCdir->zipFileCmtLen);
     */
    
    if(eoCdir->zipFileCmt != NULL) printf(".zipfile comment = %s\n", eoCdir->zipFileCmt);
    
    printf("\n-----------------End Of Central Directory setting is complete.-----------------\n\n");
    
    return 0;
}


//Central Directory initialization function
int cDir_init(FILE *fp, CENTRAL_DIRECTORY * _cDir, int offset){
    
    
    CENTRAL_DIRECTORY * cDir = _cDir;
    
    if(fp == NULL) return -2;
    
    fseek(fp, offset, SEEK_SET);
    
    /*
     fread(&test_sig, sizeof(test_sig), sizeof(char), fp);
     if(test_sig == CDIR_SIG_BIG || test_sig == CDIR_SIG_LITTLE) printf("Central Directory is found : %#.4x\n", test_sig);
     else printf("Central Directory couldn't found : %#.4x\n", test_sig);
     */
    cDir->sig                = 0;
    cDir->madeVersion        = 0;
    cDir->needVersion        = 0;
    cDir->bitFlag            = 0;
    cDir->compMethod         = 0;
    cDir->lastTime           = 0;
    cDir->lastDate           = 0;
    cDir->crc32              = 0;
    cDir->compSize           = 0;
    cDir->uncompSize         = 0;
    cDir->fileNameLen        = 0;
    cDir->extFieldLen        = 0;
    cDir->fileCmtLen         = 0;
    cDir->diskNumStart       = 0;
    cDir->internalFileAttr   = 0;
    cDir->externalFileAttr   = 0;
    cDir->localHeaderOffset  = 0;
    cDir->nextCDOffset       = 0;
    cDir->fileName           = NULL;
    cDir->extraField         = NULL;
    cDir->fileComment        = NULL;
    
    //variable setting
    fread(&cDir->sig, sizeof(cDir->sig), sizeof(char), fp);
    fread(&cDir->madeVersion, sizeof(cDir->madeVersion), sizeof(char), fp);
    fread(&cDir->needVersion, sizeof(cDir->needVersion), sizeof(char), fp);
    fread(&cDir->bitFlag, sizeof(cDir->bitFlag), sizeof(char), fp);
    fread(&cDir->compMethod, sizeof(cDir->compMethod), sizeof(char), fp);
    fread(&cDir->lastTime, sizeof(cDir->lastTime), sizeof(char), fp);
    fread(&cDir->lastDate, sizeof(cDir->lastDate), sizeof(char), fp);
//    printf("crc-32 offset = %.8x\n", ftell(fp));
    fread(&cDir->crc32, sizeof(cDir->crc32), sizeof(char), fp);
    fread(&cDir->compSize, sizeof(cDir->compSize), sizeof(char), fp);
    fread(&cDir->uncompSize, sizeof(cDir->uncompSize), sizeof(char), fp);
    fread(&cDir->fileNameLen, sizeof(cDir->fileNameLen), sizeof(char), fp);
    fread(&cDir->extFieldLen, sizeof(cDir->extFieldLen), sizeof(char), fp);
    fread(&cDir->fileCmtLen, sizeof(cDir->fileCmtLen), sizeof(char), fp);
    fread(&cDir->diskNumStart, sizeof(cDir->diskNumStart), sizeof(char), fp);
    fread(&cDir->internalFileAttr, sizeof(cDir->internalFileAttr), sizeof(char), fp);
    fread(&cDir->externalFileAttr, sizeof(cDir->externalFileAttr), sizeof(char), fp);
    fread(&cDir->localHeaderOffset, sizeof(cDir->localHeaderOffset), sizeof(char), fp);
    
//    printf("current filepointer`s position = %#.4lx -- 1\n", ftell(fp));
    
    if(cDir->fileNameLen != 0){
        cDir->fileName = (char *)malloc( (cDir->fileNameLen) + 1);
        memset(cDir->fileName, '\0', (cDir->fileNameLen) + 1);
        fread(cDir->fileName, cDir->fileNameLen, sizeof(char), fp);
    }
    
//    printf("current filepointer`s position = %#.4lx -- 2\n\n", ftell(fp));
    
    if(cDir->extFieldLen != 0){
        cDir->extraField = (char *)malloc( (cDir->extFieldLen) + 1);
        memset(cDir->extraField, '\0', (cDir->extFieldLen) + 1);
        fread(cDir->extraField, cDir->extFieldLen, sizeof(char), fp);
    }
    if(cDir->fileCmtLen != 0){
        cDir->fileComment = (char *)malloc( (cDir->fileCmtLen) + 1);
        memset(cDir->fileComment, '\0', (cDir->fileCmtLen) + 1);
        fread(cDir->fileComment, cDir->fileCmtLen, sizeof(char), fp);
    }
    
    
    cDir->nextCDOffset = (int)ftell(fp);
    
    
    printf("-----------------Centaral Directory is setting...-----------------\n\n");
    /*
     printf("central file header signature = %#.4x\n", cDir->sig);
     printf("version made by = %#.4x\n", cDir->madeVersion);
     printf("version needed to extract = %#.4x\n", cDir->needVersion);
     printf("general purpose bit flag = %#.4x\n", cDir->bitFlag);
     printf("compression method = %#.4x\n", cDir->compMethod);
     printf("last mod file time = %#.4x\n", cDir->lastTime);
     printf("last mod file date = %#.4x\n", cDir->lastDate);
     printf("crc-32 = %d\n", cDir->crc32);
     printf("compressed size = %d\n", cDir->compSize);
     printf("uncompressed size = %d\n", cDir->uncompSize);
     printf("file name length  = %d\n", cDir->fileNameLen);
     printf("extra field length = %d\n", cDir->extFieldLen);
     printf("file comment length = %d\n", cDir->fileCmtLen);
     printf("disk number start = %d\n", cDir->diskNumStart);
     printf("internal file attributes = %#.4x\n", cDir->internalFileAttr);
     printf("external file attributes = %#.4x\n", cDir->externalFileAttr);
     printf("localHeaderOffset = %#.4x\n", cDir->localHeaderOffset);
     printf("file Name = %s\n", cDir->fileName);
     
     if(cDir->extraField != NULL) printf("extraField = %s\n", cDir->extraField);
     if(cDir->fileComment != NULL) printf("fileComment = %s\n", cDir->fileComment);
     */
    
    //for debug
    printf("central file header signature = %#.8x\n", cDir->sig);
    printf("version made by = %#.4x\n", cDir->madeVersion);
    printf("version needed to extract = %#.4x\n", cDir->needVersion);
    printf("general purpose bit flag = %#.4x\n", cDir->bitFlag);
    printf("compression method = %#.4x\n", cDir->compMethod);
    printf("last mod file time = %#.4x\n", cDir->lastTime);
    printf("last mod file date = %#.4x\n", cDir->lastDate);
    printf("crc-32 = %#.4x\n", cDir->crc32);
    printf("compressed size = %#.4x\n", cDir->compSize);
    printf("uncompressed size = %#.4x\n", cDir->uncompSize);
    printf("file name length  = %#.4x\n", cDir->fileNameLen);
    printf("extra field length = %#.4x\n", cDir->extFieldLen);
    printf("file comment length = %#.4x\n", cDir->fileCmtLen);
    printf("disk number start = %#.4x\n", cDir->diskNumStart);
    printf("internal file attributes = %#.4x\n", cDir->internalFileAttr);
    printf("external file attributes = %#.4x\n", cDir->externalFileAttr);
    printf("localHeaderOffset = %#.4x\n", cDir->localHeaderOffset);
    printf("file Name = %s\n", cDir->fileName);
    
    if(cDir->extraField != NULL){
        
        printf("extraField = ");
        printHexString(cDir->extraField, cDir->extFieldLen);
        
        
    }
    
    if(cDir->fileComment != NULL) printf("fileComment = %s\n", cDir->fileComment);
    
    printf("\n-----------------Central Directory setting is complete.-----------------\n\n");
    
    return 0;
}


//Local Header initialization function
int localHeader_init(FILE *fp, LOCAL_HEADER *lHeader, int offset){
    
    fseek(fp, offset, SEEK_SET);
    
    //local header struct initailization
    lHeader->sig         = 0;
    lHeader->needVersion = 0;
    lHeader->bitFlag     = 0;
    lHeader->compMethod  = 0;
    lHeader->lastTime    = 0;
    lHeader->lastDate    = 0;
    lHeader->crc32       = 0;
    lHeader->compSize    = 0;
    lHeader->uncompSize  = 0;
    lHeader->fileNameLen = 0;
    lHeader->extFieldLen = 0;
    lHeader->fileName    = NULL;
    lHeader->extraField  = NULL;
    
    printf("-----------------Local file header is setting...-----------------\n\n");
    
    //variable setting..
    fread(&lHeader->sig, sizeof(lHeader->sig), sizeof(char), fp);
    fread(&lHeader->needVersion, sizeof(lHeader->needVersion), sizeof(char), fp);
    fread(&lHeader->bitFlag, sizeof(lHeader->bitFlag), sizeof(char), fp);
    fread(&lHeader->compMethod, sizeof(lHeader->compMethod), sizeof(char), fp);
    fread(&lHeader->lastTime, sizeof(lHeader->lastTime), sizeof(char), fp);
    fread(&lHeader->lastDate, sizeof(lHeader->lastDate), sizeof(char), fp);
    fread(&lHeader->crc32, sizeof(lHeader->crc32), sizeof(char), fp);
    fread(&lHeader->compSize, sizeof(lHeader->compSize), sizeof(char), fp);
    fread(&lHeader->uncompSize, sizeof(lHeader->uncompSize), sizeof(char), fp);
    fread(&lHeader->fileNameLen, sizeof(lHeader->fileNameLen), sizeof(char), fp);
    fread(&lHeader->extFieldLen, sizeof(lHeader->extFieldLen), sizeof(char), fp);
    
    if(lHeader->fileNameLen != 0){
        lHeader->fileName = (char *)malloc((lHeader->fileNameLen) + 1);
        memset(lHeader->fileName, '\0', lHeader->fileNameLen);
        fread(lHeader->fileName, lHeader->fileNameLen, sizeof(char), fp);
    }
    
    if(lHeader->extFieldLen != 0){
        lHeader->extraField = (char *)malloc((lHeader->extFieldLen) + 1);
        memset(lHeader->extraField, '\0', lHeader->extFieldLen);
        fread(lHeader->extraField, lHeader->extFieldLen, sizeof(char), fp);
    }
    
    printf("local file header signature = %#.8x\n", lHeader->sig);
    printf("version needed to extract = %#.4x\n", lHeader->needVersion);
    printf("general purpose bit flag = %#.4x\n", lHeader->bitFlag);
    printf("compression method = %#.4x\n", lHeader->compMethod);
    printf("last mod file time = %#.4x\n", lHeader->lastTime);
    printf("last mod file date = %#.4x\n", lHeader->lastDate);
    printf("crc-32 = %#.4x\n", lHeader->crc32);
    printf("compressed size = %#.4x\n", lHeader->compSize);
    printf("uncompressed size = %#.4x\n", lHeader->uncompSize);
    printf("file name length = %#.4x\n", lHeader->fileNameLen);
    printf("extra field length = %#.4x\n", lHeader->extFieldLen);
    
    if(lHeader->fileName != NULL) printf("file name = %s\n", lHeader->fileName);
    if(lHeader->extraField != NULL){
        printf("extra field = ");
        printHexString(lHeader->extraField, lHeader->extFieldLen);
    }
    
    printf("\n-----------------Local file header setting is complete.-----------------\n\n");
    
    return 0;
}

