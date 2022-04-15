#define macros
#uncomment dir_bin_x86 and dir_inter_x86 for x86 compilation
EXECUTABLE_NAME = main.exe
DIR_SRC = ..\src
DIR_INCLUDE_VENDOR = ..\src\vendor
DIR_BIN = ..\bin
#DIR_BIN_X86 = $(DIR_BIN)\x86
DIR_INTERMEDIATE = ..\intermediate
#DIR_INTERMEDIATE_X86 = $(DIR_INTERMEDIATE)\x86

LIB_FILES = \

C_FLAGS = /D_CRT_SECURE_NO_WARNINGS \
  /nologo \
  /MT \
  /Zi \
  /FC \
  /c \
  /EHsc \
  /WX \
  /W4 \
  /Od \
  /wd4505 /wd4200 /wd4201 /wd4100 /wd4189 /wd4312 \
  /std:c++14 \
  /Fd$(DIR_BIN)\ \
  /DLOGGING \
  /DLOGCOLOR \


#add _x86 to dir_bin and dir_inter for x86 compilation
{$(DIR_SRC)}.cpp{$(DIR_INTERMEDIATE)}.obj :
  @echo.
  @echo Compiling $(DIR_SRC)
  @echo -----------------------------------------------
  cl $(C_FLAGS) /Fo$(DIR_INTERMEDIATE)\ $<

#add _x86 to dir_bin and dir_inter for x86 compilation
$(EXECUTABLE_NAME) : $(DIR_INTERMEDIATE)\*.obj
  @echo.
  @echo Linking $(EXECUTABLE_NAME)
  @echo -----------------------------------------------
  link /DEBUG:FULL /out:$(DIR_BIN)\$(EXECUTABLE_NAME) $(DIR_INTERMEDIATE)\*.obj $(LIB_FILES)

# build application
main: $(EXECUTABLE_NAME)

# create output directories
#add _x86 to dir_bin and dir_inter for x86 compilation
create_dirs:
 @if not exist $(DIR_BIN) mkdir $(DIR_BIN)
 @if not exist $(DIR_INTERMEDIATE) mkdir $(DIR_INTERMEDIATE)

# delete output directories
clean:
 @if exist $(DIR_BIN) rmdir /S /Q $(DIR_BIN)
 @if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

# create directories and build application
all: clean create_dirs main