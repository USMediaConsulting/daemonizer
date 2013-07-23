#ifndef __USMC_TOOLS_DAEMONIZER_H__
#define __USMC_TOOLS_DAEMONIZER_H__
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

#include <string>

namespace usmc {

namespace tools {

/**
* This template receives a functor that nows the code
* to run as a daemon. All you need to do is to provide 
* the functor at construction time and the call daemonize
*/
template<typename T>
struct daemonizer{

    daemonizer( const T& runner, 
                const std::string& user,
                const std::string& change_dir);

    pid_t daemonize();

private :

    T runner;
    std::string user;
    std::string change_dir;
};


template<typename T>
inline daemonizer<T>::daemonizer(
                    const T& runner, 
                    const std::string& user,
                    const std::string& change_dir){

    this->runner = runner;
    this->user = user;
    this->change_dir = change_dir;
}

template<typename T>
inline pid_t daemonizer<T>::daemonize(){
    
    pid_t process_id = 0;
    pid_t sid = 0;
    // Drop user if there is one, and we were run as root 
    if (getuid() == 0 || geteuid() == 0) {
        struct passwd *pw = getpwnam(user.c_str());
        if (pw) {
            setuid( pw->pw_uid );
        }
    }
    // Create child process
    process_id = fork();
    if (process_id < 0){
        return process_id;
    }
    // PARENT PROCESS. Need to kill it.
    if (process_id > 0){
        // return success in exit status
        return process_id;
    }
    //unmask the file mode
    umask(0);
    //set new session
    sid = setsid();
    if(sid < 0){
        // Return failure
        exit(1);
    }
    // Change the current working directory to the given dir.
    chdir(change_dir.c_str());
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    //run the functor
    runner();

    return 0;
}

} // usmc

} // tools

#endif
