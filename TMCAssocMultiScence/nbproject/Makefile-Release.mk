#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/tools/Markup.o \
	${OBJECTDIR}/tools/dir_info.o \
	${OBJECTDIR}/tools/log.o \
	${OBJECTDIR}/tools/macro.o \
	${OBJECTDIR}/tools/sem_unix.o \
	${OBJECTDIR}/tools/toolkit.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tmcassocmultiscence

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tmcassocmultiscence: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tmcassocmultiscence ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/tools/Markup.o: tools/Markup.cpp 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/Markup.o tools/Markup.cpp

${OBJECTDIR}/tools/dir_info.o: tools/dir_info.c 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/dir_info.o tools/dir_info.c

${OBJECTDIR}/tools/log.o: tools/log.cpp 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/log.o tools/log.cpp

${OBJECTDIR}/tools/macro.o: tools/macro.cpp 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/macro.o tools/macro.cpp

${OBJECTDIR}/tools/sem_unix.o: tools/sem_unix.c 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/sem_unix.o tools/sem_unix.c

${OBJECTDIR}/tools/toolkit.o: tools/toolkit.c 
	${MKDIR} -p ${OBJECTDIR}/tools
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tools/toolkit.o tools/toolkit.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tmcassocmultiscence

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
