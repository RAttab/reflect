/* signals.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>

#define BOOST_TEST_IGNORE_SIGCHLD

namespace test {

/******************************************************************************/
/* CHECK_SIGNAL                                                               */
/******************************************************************************/

int signalNet()
{
    signal(SIGCHLD, SIG_DFL);

    pid_t pid = fork();
    if (pid == -1) reflectError("unable to create signal net");
    if (pid == 0){
        signal(SIGABRT, SIG_DFL);
        return -1;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
        reflectError("unable to wait on signal net: %d", pid);

    return WIFSIGNALED(status) ? 1 : 0;
}

#define check_signal(p)                         \
    do {                                        \
        int ret = test::signalNet();            \
        if (ret == -1) { (p); exit(0); }        \
        BOOST_CHECK_EQUAL(ret, 1);              \
    } while (false)


} // namespace test
