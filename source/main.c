#include <stdio.h>
#include <stdarg.h>
#include <unistd.h> // chdir
#include <dirent.h> // mkdir
#include <switch.h>

#include "download.h"
#include "unzip.h"


#define ROOT                    "/"
#define APP_PATH                "/switch/ShallowSea-updater/"
#define APP_OUTPUT              "/switch/ShallowSea-updater/ShallowSea-updater.nro"
#define OLD_APP_PATH            "/switch/ShallowSea-updater.nro"

#define APP_VERSION             "1.0"
#define CURSOR_LIST_MAX         1

const char *OPTION_LIST[] =
{
    "= Update ShallowSea-ams",
    "= Update this app"
};

void refreshScreen(int cursor)
{
    consoleClear();

    printf("\x1B[36mShallowSea-updater: v%s.\x1B[37m\n\n\n", APP_VERSION); // \n //
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
}

int main(int argc, char **argv)
{
    // init stuff
    appInit();
    mkdir(APP_PATH, 0777);
    mkdir("updating", 0777);
    
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
            switch (cursor)
            {
            case UP_AMS:
                if (downloadFile(AMS_URL, TEMP_FILE, OFF))
		        {
					char *path = ("/updating/");
					chdir(path);
                    unzip(TEMP_FILE);
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
