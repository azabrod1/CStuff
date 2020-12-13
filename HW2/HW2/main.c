/*
    File: main.c
 
    Program: myar
 
    Description: emulates ar UNIX archiving utility with limited set of functionality
 
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ar.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>


#define END_FILE 1
#define HEADER_NUM_CHARS 60 /* (16 + 12 + 6 + 6 + 8 + 10 + 2)*/
#define NEW_FILE_TIME 7200 /* 2 hours */


/* this flag enables the logic that adds newline if filesize is odd */
#define ADDNEWLINE

typedef struct ar_hdr Header;
typedef struct stat   Stat;


int extractFile(char* aname, char* fname);
int contents(char* const aname, short verbose);
mode_t convertMode(char* smode);
off_t skipToNextHeader(int archive, int charsToSkip);
int archiveFile(char* afile, char* fname);
int archiveNew(char* aname);
int deleteFile(char* const aname, char* const fname);

/***************************
 
 Function: main()
 
***************************/


int main(int argc,  char * argv[]) {

    /* TESTING
    archiveFile("archive", "a.txt");
    archiveFile("archive", "b.txt");
    archiveFile("archive", "e.txt");
    archiveFile("archive", "test4.txt");
    
    unarchive("archive", "e.txt");
    
    contents("archive", 1);
    
    return 0;
    */
    
    if (argc == 1){
        printf("Usage:\n");
        printf("myar -{qtvxdA} archive [file]\n");
        return 0;
    }
    if (strcmp(argv[1],"q")==0 || strcmp(argv[1],"-q")==0){
        if (argc < 4){
            printf("Insufficient arguments for option q\n");
            return 1;
        }
        
        /* if you want to support multiple files added at once
        int file = 3;
        int toReturn = 0;
        while(file < argc)
            toReturn |= archiveFile(argv[2], argv[file++]);
        
        return toReturn;
        
         */
        
        
        
        return archiveFile(argv[2], argv[3]);
    }
    if (strcmp(argv[1],"t")==0 || strcmp(argv[1],"-t")==0){
        if (argc < 3){
            printf("Insufficient arguments for option t\n");
            return 1;
        }
        return contents(argv[2], 0);
    }
    if (strcmp(argv[1],"v")==0 || strcmp(argv[1],"-v")==0){
        if (argc < 3){
            printf("Insufficient arguments for option v\n");
            return 1;
        }
        return contents(argv[2], 1);
    }
    if (strcmp(argv[1],"x")==0 || strcmp(argv[1],"-x")==0){
        if (argc < 4){
            printf("Insufficient arguments for option x\n");
            return 1;
        }
        return extractFile(argv[2], argv[3]);
    }
    if (strcmp(argv[1],"A")==0 || strcmp(argv[1],"-A")==0){
        if (argc < 3){
            printf("Insufficient arguments for option A\n");
            return 1;
        }
        return archiveNew(argv[2]);
    }
    if (strcmp(argv[1],"d")==0 || strcmp(argv[1],"-d")==0){
        if (argc < 4){
            printf("Insufficient arguments for option d\n");
            return 1;
        }
        return deleteFile(argv[2], argv[3]);
    }
    printf("Invalid option %s\n", argv[1]);
    printf("Usage:\n");
    printf("myar -{qtvxdA} archive [file]\n");
    return 1;

}


/***************************
 
 Function: extractHeader()
 
 reads the file header in the archive. The archive pointer points to the start of the header
 
 ***************************/

int extractHeader(int archive, Header* header){
    
    char buffer[HEADER_NUM_CHARS];
   
    //read the header, return END_FILE if there is nothing more to read
    if(read(archive, buffer, sizeof(buffer)) < sizeof(buffer))
        return END_FILE;
    
    //Construct the header
    sscanf(buffer, "%s %s %s %s %s %s %s", header->ar_name, header->ar_date, header->ar_uid, header->ar_gid, header->ar_mode, header->ar_size, header->ar_fmag);
    
    
    return 0;
}

/***************************
 
 Function: skipToNextHeader()
 
 finds the next header
 
 returns offset number, negative if not successful
 
 ***************************/
off_t skipToNextHeader(int archive, int charsToSkip){
    
#ifdef ADDNEWLINE
    
    if (charsToSkip % 2 == 1) charsToSkip++;
    
#endif
    
    off_t t = lseek(archive, charsToSkip, SEEK_CUR);

    return t;
    
}

/***************************
 
 Function: deleteError()
 
Handles delete errors
 ***************************/
int deleteError(int archive, int ffile, char* file_buffer, char* fname){
    printf("Error deleting file %s from archive\n", fname); close(archive); close(ffile);
    free(file_buffer); return -1;
}


/***************************
 
 Function: deleteFile()
 
 deletes a file from archive
 
 ***************************/

int deleteFile(char* const aname, char* const fname){
    
    Header fileHeader;
    
    int archive = open(aname, O_RDWR);
    
    if(archive < 0){
        printf("Error opening %s\n", aname);
        return 1;
    }
    
    lseek(archive, SARMAG, SEEK_SET); /*Skip initial !<arch>\n  string*/

    if(extractHeader(archive, &fileHeader) == END_FILE){
        printf("\nFile %s not found in the archive %s.\n", fname, aname); return 1;
    }
    
    while(strcmp(fileHeader.ar_name, fname)){
        skipToNextHeader(archive, atoi(fileHeader.ar_size));
        if(extractHeader(archive, &fileHeader) == END_FILE){
            printf("\nFile %s not found in the archive %s.\n", fname, aname); return 1;
        }
    }
    
    off_t start_target = lseek(archive, 0, SEEK_CUR) - HEADER_NUM_CHARS;
    
    
    /*We have found the file to delete.*/
    
    skipToNextHeader(archive, atoi(fileHeader.ar_size)); /*Skip the rest of the file to delete*/
    
    struct stat st;
    stat(aname, &st);
    off_t archiveSize = st.st_size;
    
    if(lseek(archive, 0, SEEK_CUR) >= archiveSize - 1){
        /*delete everything following startTarget*/
        ftruncate(archive, start_target);
    }
    else{
        
        off_t after_target = lseek(archive, 0, SEEK_CUR);
        

        
        long bytesAfter = sizeof(char) * (archiveSize - after_target);
        char buffer [bytesAfter];
        
        read(archive, buffer, bytesAfter); /*read  in rest of archive following deleted  file*/
        
        ftruncate(archive, start_target);                               /*get rid of rest of file*/
        lseek(archive, start_target, SEEK_SET);
        write(archive, buffer, bytesAfter);
        
    }
    
    close(archive);

    return 0;
}


/***************************
 
 Function: contents()
 
 displays the archive content
 
 ***************************/

int contents(char* const aname, short verbose){
    
    Header header;

    const int archive = open(aname, O_RDONLY);
    
    if(archive < 0){
        printf("Error opening %s\n", aname);
        return 1;
    }
    
    lseek(archive, SARMAG, SEEK_SET); /*Skip initial !<arch>\n  string*/

    while(extractHeader(archive, &header) != END_FILE){
        
        char buf [256];
        
        if (verbose){
            char timebuf [80];
            struct tm ts;
            time_t date = atoi(header.ar_date);
            mode_t mode = convertMode(header.ar_mode);
            
            ts = *localtime(&date);
            
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M %Y", &ts);
            
            
            sprintf(buf, "%c%c%c%c%c%c%c%c%c%8s/%-6s%9s %s %s\n",
                   (mode & S_IRUSR) == 0 ? '-' : 'r',
                   (mode & S_IWUSR) == 0 ? '-' : 'w',
                   (mode & S_IXUSR) == 0 ? '-' : 'x',
                   
                   (mode & S_IRGRP) == 0 ? '-' : 'r',
                   (mode & S_IWGRP) == 0 ? '-' : 'w',
                   (mode & S_IXGRP) == 0 ? '-' : 'x',
                   
                   (mode & S_IROTH) == 0 ? '-' : 'r',
                   (mode & S_IWOTH) == 0 ? '-' : 'w',
                   (mode & S_IXOTH) == 0 ? '-' : 'x',
                   
                   header.ar_uid,
                   header.ar_gid,
                   
                   header.ar_size,
                   timebuf,
                   
                   header.ar_name
                   
                   );
            
                printf("%s", buf);
            
        }
        else
            printf("%s\n",  header.ar_name);
        
        skipToNextHeader(archive, atoi(header.ar_size));
    }
    
    return 0;
    
}

/***************************
 
 Function: convertMode()
 
 converts string representation of file mode to mode_t
 
 ***************************/

mode_t convertMode(char* smode){
    
    int imode = atoi(smode);
    
    mode_t mode = imode % 10;
    imode  /= 10;
    
    mode += (imode % 10)* 8;
    imode /= 10;
    
    mode += (imode % 10) * 64;
    return mode;
}

/***************************
 
 Function: extractError()
 
 
 ***************************/
int extractError(int archive, int ffile, char* file_buffer, char* fname){
    printf("Error copying file %s from archive\n", fname); close(archive); close(ffile);
    free(file_buffer); return -1;
}

/***************************
 
 Function: extractFile()
 
 extracts file from  the archive
 
 ***************************/

int extractFile(char* aname, char* fname){
    
    Header fileHeader;

    const int archive = open(aname, O_RDONLY);
    
    if(archive < 0){
        printf("Error opening %s\n", aname);
        return 1;
    }
    
    
    lseek(archive, SARMAG, SEEK_SET); /*Skip initial !<arch>\n  string*/
    
    if(extractHeader(archive, &fileHeader) == END_FILE){
        printf("\nFile %s not found in the archive %s.\n", fname, aname); return 1;
    }
    
    while(strcmp(fileHeader.ar_name, fname)){
        skipToNextHeader(archive, atoi(fileHeader.ar_size));
        if(extractHeader(archive, &fileHeader) == END_FILE){
            printf("\nFile %s not found in the archive %s.\n", fname, aname); return 1;
        }
    }
    
    /* Create file, set proper mode */
    const int ffile = open(fname, O_WRONLY|O_CREAT, convertMode(fileHeader.ar_mode));
    
    int flen = atoi(fileHeader.ar_size);
    
    if(flen){ /*copy the file's contents if the file is not empty*/
        char* file_buffer = malloc(sizeof(char) * flen);
        
        /*Read into buffer*/
        if(read(archive, file_buffer, sizeof(char) * flen) < flen || write(ffile, file_buffer, sizeof(char) * flen) < flen)
            return extractError(archive, ffile, file_buffer, fname);
        
#ifdef ADDNEWLINE
      /* nothing */

#endif
        free(file_buffer);
        
    }
    
    close(archive); close(ffile);
    
    /* attempt to set file attribites (do not verify - best effort) */
    
    chmod(fname, convertMode(fileHeader.ar_mode));
    
    chown(fname, atoi(fileHeader.ar_uid), atoi(fileHeader.ar_gid));
    
    struct utimbuf tt;
    
    tt.actime = tt.modtime = atoi(fileHeader.ar_date);
    
    utime(fname, &tt);
   
    return 0;
}



/***************************
 
 Function: archiveFile()
 
 archives the file into the archive, creates the archive if not exists
 
 ***************************/

int archiveFile(char* aname, char* fname){
    
    int status;
    int afile;
    int ffile = open(fname, O_RDONLY);
    int newFile = 0;
    size_t bytes;
    

    /* archive does do exist */
    if( access(aname, F_OK ) == -1 ){
        printf("Creating archive %s\n", aname);
    
        newFile = 1;
    }
    
    /* file does not exist */
    if (ffile < 0){
        printf("Error opening %s\n", fname);
        return 1;
    }
    
    /* open or create archive */
    afile = open(aname, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR + S_IWUSR + S_IRGRP + S_IWGRP + S_IROTH + S_IWOTH);
    
    if (afile < 0){
        printf("Error accessing %s: %d \n", aname, afile);
        close(ffile);
        return 1;
    }
    
    /* for a new file, write the header */
    if (newFile)
        write(afile, ARMAG, SARMAG);
    
    /* write file header */
    
    Stat* fileStats = malloc(sizeof(Stat));
    status = stat(fname, fileStats);
    
    if(status){
        printf("Error openning file %s\n", fname);
        free(fileStats);
        close(ffile);
        close(afile);
        return -1;
    }
    /* check that file is a regular file */
    if(!S_ISREG(fileStats->st_mode)){
        printf("%s is not a regular file!\n", fname);
        free(fileStats);
        close(ffile);
        close(afile);
        return -2;
    }
    /* write the header */
    char buf[HEADER_NUM_CHARS + 1];
    
    sprintf(buf, "%-16s%-12ld%-6ld%-6ld%-8o%-10lld%s",
            fname,                                  /* file name : 16 char */
            (long) fileStats->st_mtimespec.tv_sec,  /* timestamp : 12 char */
            (long) fileStats->st_uid,               /* userID    :  6 char */
            (long) fileStats->st_gid,               /* groupID   :  6 char */
            fileStats->st_mode,                     /* mode(oct) :  8 char */
            (long long) fileStats->st_size,         /* size (ll) : 10 char */
            ARFMAG);                                /* eos       :  2 char */
    
    write(afile, buf, HEADER_NUM_CHARS);
    
    /* copy file */
    
    long bufsize = fileStats->st_size; 
    char buffer[bufsize + 1];

    

    
    bytes = read(ffile, buffer, bufsize);
    write(afile, buffer, bytes);
    
    
#ifdef ADDNEWLINE

    if (bufsize % 2 == 1){
        write(afile, "\n", 1);
    }
#endif
    
    free(fileStats);
    close(ffile);
    close(afile);
    
    /* for the new archive, set the mode */
    if (newFile)
        chmod(aname, S_IRUSR + S_IWUSR + S_IRGRP + S_IWGRP + S_IROTH + S_IWOTH);
    
    return 0;
    
}


/***************************
 
 Function: archiveNew()
 
 archive all new files (under 2 hours) except for the archive itself, non-regular, and hidden files
 
 ***************************/

int archiveNew(char* aname){
    
    DIR *directory = opendir(".");
    struct dirent *dir;
    Stat* fileStats = malloc(sizeof(Stat));
    time_t tm = time(NULL);
    
    if (directory)
    {
        while ((dir = readdir(directory)) != NULL)
        {
            /* only look for regular files, not the archive, not hidden */
            if ((dir->d_type == DT_REG) && (strcmp(dir->d_name, aname) != 0) && (dir->d_name[0] != '.')){
                stat(dir->d_name, fileStats);
                
                if (tm - fileStats->st_mtimespec.tv_sec < NEW_FILE_TIME){
                    
                    /*printf("%s: %ld \n", dir->d_name, tm - fileStats->st_mtimespec.tv_sec); */

                    archiveFile(aname, dir->d_name);
                    
                    
                }
                    
                    
            }
            
        }
        
        closedir(directory);
    }
    
    
    
    return 0;
    
}


