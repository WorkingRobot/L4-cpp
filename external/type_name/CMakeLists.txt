include(FetchContent)

FetchContent_Declare(
  type_name
  GIT_REPOSITORY https://github.com/adambadura/type_name.git
  GIT_TAG        c3c6029987e39debdae8b94fcf6aee7c6df8ed16
)
FetchContent_Populate(type_name)

s_project(ext::type_name HEADER_LIB IS_SIMPLE INCLUDED_LOCATION "${type_name_SOURCE_DIR}/include")
