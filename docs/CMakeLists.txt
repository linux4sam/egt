set(srcdir ${CMAKE_CURRENT_SOURCE_DIR})

set(DOCSET_INPUT ${CMAKE_CURRENT_BINARY_DIR}/html)
set(DOCSET_TAG ${CMAKE_CURRENT_BINARY_DIR}/egt.tag)
set(DOCSET_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/EGT.docset)
set(DOCSET_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/logo_icon.png)
set(DOCSET_INFO ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
set(DOCSET_TARBALL EGT.tgz)

configure_file(Doxyfile.in Doxyfile @ONLY)
configure_file(Info.plist.in Info.plist @ONLY)

add_custom_target(docs ${DOXYGEN})

add_custom_target(docset
    COMMAND mkdir -p ${DOCSET_OUTPUT}/Contents/Resources/Documents/
    COMMAND cp ${DOCSET_ICON} ${DOCSET_OUTPUT}/icon@2x.png
    COMMAND convert -resize 16x16 ${DOCSET_ICON} ${DOCSET_OUTPUT}/icon.png
    COMMAND cp -a ${DOCSET_INPUT}/* ${DOCSET_OUTPUT}/Contents/Resources/Documents/
    COMMAND cp ${DOCSET_INFO} ${DOCSET_OUTPUT}/Contents/
    COMMAND doxytag2zealdb --tag ${DOCSET_TAG} --db ${DOCSET_OUTPUT}/Contents/Resources/docSet.dsidx --include-parent-scopes --include-function-signatures
# There's a bug in zeal where if the first file it opens is the icon,
# it won't able to see it.  So, when creating the tarball, sort by name
# as a workaround.
    COMMAND tar --sort=name -cz -f ${DOCSET_TARBALL} EGT.docset/
    DEPENDS docs)
