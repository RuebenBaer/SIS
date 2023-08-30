CXX = g++
CPPFLAGS =  \
	-Wall \
	-pipe \
	-mthreads \
	-O2 \
	-std=gnu++11

INCLUDES =  \
	-I$(WXINCLUDE) \
	-I$(WXINCLUDESETUP)

DEFS =  \
	-D__WXMSW__ \
	-DwxUSE_UNICODE

#WXLIBPATH = -L${WXLIBPATH}
include wxlibLaptop.mk
LIBS += \
	-lkernel32 \
	-luser32 \
	-lgdi32 \
	-lcomdlg32 \
	-lwinspool \
	-lwinmm \
	-lshell32 \
	-lshlwapi \
	-lcomctl32 \
	-lole32 \
	-loleaut32 \
	-luuid \
	-lrpcrt4 \
	-ladvapi32 \
	-lversion \
	-lws2_32 \
	-lwininet \
	-loleacc \
	-luxtheme

OBJDIR = obj
SRCDIR = source
OBJS = \
	$(OBJDIR)\Vektor\Vektor.o \
	$(OBJDIR)\Anzeige.o \
	$(OBJDIR)\base.o \
	$(OBJDIR)\Dbl_Eingabe.o \
	$(OBJDIR)\SIS_Kern.o \
	$(OBJDIR)\Textanzeige.o \
	$(OBJDIR)\resource.res
OUT = GUI_SIS.exe

.PHONEY: all clean

all: $(OBJS)
	$(CXX) $(CPPFLAGS) $(DEFS) -L$(WXLIBPATH) -o $(OUT) $(OBJS) -s $(LIBS)

$(OBJDIR)\Vektor\Vektor.o: $(SRCDIR)\Vektor\Vektor.cpp
	if not exist $(OBJDIR)\Vektor mkdir $(OBJDIR)\Vektor
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\Anzeige.o: $(SRCDIR)\Anzeige.cpp
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\base.o: $(SRCDIR)\base.cpp
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\Dbl_Eingabe.o: $(SRCDIR)\Dbl_Eingabe.cpp
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\SIS_Kern.o: $(SRCDIR)\SIS_Kern.cpp
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\Textanzeige.o: $(SRCDIR)\Textanzeige.cpp
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(OBJDIR)\resource.res: source\resource.rc
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	windres.exe $(INCLUDES) -J rc -O coff -i source\resource.rc -o $(OBJDIR)\resource.res

clean:
	del $(OBJS) $(OUT)
