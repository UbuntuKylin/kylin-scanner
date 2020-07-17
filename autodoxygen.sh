#!/bin/bash

curPath=$(pwd)
docsPath="$curPath/docs"

sed -i "/^OUTPUT_DIRECTORY/c  OUTPUT_DIRECTORY  =  ${docsPath}" Doxyfile
sed -i "/^INPUT /c  INPUT  =  ${curPath}" Doxyfile
sed -i '/^PROJECT_NAME /c  PROJECT_NAME  =  "Kylin-scanner API"' Doxyfile

# Chinese
sed -i '/^OUTPUT_LANGUAGE /c  OUTPUT_LANGUAGE  =  Chinese' Doxyfile

# CHM
sed -i '/^CHM_INDEX_ENCODING /c  CHM_INDEX_ENCODING  =  UTF-8 ' Doxyfile

sed -i '/^RECURSIVE/c  RECURSIVE  =  YES' Doxyfile
sed -i '/^EXTRACT_ALL/c  EXTRACT_ALL  =  YES' Doxyfile
sed -i '/^EXTRACT_PRIVATE/c  EXTRACT_PRIVATE  =  YES' Doxyfile
sed -i '/^EXTRACT_STATIC/c  EXTRACT_STATIC  =  YES' Doxyfile

# Graph
sed -i '/^HAVE_DOT/c  HAVE_DOT  =  YES' Doxyfile
sed -i '/^DOT_NUM_THREADS/c  DOT_NUM_THREADS  =  4' Doxyfile
sed -i '/^CALL_GRAPH/c  CALL_GRAPH  =  YES' Doxyfile
sed -i '/^CALLER_GRAPH/c  CALLER_GRAPH  =  YES' Doxyfile
sed -i '/^EXTRACT_ALL/c  EXTRACT_ALL  =  YES' Doxyfile
sed -i '/^EXTRACT_STATIC/c  EXTRACT_STATIC  =  YES' Doxyfile
sed -i '/^EXTRACT_PRIVATE/c  EXTRACT_PRIVATE  =  YES' Doxyfile

sed -i '/^OPTIMIZE_OUTPUT_FOR_C/c  OPTIMIZE_OUTPUT_FOR_C  =  YES' Doxyfile
sed -i '/^GENERATE_HTML/c  GENERATE_HTML  =  YES' Doxyfile
