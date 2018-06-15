#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#define DATA "Half a league, half a league . . ."


#define NAME "/tmp/opp-sock"


class Connectorator {
  public:

    int sock;

    Connectorator() {
        struct sockaddr_un server;
        char buf[1024];


        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("opening stream socket");
            exit(1);
        }
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, NAME);


        if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
            close(sock);
            perror("connecting stream socket");
            exit(1);
        }

        /*
        char s[400];
        s[1] = 0;
        s[0] = ' ';
        while (1) {
               scanf("%s", s);
               if (s[0] == 'q')
            break;
                if (write(sock, DATA, sizeof(DATA)) < 0)
                   perror("writing on stream socket");
        }
        close(sock);
        */
    }

    ~Connectorator() {
        close(sock);
    }
};

static Connectorator loel;




/*

#include <stdlib.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
#undef gettimeofday
#undef clock_gettime
#undef time
#undef ftime

#define DEFAULT_FAKE_TIME (1234567890)

static long get_time() {
    return DEFAULT_FAKE_TIME;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
   if(tv == NULL)
        return -1;
   tv->tv_sec = get_time();
   return 0;
}

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    if(tp == NULL)
        return -1;
    tp->tv_sec = get_time();
    return 0;
}

time_t time(time_t *t) {
    int t_ = get_time();
    if(t)
        *t = t_;
    return t_;
}

int ftime(struct timeb *tp) {
    tp->time = get_time();
    return 0;
}

*/

#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

static int (*orig_clock_gettime)(clockid_t, struct timespec*) = NULL;
static int (*orig_nanosleep)(struct timespec*, struct timespec*) = NULL;
static int (*orig_gettimeofday)(struct timeval*, struct timezone*) = NULL;
static int (*orig_select)(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout) = NULL;
static int (*orig_pselect) (int nfds, fd_set *readfds, fd_set *writefds,
                   fd_set *exceptfds, const struct timespec *timeout,
                   const sigset_t *sigmask) = NULL;


static int (*orig_poll)(struct pollfd *fds, nfds_t nfds, int timeout) = NULL;
static int (*orig_ppoll)(struct pollfd *fds, nfds_t nfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask) = NULL;


static struct timespec timebase_monotonic;
static struct timespec timebase_realtime;
static struct timeval timebase_gettimeofday;

struct tiacc {
    long long int lastsysval;
    long long int lastourval;
};

static struct tiacc accumulators[3] = {{0,0}, {0,0}, {0,0}};

static int num = 1;
static int denom = 1;
static long long int shift = 0;


static long long int filter_time(long long int nanos, struct tiacc* acc) {
    long long int delta = nanos - acc->lastsysval;
    acc->lastsysval = nanos;

    delta = delta * num / denom;
    acc->lastourval+=delta;
    return acc->lastourval;
}

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    if(!orig_clock_gettime) {
        orig_clock_gettime = (int (*)(clockid_t, timespec*))dlsym(RTLD_NEXT, "clock_gettime");
        (*orig_clock_gettime)(CLOCK_MONOTONIC, &timebase_monotonic);
        (*orig_clock_gettime)(CLOCK_REALTIME , &timebase_realtime);
    }
    int ret = orig_clock_gettime(clk_id, tp);


    if (clk_id == CLOCK_MONOTONIC) {
        long long q = 1000000000LL*(tp->tv_sec - timebase_monotonic.tv_sec)
            + (tp->tv_nsec - timebase_monotonic.tv_nsec);

        q = filter_time(q, accumulators+0);

        tp->tv_sec = (q/1000000000)+timebase_monotonic.tv_sec + shift;
        tp->tv_nsec = q%1000000000+timebase_monotonic.tv_nsec;
        if (tp->tv_nsec >= 1000000000) {
            tp->tv_nsec-=1000000000;
            tp->tv_sec+=1;
        }
    }else
    { // if (clk_id == CLOCK_REALTIME) {
        long long q = 1000000000LL*(tp->tv_sec - timebase_realtime.tv_sec)
            + (tp->tv_nsec - timebase_realtime.tv_nsec);

        q = filter_time(q, accumulators+2);

        tp->tv_sec = (q/1000000000)+timebase_realtime.tv_sec + shift;
        tp->tv_nsec = q%1000000000+timebase_realtime.tv_nsec;
        if (tp->tv_nsec >= 1000000000) {
            tp->tv_nsec-=1000000000;
            tp->tv_sec+=1;
        }
    }

    return ret;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    printf("poop\n");

    int opcode = 1;
    send(loel.sock, &opcode, 4, MSG_EOR);

    int64_t secs;
    int64_t usecs;

    read(loel.sock, &secs, 8);
    read(loel.sock, &usecs, 8);

    if (tz) {
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }

    tv->tv_sec = secs;
    tv->tv_usec = usecs;

/*
    if(!orig_gettimeofday) {
        orig_gettimeofday = (int (*)(timeval*, struct timezone*))dlsym(RTLD_NEXT, "gettimeofday");
        (*orig_gettimeofday)(&timebase_gettimeofday, NULL);
    }
    int ret = orig_gettimeofday(tv, tz);

    long long q = 1000000LL * (tv->tv_sec - timebase_gettimeofday.tv_sec)
        + (tv->tv_usec - timebase_gettimeofday.tv_usec);

    q = filter_time(q*1000LL, accumulators+1)/1000;

    tv->tv_sec = (q/1000000)+timebase_gettimeofday.tv_sec + shift;
    tv->tv_usec = q%1000000+timebase_gettimeofday.tv_usec;
    if (tv->tv_usec >= 1000000) {
        tv->tv_usec-=1000000;
        tv->tv_sec+=1;
    }
*/
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if(!orig_nanosleep) {
        orig_nanosleep = (int (*)(timespec*, timespec*))dlsym(RTLD_NEXT, "nanosleep");
    }
    long long q = 1000000000LL*(req->tv_sec) + req->tv_nsec;

    q = q * denom / num;

    struct timespec ts;

    ts.tv_sec = (q/1000000000);
    ts.tv_nsec = q%1000000000;

    int ret = orig_nanosleep(&ts, rem);

    if (rem) {
        q = 1000000000LL*(rem->tv_sec) + rem->tv_nsec;

        q = q * num / denom;

        rem->tv_sec = (q/1000000000);
        rem->tv_nsec = q%1000000000;

    }

    return ret;
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout) {

    // break timeout into small chunks and poll for simtime?
    // send over fd-s so the rtscheduler can poll for us on them?

    printf("plaaap\n");
    if(!orig_select) {
        orig_select = (int (*)(int, fd_set*, fd_set*, fd_set*, timeval*))dlsym(RTLD_NEXT, "select");
    }

    struct timeval ts;
    struct timeval *tsptr = NULL;

    if (timeout) {
        long long q = 1000000LL*(timeout->tv_sec) + timeout->tv_usec;

        q = q * denom / num;

        ts.tv_sec = (q/1000000);
        ts.tv_usec = q%1000000;
        tsptr = &ts;
    }

    int ret = orig_select(nfds, readfds, writefds, exceptfds, tsptr);


    if (timeout) {
        long long q = 1000000LL*(tsptr->tv_sec) + tsptr->tv_usec;

        q = q * num / denom;

        timeout->tv_sec = (q/1000000);
        timeout->tv_usec = q%1000000;
    }

    return ret;
}
int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    // break timeout into small chunks and poll for simtime?
    // send over fd-s so the rtscheduler can poll for us on them?

    printf("pooooll\n");
    if(!orig_poll) {
        orig_poll = (int (*)(struct pollfd *fds, nfds_t nfds, int timeout))dlsym(RTLD_NEXT, "poll");
    }


    return orig_poll(fds, nfds, timeout);


    /*

    struct timeval ts;
    struct timeval *tsptr = NULL;

    if (timeout) {
        long long q = 1000000LL*(timeout->tv_sec) + timeout->tv_usec;

        q = q * denom / num;

        ts.tv_sec = (q/1000000);
        ts.tv_usec = q%1000000;
        tsptr = &ts;
    }

    int ret = orig_select(nfds, readfds, writefds, exceptfds, tsptr);


    if (timeout) {
        long long q = 1000000LL*(tsptr->tv_sec) + tsptr->tv_usec;

        q = q * num / denom;

        timeout->tv_sec = (q/1000000);
        timeout->tv_usec = q%1000000;
    }

    return ret;*/
}

int ppoll(struct pollfd *fds, nfds_t nfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask) {
    printf("ppppooooll\n");

}


int pselect(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, const struct timespec *timeout,
        const sigset_t *sigmask) {

    printf("plommp\n");

    if(!orig_pselect) {
        orig_pselect = (int (*)(int, fd_set*, fd_set*, fd_set*, const timespec*, const sigset_t*))dlsym(RTLD_NEXT, "pselect");
    }

    struct timespec ts;
    struct timespec *tsptr = NULL;

    if (timeout) {
        long long q = 1000000000LL*(timeout->tv_sec) + timeout->tv_nsec;

        q = q * denom / num;

        ts.tv_sec = (q/1000000000);
        ts.tv_nsec = q%1000000000;
        tsptr = &ts;
    }

    int ret = orig_pselect(nfds, readfds, writefds, exceptfds, tsptr, sigmask);
    return ret;
}


