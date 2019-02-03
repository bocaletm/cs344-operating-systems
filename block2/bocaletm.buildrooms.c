/**************************
 * Mario Bocaletti
 * buldrooms
 * This program generates text files
 * representing rooms
 * ************************/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOBBY "lobby"
#define CAFE "cafe"
#define BANK "bank"
#define RESTROOM "restroom"
#define OFFICE "office"
#define COFFEE "coffee"
#define LOUNGE "lounge"
#define POOL "pool"
#define ELEVATOR "elevator"
#define HELIPAD "helipad"

/**************************
 * createRoomFiles()
 * This function generates text files
 * representing rooms
 * ************************/

int createRoomFiles(){
    return 0;
}

/**************************
 * addConnections()
 * This function prints connections 
 * to other rooms in the files
 * ************************/

int addConnections() {
    return 0;
}

/**************************
 * addConnections()
 * This function prints the room types 
 * to the file rooms 
 * ************************/

int addTypes() {
    return 0;
}
int createDirectory(){
    /*set the dir name*/
    char* dirName = 0;
    const int dirNameLength = 25;
    dirName = malloc(dirNameLength * sizeof(char));
    if (dirName == 0) {
        printf("Error. Could not create directory name.\n");
        return 1;
    }
    memset(dirName,'\0',dirNameLength);

    /*add the process ID to the name */
    sprintf(dirName,"bocaletm.rooms.%d",getpid());
    int result = mkdir(dirName,0755);
    free(dirName);
    if (result != 0){
        printf("Error creating directory...\n");
        return 1;
    }
    return 0;
}

int main()
{
    int directory = createDirectory();
    return directory;
}
