#
# CASA Fits
#

add_library (casa_fits
FITS/FITSTimedTable.cc
FITS/FITSTable2.cc
FITS/CopyRecord.cc
FITS/FITSHistoryUtil.cc
FITS/FITSMultiTable.cc
FITS/hdu2.cc
FITS/FITSError.cc
FITS/FITSDateUtil.cc
FITS/FITSTable3.cc
FITS/SDFITSTable.cc
FITS/FITSTable.cc
FITS/fits.cc
FITS/BasicFITS.cc
FITS/FITSKeywordUtil.cc
FITS/FITSSpectralUtil.cc
FITS/fitsio.cc
FITS/BinTable.cc
FITS/blockio.cc
FITS/FITSReader.cc
)

set(top_level_headers
FITS.h
)

init_pch_support(casa_fits ${top_level_headers})

target_link_libraries (casa_fits casa_measures ${CFITSIO_LIBRARIES} ${CASACORE_ARCH_LIBS})

add_subdirectory (apps)

#the tests
add_subdirectory (FITS/test ${EXCL_ALL})

install (TARGETS casa_fits
LIBRARY DESTINATION lib${LIB_SUFFIX}
ARCHIVE DESTINATION lib${LIB_SUFFIX}
LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install (FILES
FITS/BasicFITS.h
FITS/BinTable.h
FITS/blockio.h
FITS/CopyRecord.h
FITS/FITS2.h
FITS/FITS2.tcc
FITS/FITSDateUtil.h
FITS/FITSError.h
FITS/FITSFieldCopier.h
FITS/fits.h
FITS/FITSHistoryUtil.h
FITS/fitsio.h
FITS/FITSKeywordUtil.h
FITS/FITSMultiTable.h
FITS/FITSSpectralUtil.h
FITS/FITSTable.h
FITS/FITSTimedTable.h
FITS/hdu.h
FITS/hdu.tcc
FITS/SDFITSTable.h
FITS/FITSReader.h
DESTINATION include/casacore/fits/FITS
)

install (FILES ${top_level_headers}
DESTINATION include/casacore/fits
)


