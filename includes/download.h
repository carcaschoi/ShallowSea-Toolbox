#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define SIGS_URL    "http://patches.totaljustice.xyz"
#define APP_URL     "https://github.com/ITotalJustice/sigpatches-updater/releases/latest/download/sigpatches-updater.nro"

int downloadFile(const char *url, const char *output);

#endif