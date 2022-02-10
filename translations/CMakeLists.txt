# ---- Translations ------------------------------------------------------

find_package(Qt5 COMPONENTS Core LinguistTools REQUIRED)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/src
	krokonf_en.ts
)

add_custom_target(translations ALL DEPENDS ${QM_FILES})

install(FILES ${QM_FILES}
		DESTINATION ${TRANSLATIONS_INSTALL_SUBDIR}
)

# vim: tabstop=4
