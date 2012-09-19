#!/bin/bash

#-D_REENTRANT -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
VIS="-fvisibility=hidden"
INL="-fvisibility-inlines-hidden"
CF="-Wall -Wextra -O0 -D_DEBUG"
CXF="-D_GLIBCXX_CONCEPT_CHECK"
LF=""
MKSPEC=""
if test `uname -s` != "Darwin" ; then
  CF="${CF} -ggdb3 "
  CXF="${CXF} -D_GLIBCXX_DEBUG -Werror -fopenmp"
  LF="${LF} -fopenmp"
else
  ALLWARN=""
  #ALLWARN='-Werror -Wno-padded -Wno-weak-vtables -Weverything'
  # -Wno-#warnings: Temporarily disable #warnings until a new release of QT is issued.
  # -Wno-padded: Don't care about 'adding bytes' to ensure byte alignment.
  # -Wno-weak-vtables: We have several classes which do not have out-of-line virtual
  #                    method definitions (resulting in the vtable being emitted in
  #                    every translation unit). Examples: Exception (
  CF="${CF}"
  CXF="${CXF} ${ALLWARN}"
  MKSPEC="-spec unsupported/macx-clang"
fi
if test -n "${QT_BIN}" ; then
    echo "Using custom qmake..."
    qm="${QT_BIN}/qmake"
else
    qm="qmake"
fi
for d in . ; do
  pushd ${d} &>/dev/null
    ${qm} \
        ${MKSPEC} \
        QMAKE_CONFIG+="debug" \
        CONFIG+="debug" \
        QMAKE_CFLAGS="${VIS} ${CF}" \
        QMAKE_CXXFLAGS="${VIS} ${INL} ${CF} ${CXF}" \
        QMAKE_CFLAGS_RELEASE="-O0" \
        QMAKE_CXXFLAGS_RELEASE="-O0" \
        QMAKE_LFLAGS="${LF}" \
        -recursive || exit 1
  popd &>/dev/null
done
