//
//  main.c
//  zipAnalyzer
//
//  Created by 신의수 on 2016. 10. 27..
//
//

#include <stdio.h>
#include <stdlib.h>
#include "ZipAnalyzer.h"

int main(int argc, const char * argv[]) {
    
    const char           * fileFullPath  = argv[1];
    FILE                 * fp            = fopen(fileFullPath, "r");
    LOCAL_HEADER         * l_header      = NULL;
    CENTRAL_DIRECTORY    * cdir_header   = NULL;
    EO_CENTRAL_DIRECTORY * eoCdir_header = NULL;
    int                    result        = 0;
    int                    i             = 0;
    int                    oldCDOffset   = 0;
    
    eoCdir_header = (EO_CENTRAL_DIRECTORY *)calloc(1, sizeof(EO_CENTRAL_DIRECTORY));
    cdir_header   = (CENTRAL_DIRECTORY *)calloc(1, sizeof(CENTRAL_DIRECTORY));
    l_header      = (LOCAL_HEADER *)calloc(1, sizeof(LOCAL_HEADER));
    
    
    printf("\n\n");
    
    if(fp == NULL)
    {
        printf("File Pointer is NULL, please Check your filePath\n");
        printf("Error Code : 2\n\n");
        return -2;
    }
    
    else
    {
        printf("File open Success\n");
        printf("read file path = %s\n\n\n", fileFullPath);
    }
    
    printf("********************Analysis Apk(zip) File Start********************\n");
    printf("1) End of Central Directory initailization\n\n");
    
    result = eoCdir_init(fp, eoCdir_header);
    if(result != 0){
        printf("eoCdir_init Error\n");
        printf("Error Code : %d\n", result);
        goto err;
    }
    
    
    for(i = 0; i < eoCdir_header->totalCdirNum; i++){
        printf("%d) file`s Central Directory information\n", i + 1);
        if(i == 0) result = cDir_init(fp, cdir_header, eoCdir_header->cDirStartOffset);
        
        else {
//            printf("cdir_header->nextCDOffset = %#.4x\n", oldCDOffset);
            result = cDir_init(fp, cdir_header, oldCDOffset);
        }
        if(result != 0){
            printf("cdir_init Error\n");
            printf("Error Code : %d\n", result);
            goto err;
        }
        
        oldCDOffset = cdir_header->nextCDOffset;
        
        
        printf("\n\n%d) file`s Local file header information\n", i + 1);
        result = localHeader_init(fp, l_header, cdir_header->localHeaderOffset);
        
        if(result != 0){
            printf("localHeader_init Error\n");
            printf("Error Code : %d\n", result);
            goto err;
        }
        
        
        free(cdir_header);
        cdir_header   = (CENTRAL_DIRECTORY *)calloc(1, sizeof(CENTRAL_DIRECTORY));
        
        free(l_header);
        l_header   = (LOCAL_HEADER *)calloc(1, sizeof(LOCAL_HEADER));
    }
    
//    printf("2) Central Directory initailization\n\n");
//    result = cDir_init(fp, cdir_header, eoCdir_header->cDirStartOffset);
//    if(result != 0){
//        printf("cdir_init Error\n");
//        printf("Error Code : %d\n", result);
//        goto err;
//    }
//    
//    
//    printf("3) Local file header initailization\n\n");
//    result = localHeader_init(fp, l_header, cdir_header->localHeaderOffset);
//    if(result != 0){
//        printf("localHeader_init Error\n");
//        printf("Error Code : %d\n", result);
//        goto err;
//    }
    
err:
    fclose(fp);
    if(l_header      != NULL) free(l_header);
    if(cdir_header   != NULL) free(cdir_header);
    if(eoCdir_header != NULL) free(eoCdir_header);
    
    return 0;
}
