cmake_minimum_required(VERSION 2.8.5)
project(staryWicher LANGUAGES C)

# ACE
add_subdirectory(ACE ace)

add_executable(
	staryWicher.elf main.c menu.c anim.c
	
) # Compiler emits .elf file, we need hunk for Amiga

target_compile_options(staryWicher.elf PRIVATE -Wall -Wextra)

target_link_libraries(staryWicher.elf ace)
target_compile_definitions(ace PUBLIC ACE_DEBUG ACE_DEBUG_UAE)

add_custom_command( # So call elf2hunk
	TARGET staryWicher.elf POST_BUILD
	COMMAND ${ELF2HUNK} staryWicher.elf staryWicher.exe
)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/res) # st�d bierzemy pliki do konwersji
set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data) # tu maj� l�dowa�
file(MAKE_DIRECTORY ${DATA_DIR}) #za���my data





set(PLT_PATH ${DATA_DIR}/falkon.plt) #tak si� b�dzie nazywa� docelowa paleta
convertPalette(staryWicher.elf ${RES_DIR}/falkon.pal ${PLT_PATH}) # z res/staryWicher.pal do build/data/staryWicher.plt

convertBitmaps(
	TARGET staryWicher.elf PALETTE ${PLT_PATH}
	INTERLEAVED SOURCES
		${RES_DIR}/staryWicher.png 
	DESTINATIONS
		${DATA_DIR}/staryWicher.bm 
)



file(
	COPY ${RES_DIR}/placeholdermod.mod ${RES_DIR}/topaz.fnt
	DESTINATION ${DATA_DIR}
)
