#include <stdio.h>
#include <stdarg.h>
#include <unistd.h> // chdir
#include <dirent.h> // mkdir
#include <switch.h>
#include <string.h> // copyfile
#include <stdlib.h> //libnx ask me to add it

#include "download.h"
#include "unzip.h"
#include "main.h"
//#include "reboot_payload.h"


#define ROOT                    "/"
#define APP_PATH                "/switch/ShallowSea-updater/"
#define APP_OUTPUT              "/switch/ShallowSea-updater/ShallowSea-updater.nro"
#define OLD_APP_PATH            "/switch/ShallowSea-updater.nro"
#define AMS                     "/updating/"

#define APP_VERSION             "0.0.5"
#define CURSOR_LIST_MAX         1

const char *OPTION_LIST[] =
{
    "= Update ShallowSea-ams",
    "= Update this app"
};

void refreshScreen(int cursor)
{
    consoleClear();

    printf("\x1B[36mShallowSea-updater by carcaschoi: v%s\x1B[37m\n\n\n", APP_VERSION); // \n //
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < CURSOR_LIST_MAX + 1; i++)
        printf("[%c] %s\n\n", cursor == i ? 'X' : ' ', OPTION_LIST[i]);

    consoleUpdate(NULL);
}

void printDisplay(const char *text, ...)
{
    va_list v;
    va_start(v, text);
    vfprintf(stdout, text, v);
    va_end(v);
    consoleUpdate(NULL);
}

int appInit()
{
    consoleInit(NULL);
    socketInitializeDefault();
    return 0;
}

void appExit()
{
    socketExit();
    consoleExit(NULL);
	appletSetAutoSleepDisabled(false);
}

// tell the app what copyFile does
void copyFile(char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        char buffer[10000]; // 10kb per write, which is fast
        size_t bytes;       // size of the file to write (10kb or filesize max)

        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
        {
            fwrite(buffer, 1, bytes, newfile);
        }
    }
    fclose(srcfile);
    fclose(newfile);
}

// Check if it's dir on 'path'
int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

// Remove all the shit. Please note that there should be no '/' char in the end. Otherwise update this snippet.
int remove_entry(const char *dir_name)
{
    if (!is_dir(dir_name))
        return unlink(dir_name);

    DIR *dir = opendir(dir_name);

    if (dir == NULL)
        return 1;

    size_t dSz = strlen(dir_name);
    struct dirent *s_dirent;
    char *full_name;

    while ((s_dirent = readdir(dir)) != NULL)
    {
        if ((strcmp(s_dirent->d_name, ".") == 0) || (strcmp(s_dirent->d_name, "..") == 0))
            continue;
        full_name = malloc(dSz + strlen(s_dirent->d_name) + 2); // '/'+'\0'

        strcpy(full_name, dir_name);
        strcat(full_name, "/");
        strcat(full_name, s_dirent->d_name);

        if (s_dirent->d_type == DT_DIR)
            remove_entry(full_name); // NOTE: Handle returning value
        else
            unlink(full_name); // NOTE: Add validation

        free(full_name);
    }

    closedir(dir);

    return rmdir(dir_name); // NOTE: Add validation
}

int main(int argc, char **argv)
{
    // init stuff
    appInit();
    mkdir(APP_PATH, 0777);
    
    // change directory to root (defaults to /switch/)
    chdir(ROOT);

    // set the cursor position to 0
    short cursor = 0;

    // main menu
    refreshScreen(cursor);
	
	padConfigureInput(8, HidNpadStyleSet_NpadStandard);
	PadState pad;
    padInitializeAny(&pad);

    // muh loooooop
    while(appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        // move cursor down...
        if (kDown & HidNpadButton_Down)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
            refreshScreen(cursor);
        }

        // move cursor up...
        if (kDown & HidNpadButton_Up)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
            else cursor--;
            refreshScreen(cursor);
        }

        if (kDown & HidNpadButton_A)
        {   
	        remove(TEMP_FILE);
            switch (cursor)
            {
            case UP_AMS:
                if (downloadFile(AMS_URL, TEMP_FILE, OFF))
		        {
					appletSetAutoSleepDisabled(true);
					remove_entry(AMS);
					mkdir(AMS, 0777);
				    chdir(AMS);
                    unzip(TEMP_FILE);
					copyFile("updating/config/ShallowSea-updater/startup.te", "/");
					//reboot_payload("/updating/bootloader/payloads/tegraexplorer.bin");
			    }
                else
                {
                    printDisplay("Failed to download ShallowSea-ams\n");
                }
                break;

            case UP_APP:
                if (downloadFile(APP_URL, TEMP_FILE, OFF))
                {
                    remove(APP_OUTPUT);
                    rename(TEMP_FILE, APP_OUTPUT);
                    remove(OLD_APP_PATH);
					printf("Please reopen the app");
                }
                else
                {
                    printDisplay("Failed to download app update\n");
                }
                break;
            }
        }
        
        // exit...
        if (kDown & HidNpadButton_Plus) break;
    }

    // cleanup then exit
    appExit();
    return 0;
}
