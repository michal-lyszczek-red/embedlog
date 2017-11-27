/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

#include <embedlog.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define WORKDIR "/tmp/embedlog-example"

int main(void)
{
    el_init();
    el_option(EL_OPT_TS, EL_OPT_TS_LONG);
    el_option(EL_OPT_TS_TM, EL_OPT_TS_TM_REALTIME);
    el_option(EL_OPT_FINFO, 1);

    if (mkdir(WORKDIR, 0755) != 0 && errno != EEXIST)
    {
        fprintf(stderr, "Couldn't create %s, error %s\n",
            WORKDIR, strerror(errno));

        goto error;
    }

    if (el_option(EL_OPT_FNAME, WORKDIR"/log") != 0)
    {
        /*
         * embedlog will try to open file now, this may fail for various  of
         * reasons, if opening fails, we cannot log to file -  all  el_print
         * to files will return error
         */

        fprintf(stderr, "Couldn't open file for logging %s\n",
            strerror(errno));

        goto error;
    }

    /*
     * instruct logger to print into both file and standard error
     */

    el_option(EL_OPT_OUT, EL_OPT_OUT_FILE | EL_OPT_OUT_STDERR);

    el_print(ELI, "This message will appear both in standard error");
    el_print(ELI, "and in file %s", WORKDIR"/log");
    el_print(ELI, "in this file there might be another logs like this "
        "from consecutive execution if this code, as embedlog "
        "does not truncate logs but append to file");

    /*
     * enable file rotation and set file  size  to  small  enough  value  to
     * present how rotation works, in real life, rotate size should be  much
     * higher to prevent unnecessary rotation
     */

    el_option(EL_OPT_FROTATE_NUMBER, 5);
    el_option(EL_OPT_FROTATE_SIZE, 512);
    el_print(ELI, "Now we enabled log rotation");
    el_print(ELI, "If log cannot fit in current file");
    el_print(ELI, "it will be stored in new file");
    el_print(ELI, "and if library creates EL_OPT_FROTATE_NUMBER files");
    el_print(ELI, "oldest file will be deleted and new file will be created");
    el_print(ELI, "run this program multiple times and see how it works");

    el_cleanup();
    return 0;

error:
    el_cleanup();
    return 1;
}
