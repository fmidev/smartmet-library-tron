%define DIRNAME tron
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: tron library
Name: %{SPECNAME}
Version: 24.8.7
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-tron
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)

%if 0%{?rhel} && 0%{rhel} < 9
%define smartmet_boost boost169
%else
%define smartmet_boost boost
%endif

BuildRequires: %{smartmet_boost}-devel
BuildRequires: gcc-c++
BuildRequires: geos312-devel
BuildRequires: make
BuildRequires: rpm-build
BuildRequires: smartmet-utils-devel >= 24.8.7
BuildRequires: smartmet-library-macgyver-devel >= 24.8.7
Requires: geos312
Provides: %{LIBNAME}
Obsoletes: libsmartmet-tron < 17.1.4
Obsoletes: libsmartmet-tron-debuginfo < 17.1.4
#TestRequires: %{smartmet_boost}-devel
#TestRequires: gcc-c++
#TestRequires: smartmet-library-macgyver-devel
#TestRequires: smartmet-library-gis-devel >= 24.8.7
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
%defattr(0775,root,root,0775)
%{_libdir}/lib%{LIBNAME}.so

%package -n %{SPECNAME}-devel
Summary: FMI Tron library development files
Provides: %{SPECNAME}-devel
Requires: %{SPECNAME} = %{version}-%{release}
BuildRequires: %{smartmet_boost}-devel
BuildRequires: gcc-c++
%if %{defined el7}
BuildRequires: devtoolset-7-gcc-c++
#TestRequires: devtoolset-7-gcc-c++
%endif
BuildRequires: geos312-devel
BuildRequires: make
BuildRequires: rpm-build
BuildRequires: smartmet-library-macgyver-devel >= 24.8.7

%description -n %{SPECNAME}-devel
FMI Tron library development files

%files -n %{SPECNAME}-devel
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}

%changelog
* Wed Aug  7 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.7-1.fmi
- Update to gdal-3.8, geos-3.12, proj-94 and fmt-11

* Mon Jul 22 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.22-1.fmi
- Replace BOOST_FOREACH and boost::array

* Fri Jul 12 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.12-1.fmi
- Replace many boost library types with C++ standard library ones

* Fri Jul 28 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.28-1.fmi
- Repackage due to bulk ABI changes in macgyver/newbase/spine

* Mon Jul 10 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.10-1.fmi
- Use postgresql 15, gdal 3.5, geos 3.11 and proj-9.0

* Fri Jun 17 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.6.17-1.fmi
- Add support for RHEL9. Update libpqxx to 7.7.0 (rhel8+) and fmt to 8.1.1

* Mon May 23 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.5.23-1.fmi
- Fixed so-file to be executable

* Fri Jan 21 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.1.21-1.fmi
- Repackage due to upgrade of packages from PGDG repo: gdal-3.4, geos-3.10, proj-8.2

* Mon Jun 21 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.6.21-1.fmi
- Silenced CodeChecker warnings

* Sat Jun 19 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.6.19-1.fmi
- Silenced CodeChecker warnings

* Thu Apr 15 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.4.15-1.fmi
- Switch from static to dynamic library

* Tue Apr 13 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.4.13-1.fmi
- Fixed saddle points to be handled similarly for isolines and isobands

* Fri Apr  9 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.4.9-2.fmi
- Use enum classes

* Fri Apr  9 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.4.9-1.fmi
- Use Robin Hood unordered_set for speed
- Added SmallVector to avoid std::vector malloc/free calls
- Optimized FlipGrid for speed
- Use unique_ptr instead of shared_ptr for speed

* Thu Apr  8 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.4.8-1.fmi
- Fixed saddle point detection
- Reserve vector sizes in advance for a great speed improvement

* Wed Feb 10 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.10-1.fmi
- Merged contouring fixes and improvements made in WGS84 branch

* Thu Jan 14 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.14-1.fmi
- Repackaged smartmet to resolve debuginfo issues

* Tue Jan 12 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.12-1.fmi
- Removed obsolete proj-epsg dependency

* Tue Jan  5 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.5-1.fmi
- Upgrade to geos39

* Thu Dec 31 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.31-1.fmi
- Require devtoolset-7-gcc-c++ to be able to use clang++ -std=c++17

* Mon Dec 28 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.28-1.fmi
- Upgrade to GEOS 3.8

* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgrade to Boost 1.69

* Sun Apr 12 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.12-1.fmi
- Moved grid validation outside to smartmet-library-gis

* Thu Apr  9 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.9-1.fmi
- Discard oblong grid cells (dx,dy ratio > 100) when contouring due to suspect coordinates

* Thu Mar 26 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.26-1.fmi
- Repackaged after GEOS 3.8 update

* Wed Feb 12 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.2.12-1.fmi
- Fixed hole geometry to be LinearRing

* Wed Dec 11 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.11-1.fmi
- Upgrade to geos38

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

