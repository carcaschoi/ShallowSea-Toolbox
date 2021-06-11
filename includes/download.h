#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

//define ShallowSea-ams URL

#define AMS_URL     "https://github.com/carcaschoi/ShallowSea/releases/latest/download/ShalowSea-ams.zip"
#define APP_URL         "https://github.com/carcaschoi/ShallowSea_Updater/releases/latest/download/ShallowSea-updater.nro"
#define TEMP_FILE       "/switch/ShallowSea-updater/temp"

#define ON              1
#define OFF             0


#include <stdbool.h>

//
bool downloadFile(const char *url, const char *output, int api);

#endif
