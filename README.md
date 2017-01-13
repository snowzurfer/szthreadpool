# szthreadpool - thread pool in C using pthreads
Threads pool in C implemented using pthreads. 
Please visit http://www.albertotaiuti.com/memalloc.php for a more detailed report.

## Installation
* If on windows
  1. Download the latest build of
[pthread-win32](https://www.sourceware.org/pthreads-win32)
  2. Extract the library where preferred
  3. Place the desired .dll under `C:\Windows`. More info on whych .dll to use
  can be found at pthreads-win32's FAQs
  4. Add the environment variable PTHREADS-WIN32 pointing to the root folder of
  the pthread-win32 library, the one which contains 3 folders
1. Clone the repo and use CMake to create a project build for your target

## Usage
See the threadpool header for the signatures and main.cpp for an example usage.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## History
* January 2015: started development.
* February 2015: changed language from C++ to C.

## Credits
Developer: Alberto Taiuti  
Mentor and supervisor: Steven Tovey

## License
TODO: Write license

