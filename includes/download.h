#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

//define ShallowSea-ams URL

#define AMS_URL     "https://github.com/carcaschoi/ShallowSea/releases/latest/download/ShallowSea-ams.zip"
#define ENG_URL     "https://github.com/carcaschoi/ShallowSea/releases/latest/download/switch_English_extra_package.zip"
#define APP_URL     "https://github.com/carcaschoi/ShallowSea-toolbox/releases/latest/download/ShallowSea-toolbox.nro"
#define TEMP_FILE   "/config/ShallowSea-updater/temp"

#define ON              1
#define OFF             0


#include <stdbool.h>

//
bool downloadFile(const char *url, const char *output, int api);

#endif
