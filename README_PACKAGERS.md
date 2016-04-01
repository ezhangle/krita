= Notes for Packagers =

== Package Contents ==

We recommend that all of Krita packaged in one package: there is no need to split Krita up.

Krita does not install header files, so there is no need for a corresponding -dev(el) package.

== Third Party Libraries ==

The top-level 3rd-party directory is not relevant for packaging: it only contains CMake projects for all of Krita's dependencies which are used for building Krita on Windows and OSX. It is not called from the top-level CMakeLists.txt project.

There are four forks of 3rd party libraries that are relevant and cannot be replaced by system libraries:

* plugins/impex/raw/3rdparty contains a fork of kdcraw. Upstread removed most functionality from this library and is in general unable to provide a stable API. The library has been renamed to avoid conflicts with upstream kdcraw.

* plugins/impex/xcf/3rdparty contains the xcftools code. This has never been released as a library

* plugins/extensions/gmic/3rdparty contains G'Mic. This has never been released as a shared library.

* libs/image/3rdparty contains einspline. This code is directly linke d into the kritaimage library and has never been released as a separate library.

== Build flags ==

Packagers must build krita with the PACKAGERS_BUILD flag enabled. This makes sure that object files for all SIMD vectorization extensions krita supports are built. The right one is then selected run-time. 

Krita no longer supports a build without OpenGL.

== Dependencies ==

Krita depends on:

  * boost and the boost-system library        
  * eigen3       
  * exiv2   
  * fftw3   
  * gsl       
  * ilmbase   
  * jpeg: Note that libjpeg-turbo is recommended.       
  * lcms2     
  * libcurl   
  * libraw    
  * opencolorio
  * openexr   
  * openjpeg   
  * png        
  * poppler-qt5
  * pthreads   
  * qt-5: Note that Qt 5.6 is _strongly_ recommended. Qt 5.5 has bugs that interfere with proper handling of tablet events
  * tiff  
  * vc
  * zlib

And the following KDE Frameworks:

 * Archive 
 * Completion
 * Config 
 * CoreAddons
 * GuiAddons 
 * I18n 
 * ItemModels 
 * ItemViews
 * KCrash
 * WidgetsAddons 
 * WindowSystem