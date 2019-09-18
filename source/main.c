#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "download.h"
#include "unzip.h"

//#define DEBUG               // enable for nxlink debug

#define ROOT                "/"
#define APP_PATH            "/switch/sigpatch-updater/"
#define SIGS_OUTPUT         "/switch/sigpatch-updater/sigpatches.zip"
#define APP_OUTPUT          "/switch/sigpatch-updater/sigpatch-updater.nro"
#define OLD_APP_PATH        "/switch/sigpatch-updater.nro"

#define APP_VERSION         "0.1.0"
#define CURSOR_LIST_MAX     1

void refreshScreen(int cursor)
{
    consoleClear();

    char *option_list[] = { "= Update Sigpatches", "= Update this app" };

    printf("\x1B[36mSigpatch-Updater: v%s.\x1B[37m\n\n\n", APP_VERSION);
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < (CURSOR_LIST_MAX+1); i++)
    {
        if (cursor != i) printf("[ ] %s\n\n", option_list[i]);
        else printf("[X] %s\n\n", option_list[i]);
    }
    consoleUpdate(NULL);
}

int appInit()
{
    consoleInit(NULL);
    socketInitializeDefault();
    #ifdef DEBUG
    nxlinkStdio();
    #endif
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

    // change directory to root (defaults to /switch/)
    chdir(ROOT);

    // set the cursor position to 0
    short cursor = 0;

    // main menu
    refreshScreen(cursor);

    // muh loooooop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
            refreshScreen(cursor);
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
            else cursor--;
            refreshScreen(cursor);
        }

        if (kDown & KEY_A)
        {
            switch (cursor)
            {
            case UP_SIGS:
                if (!downloadFile(SIGS_URL, SIGS_OUTPUT))
                    unzip(SIGS_OUTPUT);
                break;

            case UP_APP:
                if (!downloadFile(APP_URL, APP_OUTPUT))
                {
                    FILE *f = fopen(OLD_APP_PATH, "r");
                    if (f) 
                    {
                        fclose(f);
                        remove(OLD_APP_PATH);
                    }
                }
                break;
            }
        }
        
        // exit...
        if (kDown & KEY_PLUS) break;
    }

    // cleanup then exit
    appExit();

    return 0;
}