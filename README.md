Libzeep webapp starter
======================

This is a project containing a very simple web application based on libzeep.
It is meant to get new projects up and running fast.

To build this you will need to have [libzeep](https://github.com/mhekkel/libzeep) installed as well
as [libmcfp](https://github.com/mhekkel/libmcfp.git). You will also need _cmake_ but since you have
libzeep that should not be a problem. [mrc](https://github.com/mhekkel/mrc.git) should also be 
installed if your platform supports it. And you need [yarn](https://yarnpkg.com/).

Building
--------

To build this web application, make sure you've installed all requirements mentioned above.
Then follow these steps:

```bash
git clone https://github.com/mhekkel/libzeep-webapp-starter.git
cd libzeep-webapp-starter
yarn
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Running
-------

The application can run as a daemon process, but for debugging it is easier to
run the program in the foreground so you can see what happens since output will appear
in the terminal instead into the log files.

Starting the program is as simple as this when you're in the main project directory:

```bash
build/libzeep-webapp-starter start --no-daemon
```

If you omit the `--no-daemon` flag, the program will fork a daemon that runs in the
background. To stop this daemon, you can type:

```bash
build/libzeep-webapp-start stop
```

Debugging
---------

The application was built with the Debug flag (see the `-DCMAKE_BUILD_TYPE=Debug` option above). That means that no resources are used, instead, the HTML templates and java scripts are read from disk so the _docroot_ must be available. Therefore you can run this application only from the project directory, otherwise you will get errors about not being able to load pages.

If you however build a _Release_ version of the program, and if mrc is available, then the
HTML templates and scripts are stored in the executable and you can place it wherever you like and run it from any directory.

Extending
---------

Of course, this is a very simple example showing how to create a web application and
use a form and process the output of that form. A lot more is possible, e.g. you can
add security and user authentication and use REST controllers to communicate with remote
scripts. See other web applications in the [PDB-REDO](https://github.com/PDB-REDO/) repositories.