%define DIRNAME tron
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
%define debug_package %{nil}
Summary: tron library
Name: %{SPECNAME}
Version: 20.4.18
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-tron
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: rpm-build
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: boost169-devel
BuildRequires: geos-devel >= 3.5.0
Requires: geos >= 3.5.0
# Actually, this should only be required by the devel package
Requires: geos-devel >= 3.5.0
Provides: %{LIBNAME}
Obsoletes: libsmartmet-tron < 17.1.4
Obsoletes: libsmartmet-tron-debuginfo < 17.1.4
#TestRequires: boost169-devel
#TestRequires: gcc-c++
#TestRequires: smartmet-library-gis-devel
#TestRequires: smartmet-library-regression

%description
FMI tron contouring library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}
%{_libdir}/lib%{LIBNAME}.a

%changelog
* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgrade to Boost 1.69

* Wed Dec  4 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.4-1.fmi
- Use -fno-omit-frame-pointer for a better profiling and debugging experience                                                                                              

* Fri Nov  1 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.1-1.fmi
- Silence compiler warnings by avoiding shadowing of local variables

* Thu Oct 31 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.31-1.fmi
- Omit unassignable holes instead of throwing to visualize bad geometries too

* Thu Sep 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.26-1.fmi
- Full rebuild

* Wed Sep 20 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.9.20-1.fmi
- Added InfMissing mode, fixed code to work with it
- Removed -Ofast for not being compatible with infinities

* Tue Sep 12 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.9.12-1.fmi
- Refactored API in preparation for tiled contouring

* Mon Aug 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.8.28-1.fmi
- Upgrade to boost 1.65

* Tue Feb  7 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.7-1.fmi
- Fixed another problem in isoline building

* Tue Jan 31 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.31-1.fmi
- Switched from google/dense_hash to std::unordered_set, the former does not scale to huge datasets

* Mon Jan 30 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.30-1.fmi
- Fixed isoline building to handle self-touches

* Thu Jan 19 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.19-2.fmi
- Build as long isolines as possible while keeping geometry valid

* Thu Jan 19 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.19d8-1.fmi
- Removed speed optimizations which do not seem to be robust

* Wed Jan 18 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.18-2.fmi
- Fixed contouring with loglinear interpolation

* Wed Jan 18 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.18-1.fmi
- Fixed handling of self-touching contour lines

* Wed Jan  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.4-1.fmi
- Switched to open source naming conventions

* Fri Nov 18 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.18-1.fmi
- Added safety checks

* Tue Nov  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.1-1.fmi
- Discard non-convex and anti-clockwise rectangles as invalid

* Mon Apr 25 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.4.25-1.fmi
- Much improved isoline builder

* Fri Apr  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.4.1-1.fmi
- A faster hole assignment algorithm.
- Removed GeosBuilder for being much too slow in comparison.

* Tue Mar 29 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.3.29-1.fmi
- A faster polygon builder algorithm

* Thu Oct 15 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.10.15-1.fmi
- New polygon building algorithm

* Wed Sep 16 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.9.16-2.fmi
- Fixed rounding errors when cell vertex values are exactly equal to the contour limit

* Wed Sep 16 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.9.16-1.fmi
- Fixed the contourer to work at saddle points

* Thu Sep 10 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.9.10-1.fmi
- FmiBuilder can not build valid OGC simple features and is faster than GeosBuilder

* Mon Mar 30 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.3.30-1.fmi
- Added handling of missing coordinates (the rectangle is not contoured)
- Speed optimizations

* Thu Oct 23 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.10.23-1.fmi
- Added GEOS dependencies

* Thu Aug 21 2014 Tuomo Lauri <tuomo.lauri@fmi.fi> - 14.8.21-2.fmi
- Fixed memory leaks related to GEOS Polygonizer and LineMerger

* Tue May 27 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.27-1.fmi
- Fixed GEOS geometry cleaner to inspect polygons in more detail

* Mon Mar 10 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.3.10-1.fmi
- Added GEOS geometry cleanup after polygon contouring

* Fri Feb 21 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.2.21-1.fmi
- Added support for building GEOS geometries

* Tue Jul 30 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.30-1.fmi
- Added support for contouring global data by wrapping the grids in X-direction

* Mon Jul 15 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.15-1.fmi
- Fixed name lookup issues

* Wed Jul  3 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.3-1.fmi
- Update to boost 1.54
- Refactored LogLinearInterpolation and LinearInterpolation code

* Sat Jun 22 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.6.22-1.fmi
- Added more ugliness to make the code compile with gcc 4.8

* Thu Jun 20 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.6.20-1.fmi
- Added logarithmic interpolation intended for data similar to precipitation intensity

* Wed Nov  7 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.11.7-1.fmi
- Upgrade to boost 1.52

* Thu Jul  5 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.5-1.el6.fmi
- Migration to boost 1.50

* Fri Sep 23 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.9.23-1.el5.fmi
- Added detection for empty grids to avoid segmentation faults

* Wed Jul 20 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.7.20-1.el5.fmi
- Upgrade to boost 1.47

* Thu Mar 24 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.3.24-1.el5.fmi
- Upgrade to boost 1.46

* Thu Mar 17 2011 pkeranen <pekka.keranen@geosaaga.fi> - 11.3.17-1.el6.fmi
- Fixed compilation problems in RHEL6

* Wed Sep  8 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.8-1.el5.fmi
- Handle NaN properly in Savitzky-Golay smoothing

* Tue Sep  7 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.7-1.el5.fmi
- Added Savitzky-Golay smoothing

* Thu Sep  2 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.2-1.el5.fmi
- Added possibility to feed coordinates to the contourer
- The API is NOT backwards compatible

* Tue Jan 19 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.1.19-1.el5.fmi
- Upgrade to boost 1.41

* Thu Oct 23 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.10.23-1.el5.fmi
- Merged missing value handler into the traits class

* Wed Oct 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.10.22-1.el5.fmi
- Added Tron.h to make the life of consultants easier

* Wed Oct  8 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.10.8-1.el5.fmi
- Bugfix in polygon builder

* Wed Sep 24 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.24-1.el5.fmi
- Bugfix so that fills and lines match.

* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-1.el5.fmi
- Upgrade to boost 1.36 static libraries

* Wed Nov  7 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-5.el5.fmi
- Fixed regression test build system

* Mon Sep 24 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-4.el5.fmi
- Fixed "make depend".

* Fri Sep 14 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-3.el5.fmi
- Added "make tag" feature

* Thu Sep 13 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-2.el5.fmi
- Improved make system

* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

