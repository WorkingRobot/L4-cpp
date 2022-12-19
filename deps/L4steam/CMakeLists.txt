include(FetchContent)

# This may fail, in which case you should go to this link yourself and login.
# Afterwards, this call will succeed. I believe they mark your IP as whitelisted
# for some time, and you don't actually require any cookies.
FetchContent_Declare(
  steamworks_sdk
  URL https://partner.steamgames.com/downloads/steamworks_sdk.zip
  DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(steamworks_sdk)

set(STEAM_INCLUDED_LOCATION "public")
if (PROJECT_PLATFORM STREQUAL "win")
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(STEAM_IMPORTED_LOCATION "redistributable_bin/steam_api.dll")
        set(STEAM_IMPLIB_LOCATION "redistributable_bin/steam_api.lib")
    else()
        set(STEAM_IMPORTED_LOCATION "redistributable_bin/win64/steam_api64.dll")
        set(STEAM_IMPLIB_LOCATION "redistributable_bin/win64/steam_api64.lib")
    endif()
elseif(PROJECT_PLATFORM STREQUAL "lnx")
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(STEAM_IMPORTED_LOCATION "redistributable_bin/linux64/libsteam_api.so")
    else()
        set(STEAM_IMPORTED_LOCATION "redistributable_bin/linux32/libsteam_api.so")
    endif()
elseif(PROJECT_PLATFORM STREQUAL "osx")
    set(STEAM_IMPORTED_LOCATION "redistributable_bin/osx/libsteam_api.dylib")
endif()

s_project(L4::steam IMPORT_SHARED_LIB INCLUDED_LOCATION "${steamworks_sdk_SOURCE_DIR}/${STEAM_INCLUDED_LOCATION}" PLATFORMS ${PROJECT_PLATFORM})
set_target_properties(L4::steam PROPERTIES IMPORTED_LOCATION "${steamworks_sdk_SOURCE_DIR}/${STEAM_IMPORTED_LOCATION}")

if (STEAM_IMPLIB_LOCATION)
    set_target_properties(L4::steam PROPERTIES IMPORTED_IMPLIB "${steamworks_sdk_SOURCE_DIR}/${STEAM_IMPLIB_LOCATION}")
endif()
