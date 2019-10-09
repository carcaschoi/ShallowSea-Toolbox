#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <switch.h>

#include "download.h"
#include "unzip.h"

//#define DEBUG               // enable for nxlink debug

#define ROOT                    "/"
#define APP_PATH                "/switch/sigpatch-updater/"
#define SIGS_OUTPUT             "/switch/sigpatch-updater/sigpatches.zip"
#define JOON_SIGS_OUTPUT        "/switch/sigpatch-updater/kosmos.7z"
#define APP_OUTPUT              "/switch/sigpatch-updater/sigpatch-updater.nro"
#define OLD_APP_PATH            "/switch/sigpatch-updater.nro"

#define APP_VERSION             "0.1.2"
#define CURSOR_LIST_MAX         2

#define GBATEMP_FILTER_STRING   "attachments/"
#define GITHUB_FILTER_STRING    "browser_download_url\":\""


int parseSearch(char *parse_string, char *filter, char* new_string)
{
    FILE *fp = fopen(parse_string, "r");
    
    if (fp)
    {
        char c;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == *filter)
            {
                for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
                {
                    c = fgetc(fp);
                    if (i == len)
                    {
                        for (int j = 0; c != '\"'; j++)
                        {
                            new_string[j] = c;
                            new_string[j+1] = '\0';
                            c = fgetc(fp);
                        }
                        fclose(fp);
                        remove(parse_string);
                        return 0;
                    }
                }
            }
        }
    }

    printf("Failed to parse...\n");
    consoleUpdate(NULL);
    fclose(fp);
    return 1;
}

void refreshScreen(int cursor)
{
    consoleClear();

    char *option_list[] = { "= Update Sigpatches (For Atmosphere Users)", \
                            "= Update Sigpatches (For Hekate / Kosmos Users)", \
                            "= Update this app" };

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
                if (!downloadFile(SIGS_URL, TEMP_FILE, ON))
                {
                    // get the new attatchment file name, store it in file_name.
                    char file_name[128];
                    if (!parseSearch(TEMP_FILE, GBATEMP_FILTER_STRING, file_name))
                    {
                        // concatenate the gbatemp url and new attatchment name, store in new_url.
                        char new_url[256];
                        if (snprintf(new_url, sizeof(new_url), "%s%s", GBATEMP_URL, file_name))
                            // download from the concatenated url. Hacky, but it works(tm).
                            if (!downloadFile(new_url, SIGS_OUTPUT, OFF))
                                unzip(SIGS_OUTPUT);
                    }
                }
                break;

            case UP_JOONIE:
                if (!downloadFile(JOON_SIGS_URL, TEMP_FILE, ON))
                {
                    char new_url[128];
                    if (!parseSearch(TEMP_FILE, GITHUB_FILTER_STRING, new_url))
                        if (!downloadFile(new_url, SIGS_OUTPUT, OFF))
                            unzip(SIGS_OUTPUT);
                }
                break;

            case UP_APP:
                if (!downloadFile(APP_URL, TEMP_FILE, OFF))
                {
                    rename(TEMP_FILE, APP_OUTPUT);
                    remove(OLD_APP_PATH);
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