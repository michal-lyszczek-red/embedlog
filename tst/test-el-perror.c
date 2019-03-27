/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==========================================================================
          _               __            __         ____ _  __
         (_)____   _____ / /__  __ ____/ /___     / __/(_)/ /___   _____
        / // __ \ / ___// // / / // __  // _ \   / /_ / // // _ \ / ___/
       / // / / // /__ / // /_/ // /_/ //  __/  / __// // //  __/(__  )
      /_//_/ /_/ \___//_/ \__,_/ \__,_/ \___/  /_/  /_//_/ \___//____/

   ========================================================================== */


#include "el-private.h"

#include <errno.h>
#include <string.h>

#include "mtest.h"
#include "embedlog.h"


/* ==========================================================================
                                   _                __
                     ____   _____ (_)_   __ ____ _ / /_ ___
                    / __ \ / ___// /| | / // __ `// __// _ \
                   / /_/ // /   / / | |/ // /_/ // /_ /  __/
                  / .___//_/   /_/  |___/ \__,_/ \__/ \___/
                 /_/
                                   _         __     __
              _   __ ____ _ _____ (_)____ _ / /_   / /___   _____
             | | / // __ `// ___// // __ `// __ \ / // _ \ / ___/
             | |/ // /_/ // /   / // /_/ // /_/ // //  __/(__  )
             |___/ \__,_//_/   /_/ \__,_//_.___//_/ \___//____/

   ========================================================================== */


mt_defs_ext();
static char  logbuf[1024 * 1024];  /* output simulation */


/* ==========================================================================
                                   _                __
                     ____   _____ (_)_   __ ____ _ / /_ ___
                    / __ \ / ___// /| | / // __ `// __// _ \
                   / /_/ // /   / / | |/ // /_/ // /_ /  __/
                  / .___//_/   /_/  |___/ \__,_/ \__/ \___/
                 /_/
               ____                     __   _
              / __/__  __ ____   _____ / /_ (_)____   ____   _____
             / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
            / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
           /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/

   ========================================================================== */


/* ==========================================================================
    this is custom function that will be called instead of for example puts
    after el_print constructs message.  We capture that message  and  simply
    append to logbuf, so we can analyze it later if everything is in  order.
   ========================================================================== */


static int print_to_buffer
(
    const char  *s,
    void        *user
)
{
    strcat(user, s);
    return 0;
}


/* ==========================================================================
   ========================================================================== */


static void test_prepare(void)
{
    el_init();
    el_option(EL_CUSTOM_PUTS, print_to_buffer, logbuf);
    el_option(EL_PRINT_LEVEL, 0);
    el_option(EL_OUT, EL_OUT_CUSTOM);
    logbuf[0] = '\0';
}


/* ==========================================================================
   ========================================================================== */


static void test_cleanup(void)
{
    el_cleanup();
}


/* ==========================================================================
                           __               __
                          / /_ ___   _____ / /_ _____
                         / __// _ \ / ___// __// ___/
                        / /_ /  __/(__  )/ /_ (__  )
                        \__/ \___//____/ \__//____/

   ========================================================================== */


static void perror_no_message(void)
{
    errno = 1;
    el_perror(ELF, NULL);

    /*
     *  as  different  implementations  might  have  different  values   for
     *  strerror(errno),  we  check  only  first  part   of   message   that
     *  we are sure, will be printed.
     */

    mt_fok(strncmp(logbuf, "errno num: 1, strerror: ", 24));
}


/* ==========================================================================
   ========================================================================== */


static void perror_user_message(void)
{
    errno = 1;
    el_perror(ELF, "additional message");
    mt_fok(strncmp(logbuf, "additional message\n"
        "errno num: 1, strerror: ", 43));
}


/* ==========================================================================
   ========================================================================== */


static void perror_custom_options_user_message(void)
{
    struct el_options  opts;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    el_oinit(&opts);
    el_ooption(&opts, EL_CUSTOM_PUTS, print_to_buffer, logbuf);
    el_ooption(&opts, EL_PRINT_LEVEL, 0);
    el_ooption(&opts, EL_OUT, EL_OUT_CUSTOM);

    errno = 1;
    el_operror(ELF, &opts, "additional message");
    mt_fok(strncmp(logbuf, "additional message\n"
        "errno num: 1, strerror: ", 43));

}


/* ==========================================================================
             __               __
            / /_ ___   _____ / /_   ____ _ _____ ____   __  __ ____
           / __// _ \ / ___// __/  / __ `// ___// __ \ / / / // __ \
          / /_ /  __/(__  )/ /_   / /_/ // /   / /_/ // /_/ // /_/ /
          \__/ \___//____/ \__/   \__, //_/    \____/ \__,_// .___/
                                 /____/                    /_/
   ========================================================================== */


void el_perror_test_group(void)
{
    mt_prepare_test = &test_prepare;
    mt_cleanup_test = &test_cleanup;

    mt_run(perror_no_message);
    mt_run(perror_user_message);
    mt_run(perror_custom_options_user_message);
}
