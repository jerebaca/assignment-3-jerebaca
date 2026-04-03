#include "systemcalls.h"
#include "sys/wait.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    bool retval = true;
    if ( 0U != system(cmd))
    {
        retval = false;
    }

    return retval;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

/*
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    int status;
    int pid;

    pid = fork();

    /* Two processes are now running here */

    if ( pid == -1 ) /* check if fork was unsuccessful */
    {
        return -1;
    }
    else if (pid == 0)
    {
        /* pid of '0' indicates it is the child */
        /* so, go ahead and use execv to pass the command to a system shell*/
        execv(command[0],&command[1]);
        /* if successful, we won't ever come here */
        exit (-1);
    }
    else
    {
        /* still in the parent thread, wait for the child */
        /* fork returns the pid of the child to the parent*/
        if ( -1 == wait(&status) ) return -1; /* something went wrong */
        else if (WIFEXITED(status)) return WEXITSTATUS(status); /* child exited normally, forward the return*/
        else return -1; /* child did not exit normally */
    }


    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    int status;
    int pid;
    int fd = open(outputfile);
    if (fd < 0) { exit(-1); }


    pid = fork();

    /* Two processes are now running here */

    if ( pid == -1 ) /* check if fork was unsuccessful */
    {
        return -1;
    }
    else if (pid == 0)
    {
        /* redirect stdout to file */
        if (dup2(fd, 1) < 0) { exit(-1); }
        close(fd); /* close the fd as it's not needed anymore */

        /* pid of '0' indicates it is the child */
        /* so, go ahead and use execv to pass the command to a system shell*/
        execv(command[0],&command[1]);
        /* if successful, we won't ever come here */
        exit (-1);
    }
    else
    {
        close(fd);
        /* still in the parent thread, wait for the child */
        /* fork returns the pid of the child to the parent*/
        if ( -1 == wait(&status) ) return -1; /* something went wrong */
        else if (WIFEXITED(status)) return WEXITSTATUS(status); /* child exited normally, forward the return*/
        else return -1; /* child did not exit normally */
    }

    va_end(args);

    return true;
}
